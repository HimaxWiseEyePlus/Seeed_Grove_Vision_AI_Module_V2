### Compiling and Flashing images - CGP 15/8/24


1	Instructions from Himax
------------------------
    https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2

2   Build
-------------
Assuming you are using Eclipse and have opened the SDK, you need to select one of the sampel projects to work with.
As of teh time of writing, I have left the original Himax examples in the app\scenario_app flder.
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
If you make a new project, add it in its own folder under ww_projects, and edit main.c to add an entry,such as:

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
 
The compiler output is the EPII_CM55M_gnu_epii_evb_WLCSP65_s.elf file here (your folder may be different!):

D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\EPII_CM55M_APP_S\obj_epii_evb_icv30_bdv10\gnu_epii_evb_WLCSP65

3   Output processing
---------------------
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

This produces this file, ready for downloading to the device:
```
D:\Development\wildlife.ai\Seeed_Grove_Vision_AI_Module_V2\we2_image_gen_local\output_case1_sec_wlcsp\output.img
```

4	Download to the board
-------------------------
Follow this sequence:
- Open Teraterm and set the baud rate to 921600.
- Hold down a key on the keyboard (not enter).
- Press and release the reset button (black) on the board.
- Select option 1: "Xmodem download and burn FW image"
- On the Teraterm console navigate here: File > Transfer > XMODEM > send
- Browse to the output.img file (located as indicated above).
- When the download is complete, type 'y' to run your new image





 