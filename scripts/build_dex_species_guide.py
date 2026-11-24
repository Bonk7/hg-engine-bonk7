"""Generate the Pokédex's battle and learnset reference pages."""

import json
import re
from pathlib import Path
import struct
import sys
from textwrap import wrap

from ndspy.narc import NARC

from build_dex_evolution_guide import read_constants, read_names

TYPES = ("Normal", "Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel",
         "Fairy", "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark", "Typeless", "Stellar")
EGG_GROUPS = ("None", "Monster", "Water 1", "Bug", "Flying", "Field", "Fairy", "Grass",
              "Human-Like", "Water 3", "Mineral", "Amorphous", "Water 2", "Ditto", "Dragon", "Undiscovered")


def message_names(path):
    return [line.removesuffix(r"\r") for line in Path(path).read_text(encoding="utf-8").splitlines()]


def unique_names(values, names):
    return list(dict.fromkeys(names[i] for i in values if 0 < i < len(names)))


def profile_pages(entry, hidden, abilities, items):
    hp, attack, defense, speed, sp_attack, sp_defense, type1, type2, catch_rate = entry[:9]
    types = " / ".join(dict.fromkeys(TYPES[t] for t in (type1, type2) if t < len(TYPES)))
    pages = [[types or "Type unknown", f"HP {hp}  Atk {attack}  Def {defense}",
              f"SpA {sp_attack}  SpD {sp_defense}  Spe {speed}"]]
    normal = unique_names((struct.unpack_from("<H", entry, offset)[0] for offset in (22, 26)), abilities)
    pages.append(["Abilities", " / ".join(normal) or "None", f"Hidden: {abilities[hidden]}" if 0 < hidden < len(abilities) else "Hidden: none"])
    groups = " / ".join(dict.fromkeys(EGG_GROUPS[g] for g in entry[20:22] if g < len(EGG_GROUPS)))
    pages.append([f"Catch rate: {catch_rate}", f"Egg groups: {groups}"])
    held = unique_names(struct.unpack_from("<HH", entry, 12), items)
    if held:
        pages.append(["Wild held items", *held])
    return pages


def training_pages(species, entry, learnset, moves, move_ids, machines, tutors):
    evs = struct.unpack_from("<H", entry, 10)[0]
    yields = [f"{name} {(evs >> (i * 2)) & 3}" for i, name in enumerate(("HP", "Atk", "Def", "Spe", "SpA", "SpD")) if (evs >> (i * 2)) & 3]
    pages = [["Training", "EV yield: " + (", ".join(yields) or "None")]]
    for key, heading in (("LevelMoves", "Level-up moves"), ("MachineMoves", "TM/HM moves"),
                         ("EggMoves", "Egg moves")):
        display = []
        if key == "LevelMoves":
            for move in learnset.get(key, ()):
                name = moves[move_ids[move["Move"]]]
                display.append(f"L{move['Level']} {name}" if move["Level"] else f"Evo: {name}")
        else:
            display = [moves[move_ids[move]] for move in learnset.get(key, ())
                       if key != "MachineMoves" or move in machines]
        lines = wrap(", ".join(display) or "None", 34, break_long_words=False, break_on_hyphens=False)
        pages.extend([[heading, *lines[i:i + 2]] for i in range(0, len(lines), 2)])
    tutor_moves = set(learnset.get("TutorMoves", ()))
    tutor_pages = [["Tutor: " + moves[move_ids[move]], location, detail]
                   for move, (location, detail) in tutors.items() if move in tutor_moves]
    # Blackthorn's separate tutors use the species and type checks in the map script.
    blackthorn_moves = {
        "MOVE_BLAST_BURN": {6, 157, 257, 392},
        "MOVE_HYDRO_CANNON": {9, 160, 260, 395},
        "MOVE_FRENZY_PLANT": {3, 154, 254, 389},
    }
    for move, eligible in blackthorn_moves.items():
        if species in eligible:
            tutor_pages.append(["Tutor: " + moves[move_ids[move]], "Blackthorn Move Tutor House", "Max friendship required"])
    if TYPES.index("Dragon") in entry[6:8]:
        tutor_pages.append(["Tutor: " + moves[move_ids["MOVE_DRACO_METEOR"]],
                            "Blackthorn Move Tutor House", "Max friendship required"])
    pages.extend(tutor_pages or [["Tutor moves", "None"]])
    return pages


def tutor_locations():
    positions = {
        "FRONTIER_TOP_LEFT": ("Battle Frontier", "Upper-left tutor"),
        "FRONTIER_TOP_RIGHT": ("Battle Frontier", "Upper-right tutor"),
        "FRONTIER_BOTTOM_RIGHT": ("Battle Frontier", "Lower-right tutor"),
        "HEADBUTT": ("Ilex Forest", "Headbutt tutor"),
    }
    source = Path("src/field/move_tutor.c").read_text()
    return {move: positions[npc] for move, npc in re.findall(
        r"\{\s*(MOVE_\w+),\s*\d+,\s*MOVE_TUTOR_NPC_(\w+)\s*\}", source)}


def write_pages(text_file, index_file, pages_by_species):
    text = []
    index = bytearray()
    for pages in pages_by_species:
        index.extend(struct.pack("<HH", len(text), len(pages)))
        for page_number, (heading, *lines) in enumerate(pages, 1):
            text.append(r"\n".join((f"{page_number}/{len(pages)} {heading}", *lines)))
    if len(text) > 65535:
        raise ValueError(f"Too many Pokédex pages for {text_file}")
    text_file.write_text("\n".join(text) + "\n", encoding="utf-8")
    index_file.write_bytes(index)


def main():
    build, text_file, index_file, training_text, training_index = map(Path, sys.argv[1:])
    pokemon = NARC.fromFile(build / "narc/personal.narc").files
    hidden = [record[0] for record in struct.iter_unpack("<H", (build / "HiddenAbilityTable.bin").read_bytes())]
    learnsets = json.loads(Path("data/learnsets/learnsets.json").read_text())
    species_keys = read_constants(Path("include/constants/species.h"), "SPECIES_")
    move_ids = read_constants(Path("include/constants/moves.h"), "MOVE_")
    moves = read_names(build / "rawtext/750")
    abilities = message_names("data/text/720.txt")
    items = message_names("data/text/222.txt")
    learnsets_by_id = {species_keys[key]: value for key, value in learnsets.items() if key in species_keys}

    machines = set(re.findall(r"^\s*(MOVE_\w+),", Path("data/MachineMoves.c").read_text(), re.M))
    tutors = tutor_locations()
    profiles = []
    training = []
    for species in range(2048):
        if 0 < species < len(pokemon):
            profiles.append(profile_pages(pokemon[species], hidden[species] if species < len(hidden) else 0,
                                          abilities, items))
            training.append(training_pages(species, pokemon[species], learnsets_by_id.get(species, {}),
                                           moves, move_ids, machines, tutors))
        else:
            profiles.append([["No species data."]])
            training.append([["No species data."]])
    write_pages(text_file, index_file, profiles)
    write_pages(training_text, training_index, training)


if __name__ == "__main__":
    main()
