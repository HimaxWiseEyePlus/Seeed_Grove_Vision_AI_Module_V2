# HM0360 on WW500 Summary
#### CGP 9/6/25

The HM0360 is a an image sensor from Himax. Its importnat feature for WW500 is its motion detection feature.
Its web page is [here](https://www.himax.com.tw/products/cmos-image-sensor/always-on-vision-sensors/hm0360/).

There is a demo of the HM0360 being used for motion detection in this [Youtube video](https://www.youtube.com/watch?v=7tuq-vz4aVk)
at about 28:40. A graphic showing motion detection in a grid of 16 x 15 is [here](https://media.licdn.com/dms/image/v2/D5622AQEhOXRfFxke5g/feedshare-shrink_800/feedshare-shrink_800/0/1724290518546?e=1748476800&v=beta&t=yCAhWt4YXZHPhc_3XgUHit6GFtoqd1jhdqUT3vbohow) 

The HM0360 has these attributes that are of significance for the WW500:

* It has a motion detection capability which can wake the processor from sleep when there is a difference between two images that exceeds a threshold - i.e. motion detection.
8 It can connect directly to the HX6538 AI processor.
* It can takes monochrome images up to 640 x 480 pixels (VGA resolution).

The module we are using on the WW500 is the [HM0360-MWA-00FP963](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/4287/HM0360-MWA-00FP963.pdf)
which has the HM0360 sensor on the end of a short flat flexible cable, with a 30-way connector to the PCB.

## Feasibility Testing

The key questions about this device, relevant to the WW500 project, were these:

* Could we connect the HM0360 to the AI processor,communicate with it, and collect images?
* Could we put the HX6538 AI processor into a deep power down (DPD) mode, and use the HM0360 motion detection features to 
wake the processor when the scene changes?
* Would the power consumption of this design be low?

After some work, and help from Himax, the answer to all of these questions is yes. 
ower consumption for the whole WW500 is deep power down is c. 330uA.
I would hope to be able to reduce this, but 330uA is satisfactory - it represents over 300 days operation from a set 
of AA batteries PROVIDED NO OTHER PROCESSING TAKES PLACE. Of course, the expectation is that other processing 
will take place - that the WW500 will respond to motion detection and wake to process the scene.

At present the HM0360 detects motion and then captures a monochrome image at 640 x 480 resolution.


There are a large number of registers which determine the behaviour of the HM0360, including behaviour
in motion detection mode. These allow the sensitivity to be set, in particular. There are also opportunities 
to investigate lower power and faster responsiveness. These are subjects for further investigation in the future.
 
## Two Cameras

As the HX6538 has two camera interfaces,it is possible that the HM0360 can be used only for motion detection, and 
a different (Raspberry PI) camera can capture the image itself - perhaps in colour or at higher resolution. 

## HM0360 Registers

There are a large number of registers which determine the behaviour of the HM0360. These can be seen in the data sheet,
section `10. Regsiter Table` - in fact there are further undocumented registers as well. Regsiters can be thought of as
being in one of three groups:

1. Universal Regsiters - these have effect always.
2. Context A registers - these have effect when the Context bit (bit 0 in PMU_CFG_3 register 0x3024) is 0.
3. Context B registers - these have effect when the Context bit (bit 0 in PMU_CFG_3 register 0x3024) is 1.

By programming both Context A and Context B registers with values, operation can be switched between
two modes just by changing the single context bit. This in fact happens in the WW500 code,
at the time of writing. The Himax document [HM0360 Motion Detection Setting for
Context B]() touches in this. (moe to follow).

Registers are programmed using the `hx_drv_cis_setRegTable()` function. Its prototype is:
```
/**
 * \brief Set Configuration to CIS Sensor by I2C Master.
 *
 * \param[in] pSensorSetting Configuration table for CIS Sensor
 * \param[in] Length Configuration table Length for CIS Sensor
 * \return HX_CIS_ERROR_E.
 * */
HX_CIS_ERROR_E hx_drv_cis_setRegTable(HX_CIS_SensorSetting_t *pSensorSetting, uint16_t Length);

```	

This takes an array of type HX_CIS_SensorSetting_t and sends the data to the HM0360 device.
The `HX_CIS_SensorSetting_t` type is defined as follows - basically an operation, register address and value tuple.:
```
/**
 * \struct HX_CIS_SensorSetting_t
 * \brief CIS Sensor I2C Master Setting by Common Sensor Driver. You can use this structure to create table for all Sensor configuration
 */
typedef __PACKED_STRUCT {
	HX_CIS_I2C_ActionType_e I2C_ActionType;		/**< CIS Sensor I2C Master Action Write, Read, Delay */
	uint16_t RegAddree; 						/**< register address for I2C Master if I2C_ActionType == HX_CIS_I2C_Action_S, RegAddree = delay msec */
	uint8_t Value;								/**< register value for I2C Master if I2C_ActionType == HX_CIS_I2C_Action_S, don't (care fill 0) */
} HX_CIS_SensorSetting_t;
```
You can see several examples of this in action by searching our code. 

At cold boot (only) most of the registers are programmed, from the file
`HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i`

```
static HX_CIS_SensorSetting_t HM0360_md_init_setting[] = {
#include "HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i"
};
```

Following cold boot all these values are retained, even when the HX6538 enters DPD.
So subsequently, calls to `hx_drv_cis_setRegTable()` need only send a few values.

If you look at the source code in `cisdp_sensor.c` (which would benefit from 
being tidied up!) you will see that new HM0360 register settings are
only sent after emerging from warm boot (`HM0360_stream_on[]`) and prior to entering 
DPD (`HM0360_md_stream_on[]`). These two use different values for the Context bit.

* HM0360_stream_on[] uses context B and samples at 10fps
* HM0360_stream_off[] uses context A and samples at 1fps

__TODO_ I need to check the context A & B regsisters, as it seems that both
have motion detection enabled - is this right?

Context A MD_CTRL 0x354b = 0x41
Context B MD_CTRL 0x35a5 = 0x41

## Loading HM0360 Settings from a file

I have (partly) implemented a feature to load HM0360 regsiter values froma a file.
This is designed to allow testing on different settings without having to recompile the code each time.

The feature is implemented in `cis_file.c` and `cis_file.h` though this seems to be 
only in the `ww500_md_test_1` project.

The function `cis_file_process()` reads a file of binary data and places it into a buffer
as though this was an array of type `HX_CIS_SensorSetting_t`. The code then calls 
`hx_drv_cis_setRegTable()`, thus changing the HM0360 registers.

The best place to call `cis_file_process()` would be in the cold boot path, after 
the call that has initialised the HM0360 with the large register set in `HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i`.

At this point you should assume that this is untested (I probably tested it just with some dummy values).

The binary file is created by a Python script `scan_cis_settings.py` which is present in the
`_Tools` folder. It is well documented. It processes a text file and prodces a binary file.

## More to follow...
 






