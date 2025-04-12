# WW500 Oscilloscope Observations
#### CGP 6/4/25

## I2C Bus

Pulled up to 1V8.
SEN_SCL runs at 380kHz (2.64us period, equal mark-space ratio.)
SEN_SDA 
I can see I2C start and stop sequences.
I2C rise time c. 500ns
Waveforms look fine.
* So 1 register access is I2C address then 2 bytes address and 1 byte data.
* Each of 8 data bits plus an ack => 36 bits
* Plus terminal start and stop = 38 bits.
* This is c.24us per byte and c. 100us for a complete register write.

It seems that to read a register there are two separate I2C transfers:
* Send I2C address then 2 bytes address (3 bytes more or less, 75us)
* Send I2C address then 1 byte data read (2 bytes more or less, 50us, so total is c. 125us

#### I2C errors
Sometimes errors on I2C accesses. Sometimes I get this error:
`HM0360 reset workaround`
whcih suggests some other code that I don't have source of is managing HM0360 accesses. 

## Image Ouputs (earlier Himax code)

I monitored these signals:

* VSYNC - 	Runs continuously until DPD, when it goes low.
* HSYNC -   Runs while VSYNC is high until DPD, when it goes low.
* SEN_PCLK - Runs while HSYNC is high until DPD, when it goes low.

Mosty I saw these frequencies

| Signal       | Period         | High          | Low          | Notes                          |
| -------------|----------------|---------------|--------------|--------------------------------|
| VSYNC        | 23.7ms         | 21.4ms        | 2.3ms        | = 42Hz                         |
| HSYNC        | 44.0us         | 27.2us        | 17.8us       | 21400/44 = 486 say 480 lines   |
| SEN_PCLKO    | 42.4ns         | -             | -            | 24MHz 27200/42.4 = 641 say 640 pixels |

Code in the software reads frame count as the device takes images - there are about 121 frames between images.
Since there is a 3s time between each lot of image processing this corresponds to c. 40Hz frame rate.

However, if I remove and re-apply power sometimes everything happens at a different speed, such 
as inthe table below. The VSYNC period has varied from 27ms to 20ms

| Signal       | Period         | High          | Low          | Notes                          |
| -------------|----------------|---------------|--------------|--------------------------------|
| VSYNC        | 19.6ms         | 17.7ms        | 1.9ms        | = 51Hz                         |
| HSYNC        | 37.1us         | 22.4us        | 14.9us       |                                |
| SEN_PCLKO    | -              | -             | -            | Not measured. Calculated: 28MHz |                   |


## Image Ouputs (later Himax code)

the new code includes:
* Register settings in HM0360_stream_on[] for 10Hz
* Register settings in HM0360_md_stream_on[] for 2Hz
* Settings in `HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i`
  for Context A: "full-frame"
* Settings in `HM0360_OSC_Bayer_640x480_setA_VGA_setB_QVGA_md_8b_ParallelOutput_R2.i`
  for Context B: sub-sample by 2 in both H and V, so QVGA

The HM00360 is seen to run at two rates:
* Fast, while processor is awake, capturing images (10Hz, VGA)
* Slow, in DPD looking for motion detection (2Hz, QVGA)

The tables below show the timings in the 2 cases. This confirms th 
__Fast__
| Signal       | Period         | High          | Low          | Notes                          |
| -------------|----------------|---------------|--------------|--------------------------------|
| VSYNC        | 119ms          | 24ms          | 95ms         | = 8.4Hz                         |
| HSYNC        | 51.4us         | 31.2us        | 21.3us       | 24000/51.4 = 466 say 480 lines   |
| SEN_PCLKO    | 48ns           | -             | -            | 21MHz 31200/48 = 650 say 640 pixels |

__Slow__
| Signal       | Period         | High          | Low          | Notes                          |
| -------------|----------------|---------------|--------------|--------------------------------|
| VSYNC        | 584ms          | 12.2ms        | 570ms        | = 1.7Hz                         |
| HSYNC        | 51.4us         | 15.2us        | 35.4us       | 12200/51.4 = 237 say 240 lines   |
| SEN_PCLKO    | 48ns           | -             | -            | 21MHz 15200/48 = 314 say 320 pixels |



## WAKE signal to main()

The HM0360 SEN_INT signal is a 240us high pulse. I turn on an LED as early as I can in app_main().
The LED turns on 70ms later. So presumablye:
* The SEN_INT is cleared within the bootloader.
* It takes 70ms for the bootloader to execute.

I seem to get SEN_INT interrupts every frame

## MCLK Source

The initial measurements were taken with this:
```
hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_RC36M);
```
I then changed to this (as supplied by Himax):
```
hx_drv_dp_set_mclk_src(DP_MCLK_SRC_INTERNAL, DP_MCLK_SRC_INT_SEL_XTAL);
```
	
There is no change in I2C timing (makes sense since the I2C clock is sourced by the HX6538).
		
	
	
	