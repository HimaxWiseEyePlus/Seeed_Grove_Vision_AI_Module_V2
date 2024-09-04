## ww_template - CGP 14/8/24

This project is a fairly minimal project using FreeRTOS and a command line interface (CLI).

It can be used as the basis for other projects that use both of those.

Command Line Interface
-----------------------

Although the FreeRTOS_CLI.c & .h are readily available in FreeRTOS distributions, they are not
present in the Himax SDK. I have downloaded these files and added them to the project.
In due course it would be better if they could be put  in the FreeRTOS folders of the SDK,
though to do that properly the versions that match the distribution we are using should be located.

CLI-commands.c is also derived from a standard FreeRTOS example, but modified by me as follows:

- encapsulated the code in a FreeRTOS task.
- found a way to get the console UART to deliver characters and an interrupt.
- added some commands that send messages to Task1 and Task2, for testing of these tasks.

Tasks and State Machines
------------------------

There are two place-holder tasks: Task1 and Task2. These are ready to be replaced by real code.

The tasks are structured as follows:

After running one-off initialisation code, they sit forever in a loop waiting for messages to arrive in their queues.
At present the only messages are sent by the CLI, but in real code messages can be sent by interrupt-driven hardware events. 
(Actually, I set up Task1 to send one message to Task2, to show how it can be done).

Messages comprise two parts: an event identifier, and a 32-bit word of data, as shown by this structure:
```
typedef struct {
	APP_MSG_EVENT_E    msg_event;	/*!< MSG E */
	uint32_t           msg_data;	/*!< message data*/
} APP_MSG_T;
```

The msg_data can be an integer, or it can be a pointer to some other structure. 
The msg_event identifier is an integer value selected from values in app_msg.h (which is taken from a Himax scenario_app example).
I have added entries at the end of the list for the example Task1 and Task2. 
```
	// Messages directed to Task 1
	APP_MSG_TASK1_MSG0							=0x0500,
	APP_MSG_TASK1_MSG1							=0x0501,
	APP_MSG_TASK1_MSG2							=0x0502,

	// Messages directed to Task 2
	APP_MSG_TASK2_MSG0							=0x0600,
	APP_MSG_TASK2_MSG1							=0x0601,
	APP_MSG_TASK2_MSG2							=0x0602,
```

As the comment suggests, code that wants to send a message to a task should prepare an
APP_MSG_T message then place it in the task's queue, like this:
```
	APP_MSG_T task2_send_msg;
	task2_send_msg.msg_data = someValue;			// some value
	task2_send_msg.msg_event = APP_MSG_TASK2_MSG0;	// some event identifier
	if(xQueueSend( xTask2Queue , (void *) &task2_send_msg , __QueueSendTicksToWait) != pdTRUE) {...}
```

The APP_MSG_T data and event duely appear in the task's queue, and the task can then process these and act as appropriate.

I have structured Task2 to respond to the message depending on the event identifier, like this:
```    	
	switch(event)  {
			case APP_MSG_TASK2_MSG0:
				// Do something
				break;

			case APP_MSG_TASK2_MSG1:
				// Do something
				break;

			case APP_MSG_TASK2_MSG2:
				// Do something
				break;

			default:
				// Do something
				break;
            }
```
 This is OK in very simple cases, but usually what needs to be done will depend on the current state of the state machine,
 so each case of the switch command would have if-then-else statements.
 
 In contrast, Task1 uses the approach I prefer, in which:
 - Processing of the message is performed depending on the state machine state.
 - The processing steps are hived off to separate functions, to keep the main switch statement simpler
 ```    	
    		switch (task1_state) {

    		case APP_TASK1_STATE_UNINIT:
    			txMessage = flagUnexpectedEvent(rxMessage);
    			break;

    		case APP_TASK1_STATE_INIT:
    			txMessage = handleEventForInit(rxMessage);
    			break;

    		case APP_TASK1_STATE_2:
    			txMessage = handleEventForState2(rxMessage);
    			break;

    		case APP_TASK1_STATE_3:
    			txMessage = handleEventForState3(rxMessage);
    			break;

    		case APP_TASK1_STATE_ERROR:
    			txMessage = handleEventForError(rxMessage);
    			break;

    		default:
    			// should not happen
    			txMessage = flagUnexpectedEvent(event);
    			break;	
    		}
 ```       		

Note the 'txMessage = " in this code. Each of the event handling functions can return a structure in txMessage.
If it is non-null then further code can send a message on to a further task. The code has one example of this as an example:
When handleEventForState2() receives event APP_MSG_TASK1_MSG1 it constructs a message with a magic number and sends it to Task2.
Task2 looks out for that and prints an acknowledgment if it arrives.

Next Steps
-----------
I will probably leave this project as it is. I will clone it, and then I am likely to make these enhancements:
- Add I2C slave code from 'ww130_test'. I think it should be possible to route messages from the phone and WW130
to the CLI task so that commands can be issued from either the phone or the Seeed board console.
- Place the SD card FatFS into a task of its own.
- Add some of the Himax image processing code - such as that used in 'ww130_test'. The idea is if this is done well, then the
result will be cleaner and easier to maintain and extend, than the 'ww130_test' project.  


