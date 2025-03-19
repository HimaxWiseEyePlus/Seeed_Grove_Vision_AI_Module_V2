Python NRF Tools
================

#### CGP 9/2/25

(Work in progress - I don't understand this yet. And I have not tried to do DFU update using these tools.)

__WARNING: I think that when I installed these tools the version of pyyaml was changed to an earlier version. This broke 
something else (the Himax we2_local_image_gen tool?) which would not work with the earlier version.__
 
It is possible that nrfTools can perform DFU operations from a laptop, using python.
See [here](https://github.com/NordicSemiconductor/pc-nrfutil/blob/master/nordicsemi/dfu/dfu_transport_ble.py)

I had a .exe version of `nrfutils` installed. I then tried this:
```
pip install nrfutil
```
Which installed something. Looks like code was loaded here (your location will vary!):
C:\Users\charl\AppData\Roaming\Python\Python312\site-packages\nordicsemi\dfu

Then running this clearly does something:
``
nrfutil dfu ble
Usage: pc_nrfutil_legacy_v6.1.7.exe dfu ble [OPTIONS]
Try 'pc_nrfutil_legacy_v6.1.7.exe dfu ble --help' for help.

Error: Missing option '-pkg' / '--package'.
```
As does this:
```
nrfutil dfu ble --help
```
Copilot suggested doing this:
```py
import subprocess
import os
def main():
    # Path to your firmware file
    firmware_file = "path/to/firmware.hex"  # Replace with the path to your firmware file
    # Path to nrfutil
    nrfutil_path = "path/to/nrfutil"  # Replace with the path to your nrfutil executable
    # DFU package file path
    dfu_package = "path/to/dfu_package.zip"  # Replace with the path to your DFU package file
    # Generate DFU package
    subprocess.run([nrfutil_path, "--verbose", "pkg", "generate", dfu_package, firmware_file])
    # Flash the firmware
    subprocess.run([nrfutil_path, "--verbose", "dfu", "flash", dfu_package])
    print("DFU update completed successfully.")
if __name__ == "__main__":
    main()
```