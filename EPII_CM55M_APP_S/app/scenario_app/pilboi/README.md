# allon_sensor_tflm_freertos
This is the scenario_app which can capture 10 images and then sleep for 1 second before getting up and take another picture, then sleep for 1 second before getting up and take another picture, and continue.  
You can also change the setting for how many images you need to capture at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/allon_sensor_tflm_freertos/allon_sensor_tflm.c#L81).
```
#define ENTER_SLEEP_MODE			1		// 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		10
#define ENTER_PMU_MODE_FRAME_CNT	1
```
- SENSOR_AE_STABLE_CNT: how many images to capture and then enter sleep mode
- ENTER_PMU_MODE_FRAME_CNT: After waking up, capture how many images and then enter sleep mode.