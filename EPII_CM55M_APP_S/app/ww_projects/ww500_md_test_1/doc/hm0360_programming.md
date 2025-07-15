# HM0360 Programming
#### CGP 2/4/25

I have spent some time trying to unpick the places at which the HM0360 gets written to. Looks like this:

## 1. At Cold Reset

* Initialisation is does with `app_start_state(APP_STATE_ALLON)`
* This calls `cisdp_sensor_init(true)` and then `cisdp_sensor_start()`
* The `cisdp_sensor_init(true)` call which does this:
	1. Five calls to hx_drv_xxxx() - control of the HM0360 sleep, setting I2C address
	2. Writes the `M0360_stream_off` register list (One entry: sets `MODE_SELECT` to 0 = sleep)
	3. Writes the `HM0360_md_init_setting` register list (Many entries in `HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i` file)
	4. Writes the `HM0360_mirror_setting` register list (One entry: sets `IMAGE_ORIENTATION` for vertical flip & mirror)
* The `cisdp_sensor_start()` call changes `MODE_CONTROL` to 1 = continuous streaming.

This sets the camera running, so the next thing that happens is that thre is an event that says an image is done.

## 2. When an image is captured

* The data path callback sends a `SENSORDPLIB_STATUS_XDMA_FRAME_READY` event to indicate a frame is ready.
* The image is processed but there are no writes to the HM0360.
* The FreeRTOS task is delayed and when it times out there is a call to `sensordplib_retrigger_capture()`

__AFAIK there are no accesss to the HM0360. Is the HM0360 running continuoulsy while the processing and delay
is happening, or was it set up to capture just a single image?__

## 3. Before Entering DPD

* When all the images are taken there is a call to `app_start_state(APP_STATE_STOP)`
followed by a call to `cisdp_sensor_md_init()`
* The `app_start_state(APP_STATE_STOP)` calls `cisdp_sensor_stop()` which 
writes the `M0360_stream_off` register list (One entry: sets `MODE_SELECT` to 0 = sleep) 
(also other calls to `sensor_dp_lib.c`).
* The `cisdp_sensor_md_init()` call does this:
	1. Five calls to hx_drv_xxxx() (identical to those in cisdp_sensor_init())
	2. Writes the `M0360_stream_off` register list __duplicate!__
	3. Writes the `HM0360_md_init_setting` register list (Many entries in `HM0360_OSC_Bayer_640x480_setA_VGA_md_4b_ParallelOutput_R2.i` file)
	4. Writes the `HM0360_mirror_setting` register list (One entry: sets `IMAGE_ORIENTATION` for vertical flip & mirror)
	4. Writes the `HM0360_md_stream_on` register list (Two entries: at `MASK_OUTPUT_EN` output is disabled. 
	At `MODE_SELECT` set "Automatic wakeup sleep cycles").
	5. There is a 100ms delay. __Why?__
* There is a call to `app_pmu_enter_dpd()` which does not return.

## 4. At Warm Reset

* Previously there was a call to `app_start_state(APP_STATE_ALLON)` which did the same initialisation as
for a cold boot. However, I changed this to:
* Initialisation is do with `app_start_state(APP_STATE_RESTART)`
* This calls `cisdp_sensor_init(false)` and then `cisdp_sensor_start()`
* The `cisdp_sensor_init(false)` call which does this:
	1. Some initialisation of how control of the HM0360 will happen
	2. Writes the `M0360_stream_off` register list (One entry: sets `MODE_SELECT` to 0 = sleep)
* The `cisdp_sensor_start()` call changes `MODE_CONTROL` to 1 = continuous streaming.
* Then operation is as it was for the Cold Reset

## Difference between `cisdp_sensor_init(true)` and `cisdp_sensor_md_init()`

It looks like the Cost Reset writes to the HM0360 are almost identical to the calls just before entering DPD.
In particular, the long list of register values are written again. I suspect this is superfluous.

I am minded to revise `cisdp_sensor_md_init()` so it does the minimum required for setting up motion detection 