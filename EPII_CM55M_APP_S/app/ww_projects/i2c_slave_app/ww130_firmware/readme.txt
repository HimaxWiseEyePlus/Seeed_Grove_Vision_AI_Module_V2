TWI Master to Seeed - CGP 3/7/24
=====================================

This project was to test I2C master and slave operation, using one WW130.B00 as an I2C master and the Seeed Vision AI V2
as the I2C slave.

It is derived from the project 'twi_master_slave'. See the readme.txt in that for background on getting I2C working/

I striped out all the code (from 'twi_master_slave') related to the WW130 running as a slave.

I then modified the I2C protocol so it passes data bytes and does not insert the two-byte address
that is used in the EEPROM emulation in twi_master_slave.

The readme.txt for twi_master_slave reveals an unexpected set of sdk_config.h settings.
To establish the minimum working set I have changed this to:

sdk_config.h:
	NRFX_TWIM_ENABLED disabled, and instance 0 & 1 disabled	
	NRFX_TWIS_ENABLED disabled, and instance 0 & 1 disabled	
	NRFX_TWI_ENABLED (but instance 0 & 1 disabled!)
	TWIS_ENABLED (but instance 0 & 1 disabled!)
	TWI_ENABLED (instance 0 enabled, instance 1 disabled)

That seems to mean: only the (legacy) TWI_ENABLED driver needs to be present in sdk_config.h

Makefile: removed:
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twis.c \
  
Leaving:
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/integration/nrfx/legacy \
	