## Notes on ww130_test - CGP 30/7/24

This project is to establish essential functionality, based on a previous Himax project.
There are plenty of things that should be cleaned up once this essential functionality has been established.
Some of this listed in the TODO section at the end of this document.

(This started as a copy of allon_sensor_tflm_freertos. In it I have added code to operate with the WW130.
See readme.txt files in that old project for background on where I got before I made the copy).

allon_sensor_tflm_freertos notes
---------------------------------
The allon_sensor_tflm_freertos scenario_app can capture 10 images and then sleep for 1 second before getting up and take another picture, then sleep for 1 second before getting up and take another picture, and continue.  
You can also change the setting for how many images you need to capture at [here](https://github.com/HimaxWiseEyePlus/Seeed_Grove_Vision_AI_Module_V2/blob/main/EPII_CM55M_APP_S/app/scenario_app/allon_sensor_tflm_freertos/common_config.h#L24).

WW130 Test Changes
------------------
I will make a succession of changes in this app, to bring it closer to what the Wildlife Watcher will require:
- Adding colour printing
- Added code to print buffer contents for debugging I2C comms
- Getting SD card operation working
- Moving main_task to its own file
- Adding code, including ww130_cmd.c & .h - to process commands sent by WW130 over I2C port
- Added crc16_ccitt.c & .h because the Himax code generated a CRCthat I could not decode on the ww130.

Configuration
----------------
The common_config.h file includes compiler switches that change behaviour.

```
#define SUPPORT_FATFS               0       // 0 : send images via SPI, 1 : save images to SD card
#define ENTER_SLEEP_MODE			0		// 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		10		// Number of images to capture before sleeping
#define ENTER_PMU_MODE_FRAME_CNT	3		// Number of images to capture after waking
#define	APP_SLEEP_INTERVAL			5000	// Sleep interval in ms
```

- SUPPORT_FATFS : (0) send images via SPI, (1) save images to SD card
- ENTER_SLEEP_MODE : (0) always on, (1) enter Sleep mode
- SENSOR_AE_STABLE_CNT : how many images to capture and then enter sleep mode
- ENTER_PMU_MODE_FRAME_CNT : After waking up, capture how many images and then enter sleep mode.
- APP_SLEEP_INTERVAL : Sleep interval in ms


Tasks and States
----------------
See a separate document that docuemnts these.

I2C slave in comm_task
------------------------
pinmux_init() also initialises PA2 & PA3 to be I2C slave.

i2c_comm.h seems to support I2C comms.
The initial bytes in each message are treated as:
	feature
	command
	size (incl. number of bytes in the sise field)
	payload itself
	
evt_i2ccomm.h supports messages types (called "features")
If the feature is I2CCOMM_FEATURE_SYS then a further routine implements support for these commands:
	I2CCOMM_CMD_SYS_GET_VER_BSP
	I2CCOMM_CMD_SYS_GET_VER_I2C
In these case data is prepared in a buffer, which can be sent to the I2C master with:
	hx_lib_i2ccomm_enable_write()	

This mechanism can be extended for our custom communications with the WW130.

SPI transfer of JPEG data
---------------------------
It looks like the system is set up to create JPEG images automatically, every time a frame is grabbed.
It looks like this is associated with WDMA2 block. 

This is some how built into the cisdp_sensor.c code. The details of the JPEG image are available 
using the cisdp_get_jpginfo() which provides the pointer to the jpeg data and its size.

Looks like pinmux_init() calls spi_m_pinmux_cfg() then it sets up PB2, 3, 4 & 11 as an SPI master
	These pins are on the Seeed Vision AI V2 connector.
	
The algo_task initialises the SPI master. The default code calls hx_drv_spi_mst_protocol_write_sp() 
specifying DATA_TYPE_JPG - in other words, every image taken has its data transferred to the SPI bus.

Also, this same data path can be used to transfer the JPEG as files to the SD card.

TODO
----

There is plenty of stuff here that should be cleaned up as soon as basic functionality has been proven. Examples:
- comm_task.c assumes an array of buffers gWrite_buf[] whereas we will only need one. Change this to simplify.
- re-write state machines (see the Tasks_and_states document).
-  