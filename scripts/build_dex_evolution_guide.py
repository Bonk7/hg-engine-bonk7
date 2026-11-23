"""Build the alternate Pokédex text from the game's evolution rules."""

from collections import defaultdict
from pathlib import Path
import re
import sys
from textwrap import wrap

PAGE_COUNT = 4
LINES_PER_PAGE = 3
LINE_WIDTH = 40

# These are the conditional level-up methods, including the stat comparisons.
LEVEL_RULES = {
    "LEVEL": "Lv. {param}",
    "LEVEL_MALE": "Male, Lv. {param}",
    "LEVEL_FEMALE": "Female, Lv. {param}",
    "LEVEL_DAY": "Lv. {param} by day",
    "LEVEL_NIGHT": "Lv. {param} at night",
    "LEVEL_DUSK": "Lv. {param} at dusk",
    "LEVEL_RAIN": "Lv. {param} in rain",
    "LEVEL_ATK_GT_DEF": "Lv. {param}, Atk above Def",
    "LEVEL_ATK_EQ_DEF": "Lv. {param}, Atk = Def",
    "LEVEL_ATK_LT_DEF": "Lv. {param}, Atk below Def",
    "LEVEL_PID_LO": "Lv. {param}, based on personality",
    "LEVEL_PID_HI": "Lv. {param}, based on personality",
    "LEVEL_NATURE_AMPED": "Lv. {param}, Amped nature",
    "LEVEL_NATURE_LOW_KEY": "Lv. {param}, Low Key nature",
    "LEVEL_NINJASK": "Lv. {param}",
    "LEVEL_SHEDINJA": "Lv. {param}; spare slot + Poké Ball",
    "LEVEL_DARK_TYPE_MON_IN_PARTY": "Lv. {param} with a Dark-type ally",
}
OTHER_RULES = {
    "FRIENDSHIP": "High friendship, then level up",
    "FRIENDSHIP_DAY": "High friendship, level up by day",
    "FRIENDSHIP_NIGHT": "High friendship, level up at night",
    "MAGNETIC_FIELD": "Level up on Route 43 or at the Power Plant",
    "MOSSY_ROCK": "Level up near a Moss Rock",
    "ICY_ROCK": "Level up near an Ice Rock",
    "TRADE": "Trade",
    "BEAUTY": "Level up with Beauty {param} or higher",
    "AMOUNT_OF_CRITICAL_HITS": "{param} critical hits in a battle",
    "HURT_IN_BATTLE_AMOUNT": "Level up missing {param} HP or more",
}


def read_names(directory):
    return [p.read_text(encoding="utf-8").strip() for p in sorted(directory.glob("*.txt"), key=lambda p: int(p.stem))]


def read_constants(path, prefix):
    return {name: int(value) for name, value in re.findall(rf"^#define ({prefix}\w+)\s+(\d+) *(?://[^\n]*)?$", path.read_text(), re.M)}


def display_name(names, constants, key):
    return names[int(key) if key.isdecimal() else constants[key]]


def condition(method, param, species_names, species, item_names, items, move_names, moves):
    if method in LEVEL_RULES:
        return LEVEL_RULES[method].format(param=param)
    if method in OTHER_RULES:
        return OTHER_RULES[method].format(param=param)
    if method in {"STONE", "STONE_MALE", "STONE_FEMALE", "ITEM_DAY", "ITEM_NIGHT", "TRADE_ITEM"}:
        item = display_name(item_names, items, param)
        suffix = {"STONE_MALE": " (male)", "STONE_FEMALE": " (female)", "ITEM_DAY": " by day", "ITEM_NIGHT": " at night"}.get(method, "")
        if method in {"ITEM_DAY", "ITEM_NIGHT"}:
            return f"Level up holding {item}{suffix}"
        return f"Use {item}{suffix}"
    if method == "TRADE_SPECIFIC_MON":
        return f"Trade for {display_name(species_names, species, param)}"
    if method == "HAS_MOVE":
        return f"Level up knowing {display_name(move_names, moves, param)}"
    if method == "HAS_MOVE_TYPE":
        return f"High friendship + {param.removeprefix('TYPE_').title()} move; level up"
    if method == "OTHER_PARTY_MON":
        return f"Level up with {display_name(species_names, species, param)}"
    raise ValueError(f"Unknown evolution method {method}")


def read_evolutions(source):
    blocks = re.findall(r"(?ms)^    \[(\w+)\] = \{\s*\.entries = \{(.*?)^        \},", source)
    if not blocks:
        raise ValueError("No evolution entries found")
    return {key: re.findall(r"\{ EVO_(\w+), ([^,]+), (.+?) \},", body) for key, body in blocks}


def make_pages(lines):
    pages = [[]]
    for line in lines:
        wrapped = wrap(line, LINE_WIDTH, break_long_words=False, break_on_hyphens=False)
        if len(pages[-1]) + len(wrapped) > LINES_PER_PAGE:
            pages.append([])
        pages[-1].extend(wrapped)
    if len(pages) > PAGE_COUNT:
        # Keep four complete pages and make any truncation visible in the guide.
        pages = pages[:PAGE_COUNT]
        pages[-1] = pages[-1][:LINES_PER_PAGE - 1] + ["More evolution options exist."]
    return [r"\n".join(page) for page in pages] + ["No more evolution entries."] * (PAGE_COUNT - len(pages))


def main():
    raw_text, output = map(Path, sys.argv[1:])
    species_names, move_names = (read_names(raw_text / archive) for archive in ("237", "750"))
    item_names = Path("data/text/222.txt").read_text(encoding="utf-8").splitlines()
    species = read_constants(Path("include/constants/species.h"), "SPECIES_")
    items = read_constants(Path("include/constants/item.h"), "ITEM_")
    moves = read_constants(Path("include/constants/moves.h"), "MOVE_")
    evolutions = read_evolutions(Path("data/Evolutions.c").read_text())

    lines_by_species = defaultdict(list)
    for key, entries in evolutions.items():
        if key not in species:
            continue
        for method, param, target in entries:
            if method in {"NONE", "DUMMY", "FORM_ARGUMENT", "LETS_GO"} or method.startswith("SPIN_"):
                continue
            if method in ("TRADE", "TRADE_SPECIFIC_MON") and any(
                other == "LEVEL" and other_target == target for other, _, other_target in entries
            ):
                continue
            if target.startswith("MON_WITH_FORM("):
                target = target.removeprefix("MON_WITH_FORM(").split(",", 1)[0]
            target_name = display_name(species_names, species, target)
            rule = condition(method, param, species_names, species, item_names, items, move_names, moves)
            line = f"{target_name}: {rule}"
            if line not in lines_by_species[species[key]]:
                lines_by_species[species[key]].append(line)

    text = []
    for index in range(len(species_names)):
        text.extend(make_pages(lines_by_species[index] or ["No known evolutions."]))
    output.write_text("".join(line + "\n" for line in text), encoding="utf-8")


if __name__ == "__main__":
    main()
