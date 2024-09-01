## I2C vs I3C on HX6538 - CGP 1/9/24

I wasted a load of time trying to get the I2C communications working between the Seeed board (I2C slave) and the WW130
(using nRF52832 as I2C master). 

I had this working correctly with my ww130_test app. But I could not make it work with the ww130_cli application.
The hardware was identical and as far as I could see the software had the same code (based on the I2C comms library in i2c_comm.h,
using hx_lib_i2ccomm_init() etc). But the WW130 (I2C master) gave errors showing that the Seeed board was not responding by 
asserting the ACK.

It took a long time to find but the answer is in the drivers that are selected by makefiles.
The ww130_cli project, which was (probably) derived by copying hello_world_freertos_tz_s_only, includes this in drv_user_defined.mk:

```
DRIVERS_IP_INSTANCE  ?= RTC0 \
		<snip>
		IIIC_SLAVE0 \
		IIIC_SLAVE1 \
```
Whereas ww130_test, derived from allon_sensor_tflm_freertos, has this in drv_onecore_cm55m_s.mk:
```
DRIVERS_IP_INSTANCE  ?= RTC0 \
		<snip>
		IIC_SLAVE0 \
		IIC_SLAVE1 \
```

Both versions will compile but only the IIC version will run.

Apparently I2C and I3C are both available in the HX6538. I3C is capable of higher speed operation, but this is not supported by
the nRF52832 (which is the I2C master) so there is no point in trying to get I3C working.

It would be good if Himax could document this issue, and add a compiler error if you try to use the configuration that does not work.

