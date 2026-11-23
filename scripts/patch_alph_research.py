"""Route the Ruins of Alph researcher through the expedition dialog postgame."""

from pathlib import Path
import struct
import sys


map_path, wrapper_path = map(Path, sys.argv[1:])
data = bytearray(map_path.read_bytes())
wrapper = bytearray(wrapper_path.read_bytes())
if data[24:26] != b"\x13\xfd" or wrapper[-6:-4] != b"\x16\x00":
    raise ValueError("Unexpected Alph researcher script layout")

# The final relative jump returns to the original researcher entry.
old_entry = struct.unpack_from("<I", data)[0] + 4
if data[old_entry:old_entry + len(wrapper) - 4] == wrapper[:-4]:
    sys.exit(0)
struct.pack_into("<i", wrapper, len(wrapper) - 4, old_entry - len(data) - len(wrapper))
struct.pack_into("<I", data, 0, len(data) - 4)
data.extend(wrapper)
map_path.write_bytes(data)
