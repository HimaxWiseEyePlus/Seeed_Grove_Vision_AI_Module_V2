## Notes on i2c_slave_app - CGP 7/8/24

This was based on sample code received from Himax on 28/6/24, in response to a request for an example
of using the I2C slave.

I made some changes - most of which are bracketed by this switch:
'''
#define CGP_MODS
'''

More important changes are:
- This works with the WW130 project 'twi_master_seeed' and allows data to be transferred both ways.
- Added code to print data buffers for debugging
- Added code to use PA0 as a bi-directional interrupt.




