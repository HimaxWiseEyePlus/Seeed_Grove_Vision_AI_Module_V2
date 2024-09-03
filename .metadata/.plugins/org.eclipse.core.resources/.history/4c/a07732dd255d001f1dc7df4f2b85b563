## ww_cli - CGP 18/8/24

This project is derived from ww_template. See the readme.md in that project folder.

This project seeks to make these enhancements:
- Add I2C slave code from 'ww130_test'. Route messages from the phone and WW130 to the CLI task so that commands 
can be issued from either the phone or the Seeed board console.
- Place the SD card FatFS into a task of its own.
- Add some of the Himax image processing code - such as that used in 'ww130_test'. The idea is if this is done well, then the
result will be cleaner and easier to maintain and extend, than the 'ww130_test' project.  

To use this project with the WW130, make sure the WW130 has the same firmware as in the ww130_test project.
I am aware that this is not returning messages all teh way to the phone, but they can be seen arriving on the WW130 console.
I will fix this soon.

Refactor CLI-commands.c
-----------------------
I modified this so that the FreeRTOS waits on something arriving in its queue, rather than waiting on a semaphore.
That is a prerequisite to also receiving commands from the I2C port. Also:
- Added a facility to process command strings from the phone (or WW130). These arrive as messages from the if_task.
- Added more commands such as 'status', 'enable', 'disable', 'int nnn' to allow more realistic testing of commands from the phone.

Add I2C slave code from 'ww130_test'
------------------------------------
I replaced task2.c & .h with if_task.c & .h. The idea is that this task handles all interfaces with the WW130;
- Implemented receiving text messages from the WW130 via the I2C interface, and returning a response.
- Added code that notes incoming interrupts on the PA0 pin: at present the user has to press SW2 briefly, 
and at present the Seeed board only prints a message.
- Added code that allows the Seeed board to interrupt the WW130 by taking PA0 low. This is timer-operated and in practise,
at the moment, the interval has to be > 100ms for the WW130 to notice.

Still to do
--------------
- Cleaning up debug messages
- Adding better error handling, especially related to unhandled events.
- FatFS 
- Image capture
- Allow long messages to be sent to the WW130 (such as meta data), broken into a sequence of I2C messages.


