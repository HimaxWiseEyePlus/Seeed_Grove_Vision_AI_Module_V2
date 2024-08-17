## ww_cli - CGP 14/8/24

This project is derived from ww_template. See the readme.md in that project folder.

This project seeks to make these enhancements:
- Add I2C slave code from 'ww130_test'. I think it should be possible to route messages from the phone and WW130
to the CLI task so that commands can be issued from either the phone or the Seeed board console.
- Place the SD card FatFS into a task of its own.
- Add some of the Himax image processing code - such as that used in 'ww130_test'. The idea is if this is done well, then the
result will be cleaner and easier to maintain and extend, than the 'ww130_test' project.  

Refactor CLI-commands.c
-----------------------
I modified this so that the FreeRTOS waits on something arriving in its queue, rather than waiting on a semaphore.
That is a prerequisite to also receiving commands from the I2C port.

Add I2C slave code from 'ww130_test'
------------------------------------
I replaced task2.c & .h with if_task.c & .h. The idea is that this task handles all interfaces with the WW130.


