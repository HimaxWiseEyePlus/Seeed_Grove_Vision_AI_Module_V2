"""
Python script to parse a text file containing entries of type HX_CIS_SensorSetting_t, like this:
		{HX_CIS_I2C_Action_W, 0x0103, 0x00},
		{HX_CIS_I2C_Action_S, 0x0001, 0x00},

The output is a file containing binary data.
Thanks to ChatGPT!

This script reads an input text file containing I2C action commands, parses each line, 
and writes the corresponding binary data to an output file.

### How It Works:
1. **Parsing the input file**: Reads each line and extracts three elements using a regex.
2. **Mapping the action type**: Converts the action string into its corresponding byte value using a dictionary.
3. **Converting to binary format**: The 16-bit register is stored in little-endian format, and all values are packed into a binary structure.
4. **Writing to a binary file**: The parsed data is written to an output `.bin` file.

### Usage:
- Run the script from the command line with specified input and output file names:
  ```sh
  python script.py input.txt output.bin
  ```

"""

import struct
import re
import sys

i2c_action_map = {
    "HX_CIS_I2C_Action_W": 0,
    "HX_CIS_I2C_Action_R": 1,
    "HX_CIS_I2C_Action_S": 2,
    "HX_CIS_I2C_Action_W_1Byte_Reg": 3,
    "HX_CIS_I2C_Action_R_1Byte_Reg": 4,
}

def parse_line(line):
    pattern = re.compile(r"\{(.*?),\s*(0x[0-9A-Fa-f]+),\s*(0x[0-9A-Fa-f]+)\}")
    match = pattern.match(line.strip())
    
    if match:
        action_str, reg_hex, val_hex = match.groups()
        action = i2c_action_map.get(action_str.strip())
        reg = int(reg_hex, 16)
        val = int(val_hex, 16)
        
        if action is None:
            raise ValueError(f"Unknown action type: {action_str}")
        
        return struct.pack("<BHB", action, reg, val)
    
    return None

def convert_file(input_filename, output_filename):
    try:
        with open(input_filename, "r") as infile, open(output_filename, "wb") as outfile:
            for line in infile:
                binary_data = parse_line(line)
                if binary_data:
                    outfile.write(binary_data)
        print(f"Binary data written to {output_filename}")
    except FileNotFoundError:
        print(f"Error: File '{input_filename}' not found!")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_file> <output_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    convert_file(input_file, output_file)
