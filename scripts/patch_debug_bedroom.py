"""Schedule the debug opt-in on the first frame of a new save's bedroom."""

from pathlib import Path
import struct
import sys


def patch(header: Path, bedroom: Path) -> None:
    # Local script IDs start at 1. A frame condition compares the save
    # variable holding the new-game marker against the literal 1.
    frame_table = struct.pack("<BI B HHHH", 1, 1, 0, 0x416F, 1, 3, 0)
    original = bedroom.read_bytes()
    if header.read_bytes().rstrip(b"\0") == frame_table.rstrip(b"\0"):
        if original[12:14] != b"\x13\xfd":
            raise ValueError("Unexpected patched bedroom script table")
        return
    if any(header.read_bytes()) or original[8:10] != b"\x13\xfd":
        raise ValueError("Unexpected bedroom script table")
    # Keep both original scripts. Inserting one slot shifts their relative
    # offsets by four bytes, but leaves their internal branches intact.
    first, second = struct.unpack_from("<II", original)
    setup_script = struct.pack("<HHH", 20, 2076, 2)  # CallStd; End
    data = struct.pack("<III", first + 4, second + 4, len(original) - 8) + original[8:] + setup_script
    bedroom.write_bytes(data + bytes(-len(data) % 4))
    header.write_bytes(frame_table + bytes(-len(frame_table) % 4))


if __name__ == "__main__":
    patch(*map(Path, sys.argv[1:]))
