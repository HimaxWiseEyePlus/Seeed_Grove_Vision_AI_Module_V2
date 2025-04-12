# Summary of `ww500_md_test_1`
#### CGP - 12/4/25

The original version of this was provided by Himax to Wildlife.ai to test the WW500 with HM0360
[Available at Github](https://github.com/stevehuang82/for_wildlife_ai/tree/main/allon_sensor_tflm_freertos)

I have made quite a few changes, including:

* Adding colour, then use the colour to make the console output easier to see important information.
* Adding code that uses the RTC to get names of files, and report the time when it wakes from DPD etc.
* Refactored the code that sends register settings to the HM0360, especially to stop duplication
of register writes when it enters DPD.
* Add the ability to save an image sequence number in a file on the SD card.
* Read some HM0360 registers, including the motion detection output registers and the frame count registers.
* Motion detection output can be displayed by a python script [md_display.py](../../../../../_Tools/md_display.py) 
documented [here](../../../../../_Documentation/md_display.md)
* Debugging code while investigating problems, especially errors reading/writing HM0360 registers.
* Code to write HM0360 registers from SD card files. This uses a python script [scan_cis_settings.py](../../../../../_Tools/scan_cis_settings.py) 

__LATER__

On 8/4/25 Himax sent another version of allon_sensor_tflm_freertos with different HM0360 register settings:
1. Configure HM0360 to 640x480 10 fps(Context A) when WE2 is in all-on mode.
2. Configure HM0360 to 320x240 2 fps (Context B) when WE2 is in DPD mode to save power.
3. User can fine-tune the sleep time counter in the file ~\cis_hm0360\cisdp_sensor.c to control the frame rate of HM0360.

I have saved both versions, one as `allon_sensor_tflm_freertos_1` and the other as `allon_sensor_tflm_freertos_2` 
So I introduced a compiler switch to allow this to be tested:
```
#define CONTEXTSWITCH
```

My email to Himax of 12/4/25 explains results (so far).

#### Errors reading/writing HM0360 registers

These seemed to largely go away when I attach a scope probe to the SCL signal, so
I guess there is noise on this line which is causing trouble. Add RC network on the next PCB revision!

#### Code to write HM0360 regsters from SD card files. 

