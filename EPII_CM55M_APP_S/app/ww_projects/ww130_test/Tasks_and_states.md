## Tasks and States - CGP 3/8/24

The app uses four FreeRTOS states, which are created at reset in app_main(). 
These communicate by passing messages to each other - they write to their queues.
The tasks sleep until something appears in their queue.

Having worked with this for some time, I would like to see the state machines re-written in the way I am using
on the WW130 code. See notes at the end.

Four FreeRTOS tasks:
-----------------------
	main_task
	dp_task
	algo_task
	comm_task
	
Each tasks has its own set of states (see app_state.h). The states can change as messages arrive in the queues and are processed.
Each task executes some initialisation code after reset (including warm boot after sleeping). 
They then wait for evensts to arrive in their input queues. Events can come from interrupt events, or as messages passed from other tasks.
	
main_task
-----------
Init: determines warm or cold boot, initialises NN engine, FATFS and sensor. 
Initialises vision sensor before entering its queue loop.
When the frame capture event arrives it wakes the algo_task. 
When the algo_task has finished its NN processing it informs the main_task which puts the board to sleep.


dp_task
----------
Init: only initialises state.
Looks like this has a callback from interrupts: os_app_dplib_cb()
This receives interrupt events and converts them to events in the task's queue.

 Messages are:
 	APP_MSG_DPEVENT_XDMA_FRAME_READY
 	- it prints stuff then informs the main_task
 	APP_MSG_DPEVENT_RECAPTURE
 	- calls sensordplib_retrigger_capture() - to capture another image?

algo_task
-----------
Init: FATFS and sensor. 
Responsible for the NN processing - see cv_run()
It looks like it also prepares a JPEG image and sends it (via SPI)
This includes code that can write images to the SD card - if SUPPORT_FATFS is set to 1.	

comm_task
-------------
Init: PA0 interrupt and I2C comms.

This handles the PA0 interrupt (generates a APP_MSG_COMMEVENT_AON_GPIO0_INT event)
I modified aon_gpio0_interupt_init() to match what I had done earlier, to get the PA0 interrupt working with the WW130.
This pin now serves to wake the processor when it is sleeping.

The task looks out for events such as:
	The PA0 interrupt goes to main_task - where commented out code seems to stop image capturing.
	I2C slave RX, TX and error events

## Typical Sequence
	
The following is typical sequence as images are grabbed and processed:

- dp_task receives APP_MSG_DPEVENT_XDMA_FRAME_READY and notifies main_task
- main_task receives APP_MSG_MAINEVENT_CAP_FRAME_DONE and notifies algo_task
- algo_task receives APP_MSG_VISIONALGOEVENT_START_ALGO then:

	- reads info about the jpeg image
	- writes the jpeg to the SD card
	- invokes the NN engine which prints the NN outout
	- notifies main_task

- main_task receives APP_MSG_MAINEVENT_VISIONALGO_STARTDONE and either:

	- sends APP_MSG_DPEVENT_RECAPTURE to dp_task
	- sleeps
 	
- if dp_task receives APP_MSG_DPEVENT_RECAPTURE	and initiates another frame capture
	
## Re-writing state machines

I think we should adopt the pattern I am using with the WW130 which will make for much more readable
and reliable code. The general pattern is:

- An event arrives in the task's queue.
- An optional debug message prints the event (as a string!) and the current state (as a string!)
- The current state is recorded
- There is then a switch statement based on the current state. Each case statement calls its own function,
 with the event as a parameter. The processing of the event takes palce in those sepaarte functions. These functions might cause a state change.
 - after the switch statement a debug function prints the new state (if the state has changed).
 - Some post-processing operations might happen.
 - In fact, it would be cleaner if any messages sent to a different task were handled by a single function at the end
 
Pseudo-code would look something like this:

```
for(;;) {
  xQueueReceive(event);
  oldState = currentState;
  txMessage = NULL;
  taskHandle = NULL;
  reportEventAndCurrentState(event, currentState);	// debug message
  switch (currentState) {
    case STATE0:
    	processEventForState0(event);	// might change currentState and set txMessage, taskHandle
    	break;
    	
   case STATE1:
     	processEventForState1(event);	// might change currentState and set txMessage, taskHandle
    	break;

	default:
	   	// should never happen
	}
	reportNewState(oldState, currentState);	// debug message   	
	anyCommonProcessing();				
	// This might be to call xQueueSend() to send a message to a different task.
	sendMessageToOtherTask(taskHandle, txMessage);
	// Now wait for the next event
}
```
 

	

	