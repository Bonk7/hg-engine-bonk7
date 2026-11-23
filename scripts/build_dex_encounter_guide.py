"""Generate paginated Pokédex lookup entries from the built encounter archives."""

from collections import defaultdict
from pathlib import Path
import re
import struct
import sys

from ndspy.narc import NARC



PERIODS = ("Morning", "Day", "Night")
METHODS = ("Surf", "Rock Smash", "Old Rod", "Good Rod", "Super Rod")
SPECIES_MASK = 0x7FF


def title(label):
    text = label.replace("_", " ").title()
    return re.sub(r"\b([Bb]?\d+)[Ff]\b", lambda match: match[0].upper(), text)


def area_names():
    header = Path("include/constants/encounter_tables.h").read_text()
    return {
        int(index): title(name.replace("UNKNOWN", ""))
        for name, index in re.findall(r"ENCDATA_[A-Z]\w+?_(\w+) = (\d+)", header)
        if "UNUSED" not in name and "PLACEHOLDER" not in name
    }


def add(data, slots, area, method, period):
    for species, low, high in slots:
        if species & SPECIES_MASK and 0 < low <= high <= 100:
            data[species & SPECIES_MASK][(area, method)][period].update(range(low, high + 1))


def standard_encounters(data, archive):
    names = area_names()
    for index, entry in enumerate(archive.files):
        if index not in names:
            continue
        area = names[index]
        rates = entry[:6]
        levels = entry[8:20]
        land = [struct.unpack_from("<12H", entry, 20 + 24 * time) for time in range(3)]
        radio = struct.unpack_from("<4H", entry, 92)
        land_swarm, surf_swarm, night_fish, fish_swarm = struct.unpack_from("<4H", entry, 188)
        if rates[0]:
            for period, species in zip(PERIODS, land):
                add(data, ((mon, level, level) for mon, level in zip(species, levels)), area, "Walking", period)
            for group, method in enumerate(("Hoenn Sound", "Sinnoh Sound")):
                add(data, ((radio[group * 2 + i // 2], levels[2 + i], levels[2 + i]) for i in range(4)), area, method, "Any time")
            add(data, ((land_swarm, level, level) for level in levels[:2]), area, "Walking, swarm", "Any time")

        cursor = 100
        for method, rate, count in zip(METHODS, rates[1:], (5, 2, 5, 5, 5)):
            slots = [(species, low, high) for low, high, species in struct.iter_unpack("<BBH", entry[cursor:cursor + count * 4])]
            cursor += count * 4
            if not rate:
                continue
            if method in ("Good Rod", "Super Rod"):
                night_slot = 3 if method == "Good Rod" else 1
                if night_fish != slots[night_slot][0]:
                    add(data, slots, area, method, "Daytime")
                    night_slots = list(slots)
                    night_slots[night_slot] = (night_fish, *slots[night_slot][1:])
                    add(data, night_slots, area, method, "Night")
                else:
                    add(data, slots, area, method, "Any time")
            else:
                add(data, slots, area, method, "Any time")
            swarm_indices = {"Surf": (0,), "Old Rod": (2,), "Good Rod": (0, 2, 3), "Super Rod": range(5)}
            if method in swarm_indices:
                swarm = surf_swarm if method == "Surf" else fish_swarm
                add(data, ((swarm, *slots[i][1:]) for i in swarm_indices[method]), area, f"{method}, swarm", "Any time")


def safari_encounters(data, archive):
    names = re.findall(r"^    \[SAFARI_ZONE_AREA_(\w+)\]", Path("data/SafariEncounters.c").read_text(), re.M)
    assert len(names) == len(archive.files)
    for name, entry in zip(names, archive.files):
        area = "Safari: " + title(name)
        cursor = 8
        for method, bonus in zip(("Walking", "Surf", "Old Rod", "Good Rod", "Super Rod"), (10, 3, 2, 2, 2)):
            for phase, count in ((method, 10), (f"{method}, blocks", bonus)):
                for period in PERIODS:
                    slots = ((species, level, level) for species, level in struct.iter_unpack("<HH", entry[cursor:cursor + count * 4]))
                    add(data, slots, area, phase, period)
                    cursor += count * 4
            cursor += bonus * 4  # Object requirements follow the bonus slots.
        assert cursor == len(entry), name


def headbutt_encounters(data, archive):
    names = dict(re.findall(r"^    HeadbuttFile_(\d+)_(\w+) \w+;", Path("data/Headbutt.c").read_text(), re.M))
    for index, entry in enumerate(archive.files):
        normal, special = struct.unpack_from("<HH", entry)
        if not (normal or special):
            continue
        area = title(names[f"{index:03d}"]).replace("Pokmon", "Pokémon")
        for method, count, start, end in (("Headbutt", normal, 4, 52), ("Special Headbutt tree", special, 52, 76)):
            if count:
                add(data, struct.iter_unpack("<HBB", entry[start:end]), area, method, "Any time")


def level_display(levels):
    runs = []
    for level in sorted(levels):
        if runs and runs[-1][1] == level - 1:
            runs[-1][1] = level
        else:
            runs.append([level, level])
    return "Lv. " + ", ".join(str(lo) if lo == hi else f"{lo}-{hi}" for lo, hi in runs)


def pages_for_species(areas):
    pages = []
    for (area, method), periods in areas.items():
        by_level = defaultdict(list)
        for period, levels in periods.items():
            by_level[tuple(sorted(levels))].append(period)
        for levels, matching_periods in by_level.items():
            period = ", ".join(matching_periods)
            if period in ("Morning, Day, Night", "Daytime, Night"):
                period = "Any time"
            summary = f"{method}, {period}" if period != "Any time" else method
            # Each page uses the three native Pokédex description lines.
            pages.append([area, summary, level_display(levels)])
    count = len(pages)
    return [r"\n".join((f"{i}/{count} {area}", summary, levels))
            for i, (area, summary, levels) in enumerate(pages, 1)] or ["No listed wild encounters."]


def main():
    build, text_file, index_file = map(Path, sys.argv[1:])
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(set)))
    for reader, archive in ((standard_encounters, "encounters"), (safari_encounters, "safari"), (headbutt_encounters, "headbutt")):
        reader(data, NARC.fromFile(build / "narc" / f"{archive}.narc"))

    # Reserve all 11-bit dex species numbers so the ARM lookup cannot overrun the index.
    text = []
    index = bytearray()
    for species in range(SPECIES_MASK + 1):
        pages = pages_for_species(data.get(species, {}))
        index.extend(struct.pack("<HH", len(text), len(pages)))
        text.extend(pages)
    assert len(text) <= 65535
    text_file.write_text("\n".join(text) + "\n", encoding="utf-8")
    index_file.write_bytes(index)


if __name__ == "__main__":
    main()
