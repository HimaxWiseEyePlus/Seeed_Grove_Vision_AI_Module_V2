# WW500 firmware for this application

#### BLE processor firmware

This is the firmware that runs on the BLE processor - the MKL62BA.

The 'ww500_md' project works with code running on the  MKL62BA (BLE and LoRaWAN processor). 
This is created from the project 'WildlifeWatcher_1'. 
At the time of writing the source code is not in this repository.

This folder includes firmware for the BLE processor:

- WildlifeWatcher_1_ww500_a00_000701.zip. 
- WildlifeWatcher_1_ww500_a00_000800.zip. 

One of these files (usiually the latest) can be installed on the BLE processor
using the DFU mechanism which is documented in the [WW130_DFU.md](../../../../../_Documentation/WW130_DFU.md) file.

#### AI processor firmware

When this project is compiled following the instructions in the 
[Compile_and_flash.md](../../../../../_Documentation/Compile_and_flash.md) file,
a firmware image for the AI processor is created, called `output.img`.

However, not all of teh development team can easily perform this compilation process to create `output.img`.

Accordingly, I have copied the "latest" version of that file into this directory, for convenience. 
This does not happen automatically as part of the build process (so it might get out of date).

The `output.img` file can be loaded into the AI processor using the steps section 4 "Download to the board using bootloader" 
in the [WW130_DFU.md](../../../../../_Documentation/WW130_DFU.md) file.





