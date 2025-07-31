# batch_exif_dump.py
# ------------------
# Scans a folder for JPEG files and extracts EXIF metadata into a CSV file.
# Uses a lightweight custom TIFF parser to read selected tags.
# Adds filename, file size in KB, and file timestamp for spreadsheet analysis.

import os
import csv
import struct
import argparse
from datetime import datetime

EXIF_TAGS = {
    0x010F: "Make",
    0x0110: "Model",
    0x0132: "DateTime",
    0x8769: "ExifIFDPointer",
    0x8825: "GPSInfoIFDPointer",
    0x0001: "GPSLatitudeRef",
    0x0002: "GPSLatitude",
    0x0003: "GPSLongitudeRef",
    0x0004: "GPSLongitude",
    0x0005: "GPSAltitudeRef",
    0x0006: "GPSAltitude",
}

TYPE_SIZES = {
    1: 1, 2: 1, 3: 2, 4: 4,
    5: 8, 7: 1, 9: 4, 10: 8,
}

def format_value(value, type_id):
    if isinstance(value, bytes):
        if type_id == 2:  # ASCII
            try:
                return value.decode('ascii', errors='replace').strip('\x00')
            except Exception:
                return "<non-decodable>"
        elif type_id in (5, 10):  # RATIONAL
            pairs = []
            for i in range(0, len(value), 8):
                if i + 8 > len(value):
                    break
                num, denom = struct.unpack('<II' if type_id == 5 else '<ii', value[i:i+8])
                pairs.append(f"{num}/{denom}")
            return ", ".join(pairs)
        elif type_id in (1, 7):  # BYTE/UNDEFINED
            return ' '.join(f"{b:02X}" for b in value)
        else:
            return value.hex()
    return str(value)

def parse_ifd(fp, base_offset, ifd_offset, endian, tags, collected, check_next_ifd=True):
    try:
        fp.seek(base_offset + ifd_offset)
        raw = fp.read(2)
        if len(raw) < 2:
            return
        num_entries = struct.unpack(endian + 'H', raw)[0]
    except Exception:
        return

    for _ in range(num_entries):
        entry = fp.read(12)
        if len(entry) < 12:
            return
        tag, type_id, count, value_offset = struct.unpack(endian + 'HHII', entry)
        tag_name = EXIF_TAGS.get(tag)
        type_size = TYPE_SIZES.get(type_id, 1)
        total_size = type_size * count

        # Inline value
        if total_size <= 4:
            raw_bytes = struct.pack(endian + 'I', value_offset)
            value = raw_bytes[:total_size]
        else:
            cur = fp.tell()
            try:
                fp.seek(base_offset + value_offset)
                value = fp.read(total_size)
            except Exception:
                value = b''
            fp.seek(cur)

        if tag_name:
            collected[tag_name] = format_value(value, type_id)

        # Handle pointer tags
        if tag == 0x8825:  # GPSInfoIFDPointer
            parse_ifd(fp, base_offset, value_offset, endian, tags, collected, check_next_ifd=False)
        elif tag == 0x8769:  # ExifIFDPointer
            parse_ifd(fp, base_offset, value_offset, endian, tags, collected, check_next_ifd=False)

    if check_next_ifd:
        next_ifd = fp.read(4)
        if len(next_ifd) == 4:
            next_offset = struct.unpack(endian + 'I', next_ifd)[0]
            if next_offset != 0:
                parse_ifd(fp, base_offset, next_offset, endian, tags, collected, check_next_ifd=True)

def extract_exif_tags(filepath):
    collected = {}
    try:
        with open(filepath, 'rb') as fp:
            while True:
                byte = fp.read(1)
                if not byte or byte != b'\xFF':
                    continue
                marker = fp.read(1)
                if marker in [b'\xD8', b'\xD9']:  # SOI/EOI
                    continue
                length_bytes = fp.read(2)
                if len(length_bytes) < 2:
                    break
                length = struct.unpack('>H', length_bytes)[0]
                segment_start = fp.tell()
                segment_data = fp.read(length - 2)

                if marker == b'\xE1' and segment_data.startswith(b'Exif\x00\x00'):
                    endian_flag = segment_data[6:8]
                    endian = '<' if endian_flag == b'II' else '>'
                    if len(segment_data) < 14:
                        break
                    magic = struct.unpack(endian + 'H', segment_data[8:10])[0]
                    if magic != 0x2A:
                        break
                    first_ifd_offset = struct.unpack(endian + 'I', segment_data[10:14])[0]
                    tiff_offset = segment_start + 6
                    parse_ifd(fp, tiff_offset, first_ifd_offset, endian, EXIF_TAGS, collected)
                    break
    except Exception:
        pass
    return collected

def write_csv(output_csv, rows, fieldnames):
    with open(output_csv, 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)

def write_log(output_txt, log_entries):
    with open(output_txt, 'w', encoding='utf-8') as f:
        for entry in log_entries:
            f.write(entry + "\n")

def main():
    parser = argparse.ArgumentParser(description='Batch EXIF extractor')
    parser.add_argument('--input_folder', required=True, help='Folder containing JPEG files')
    parser.add_argument('--output_csv', required=True, help='Output CSV filename')
    parser.add_argument('--output_txt', required=False, help='Optional TXT log filename')
    args = parser.parse_args()

    image_rows = []
    log_entries = []

    for root, _, files in os.walk(args.input_folder):
        for name in files:
            if name.lower().endswith(('.jpg', '.jpeg')):
                path = os.path.join(root, name)
                tags = extract_exif_tags(path)

                try:
                    size_kb = os.path.getsize(path) // 1024
                    timestamp = datetime.fromtimestamp(os.path.getmtime(path))
                except Exception:
                    size_kb = 0
                    timestamp = ''

                row = {
                    "FileName": name,
                    "SizeKB": size_kb,
                    "FileTime": timestamp.strftime('%Y-%m-%d %H:%M') if timestamp else ''
                }

                for key in EXIF_TAGS.values():
                    row[key] = tags.get(key, '')

                image_rows.append(row)

                if args.output_txt:
                    log_entries.append(f"File: {name}")
                    for k, v in tags.items():
                        log_entries.append(f"  {k}: {v}")
                    log_entries.append("")

    # Determine column order
    fieldnames = ["FileName", "SizeKB", "FileTime"] + list(EXIF_TAGS.values())
    write_csv(args.output_csv, image_rows, fieldnames)

    if args.output_txt:
        write_log(args.output_txt, log_entries)

if __name__ == "__main__":
    main()