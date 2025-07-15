"""
compare_rodata_maps.py

Compares `.rodata` entries between two GNU linker `.map` files.
Adds memory region classification, computes deltas, and outputs both .txt and .csv.

Usage:
    python compare_rodata_maps.py <map1> <map2> <output.txt> <output.csv>
"""

import subprocess
import re
import sys
from pathlib import Path
from collections import defaultdict

# ---------------- CONFIG ----------------
CXXFILT = r"C:\Path\To\Toolchain\bin\arm-none-eabi-c++filt.exe"
MAX_PATH_COMPONENTS = 2
# Manually defined from linker file:
MEMORY_REGIONS = {
    "CM55M_S_APP_DATA": (0x30000000, 0x30040000),
    "CM55M_S_SRAM":     (0x3401F000, 0x34040000),
    "CM55M_S_APP_ROM":  (0x10000000, 0x10040000),
}
# ----------------------------------------

if len(sys.argv) != 5:
    print("Usage: python compare_rodata_maps.py <map1> <map2> <output.txt> <output.csv>")
    sys.exit(1)

map1_file = sys.argv[1]
map2_file = sys.argv[2]
text_output_file = sys.argv[3]
csv_output_file = sys.argv[4]

symbol_line_re = re.compile(r'\s+\.rodata\.(\S+)$')
detail_line_re = re.compile(r'\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(.+)')
inline_re = re.compile(r'\s+\.rodata\.(\S+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(.+)')


def is_stdlib(obj_path):
    """Return True if the object path appears to come from a standard library."""
    return (
        "Program Files" in obj_path or
        "gcc" in obj_path.lower() and
        ("libc.a" in obj_path or ".a(" in obj_path)
    )


def shorten_path(path, max_parts=2):
    """Truncate long object file paths, or return <stdlib> if path is a library file."""
    if is_stdlib(path):
        return "<stdlib>"
    parts = Path(path).parts
    return "/".join(parts[-max_parts:]) if len(parts) > max_parts else path


def demangle(symbol):
    """Demangle a C++ symbol using c++filt."""
    try:
        return subprocess.check_output([CXXFILT, symbol], text=True).strip()
    except Exception:
        return symbol


def get_region_for_address(addr):
    """Return memory region name for given symbol address, or 'Unknown' if unmatched."""
    for region, (start, end) in MEMORY_REGIONS.items():
        if start <= addr < end:
            return region
    return "Unknown"


def parse_map_file(filepath):
    """
    Parse a .map file and extract .rodata symbol sizes.

    Returns:
        dict of {(symbol, object): (size, address)}
    """
    entries = {}
    pending_symbol = None

    with open(filepath, "r", encoding="utf-8") as f:
        for line in f:
            match_symbol = symbol_line_re.match(line)
            if match_symbol:
                pending_symbol = match_symbol.group(1)
                continue

            match_detail = detail_line_re.match(line)
            if match_detail and pending_symbol:
                addr_str, size_str, obj = match_detail.groups()
                addr = int(addr_str, 16)
                size = int(size_str, 16)
                key = (pending_symbol, shorten_path(obj, MAX_PATH_COMPONENTS))
                entries[key] = (size, addr)
                pending_symbol = None
                continue

            inline_match = inline_re.match(line)
            if inline_match:
                sym, addr_str, size_str, obj = inline_match.groups()
                addr = int(addr_str, 16)
                size = int(size_str, 16)
                key = (sym, shorten_path(obj, MAX_PATH_COMPONENTS))
                entries[key] = (size, addr)
                pending_symbol = None

    return entries


# Main comparison logic
map1 = parse_map_file(map1_file)
map2 = parse_map_file(map2_file)
all_keys = set(map1.keys()).union(set(map2.keys()))

diff_entries = []
region_totals = defaultdict(lambda: [0, 0])  # region -> [size1, size2]

for key in all_keys:
    sym, obj = key
    size1, addr1 = map1.get(key, (0, 0))
    size2, addr2 = map2.get(key, (0, 0))
    delta = size2 - size1
    addr = addr2 if size2 else addr1
    region = get_region_for_address(addr)
    region_totals[region][0] += size1
    region_totals[region][1] += size2
    demangled = demangle(sym)
    diff_entries.append((region, size1, size2, delta, demangled, obj))

# Sort by region name, then by absolute delta
diff_entries.sort(key=lambda x: (x[0], -abs(x[3])))

# Write plain text output
with open(text_output_file, "w", encoding='utf-8') as out:
    out.write(f"# Map file 1: {map1_file}\n")
    out.write(f"# Map file 2: {map2_file}\n\n")

    for region in sorted(region_totals.keys()):
        s1, s2 = region_totals[region]
        out.write(f"# Region {region}: {s1} bytes (map1), {s2} bytes (map2), delta = {s2 - s1} bytes\n")
    out.write("\n")

    out.write(f"{'Region':<18} | {'Size1':>8} | {'Size2':>8} | {'Delta':>7} | {'Symbol':<60} | Object File\n")
    out.write("-" * 130 + "\n")
    for region, s1, s2, delta, name, obj in diff_entries:
        out.write(f"{region:<18} | {s1:8} | {s2:8} | {delta:7} | {name:<60} | {obj}\n")

# Write CSV output
with open(csv_output_file, "w", encoding='utf-8') as csv_out:
    csv_out.write("Region,Size1,Size2,Delta,Symbol,ObjectFile\n")
    for region, s1, s2, delta, name, obj in diff_entries:
        csv_out.write(f"{region},{s1},{s2},{delta},\"{name}\",\"{obj}\"\n")

print(f"Comparison complete.")
print(f"Text output written to: {text_output_file}")
print(f"CSV output written to:  {csv_output_file}")
