# WW500 firmware for this application

#### BLE processor firmware

This is the firmware that runs on the BLE processor - the MKL62BA.

The 'ww500_md' project works with code running on the  MKL62BA (BLE and LoRaWAN processor). 
This is created from the project 'WildlifeWatcher_1'. 
At the time of writing the source code is not in this repository.

This folder includes firmware for the BLE processor:

- WildlifeWatcher_1_ww500_a00_000800.zip. 

One of these files (usiually the latest) can be installed on the BLE processor
using the DFU mechanism which is documented in the [WW130_DFU.md](../../../../../_Documentation/WW130_DFU.md) file.

#### AI processor firmware

(NOTE: the output.img might not be here - I will ad it if required)

When this project is compiled following the instructions in the 
[Compile_and_flash.md](../../../../../_Documentation/Compile_and_flash.md) file,
a firmware image for the AI processor is created, called `output.img`.

However, not all of the development team can easily perform this compilation process to create `output.img`.

Accordingly, I have copied the "latest" version of that file into this directory, for convenience. 
This does not happen automatically as part of the build process (so it might get out of date).
I have had to zip it up, since the git ignore file ignores .img files. Unzip this to get the .img file.

The `output.img` file can be loaded into the AI processor using the steps section 4 "Download to the board using bootloader" 
in the [Compile_and_flash.md](../../../../../_Documentation/Compile_and_flash.md) file.




