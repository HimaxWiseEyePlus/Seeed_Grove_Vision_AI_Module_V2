# allon_sensor_tflm_freertos

Provided by Himax to Wildlifa.ai to test the WW500 with HM0360
[Available at Github](https://github.com/stevehuang82/for_wildlife_ai/tree/main/allon_sensor_tflm_freertos) 
though in the original version version of 3/3/25.

I had to make quite a few changes to stop compiler error and warning messages. Other than this, no functional changes.
I made a copy of this as `ww500_md_test_1` and made quite a few changes there. Then more extensive changes in `ww500_md_test_2`

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

## Issues or Questions for HiMax

1. What does the following code do, and should it be using `DP_MCLK_SRC_INT_SEL_RC36M` instead?
```
	hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
```

what is this code?
```
    hx_drv_cis_init(DEAULT_XHSUTDOWN_PIN, SENSORCTRL_MCLK_DIV1);
    hx_drv_sensorctrl_set_xSleepCtrl(SENSORCTRL_XSLEEP_BY_CPU);
    hx_drv_sensorctrl_set_xSleep(1);
```

Why add a delay before shutting down?
```
	hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
```	

What do these do? Do they access HM0360?
```
sensordplib_set_mclkctrl_xsleepctrl_bySCMode();
sensordplib_set_sensorctrl_start();
```   	
   	
   	