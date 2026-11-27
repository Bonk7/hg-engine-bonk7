"""Append custom local-player portraits to the Trainer Card archive."""
from pathlib import Path
import sys

import ndspy.narc


archive = Path(sys.argv[1])
portraits = Path(sys.argv[2])
narc = ndspy.narc.NARC.fromFile(str(archive))
first_portrait = 78
portrait_count = 12
portrait_size = 32 + 10 * 11 * 64

if len(narc.files) not in (first_portrait, first_portrait + portrait_count):
    raise ValueError("Unexpected Trainer Card archive size")

assets = [(portraits / f"{i:02d}.bin").read_bytes() for i in range(1, portrait_count + 1)]
if any(len(asset) != portrait_size for asset in assets):
    raise ValueError("A Trainer Card portrait has the wrong dimensions")
narc.files[first_portrait:] = assets
narc.saveToFile(str(archive))
