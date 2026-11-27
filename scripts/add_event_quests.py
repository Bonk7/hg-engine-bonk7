"""Place the regional event researchers and gift NPCs."""
from pathlib import Path
import sys

from patch_mega_story import add_quest_object


if __name__ == "__main__":
    directory = Path(sys.argv[1])
    for bank, x, z, sprite, script in (
        (424, 9, 14, 99, 2182),   # Pewter Museum
        (158, 3, 4, 325, 2183),  # Kurt's house
        (129, 8, 4, 341, 2184),  # Mahogany east house
        (93, 50, 86, 344, 2187),  # National Park
        (292, 5, 7, 99, 2185),   # Ruins of Alph research center
    ):
        add_quest_object(directory / f"2_{bank:03d}", x, z, sprite, script)
