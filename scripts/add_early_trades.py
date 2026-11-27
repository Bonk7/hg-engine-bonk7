"""Append four early Johto trades and place their NPCs."""
from pathlib import Path
import struct
import sys

from ndspy.narc import NARC

from build_mega_research import define_values
from patch_mega_story import add_quest_object


TRADES = (
    # Event bank, x, z, sprite, requested species, offered species.
    (67, 3, 4, 341, "SENTRET", "SURSKIT"),
    (155, 8, 5, 325, "KRICKETOT", "NUMEL"),
    (158, 9, 4, 344, "SANDSHREW", "SNUBBULL"),
    (197, 8, 4, 341, "DROWZEE", "SWINUB"),
)


def update_archive(path):
    species = define_values("include/constants/species.h", "SPECIES")
    archive = NARC.fromFile(path)
    if len(archive.files) not in (13, 13 + len(TRADES)) or any(
        len(record) != 0x54 for record in archive.files
    ):
        raise ValueError("Unexpected NPC trade archive layout")

    records = []
    for index, (_, _, _, _, requested, offered) in enumerate(TRADES):
        records.append(struct.pack(
            "<21I", species[offered], 16, 14, 15, 13, 15, 14, 0,
            51000 + index, 0, 0, 0, 0, 0, 120000 + 25 * index,
            0, 0, 0, 2, species[requested], 0,
        ))
    archive.files[13:] = records
    archive.saveToFile(path)


if __name__ == "__main__":
    mode, path = sys.argv[1:]
    if mode == "archive":
        update_archive(path)
    elif mode == "events":
        for index, (bank, x, z, sprite, _, _) in enumerate(TRADES):
            add_quest_object(Path(path) / f"2_{bank:03d}", x, z, sprite, 2178 + index)
    else:
        raise ValueError(mode)
