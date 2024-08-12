## Notes on ww130_test - CGP 30/7/24

Work in progress document - expect changes as I fiddle with thc code...

This project is to establish essential functionality, based on a previous Himax project.
There are plenty of things that should be cleaned up once this essential functionality has been established.
Some of this listed in the TODO section at the end of this document.)

(This started as a copy of allon_sensor_tflm_freertos. In it I have added code to operate with the WW130.
See readme.txt files in that old project for background on where I got before I made the copy).

allon_sensor_tflm_freertos notes
---------------------------------
The allon_sensor_tflm_freertos scenario_app can capture 10 images and then sleep for 1 second before getting up and take another picture, then sleep for 1 second before getting up and take another picture, and continue.  
You can also change the settings for how many images you need to capture
with SENSOR_AE_STABLE_CNT and ENTER_PMU_MODE_FRAME_CNT)

WW130 Test Changes
------------------
I am making a succession of changes in this app, to bring it closer to what the Wildlife Watcher will require:
- Adding colour printing (in printf_x.c & .h)
- Added code to print buffer contents for debugging I2C comms (also in printf_x.c & .h)
- Getting SD card operation working
- Moving main_task to its own file (main_task.c & .h) so all 4 tasks have their own files.
- Got SPI slave data transfer working with WW130
- Added extra processing of I2C messages (in ww130_cmd.c & .h) - to process commands sent by WW130.
- Added crc16_ccitt.c & .h because the Himax code generated a CRC that I could not decode on the ww130.
- Added code to responds to interrupts from the WW130 on the PA0 pin (current code uses that to take a photo).

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

How to play with this code
----------------------------
The app is designed to work with a WW130 talking over BLE to a smart phone.
The phone can be used to send commands to the WW130 which can then send soem of these commands to the 
Seeed board.

The Seeed board responds and messages are sent back in the opposite direction - to the WW130 then t the phone.

Here are some steps to get it working:

- Plug together the Seeed board, camera, SD card, WW130, WWIF100.
- The setup is powered by the USB-C connector in the Seeed board, which alos provides console output with Teraterm (at 921600 baud).
- Remove the 5V power pin from theh 6-way connector on the TTL-232R-3V3 USB serial adapter.
- Plug this into the 6-way pin header on the WWIF100. 
- Set up Teraterm for the WW130 console at 115200 baud.
- Get a smart phone running the nRFToolbox app and scroll to the UART app.
- Start the WW130 advertsing by pressing SW1 on the WWIF100 then connect.
- Type commands to the WW130. For example, "status" reports some status of the WW130.
- Any command beginning "AI " will be sent to the Seeed board. The Seeed baord will cosntruct a response and return this to the WW130 then to the phone.

List of commands for teh Seeed board follows. This is quite limited at present. I will add a few more.
I don't intend making this app the real Wildlife Watcher app so I won't extend this list too far. It is mainly here to test key concepts.

The commands are parsed by processCmd() in ww130_cmd.c and is driven from the table expectedMessages[]

- AI status - reports the status of a boolean variable.
- AI enable - sets this variable
- AI disable - clears this variable.
- AI snap - Takes a photo.
- AI exif - intended to show how to return a reasonably large mount of binary data - as will be required to get EXIF, JPEG images etc. (Not yet implemented)
- AI int <nnn> - asks Seeed to pulse PA0 for <nnn>ms to test that the Seeed board can interrupt the WW130. (Turns out nnn must be 110)



Tasks and States
----------------
The program uses FreeRTOS with 4 tasks each with a state machine (or sorts!).
See a separate document that documents these.

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

EXIF metadata
-------------------
See the email from Himax of 2/8/24 which includes suggestions on metadata and generating JPEGs (for the thumbnail).
It turns out the Himax have added a 3rd party JPEG library for a JPEG encoder, in library/JPEGENC.
If you google the author you end up at the [JPEGENC library](https://github.com/bitbank2/JPEGENC)
He also offers the [tifftool library](https://github.com/bitbank2/tifftool) that peers inside JPEfiles for metadata etc.

TODO
----
There is plenty of stuff here that should be cleaned up as soon as basic functionality has been proven. Examples:
- comm_task.c assumes an array of buffers gWrite_buf[] whereas we will only need one. Change this to simplify.
- re-write state machines (see the Tasks_and_states document).
- Complete the AI exif command to show how to return large binary data to the WW130 in multiple blocks.
- Add real EXIF inside JPEG files (see note above).
- Make SD card more robust to use - it does not work with all the SD cards I have, and sometimes the initial mount fails.
 - Probably place the FATFS cpde in its own FreeRTOS task?
 