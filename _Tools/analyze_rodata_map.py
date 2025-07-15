import subprocess
import re
import sys
from pathlib import Path

# -------------------- CONFIG --------------------
# Path to your toolchain's c++filt
CXXFILT = r"C:\ST\STM32CubeIDE_1.10.1\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.0.202111181127\tools\bin\arm-none-eabi-c++filt.exe"
MAX_PATH_COMPONENTS = 2  # how many path components to keep in output
# ------------------------------------------------

# Check args
if len(sys.argv) != 3:
    print("Usage: python analyze_rodata_map.py <input.map> <output.txt>")
    sys.exit(1)

map_file = sys.argv[1]
output_file = sys.argv[2]

# Regex for symbols and their details
symbol_line_re = re.compile(r'\s+\.rodata\.(\S+)$')
detail_line_re = re.compile(r'\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(\S+)')
inline_re = re.compile(r'\s+\.rodata\.(\S+)\s+(0x[0-9a-fA-F]+)\s+(0x[0-9a-fA-F]+)\s+(\S+)')

entries = []
pending_symbol = None

def shorten_path(path, max_parts=2):
    parts = Path(path).parts
    return "/".join(parts[-max_parts:]) if len(parts) > max_parts else path

with open(map_file, "r", encoding='utf-8') as f:
    for line in f:
        # Multiline case (first line)
        match_symbol = symbol_line_re.match(line)
        if match_symbol:
            pending_symbol = match_symbol.group(1)
            continue

        # Multiline case (second line)
        match_detail = detail_line_re.match(line)
        if match_detail and pending_symbol:
            addr, size, obj = match_detail.groups()
            try:
                demangled = subprocess.check_output([CXXFILT, pending_symbol], text=True).strip()
            except Exception:
                demangled = pending_symbol
            entries.append((int(size, 16), demangled, addr, shorten_path(obj, MAX_PATH_COMPONENTS)))
            pending_symbol = None
            continue

        # Single-line entry
        inline_match = inline_re.match(line)
        if inline_match:
            sym, addr, size, obj = inline_match.groups()
            try:
                demangled = subprocess.check_output([CXXFILT, sym], text=True).strip()
            except Exception:
                demangled = sym
            entries.append((int(size, 16), demangled, addr, shorten_path(obj, MAX_PATH_COMPONENTS)))
            pending_symbol = None

# Sort by size descending
entries.sort(reverse=True)

# Write to output file
with open(output_file, "w", encoding='utf-8') as out:
    out.write(f"{'Size (bytes)':>12} | {'Address':<10} | {'Symbol':<60} | Object File\n")
    out.write("-" * 100 + "\n")
    for size, name, addr, obj in entries:
        out.write(f"{size:12} | {addr:<10} | {name:<60} | {obj}\n")

print(f"Analysis complete. Output written to {output_file}")
