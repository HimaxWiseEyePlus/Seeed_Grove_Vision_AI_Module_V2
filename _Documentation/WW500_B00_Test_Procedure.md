# WW500 Rev B00 Test Procedure

#### CGP 28/5/25

This document describes how to test a WW500 after assembly.

__Ensure an antenna is connected to the MKL62BA.__

## Initial Power On

1. Connect the WW500 to a WWIF100 at J5 using a 10-way flat flexible cable (FFC).
2. Power the WWIF100 using an FTDI TTL-232R-3V3 cable.
3. Check voltages as follows (use the hole P3 as GND reference).
	1. 3.3V (VBATT) at J8.
	2. 3.3V (3V3) at C27 (near 4-way connector J4).
	3. 2.8V at R45 (near 30-way connector J1).
	4. 1.8V at R47 (near 30-way connector J1).
	5. 3.3V (3V3_WE) at J6 pin 15.
	6. 1.8V (1V8_WE) at the broad track below J8 and J3 (otherwise at R5 on the back).
	7. 0.9V (VDD) at C38 on the back.

## Program MKL62BA with bootloader

This process programs the DFU bootloader. After that, the DFU mechanism is used to program the 
WildlifeWatcher application. (TODO: can we do this from the console, not from within Eclipse?).

1.	Disconnect the WWIF100.
2.	Run Eclipse, selecting the Mokotech workspace and then the `dfu` project.
3.	Connect and power the PCA10040 operatng at a programmer. Plug the TC2030 cable into TC2.
	Execute three nrfprog commands:
	1.  Project > Build Targets > Build > Erase
	2.  Project > Build Targets > Build > flash_softdevice
	3.  Project > Build Targets > Build > flash
4.	Disconnect the PCA10040

Try this:
Open a command prompt and follow these steps:
```
cd D:\Development\MokoTech\Workspace\dfu\secure_bootloader\ww500_a00_s132_ble\armgcc\_build
nrfjprog -f nrf52 --eraseall
nrfjprog -f nrf52 --program D:\Development\MokoTech\MokoTechSDK\src\nordic_sdk\nRF5_SDK_16.0.0_98a08e2\components\softdevice\s132\hex\s132_nrf52_7.0.1_softdevice.hex --sectorerase --verify
nrfjprog -f nrf52 --program nrf52832_xxaa_s132.hex --sectorerase --verify
nrfjprog -f nrf52 --reset

```

## Program MKL62BA with WildlifeWatcher app

The first  time the board is powered on it will enter DFU mode, and will wait for the 
WildlifeWatcher app to be sent from a smart phone.

1. Configure Teraterm for 115200 baud.
2. Reconnect the WWIF100 to J5 and power it. Expect a signon message on Teraterm. 
3. Load the signe Wildlife Watcher app onto a smart phone downloads folder. The file will
have a name something like `WildlifeWatcher_1_ww500_a00_nus_000802.zip`
4.	Run the nRFToolbox app and select the Device Firmware Update option.
5.	Select the file from the Download folder.
6.	Select the WW500 from the Device list.
7.	Click on Program and wait for confirmation that the app has downloaded successfully.

## MKL62BA LoRaWAN Credentials

The WW500 will now run the WildlifeWatcher_1 app. As it has the LoRaWAN credentials for the
Provisioning app, it will attempt to load new credentials from the server.

TODO - figure out whether it is necessary to delete the credential memory by holding down SW1 while applying power_.
Also add instructions fro setting up the Gateway.


## MKL62BA LoRaWAN Functionality

At this point the WW500 should have final LoRaWAN credentials, and the name of the board will be revealed.
Something like `WILD-OMUZ`

1. Configure Teraterm for 115200 baud.
2. Reconnect the WWIF100 to J5 and power it. Expect a signon message on Teraterm.
3. Press the Reset button on the WWIF100 and watch for a flashing sequnce on the 3 LEDs on the WWIF100 and
also on the three LEDs on the WW500.
4.	Press SW1 on the WW500 and look for a message "Starting advertising".
5.	Move a magnet near IC4. Look for "Magnet detected" and "Magnet removed" messages.

## MKL62BA LoRaWAN Credentials

To follow.

## MKL62BA BLE tests

To follow.

## Program HX6538

1.	Disconnect the WWIF100.
2.  Connect a SWD100 programmer to a laptop using a USB cable.
3.	On a laptop do this:

```
cd D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\swd_debugging\swdflash
copy ..\..\we2_image_gen_local_dpd\output_case1_sec_wlcsp\output.img .
```
4.	Now connect the hardware:

- Laptop -> SWD100
- SWD100 -> cable assembly (ending with TC2030 pins)
- Hold the TC2030 assembly onto TC1. 
 
5.	Now run the flash batch file:
```
.\burn.bat output.img
```

6.	Expect the result to be something like this:
```
burn.bat
output.img
swdflash.py version 1.0.6
===== start =====
current pc = 0x18003DC0 after reset_and_halt
pyOCD script method select FlashAlgoDB
program flash addr 0x00000000 size 0x0009D000
Progress: [██████████████████████████████] 100.00% 643072/643072
Progress: [██████████████████████████████] 100.00% 643072/643072
The elapsed time = 14.925381 sec
===== end =====
```

## HX6538 Functionality

to follow...

	
	
