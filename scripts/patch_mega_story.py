"""Add Elm in Sprout Tower and require his research before Falkner."""

import struct
import sys
from pathlib import Path


def add_elm(path):
    events = path.read_bytes()
    count_offset = 4 + 20 * struct.unpack_from("<I", events)[0]
    count = struct.unpack_from("<I", events, count_offset)[0]
    start = count_offset + 4
    layout = struct.Struct("<14HI")
    end = start + count * layout.size
    objects = list(layout.iter_unpack(events[start:end]))
    if any(obj[5] == 2082 for obj in objects):
        return
    x, z = 14, 25
    if any(obj[12:14] == (x, z) for obj in objects):
        raise ValueError("Elm's position in Sprout Tower is occupied")
    elm = layout.pack(max(obj[0] for obj in objects) + 1, 99, 15, 0, 0, 2082,
                      1, 0, 0, 0, 0, 0, x, z, 0)
    path.write_bytes(events[:count_offset] + struct.pack("<I", count + 1)
                     + events[start:end] + elm + events[end:])


def require_research(path, wrapper_path):
    script = bytearray(path.read_bytes())
    wrapper = bytearray(wrapper_path.read_bytes())
    entry_offset = 4  # Falkner is the second script in the Violet Gym bank.
    original = struct.unpack_from("<I", script, entry_offset)[0] + entry_offset + 4
    if script[original:original + len(wrapper) - 4] == wrapper[:-4]:
        return
    if wrapper[-6:-4] != b"\x16\x00" or not 8 <= original < len(script):
        raise ValueError("Unexpected Violet Gym script layout")
    struct.pack_into("<i", wrapper, len(wrapper) - 4, original - len(script) - len(wrapper))
    struct.pack_into("<I", script, entry_offset, len(script) - entry_offset - 4)
    path.write_bytes(script + wrapper)


if __name__ == "__main__":
    operation, *arguments = sys.argv[1:]
    paths = list(map(Path, arguments))
    if operation == "elm":
        add_elm(*paths)
    elif operation == "gym":
        require_research(*paths)
    else:
        raise ValueError(operation)
