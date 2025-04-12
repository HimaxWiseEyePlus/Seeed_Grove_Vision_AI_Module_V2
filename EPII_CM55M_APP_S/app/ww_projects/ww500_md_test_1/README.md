# WW500_MD_TEST_1

Original code was provided by Himax to Wildlife.ai to test the WW500 with HM0360
(Available at Github)[https://github.com/stevehuang82/for_wildlife_ai/tree/main/allon_sensor_tflm_freertos]

That code was then updated on 8/4/25, and the changes in there have been added to this project as well.

CGP added quite a few changes. Then for more substantial changes I cloned this app as `ww500_md_test_2`

__For the summary of what I have done in my version of this project, visit [/doc/summary.md](./doc/summary.md)__

This is the scenario_app which can capture 10 images and then sleep for 1 second before getting up and take another picture, then sleep for 1 second before getting up and take another picture, and continue.  
You can also change the setting for how many images you need to capture at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/allon_sensor_tflm_freertos/common_config.h#L24).

```
#define FLASH_XIP_MODEL             0
#define MEM_FREE_POS                (BOOT2NDLOADER_BASE)

#define SUPPORT_FATFS               1       // 0 : send images via SPI, 1 : save images to SD card
#define ENTER_SLEEP_MODE            1       // 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		4
#define ENTER_PMU_MODE_FRAME_CNT	10

// delay in milli-seconds
#define DELAYBETWEENPICS	2000
```

- SUPPORT_FATFS : (0) send images via SPI, (1) save images to SD card
- ENTER_SLEEP_MODE : (0) always on, (1) enter Sleep mode
- SENSOR_AE_STABLE_CNT : how many images to capture and then enter sleep mode
- ENTER_PMU_MODE_FRAME_CNT : After waking up, capture how many images and then enter sleep mode.

