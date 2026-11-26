"""Generate the scripts, trainer parties and journal text for Mega Stone sources."""
import csv
import re
import sys
import textwrap
from pathlib import Path


def rows(path):
    with open(path, encoding="utf-8") as file:
        return [row for row in csv.reader(file, delimiter="|") if row and not row[0].startswith("#")]


def define_values(path, prefix):
    return {name: int(value) for name, value in re.findall(
        rf"(?m)^#define {prefix}_(\w+)\s+(\d+)\b", Path(path).read_text()
    )}


def wrap(text, heading=None):
    lines = textwrap.wrap(text, width=36, break_long_words=False, break_on_hyphens=False)
    if heading is not None:
        lines.insert(0, heading)
    return "".join(line + (r"\r" if index % 2 else r"\n")
                   for index, line in enumerate(lines))[:-2]


def main(output):
    quests = rows("data/mega_stone_quests.tsv")
    pickups = rows("data/mega_stone_pickups.tsv")
    stones = {item: species for species, item in re.findall(
        r"MEGA_STONE\(SPECIES_(\w+), ITEM_(\w+)", Path("include/mega_stones.inc").read_text()
    )}
    known = ["BEEDRILLITE", "HERACRONITE", "STEELIXITE", "PIDGEOTITE", "AMPHAROSITE"]
    sources = known + [row[0] for row in quests + pickups]
    if len(quests) != 66 or len(pickups) != 20 or len(set(sources)) != len(sources) or set(sources) != stones.keys():
        raise ValueError("Every Mega Stone must have exactly one reward source (66 new quests and 20 new pickups)")
    item_ids = define_values("include/constants/item.h", "ITEM")
    item_names = Path("data/text/222.txt").read_text().splitlines()
    occupied = {int(number) for number in re.findall(r"(?m)^    \[(\d+)\] =", Path("data/Trainers.c").read_text())}
    texts = []
    table, trainers, definitions, scripts, events = [], [], [], [], []
    objective = {"trade": 1, "show": 2, "quiz": 3, "battle": 4}
    trainer_id = 738

    for index, row in enumerate(quests):
        stone, map_id, area, title, kind, arg, briefing, *choices = row
        if len(title) > 25:
            raise ValueError(f"Quest title too long: {title}")
        start_flag = 2635 + 3 * index
        msg = len(texts)
        reward = item_names[item_ids[stone]].removesuffix(r"\r")
        answers = choices[0].split(";") if choices else []
        if kind == "quiz" and len(answers) != 3:
            raise ValueError(f"{title}: expected three answers")
        if kind == "quiz":
            rotation = index % 3
            answers = answers[-rotation:] + answers[:-rotation]
            arg = str(rotation)
        texts.extend((
            f"To be discovered · {area}",
            f"{{COLOR 3}}{title}{{COLOR 0}}",
            f"{{COLOR 2}}{title}{{COLOR 0}}",
            wrap(f"Ask the researcher at the {area} Pokémon Center."),
            wrap(briefing) + r"\rReward: " + reward,
            wrap(f"{title} completed.") + r"\nReward: " + reward,
            title,
            *(answers + ["-"] * (3 - len(answers))),
        ))
        if kind == "trade":
            arg = "ITEM_" + arg
        elif kind == "show":
            arg = "SPECIES_" + arg
        elif kind == "battle":
            if trainer_id in occupied:
                raise ValueError(f"Trainer slot {trainer_id} is already occupied")
            level, *partners = arg.split(",")
            members = [(sp, "NONE", int(level) - 2) for sp in partners] + [(stones[stone], stone, int(level))]
            party = "\n".join(f"            {{.ivs = 90, .level = {lv}, .species = SPECIES_{sp}, .item = ITEM_{item}}},"
                              for sp, item, lv in members)
            trainers.append(f"""    [{trainer_id}] = {{
        .name = "Researcher",
        .data = {{.trainerType = TRAINER_DATA_TYPE_ITEMS, .trainerClass = TRAINERCLASS_ACE_TRAINER_M,
                 .aiFlags = F_PRIORITIZE_SUPER_EFFECTIVE | F_EVALUATE_ATTACKS, .battleType = SINGLE_BATTLE}},
        .party = {{
{party}
        }},
    }},""")
            arg = str(trainer_id)
            trainer_id += 1
        table.append(
            f'    {{{start_flag}, {start_flag + 1}, {{{msg}, {msg+1}, {msg+2}}}, '
            f'{{{msg+3}, {msg+4}, {msg+5}}}, '
            f'{{"{title.lower()}", "{area.lower()}", "{reward.lower()}"}}, '
            f'{map_id}, ITEM_{stone}, {objective[kind]}, {arg}, {msg+6}}},'
        )
    for index, (stone, area, bank, x, z) in enumerate(pickups):
        event_script = 2089 + index
        flag = 2833 + index
        local_id = 31 + index
        definitions.append(f"scrdef MegaStonePickup{index}")
        scripts.append(f"mega_pickup MegaStonePickup{index}, {item_ids[stone]}, {flag}, {local_id}")
        events.append(f"{bank},{x},{z},{event_script},{flag},{local_id}")
    output.mkdir(parents=True, exist_ok=True)
    for name, lines in {
        "mega_research_table.inc": table,
        "mega_research_trainers.inc": trainers,
        "mega_research_definitions.inc": definitions,
        "mega_research_pickups.inc": scripts,
        "mega_research_events.csv": events,
        "rawtext/860.txt": texts,
    }.items():
        path = output / name
        path.parent.mkdir(parents=True, exist_ok=True)
        content = "\n".join(lines) + "\n"
        if name.endswith(".txt"):
            content = content.replace("'", "’").replace("\n", "\r\n")
        path.write_bytes(content.encode("utf-8"))


if __name__ == "__main__":
    main(Path(sys.argv[1]))
