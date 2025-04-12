# allon_sensor_tflm_freertos_2

The second version from Himax [here](https://github.com/stevehuang82/for_wildlife_ai/tree/main/allon_sensor_tflm_freertos)

Email of 8/4/25 includes this:


1. Configure HM0360 to 640x480 10 fps(Context A) when WE2 is in all-on mode.
2. Configure HM0360 to 320x240 2 fps (Context B) when WE2 is in DPD mode to save power.
3. User can fine-tune the sleep time counter in the file ~\cis_hm0360\cisdp_sensor.c to control the frame rate of HM0360.
```
        HX_CIS_SensorSetting_t  HM0360_md_stream_on[] = {
                        {HX_CIS_I2C_Action_W, 0x3024, 0x01},        // select context B
                        {HX_CIS_I2C_Action_W, 0x3029, 0x40},         // 2fps sleep count H
                        {HX_CIS_I2C_Action_W, 0x302A, 0x20},        // 2fps sleep count L
                        {HX_CIS_I2C_Action_W, 0x3510, 0x00},         // disable parallel output
                        {HX_CIS_I2C_Action_W, 0x0100, 0x02},         };
```                        
4. add \doc\ HM0360_Motion_Detection_Setting_Context_B_20250408.pdf

#### Main differences between this and the first version:

1. A different set of register settings.
2. Different values in HM0360_md_stream_on[] and HM0360_stream_on[] (different delays on 0x3029, 0x302a and select different context A/B)


__Original Notes__

This is the scenario_app which can capture 10 images and then sleep for 1 second before getting up and take another picture, then sleep for 1 second before getting up and take another picture, and continue.  
You can also change the setting for how many images you need to capture at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/allon_sensor_tflm_freertos/common_config.h#L24).
```
#define SUPPORT_FATFS               0       // 0 : send images via SPI, 1 : save images to SD card
#define ENTER_SLEEP_MODE			0		// 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		10
#define ENTER_PMU_MODE_FRAME_CNT	3
```
- SUPPORT_FATFS : (0) send images via SPI, (1) save images to SD card
- ENTER_SLEEP_MODE : (0) always on, (1) enter Sleep mode
- SENSOR_AE_STABLE_CNT : how many images to capture and then enter sleep mode
- ENTER_PMU_MODE_FRAME_CNT : After waking up, capture how many images and then enter sleep mode.