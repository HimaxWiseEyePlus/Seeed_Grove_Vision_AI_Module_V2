# WW500 Operational Parameters
#### CGP - 25 November 2025

The WW500_MD code supports reading "Operational Parameters" from a file on the SD card.
The file name can be set in the source code and at the time of writing is `CONFIG.TXT`.
This allows certain parameters to be set by editing a file (or sending values from the app)
instead of having to recompile the code to make the changes.

## Operational Parameters

The CONFIG.TXT file can be used to set initial values into the `op_parameter[]` array which is located in
`fatfs_task.c`. At the time of writing this array has 14 entries, but can easily be extended as further 
parameters are required.

The index into the array are these constants which are defined by the `OP_PARAMETERS_E` enum in `fatfs_task.h`:

```
/*
 * This enum enumerates the index numbers of the Operational Parameters array, `op_parameter[]`
 *  
 */
typedef enum {
	OP_PARAMETER_SEQUENCE_NUMBER,	// 0 Image file number. Used as part of the image file name. Increments when the file is written.
	OP_PARAMETER_NUM_NN_ANALYSES,	// 1 he number of times the neural network model has run.
	OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES,	// 2 The number of times the neural network model detects the target.
	OP_PARAMETER_NUM_COLD_BOOTS,	// 3 The number of AI processor cold boots.
	OP_PARAMETER_NUM_WARM_BOOTS,	// 4 The number of AI processor warm boots.
	OP_PARAMETER_NUM_PICTURES,		// 5 The number of images to capture each time the processor receives a motion detect event or a time lapse event.
	OP_PARAMETER_PICTURE_INTERVAL,	// 6 The interval (in ms) between each of the above images. Limited to about 2000 for HM0360
	OP_PARAMETER_TIMELAPSE_INTERVAL,// 7 The interval (in s) between entering DPD and waking again to take the next timelapse image (0 inhibits)
	OP_PARAMETER_INTERVAL_BEFORE_DPD, // 8 The interval (in ms) between when all FreeRTOS task activity ceases and the AI processor entering DPD.
	OP_PARAMETER_LED_BRIGHTNESS_PERCENT,	// 9 Flash LED duty cycle (brightness) in percent (0 inhibits LED flash)
	OP_PARAMETER_CAMERA_ENABLED,	// 10 0 = Camera and NN system disabled, 1 = Camera and NN system enabled 
	OP_PARAMETER_MD_INTERVAL,		// 11 Interval (ms) between frames in motion detect mode (0 inhibits motion detection) 
	OP_PARAMETER_FLASH_DURATION,	// 12 Duration (ms) that LED flash is on
	OP_PARAMETER_FLASH_LED,			// 13 LED bit mask: visible LED used = 1, infra-red LED used =2, none = 0
	OP_PARAMETER_NUM_ENTRIES		// Not an Operational Parameters - serves to count the of entries above here
} OP_PARAMETERS_E;
```
OP_PARAMETER_NUM_ENTRIES is only used to establish the number of entries.

Thus `op_parameter[OP_PARAMETER_SEQUENCE_NUMBER]` is the same as `op_parameter[0]` etc.

#### Setting Values

The `op_parameter[]` array is initialised with default values in `fatsfs_task.c`

Shortly after the AI processor boots (either a cold boot or a warm boot) the `CONFIG.TXT` file is read and 
and values present in the file are used to over-write the default values.

Some values are incremented automatically as the program runs.

The app can over-write (or update) values.

#### Saving values to CONFIG.TXT

The entire `op_parameter[]` array is written to `CONFIG.TXT` on the SD card before the AI processor enters DPD. 
Thus the `CONFIG.TXT` is used to save state during DPD, as the contents of the `op_parameter[]` array would otherwsie be lost.

If for some reason the AI processor is reset before entering DPD in the normal manner, the configuration file is not written.
This might give rise to incorrect data in the Operational Parameters. (Should not happen in normal operation). 

#### Programmatic Access

There are two "getters and setters" and an incrementing function that can be called by other functions:

```
// Get one of the Operational Parameters
int32_t fatfs_getOperationalParameter(OP_PARAMETERS_E parameter);

// Set one of the Operational parameters
void fatfs_setOperationalParameter(OP_PARAMETERS_E parameter, int32_t value);

// Increment one of the Operational Parameters
void fatfs_incrementOperationalParameter(OP_PARAMETERS_E parameter);
```

## Operational Parameters likely to change autonomously.

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

Entries in CONFIG.TXT are mainly lines with two numbers:
* Index
* Value

Lines starting with '#' are treated as comments and are ignored.

For example two lines might be:
```
5 2
6 500
```

In this example:
* '5' is the index and '2' is the value
* '6' is the index and '500' is the value

From the table below, 5 is the index for `OP_PARAMETER_NUM_PICTURES` and the value 2 means the WW500 takes 2 
pictures when triggered. 

From the table below, 6 is the index for `OP_PARAMETER_PICTURE_INTERVAL` and the value 500 means 
the pictured are taken at 500ms intervals.

__The Operational Parameters are stored in 16-bit integers so must be in the range 0-65535. If incremented beyond
65535 the value wraps around (i.e. from 65535 to 0).__

Operational Parameters which are not present in the file are given their default values at warm boot or cold boot..

Comments can be added to the CONFIG.TXT file. A comment is a line that starts with '#'.
There are limits to the number of comments and their length.
Those comments preceding the first index/value pair are preserved when the file is saved at DPD.

## Operational Parameters Table

| Index | Name                                  | Default Value | Notes                                                |
| ----- | ------------------------------------- | ------------- | ---------------------------------------------------- |
|     0 | OP_PARAMETER_SEQUENCE_NUMBER          | 0             | Image file number. Used as part of the image file name. Increments when the file is written.  |
|     1 | OP_PARAMETER_NUM_NN_ANALYSES          | 0             | The number of times the neural network model has run. |
|     2 | OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES | 0             | The number of times the neural network model detects the target. |
|     3 | OP_PARAMETER_NUM_COLD_BOOTS           | 0             | The number of AI processor cold boots.  |
|     4 | OP_PARAMETER_NUM_WARM_BOOTS           | 0             | The number of AI processor warm boots. |
|     5 | OP_PARAMETER_NUM_PICTURES             | 3             | The number of images to capture each time the processor receives a motion detect event or a time lapse event. |
|     6 | OP_PARAMETER_PICTURE_INTERVAL         | 1500          | The interval (in ms) between each of the above images. Limited to about 2000 for HM0360 |
|     7 | OP_PARAMETER_TIMELAPSE_INTERVAL       | 60            | The interval (in s) between entering DPD and waking again to take the next timelapse image (0 inhibits) |
|     8 | OP_PARAMETER_INTERVAL_BEFORE_DPD      | 10000         | The interval (in ms) between when all FreeRTOS task activity ceases and the AI processor entering DPD.|
|     9 | OP_PARAMETER_LED_BRIGHTNESS_PERCENT   | 5             | Flash LED duty cycle (brightness) in percent (0 inhibits LED flash) |
|    10 | OP_PARAMETER_CAMERA_ENABLED           | 1             | Camera and NN system disabled, 1 = Camera and NN system enabled |
|    11 | OP_PARAMETER_MD_INTERVAL              | 1000          | Interval (ms) between frames in motion detect mode (0 inhibits motion detection)|
|    12 | OP_PARAMETER_FLASH_DURATION           | 100           | Duration (ms) that LED flash is on                  |
|    13 | OP_PARAMETER_FLASH_LED                | 0             | LED bit mask: visible LED used = 1, infra-red LED used =2, none = 0              |


## Syncronisation with BLE Processor Code

The values are passed periodically from the AI processor to the BLE processor and can be used by that code. 

Therefore when changes are made to the `OP_PARAMETERS_E` enumeration for the AI processor, the same change must be
 made in the BLE processor code. This is done in the `aiProcessor.h` file.
 
## Operational Parameters Detailed Description

(The detailed description may follow in due course.)

#### OP_PARAMETER_SEQUENCE_NUMBER
Detailed description to follow

#### OP_PARAMETER_NUM_NN_ANALYSES
Detailed description to follow

#### OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES
Detailed description to follow

#### OP_PARAMETER_NUM_COLD_BOOTS
Detailed description to follow

#### OP_PARAMETER_NUM_WARM_BOOTS
Detailed description to follow

#### OP_PARAMETER_NUM_PICTURES
Detailed description to follow

#### OP_PARAMETER_PICTURE_INTERVAL
Detailed description to follow

#### OP_PARAMETER_TIMELAPSE_INTERVAL
Detailed description to follow

#### OP_PARAMETER_INTERVAL_BEFORE_DPD
Detailed description to follow

#### OP_PARAMETER_LED_BRIGHTNESS_PERCENT
Detailed description to follow

#### OP_PARAMETER_CAMERA_ENABLED
Detailed description to follow

#### OP_PARAMETER_MD_INTERVAL
Detailed description to follow

#### OP_PARAMETER_FLASH_DURATION
Detailed description to follow

#### OP_PARAMETER_FLASH_LED
Detailed description to follow


## Possible Additional Operational Parameters

Preliminary discussions between developers suggest that further operational parameters should be added,
to further control the operation of the WW500. These might include:

```
	OP_PARAMETER_NN_THRESHOLD		// NN output value (percentage) that determines of an image is saved to SD card
	OP_PARAMETER_IMAGE_RESOLUTION	// Value determines the size of image saved to SD card  (options t.b.d. e.g. 640x480, 1280x960)
	OP_PARAMETER_NN_INDEX			// Index of NN model to deploy (0 means no NN processing).
	OP_PARAMETER_NN_NUM_CHANNELS	// Number of channels used in the NN model (e.g. 3 for RGB)
	OP_PARAMETER_NN_X_RESOLUTION	// Camera image is scaled to this X resolution to provide to the NN
	OP_PARAMETER_NN_Y_RESOLUTION	// Camera image is scaled to this Y resolution to provide to the NN
```	


