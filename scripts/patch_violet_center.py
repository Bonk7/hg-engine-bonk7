"""Attach the new services to existing Violet Pokémon Center NPCs."""

from pathlib import Path
import struct
import sys


PATCH_MARKER = b"VIOLET_QOL\0\0"


def main():
    map_script, reminder, team_exp, training = map(Path, sys.argv[1:])
    data = bytearray(map_script.read_bytes())
    table_end = 6 * 4
    if data[table_end:table_end + 2] != b"\x13\xfd":
        raise ValueError("Unexpected Violet Pokémon Center script table")
    if data.endswith(PATCH_MARKER):
        original_size = struct.unpack_from("<I", data, len(data) - len(PATCH_MARKER) - 4)[0]
        if not table_end < original_size <= len(data) - len(PATCH_MARKER) - 4:
            raise ValueError("Invalid Violet Pokémon Center patch footer")
        del data[original_size:]
    else:
        original_size = len(data)

    for entry, script in ((1, training), (2, reminder), (3, team_exp)):
        entry_offset = entry * 4
        struct.pack_into("<I", data, entry_offset, len(data) - entry_offset - 4)
        data.extend(script.read_bytes())
        data.extend(b"\0" * (-len(data) % 4))
    data.extend(struct.pack("<I", original_size))
    data.extend(PATCH_MARKER)
    map_script.write_bytes(data)


if __name__ == "__main__":
    main()
