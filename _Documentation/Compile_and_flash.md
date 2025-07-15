### Compiling and Flashing images - CGP 15/8/24


1	Instructions from Himax
------------------------
    https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2

2   Build
-------------
Assuming you are using Eclipse and have opened the SDK, you need to select one of the sample projects to work with.
As of the time of writing, I have left the original Himax examples in the app\scenario_app folder.
I have placed our projects in app\ww_projects.

I have edited the Makefile with these two options - 
```
# Uncomment one of the following lines to define the project folder. There must be a corresponding entry in app.mk
#PROJECT_FOLDER = scenario_app
PROJECT_FOLDER = ww_projects
```

And I have created a makefile for our projects at app/ww_projects/ww.mk
Make a selection of which project to work with by uncommenting one option:
```
# Uncomment one project to compile:
#APP_TYPE = i2c_slave_app
#APP_TYPE = ww130_test 
#APP_TYPE = ww_template
APP_TYPE = ww130_cli
```
If you make a new project, add it in its own folder under ww_projects, and edit main.c to add an entry, such as:

```
#ifdef WW130_CLI
#include "ww130_cli.h"

/** main entry */
int main(void)
{
	board_init();
	app_main();
	return 0;
}
#endif // WW130_CLI
```
Your new project must have its own makefile, such as ww130_cli.mk, and edit this line to match the project folder name and the 
new entry in main.c
```
APPL_DEFINES += -DWW130_CLI
```
 
Make other edits of names, such as:
* Change the names of the .lk and .sct files to match the new project
* In the makefile for the new project, change the references to these linker files, like this:
```
override LINKER_SCRIPT_FILE := $(SCENARIO_APP_ROOT)/$(APP_TYPE)/ww130_cli.ld
```
 
The compiler output is the 'EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf' file here (your folder may be different!):

'D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\EPII_CM55M_APP_S\obj_epii_evb_icv30_bdv10\gnu_epii_evb_WLCSP65'

3a   Output processing (with normal bootloader)
-----------------------------------------------
Open a command prompt and change to here (adjust for your case):
```
cd D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\we2_image_gen_local
```

Copy the .elf file:
```
copy ..\EPII_CM55M_APP_S\obj_epii_evb_icv30_bdv10\gnu_epii_evb_WLCSP65\EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf input_case1_secboot
```

Run a processing script:
```
we2_local_image_gen project_case1_blp_wlcsp.json
```

This produces `output.img`, ready for downloading to the device (your folder may be different!):
```
D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\we2_image_gen_local\output_case1_sec_wlcsp\output.img
```

The bootloader displays this message: `1st BL Modem Build DATE=Nov 30 2023, 0x0002000b`

3b   Output processing (with bootloader for deep-power-down)
------------------------------------------------------------
When using the deep-power-down (DPD) mode a different bootloadr is required. I assume this is because when the processor wakes in
DPD mode it behaves as though the processor has been reset. For this to be efficient the bootloader needs to execute quickly.

The steps are the same, but in different directory and a different script are needed:

Open a command prompt and change to here (adjust for your case):
```
cd D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\we2_image_gen_local_dpd
```

Copy the .elf file:
```
copy ..\EPII_CM55M_APP_S\obj_epii_evb_icv30_bdv10\gnu_epii_evb_WLCSP65\EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf input_case1_secboot
```

Run a processing script:
```
we2_local_image_gen project_case1_blp_wlcsp_rc24m.json
```

This produces `output.img`, ready for downloading to the device (your folder may be different!):
```
D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\we2_image_gen_local_dpd\output_case1_sec_wlcsp\output.img
```

The bootloader displays this message: `1st BL Modem Build DATE=Jan 17 2025, Version: 2.12`


4	Download to the board using bootloader
------------------------------------------
Follow this sequence:

- Open Teraterm and set the baud rate to 921600.
- Hold down a key on the keyboard (not enter).
- Press and release the reset button (black) on the board.
- Select option 1: "Xmodem download and burn FW image"
- On the Teraterm console navigate here: File > Transfer > XMODEM > send
- Browse to the output.img file (located as indicated above).
- When the download is complete, type 'y' to run your new image

5	Preparing pyOCD tools
-------------------------
Do the steps in this section once only. 

The Himax HX6538 chip implements an SWD port which can be used for programming the flash and for debugging. When programming
a bootloader the SWD system must be used. It is described briefly [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/tree/main/swd_debugging)

The SWD process requires both hardware and software. The hardware is a debugger module that connects to the laptop and to the SWD port of the target board.
Apparently modules that implement the "CMSIS-DAP" standard can be used (amongst others?). 
Himax use the [pyOCD](https://pyocd.io/docs/debug_probes.html) software to communicate with the CMSIS-DAP programmer module.

The Himax instructions use `burn.sh` which is a Linux script. For Windows we need an equivalent
Windows batch file, called `burn.bat` as follows:

```
@echo off
echo %0
echo %1
if "%1" == "" (
    echo "please input flash image name"
) else (
    python swdflash.py --bin="%1" --addr=0x0
)
```

The Himax instructions say to install a modified version of pyOCD, which I did (adjust the directory to suit your installation):

```
cd D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\swd_debugging\pyocd>
python -m pip uninstall pyocd
python -m pip uninstall pyocd_hx
python -m pip install pyocd_hx-0.34.3.dev0+dirty-py3-none-any.whl
```
This gave 3 warnings of this form:
```
WARNING: The scripts pyocd-gdbserver.exe and pyocd.exe are installed in 'C:\Users\charl\AppData\Roaming\Python\Python312\Scripts' 
which is not on PATH. Consider adding this directory to PATH...
```
So I added these to the PATH - and I can then run `pyocd` from a command prompt. 

I then tried to find a programmer module that might work with pyOCD. I tried 3 options (suceeding with the third):

I connected a NUCLEO-L4R5ZI (it has a ST-LINK installed, which is apparently supported). However `pyocd list` reports `No available debug probes are connected`

I then connected the Nordic PCA10040 and had initials success. `pyocd list` reports:

```
  #   Probe/Board                              Unique ID   Target
-------------------------------------------------------------------
  0   Segger J-Link OB-SAM3U128-V2-NordicSem   682540566   n/a
```

(I am not sure if the Nordic PCA10040 works: I got errors from python - not sure if it relates to the PCA10040 or to the WW500 - I might check back later).

I then dusted off the [MAX32625PICO](https://os.mbed.com/platforms/MAX32625PICO/) which I had from an earlier project.
It is also documented [here.](https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32625pico.html#eb-overview)

This module gave this result:
``` 
   #   Probe/Board        Unique ID                                          Target
----------------------------------------------------------------------------------------
  0   ARM CMSIS-DAP v1   04091702b4de08fb00000000000000000000000097969906   v max32630
      MAX32630FTHR
``` 
I then constructed a cable assembly that links the MAX32625PICO to the WW500 using an unpopulated  WWIF100 as an 
intermediate routing board, plus an existing DB9 male which I connect to 6-way Molex cable assembly. This allows me to connect
my laptop to the MAX32625PICO and via a cable assembly to the 6-way SWD port TC1 on the WW500.

(I documented this cable assembly elsewhere... TC2030_UART.txt).     

6	Download bootloader to the board using SWD
----------------------------------------------
This process is described [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/swd_debugging/README.md)
Go to the "Flash programming by SWD" section. Steps 1 & 2 duplicate steps already documented above. Start from step 3, 
which are listed below with some adjustments for Windows.

If using the standard bootloader, do this:
```
cd ..\swd_debugging\swdflash\
copy ..\..\we2_image_gen_local\output_case1_sec_wlcsp\output.img .
```

If using the DPD bootloader, do this:
```
cd ..\swd_debugging\swdflash\
copy ..\..\we2_image_gen_local_dpd\output_case1_sec_wlcsp\output.img .
```
Now connect the hardware:

- The MAX32625PICO can't power the WW500, so power through the FTDI USB cable plus WWIF100
- Laptop -> MAX32625PICO
- MAX32625PICO -> cable assembly (ending with TC2030 pins)
- Hold the TC2030 assembly onto TC1. 
 

Now run the flash batch file:
```
.\burn.bat output.img
```

The result was this:
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

Success!

DPD Bootloader
------------------
(These notes pre-date my work with the DPD bootloader)

On 7 January 2025 Himax emailed instructions for using deep power-down (DPD) mode. The email included this:

```
Since dpd mode applications require the dpd mode bootloader, please use this new image generation tool (we2_image_gen_local_dpd) to generate firmware images.
1. allon_sensor_tflm_freertos : dpd mode sample code
sleep_mode.c : call app_pmu_enter_dpd() to enter dpd mode and wake up via PA0
2. we2_image_gen_local_dpd : bootloader for dpd mode
execute following command to generate dpd mode firmware image
./we2_local_image_gen project_case1_blp_wlcsp_rc24m.json
Please make sure the bootloader message is as follows.
“1st BL Modem Build DATE=Jan  7 2025, Version: 2.12”
```

From the code provided in [github](https://github.com/stevehuang82/for_wildlife_ai/tree/main) it looks like a new
folder `we2_image_gen_local_dpd' is required along side the existing folder `we2_image_gen_local' 



 