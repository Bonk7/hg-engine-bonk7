"""Add the team-services attendant to Pokémon Center ground floors."""

from pathlib import Path
import struct
import sys


SERVICE_SCRIPT = 2079
NURSE_SPRITE = 335
ATTENDANT_SPRITE = 341
OBJECT = struct.Struct("<14HI")

# Center ground floors share one lobby layout. The Pokémon League has
# its own layout and healing counter.
CENTER_LOBBIES = (
    66, 78, 153, 160, 163, 178, 212, 222, 232, 264, 315, 350,
    363, 383, 389, 420, 428, 435, 453, 460, 463, 466, 480, 486,
)
POSITIONS = {**dict.fromkeys(CENTER_LOBBIES, (10, 14)), 271: (8, 21)}


def add_attendant(path: Path, x: int, z: int) -> None:
    data = path.read_bytes()
    count_offset = 4 + 20 * struct.unpack_from("<I", data)[0]
    count = struct.unpack_from("<I", data, count_offset)[0]
    start = count_offset + 4
    end = start + count * OBJECT.size
    objects = list(OBJECT.iter_unpack(data[start:end]))
    if not any(obj[1] == NURSE_SPRITE for obj in objects):
        raise ValueError(f"{path}: missing the Pokémon Center nurse")
    attendants = [obj for obj in objects if obj[5] == SERVICE_SCRIPT]
    if attendants:
        if len(attendants) != 1 or attendants[0][12:14] != (x, z):
            raise ValueError(f"{path}: unexpected team-services attendant")
        return

    warp_count = struct.unpack_from("<I", data, end)[0]
    warps = [struct.unpack_from("<HH", data, end + 4 + j * 12) for j in range(warp_count)]
    if (x, z) in warps or any(
        abs(x - obj[12]) <= obj[10] and abs(z - obj[13]) <= obj[11] for obj in objects
    ):
        raise ValueError(f"{path}: attendant position ({x}, {z}) is occupied")

    local_id = max(obj[0] for obj in objects) + 1
    attendant = OBJECT.pack(local_id, ATTENDANT_SPRITE, 15, 0, 0, SERVICE_SCRIPT, 1,
                            0, 0, 0, 0, 0, x, z, 0)
    path.write_bytes(data[:count_offset] + struct.pack("<I", count + 1)
                     + data[start:end] + attendant + data[end:])


if __name__ == "__main__":
    directory = Path(sys.argv[1])
    for bank, (x, z) in POSITIONS.items():
        add_attendant(directory / f"2_{bank:03d}", x, z)
