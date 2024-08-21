## How to install firmware images onto the WW130 using DFU - CGP 10/8/24

The WW130 boards have a "device firmware update" mechanism which allows new frmware to be installed
over-the-air using BLE. This document describes how to do this.

The steps are:
- Copy the new firmware to a phone.
- Run the DFU app on the phone and select the new firmware file.
- Place the WW130 in DFU mode (waiting for a new image).
- On the app, select the WW130 device and make a BLE connection to it.
- Send the new firmware and check the progress of the download.

1 Select the right file.
------------------------
DFU files are .zip files. They have names such as this: 'twi_master_seeed_ww130_b00_000402.zip' 
The file name is understood as follows:
- twi_master_seeed - this is the name of the WW130 project in the WW130's SDK.
- ww130_b00 - this identifies the board type (ww130) and revision (b00).
- 000402 - firmware version - here 0.4.2

The DFU process checks the firmware version and does not permit an earlier version to be loaded.
Copy the .zip file to your phone - I place these in the Downloads folder of my Android phone.

2 Run the DFU app
-----------------
This is part of the nRFToolbox suite from Nordic Semiconductor (manufacturer of the BLE processor).
Open this app and scroll down to the Device Firmware Update option.
Select the correct file. (They are arranged in chronological order, so the top file is the latest download).

3	Place the WW130 in DFU mode.
---------------------------------
There are potentially two ways to do this. The first should always work, and the second depends on whether
I have included code for it in the app that is currently running on the WW130.
- Press SW2 (on the small board labelled WWIF100) and apply power.
- Press SW2 for > 1s while the current app is running. 

In DFU mode the red LED (LED1) and the blue LED (LED2) come on together. The board stays in DFU mode for about 2 minutes.
If a download does not start in that time the existing app is started.

4	Select the DFU device
----------------------------
On the app press the Select button, then select the DFU device. At the time of writing this is called 'DfuTarg'
but I am likely to change this, to include part of the BLE address.

5 Send the new firmware
-------------------------
Pss the Start button on the app. The Blue LED switches off and the third LED (LED3) turns on. 
Watch the progress on the app. If successful all four status lines will have green ticks.

On completion the new app starts executing.





