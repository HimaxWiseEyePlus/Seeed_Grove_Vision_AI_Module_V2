Notes on this program - CGP 29/7/24
=====================================

Creates 4 FreeRTOS tasks:
-----------------------
	main_task (in allon_sensor_tflm.c)
	algo_task
	comm_task
	dp_task
	
These communicate by passing messages to each other - they write to their queues.
The tasks sleep untilsomething appears in their queue.
	
main_task
-----------
Executes at reset (including warm boot after sleeping)
Initailises vision sensor before entering it queue loop.
When the frame capture event arrives it wakes the algo_task 
When the algo_task has finished its NN processing it informs the main_task which puts the board to sleep.

algo_task
-----------
Responsible for the NN processing - see cv_run()
It looks like it also prepares a JPEG image and sends it (via SPI)	

dp_task
----------
Looks like this has a callback from interrupts: os_app_dplib_cb()
This receives interrupt events and converts them to events in the task's queue.

 Messages are:
 	APP_MSG_DPEVENT_XDMA_FRAME_READY
 	- it prints stuff then informs the main_task
 	APP_MSG_DPEVENT_RECAPTURE
 	- calls sensordplib_retrigger_capture() - to capture another image?

comm_task
-------------
Initialises seems to include initialising the I2C slave - see section below

This handles the PA) interrupt (generates a APP_MSG_COMMEVENT_AON_GPIO0_INT event)

The task looks out for events such as:
	The PA0 interrupt goes to main_task - where commented out code seems to stop image capturing.
	I2C slave RX, TXand error events



I2C slave in comm_task
------------------------
pinmux_init() also initialises PA2 & PA3 to be I2C slave.

i2c_comm.h seems to support I2C comms 
Message bytes are treated as:
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


SPI transfer of JPEG data
---------------------------
Looks like pinmux_init() calls spi_m_pinmux_cfg() then it sets up PB2, 3, 4 & 11 as an SPI master
	These pins are on the Seeed Vision AI V2 connector.
The algo_task initialises the SPI master.
The algo task calls hx_drv_spi_mst_protocol_write_sp() specifying DATA_TYPE_JPG
The pointer to the jpeg data and size are obtained by a call to cisdp_get_jpginfo()
and it looks like this is associated with WDMA2 block - perhaps this is set up
to perform a JPEG operation?

