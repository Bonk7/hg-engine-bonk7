"""Place the three Operation Aftershock characters in Kanto."""
from pathlib import Path
import sys

from patch_mega_story import add_quest_object


def place_rocket_story(directory: Path) -> None:
    # Silph lobby, Lavender Radio Station, Seafoam Islands B3F.
    add_quest_object(directory / "2_359", 9, 12, 34, 2175)
    add_quest_object(directory / "2_394", 7, 7, 222, 2176)
    # Match the hide flag and local object ID used by the finale script.
    add_quest_object(directory / "2_410", 7, 5, 385, 2177, 2855, 55)


if __name__ == "__main__":
    place_rocket_story(Path(sys.argv[1]))
