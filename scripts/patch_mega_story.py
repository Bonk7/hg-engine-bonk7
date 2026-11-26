"""Add Elm in Sprout Tower and require his research before Falkner."""

import csv
import struct
import sys
from pathlib import Path


def add_quest_object(path, x, z, sprite, script, flag=0, local_id=None):
    events = path.read_bytes()
    count_offset = 4 + 20 * struct.unpack_from("<I", events)[0]
    count = struct.unpack_from("<I", events, count_offset)[0]
    start = count_offset + 4
    layout = struct.Struct("<14HI")
    end = start + count * layout.size
    objects = list(layout.iter_unpack(events[start:end]))
    if any(obj[5] == script for obj in objects):
        return
    if any(obj[12:14] == (x, z) for obj in objects):
        raise ValueError(f"{path}: quest NPC position ({x}, {z}) is occupied")
    if local_id is None:
        local_id = max(obj[0] for obj in objects) + 1
    if any(obj[0] == local_id for obj in objects):
        raise ValueError(f"{path}: quest object ID {local_id} is occupied")
    new_object = layout.pack(local_id, sprite, 15, 0, flag, script,
                      1, 0, 0, 0, 0, 0, x, z, 0)
    path.write_bytes(events[:count_offset] + struct.pack("<I", count + 1)
                     + events[start:end] + new_object + events[end:])


def remove_generated_pickups(path, script_to_flag):
    events = path.read_bytes()
    count_offset = 4 + 20 * struct.unpack_from("<I", events)[0]
    count = struct.unpack_from("<I", events, count_offset)[0]
    start = count_offset + 4
    layout = struct.Struct("<14HI")
    end = start + count * layout.size
    objects = list(layout.iter_unpack(events[start:end]))
    kept = [obj for obj in objects if not (obj[1] == 87 and script_to_flag.get(obj[5]) == obj[4])]
    if len(kept) != count:
        path.write_bytes(events[:count_offset] + struct.pack("<I", len(kept))
                         + b"".join(layout.pack(*obj) for obj in kept) + events[end:])


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
        add_quest_object(*paths, 14, 25, 99, 2082)
    elif operation == "grove":
        add_quest_object(*paths, 12, 14, 325, 2085)
    elif operation == "early-stones":
        union_cave, tower, route_center, route_32 = paths
        add_quest_object(union_cave, 27, 31, 87, 2086, 2632, 30)
        add_quest_object(tower, 18, 8, 87, 2087, 2633, 30)
        remove_generated_pickups(route_center, {2088: 2634})
        add_quest_object(route_32, 464, 421, 87, 2088, 2634, 30)
    elif operation == "research":
        directory, placements = paths
        with placements.open() as file:
            rows = [tuple(map(int, row)) for row in csv.reader(file)]
        script_to_flag = {script: flag for _, _, _, script, flag, _ in rows}
        for path in directory.glob("2_*"):
            remove_generated_pickups(path, script_to_flag)
        for bank, x, z, script, flag, local in rows:
            add_quest_object(directory / f"2_{bank:03d}", x, z, 87, script, flag, local)
    elif operation == "gym":
        require_research(*paths)
    else:
        raise ValueError(operation)
