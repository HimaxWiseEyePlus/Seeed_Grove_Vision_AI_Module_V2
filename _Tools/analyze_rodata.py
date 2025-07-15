#!/usr/bin/env python3
"""
Accurate .rodata section analyzer for GCC linker map files.
Handles indented lines, decimal/hex sizes, long paths, and aggregates totals.
"""

import sys
import re
from collections import defaultdict

def parse_rodata_sections(filename):
    symbols = defaultdict(int)
    total_rodata = 0

    with open(filename, 'r') as f:
        for line in f:
            line = line.rstrip()

            # Accept lines even if they start with spaces
            if not line.lstrip().startswith('.rodata'):
                continue

            # Extract parts (flexible spacing or tabs)
            parts = re.split(r'\s+', line.strip())
            if len(parts) < 4:
                continue

            size_str = parts[2]
            obj_path = parts[-1]

            try:
                size = int(size_str, 0)  # auto-detect hex or decimal
                symbols[obj_path] += size
                total_rodata += size
            except ValueError:
                continue

    return symbols, total_rodata

def shorten(path):
    # Try to preserve archive and object file
    match = re.search(r'([^/\\]+\.a)\(([^/\\]+\.o)\)', path)
    if match:
        lib, obj = match.groups()
        return f"…{lib}({obj})"
    else:
        return f"…{path[-35:]}" if len(path) > 40 else path

if len(sys.argv) != 2:
    print("Usage: python analyze_rodata_final.py <mapfile>")
    sys.exit(1)

symbol_sizes, total = parse_rodata_sections(sys.argv[1])
entries = [(size, shorten(name)) for name, size in symbol_sizes.items()]
entries.sort(reverse=True)

print(f"{'Object File':40} {'Size (bytes)':>12}")
print("-" * 55)
for size, name in entries:
    print(f"{name:40} {size:12}")
print("-" * 55)
print(f"{'TOTAL .rodata':40} {total:12}")