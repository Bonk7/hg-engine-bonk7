"""Build route-first Pokédex pages from the encounter guide's source tables."""

from collections import defaultdict
from pathlib import Path
import re
import struct
import sys

from ndspy.narc import NARC

from build_dex_encounter_guide import level_display, standard_encounters
from build_dex_evolution_guide import read_names


def main():
    build, output, index_path = map(Path, sys.argv[1:])
    names = read_names(build / "rawtext/237")
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(set)))
    standard_encounters(data, NARC.fromFile(build / "narc/encounters.narc"))
    maps = {
        int(route): int(map_id)
        for route, map_id in re.findall(r"^\.equ MAP_[RW](\d\d), (\d+)$", Path("asm/include/maps.inc").read_text(), re.M)
    }
    routes = defaultdict(list)
    for species, locations in sorted(data.items()):
        if species >= len(names) or not names[species]:
            continue
        for (area, method), periods in locations.items():
            match = re.fullmatch(r"Route (\d+)", area)
            if not match or int(match[1]) not in maps:
                continue
            grouped = defaultdict(list)
            for period, levels in periods.items():
                grouped[tuple(sorted(levels))].append(period)
            for levels, matching in grouped.items():
                when = ", ".join(matching)
                if when in ("Morning, Day, Night", "Daytime, Night", "Any time"):
                    when = ""
                routes[maps[int(match[1])]].append((species, area, method, when, level_display(levels)))

    text = ["Open this guide while on a route."]
    index = bytearray()
    for route_map in range(1024):
        records = routes[route_map]
        index.extend(struct.pack("<HH", len(text) if records else 0, len(records) or 1))
        for number, (species, area, method, when, levels) in enumerate(records, 1):
            detail = f"{method}, {when}" if when else method
            page = r"\n".join((f"{area} {number}/{len(records)} {names[species]}", detail, levels))
            text.extend((page, page + "  Caught"))
    for route_map in range(1024):
        for species, *_ in routes[route_map]:
            index.extend(struct.pack("<H", species))
    assert len(text) <= 65535
    output.write_text("\n".join(text) + "\n", encoding="utf-8")
    index_path.write_bytes(index)


if __name__ == "__main__":
    main()
