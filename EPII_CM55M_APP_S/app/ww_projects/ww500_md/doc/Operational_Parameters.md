# WW500_MD Configuration File
#### CGP 21/6/25

The WW500_MD code supports reading "Operational Parameters" from a file on the SD card.
The file name can be set in the source code and at the time of writing is ` 'configuration.txt'`.
This allows certain parameters to be set by editing a file (or sending values from the app)
instead of having to recompile the code to make the changes.

## Operational Parameters
The file can be used to set initial values into the `op_parameter[]` array which is located in
`fatfs_task.c`. At the time of writing this array has 10 entries, but can easily be extended as further 
parameters are required.

The index into the array are these constants which are defined in `fatfs_task.h`:

```
// Operational parameters to get/set.
// Typically the values are saved to SD card before entering DPD
// OP_PARAMETER_NUM_ENTRIES is only used to establish the number of entries
typedef enum {
	OP_PARAMETER_SEQUENCE_NUMBER,	// 0 Image file number
	OP_PARAMETER_NUM_PICTURES,		// 1 Num pics when triggered
	OP_PARAMETER_PICTURE_INTERVAL,	// 2 Pic interval when triggered (ms)
	OP_PARAMETER_TIMELAPSE_INTERVAL,// 3 Interval (s) (0 inhibits)
	OP_PARAMETER_INTERVAL_BEFORE_DPD, // 4 Delay before DPD (ms)
	OP_PARAMETER_LED_FLASH_DUTY,	// 5 in percent
	OP_PARAMETER_NUM_NN_ANALYSES,	// 6 # times the NN model has run
	OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES,	// 7 # times the NN model says "yes"
	OP_PARAMETER_NUM_COLD_BOOTS,	// 8 # of AI processor cold boots
	OP_PARAMETER_NUM_WARM_BOOTS,	// 9 # of AI processor warm boots
	OP_PARAMETER_NUM_ENTRIES		// Count of entries above here
} OP_PARAMETERS_E;
```

Thus `op_parameter[OP_PARAMETER_SEQUENCE_NUMBER]` is the same as `op_parameter[0]` etc.

There are two "getters and setters" and an incrementing function that can be called by other functions:

```
// Get one of the Operational Parameters
int32_t fatfs_getOperationalParameter(OP_PARAMETERS_E parameter);

// Set one of the Operational parameters
void fatfs_setOperationalParameter(OP_PARAMETERS_E parameter, int32_t value);

// Increment one of the Operational Parameters
void fatfs_incrementOperationalParameter(OP_PARAMETERS_E parameter);
```

If the ` 'configuration.txt'` file is missing then each of the `op_parameter[]` array values is 
initialised by constants (in `vFatFsTask()`). Otherwise the `op_parameter[]` values are loaded from the file.

## Loading and Saving the Configuration File

The Operational Parameters are loaded from file when the FatFS task starts. This happens shortly after any reset,
including the reset that comes from leaving the deep power-down (DPD) state.

The Operational Parameters are saved to the file just before the AI processor enters DPD.
Thus the values are saved in the file when the AI processor loses all of it state, in DPD.

Since some of the Operational Parameter values are likely to have changed, the changed "current" values are
restored when the AI processor exits DPD (or any other reset).

If for some reason the AI processor is reset before entering DPD in the normal manner, the configuration file is not written.
This might give rise to incorrect data in the Operational Parameters. 

## Operational Parameters likely to change.

At present, the following parameters are likely to change (be incremented) in normal operation:

1. `op_parameter[OP_PARAMETER_SEQUENCE_NUMBER]` - increments every time an image is saved to SD card
2. `op_parameter[OP_PARAMETER_NUM_NN_ANALYSES]` - increments every time the neural network analyses an image.
3. `op_parameter[OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES]` - increments every time the neural network produces a "postive" analysis.
4. `op_parameter[OP_PARAMETER_NUM_COLD_BOOTS]` - increments every time the AI processor does a cold boot
5. `op_parameter[OP_PARAMETER_NUM_WARM_BOOTS]` - increments every time the AI processor does a warm boot

These values might be sent in LoRaWAN uplink packets.

## Changing Operational Parameters by the App

There are CLI functions which read and write Operational Parameters. Because any CLI command can be run by the app,
when connected, this means that there is essentially a "getter" and a "setter" available to the app.

1. To read an Operational Parameter, type `getop <index>` e.g. `getop 3`.
This returns the integer value of the selected Operational Parameter.
2. To set an Operational Parameter, type `setop <index> <value>` e.g. `setop 3 60`.
This sets the selected Operational Parameter to a new value.

The corresponding commands issued by the app are:
1. `AI getop <index>`
2. `AI setop <index> <value>`

## File Format

File entries take the form of two integers on a line. For example:
```
1 2
2 2000
```
In that example, index 1 (`OP_PARAMETER_NUM_PICTURES`) is set to 2 (the number of images to take).
Index 2 (`OP_PARAMETER_PICTURE_INTERVAL`) is set to 2000 (the number of milliseconds between these images).

Operational Parameters which are not present in the file are given their default values.

Comments can be added to the configuration.txt file. A comment is a line that starts with '#'.
There are limits to the number of comments and their length).
Those comments preceding the first index/value pair are preserved when the file is saved at DPD.

I added the comment feature mainly so I could include the `OP_PARAMETERS_E` enum for reference.

## Detailed behaviour of the Operational Parameters

| Index | Name                       | Behaviour                                             |
|---|---|----------------------------| ------------------------------------------------------|
| 0 | OP_PARAMETER_SEQUENCE_NUMBER   | Used as part of the image file name. Increments when the file is written. |
| 1 | OP_PARAMETER_NUM_PICTURES      | The number of images to capture each time the processor receives a motion detect event or a time lapse event. |
| 2 | OP_PARAMETER_PICTURE_INTERVAL  | The interval (in ms) between each of the above images. Limited to about 2000 for HM0360 |
| 3 | OP_PARAMETER_TIMELAPSE_INTERVAL| The interval (in s) between entering DPD and waking again to take the next timelapse image. |
| 4 | OP_PARAMETER_INTERVAL_BEFORE_DPD | The interval (in ms) between when all FreeRTOS task activity ceases and the AI processor entering DPD.|
| 5 | OP_PARAMETER_LED_FLASH_DUTY    | Flash LED duty cycle (brightness). Awaiting development! |
| 6 | OP_PARAMETER_NUM_NN_ANALYSES   | The number of times the neural network model has run. |
| 7 | OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES| The number of times the neural network model detects the target. (This will work for binary analysis only). |
| 8 | OP_PARAMETER_NUM_COLD_BOOTS    | The number of cold boots. |
| 9 | OP_PARAMETER_NUM_WARM_BOOTS    | The number of warm boots. |

