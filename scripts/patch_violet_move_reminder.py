"""Attach the free Move Reminder to the third Violet Pokémon Center script."""

from pathlib import Path
import struct
import sys


def main():
    map_script, reminder = map(Path, sys.argv[1:])
    data = bytearray(map_script.read_bytes())
    entry_offset = 2 * 4
    table_end = 6 * 4
    if data[table_end:table_end + 2] != b"\x13\xfd":
        raise ValueError("Unexpected Violet Pokémon Center script table")
    struct.pack_into("<I", data, entry_offset, len(data) - entry_offset - 4)
    data.extend(reminder.read_bytes())
    data.extend(b"\0" * (-len(data) % 4))
    map_script.write_bytes(data)


if __name__ == "__main__":
    main()
