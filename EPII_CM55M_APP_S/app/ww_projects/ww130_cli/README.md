## ww_cli - CGP 1/9/24

This project is derived from ww_template. See the readme.md in that project folder.

This project seeks to make these enhancements:
- Add I2C slave code from 'ww130_test'. Route messages from the phone and WW130 to the CLI task so that commands 
can be issued from either the phone or the Seeed board console.
- Place the SD card FatFS into a task of its own.
- Add some of the Himax image processing code - such as that used in 'ww130_test'. The idea is if this is done well, then the
result will be cleaner and easier to maintain and extend, than the 'ww130_test' project.  

(As of 1/9/24 - basic functionality on the first of those 2 objectives, none on the third).

To use this project with the WW130, make sure the WW130 has the same firmware in the ww130_firmware directory

Refactor CLI-commands.c
-----------------------
I modified this so that the FreeRTOS waits on something arriving in its queue, rather than waiting on a semaphore.
That is a prerequisite to also receiving commands from the I2C port. Also:
- Added a facility to process command strings from the phone (or WW130). These arrive as messages from the if_task. 
So the same command can be issued from the console or from the phone.
- Added more commands such as 'status', 'enable', 'disable', 'int nnn' to allow more realistic testing of commands from 
the phone.
- Added facility for the FatFS (SD card) task to also define CLI commands, such as 'dir', 'cd', mkdir', 'copy' etc.
These do not use FreeRTOS messages, sent to the FatFS task. 
- Added 'filewrite' and 'fileread' commands which read and write files of 1024 bytes. 
These use FreeRTOS messages, sent to the FatFS task, so they are representative of how a readl application might write JPEG files etc. 

Add I2C slave code from 'ww130_test'
------------------------------------
I replaced task2.c & .h with if_task.c & .h. The idea is that this task handles all interfaces with the WW130;
- Implemented receiving text messages from the WW130 via the I2C interface, and returning a response.
- Added code that notes incoming interrupts on the PA0 pin: at present the user has to press SW2 briefly. 
- I added code to this PA0 event that writes a short test file then reads it (on alternating button presses). 
These use FreeRTOS messages, sent to the FatFS task. 
- Added code that allows the Seeed board to interrupt the WW130 by taking PA0 low. This is timer-operated and in practise,
at the moment, the interval has to be > 100ms for the WW130 to notice.
- Still to do - sending and receiving longer messages, which will be required for sending metadata including thumbnail images.

FatFs
------
The SD Card (FatFs) code is in its own task. This is being implemented in the fatfs_task.c & .h.
The idea is that SD card operations can be invoked by sending messages from other task to fatfs_task.
I am also implementing CLI-FATFS-commands.c & .h which can provide a CLI interface to the fatfs_task.
In that way, during development, you can type commands at a keyboard and implement disk operations, 
such as 'dir', 'cd', mkdir', 'copy' etc. 

At present file operations are asynchronus. That is, one task sends a file operation request to the FatFS task.
Later, when the command is complete, the FatFs tasks returns the result of the operation. Check out the fileOperation_t
structure which is used for this.

At present only one disk operation is permitted at once.

Still to do
--------------
- Cleaning up debug messages
- Adding better error handling, especially related to unhandled events.
- FatFS - enahnce this so other tasks can request meaningful actions. Also build out the CLI-FATFS-commands.c & .h 
- Image capture. Not implemented at all. Some part of ww130_test needs to be brought across BUT IMPORTANTLY!!! this
needs to be cleaned up and using the same event and state machine structure as our other tasks.
- Allow long messages to be sent to the WW130 (such as meta data), broken into a sequence of I2C messages.
- Get the WW130 to respond to short pulses on PA0.
- Allow for multiple disk operations to run concurrently?

But should be OK to play with now.
