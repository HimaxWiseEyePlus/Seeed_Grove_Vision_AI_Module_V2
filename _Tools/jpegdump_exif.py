# -*- coding: utf-8 -*-
# jpegdump_exif.py
# ----------------
# Standalone EXIF segment parser for JPEG files.
# Scans APP1 markers, locates embedded TIFF structures,
# and recursively dumps all IFD entries, including nested
# EXIF and GPS IFDs.
#
# Supports safe decoding of ASCII, BYTE, and RATIONAL types,
# with inline value handling and offset-based value dereferencing.
# Output is fully ASCII-safe for compatibility in constrained or
# embedded environments.

import struct

EXIF_TAGS = {
    0x010F: "Make",
    0x0110: "Model",
    0x011A: "X Resolution",
    0x011B: "Y Resolution",
    0x0128: "Resolution Unit",
    0x9003: "Datetime Original",
    0x9004: "Datetime Create",
    0x0132: "DateTime",
    0xC000: "Custom Data",
    0x8769: "ExifIFDPointer",
    0x8825: "GPSInfoIFDPointer",
    0x0001: "GPS Latitude Reference",
    0x0002: "GPS Latitude",
    0x0003: "GPS Longitude Reference",
    0x0004: "GPS Longitude",
    0x0005: "GPS Altitude Reference",
    0x0006: "GPS Altitude"
}

TYPE_SIZES = {
    1: 1,   # BYTE
    2: 1,   # ASCII
    3: 2,   # SHORT
    4: 4,   # LONG
    5: 8,   # RATIONAL
    7: 1,   # UNDEFINED
    9: 4,   # SLONG
    10: 8,  # SRATIONAL
}

def format_value(value, type_id):
    if isinstance(value, bytes):
        if type_id == 2:  # ASCII
            try:
                return value.decode('ascii', errors='replace').strip('\x00')
            except Exception:
                return "<non-decodable ASCII>"
        elif type_id in (5, 10):  # RATIONAL or SRATIONAL
            pairs = []
            for i in range(0, len(value), 8):
                if i + 8 > len(value):
                    break
                num, denom = struct.unpack('<II' if type_id == 5 else '<ii', value[i:i+8])
                pairs.append(f"{num}/{denom}")
            return ", ".join(pairs)
        elif type_id in (1, 7):  # BYTE or UNDEFINED
            return ' '.join(f"{b:02X}" for b in value)
        else:
            return "0x" + value.hex()[:32] + "..."
    return str(value)
    
def parse_ifd(fp, base_offset, ifd_offset, endian, check_next_ifd=True):
    try:
        fp.seek(base_offset + ifd_offset)
        raw = fp.read(2)
        if len(raw) < 2:
            print("ERROR: Failed to read number of IFD entries.")
            return
        num_entries = struct.unpack(endian + 'H', raw)[0]
        print(f"\nIFD at 0x{base_offset + ifd_offset:08X} contains {num_entries} entries:")
    except Exception as e:
        print(f"ERROR: Failed to parse IFD header: {e}")
        return

    for _ in range(num_entries):
        entry_offset = fp.tell()
        entry = fp.read(12)
        if len(entry) < 12:
            print("ERROR: Incomplete IFD entry.")
            return

        tag, type_id, count, value_offset = struct.unpack(endian + 'HHII', entry)
        type_size = TYPE_SIZES.get(type_id, 1)
        total_size = type_size * count

        tag_name = EXIF_TAGS.get(tag, f"Unknown_0x{tag:04X}")

        # Handle inline values
        if total_size <= 4:
            raw_bytes = struct.pack(endian + 'I', value_offset)
            value = raw_bytes[:total_size]
        else:
            # Value is a pointer
            current_pos = fp.tell()
            try:
                fp.seek(base_offset + value_offset)
                value = fp.read(total_size)
            except Exception as e:
                print(f"ERROR: Failed to read value at offset 0x{value_offset:X}: {e}")
                value = b''
            fp.seek(current_pos)

        try:
            safe_tag_name = tag_name.encode('ascii', errors='replace').decode('ascii')
        except Exception:
            safe_tag_name = "<undecodable>"

        print(f"  Tag: {safe_tag_name:<22} (0x{tag:04X})")
        print(f"    Type: {type_id}  Count: {count}  Offset: 0x{entry_offset:08X}")
        print(f"    Value: {format_value(value, type_id)}")

        # Auto-follow pointer tags
        if tag == 0x8825:  # GPSInfoIFDPointer
            print(f"    --> Following GPSInfoIFDPointer to 0x{value_offset:08X}")
            parse_ifd(fp, base_offset, value_offset, endian, check_next_ifd=False)
        elif tag == 0x8769:  # ExifIFDPointer
            print(f"    --> Following ExifIFDPointer to 0x{value_offset:08X}")
            parse_ifd(fp, base_offset, value_offset, endian, check_next_ifd=False)

    # Parse next IFD in chain (if any)
    if check_next_ifd:
        next_ifd = fp.read(4)
        if len(next_ifd) == 4:
            next_ifd_offset = struct.unpack(endian + 'I', next_ifd)[0]
            if next_ifd_offset != 0:
                parse_ifd(fp, base_offset, next_ifd_offset, endian)

def parse_exif_segment(fp, segment_data, segment_offset):
    if not segment_data.startswith(b'Exif\x00\x00'):
        print("Warning: Segment does not start with EXIF header.")
        return

    endian_flag = segment_data[6:8]
    if endian_flag == b'II':
        endian = '<'
        endian_label = 'Little Endian'
    elif endian_flag == b'MM':
        endian = '>'
        endian_label = 'Big Endian'
    else:
        print("Invalid endian flag in TIFF header.")
        return

    if len(segment_data) < 14:
        print("Invalid or truncated EXIF segment.")
        return

    tiff_header_offset = segment_offset + 6  # TIFF header is after 'Exif\0\0'
    magic = struct.unpack(endian + 'H', segment_data[8:10])[0]
    if magic != 0x2A:
        print("Invalid TIFF magic number (expected 0x002A).")
        return

    first_ifd_offset = struct.unpack(endian + 'I', segment_data[10:14])[0]
    absolute_ifd_address = tiff_header_offset + first_ifd_offset

    # Preview first two bytes of IFD
    fp.seek(absolute_ifd_address)
    raw = fp.read(2)
    if len(raw) < 2:
        print(f"Unable to read IFD entry count at 0x{absolute_ifd_address:08X}")
        return
    num_entries = struct.unpack(endian + 'H', raw)[0]

    # Print diagnostic info
    print("\n--- EXIF Segment Diagnostic Info ---")
    print(f"  APP1 Segment Offset      : 0x{segment_offset:08X}")
    print(f"  EXIF Header Start        : 0x{segment_offset:08X} (+0 bytes)")
    print(f"  TIFF Header Start        : 0x{tiff_header_offset:08X} (+6 bytes)")
    print(f"  Endian Format            : {endian_label}")
    print(f"  TIFF Magic Number        : 0x{magic:04X} (should be 0x002A)")
    print(f"  First IFD Offset         : 0x{first_ifd_offset:08X}")
    print(f"  Absolute First IFD Addr  : 0x{absolute_ifd_address:08X}")
    print(f"  IFD Entry Count          : {num_entries}")
    print("--- End of EXIF Diagnostic ---\n")

    parse_ifd(fp, tiff_header_offset, first_ifd_offset, endian)

def dump_jpeg_with_exif(path):
    with open(path, 'rb') as fp:
        while True:
            marker_start = fp.read(1)
            if not marker_start:
                break
            if marker_start != b'\xFF':
                continue
            marker = fp.read(1)
            if marker in [b'\xD8', b'\xD9']:
                continue
            length_bytes = fp.read(2)
            if len(length_bytes) < 2:
                break
            length = struct.unpack('>H', length_bytes)[0]
            segment_offset = fp.tell()
            segment_data = fp.read(length - 2)
            if marker == b'\xE1':
                print(f"\nFound APP1 (EXIF) segment at 0x{segment_offset - 4:08X}, length: {length} bytes")
                parse_exif_segment(fp, segment_data, segment_offset)
                break

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print("Usage: python jpegdump_exif_ascii.py <image.jpg>")
    else:
        dump_jpeg_with_exif(sys.argv[1])