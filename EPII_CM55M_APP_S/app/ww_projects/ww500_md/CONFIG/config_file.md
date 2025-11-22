# Description of the CONFIG.TXT File
#### CGP 22 November 2025

The CONFIG.TXT file contains "Operational Parameters" for the WW500.

Entries in that file have two numbers:
* Index
* Value

Lines starting with '#' are treated as comments and are ignored.

The index numbers can be obtained from the `OP_PARAMETERS_E` enumeration in the `fatfs_task.h` file. This is shown below (current at the time of writing):

```
/*
 * This enum enumerates the index numbers of the Operational Parameters array, `op_parameter[]`
 *  
 */
typedef enum {
	OP_PARAMETER_SEQUENCE_NUMBER,	// 0 Image file number
	OP_PARAMETER_NUM_NN_ANALYSES,	// 1 # times the NN model has run
	OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES,	// 2 # times the NN model says "yes"
	OP_PARAMETER_NUM_COLD_BOOTS,	// 3 # of AI processor cold boots
	OP_PARAMETER_NUM_WARM_BOOTS,	// 4 # of AI processor warm boots
	OP_PARAMETER_NUM_PICTURES,		// 5 Num pics when triggered
	OP_PARAMETER_PICTURE_INTERVAL,	// 6 Pic interval when triggered (ms)
	OP_PARAMETER_TIMELAPSE_INTERVAL,// 7 Interval (s) (0 inhibits)
	OP_PARAMETER_INTERVAL_BEFORE_DPD, // 8 Delay before DPD (ms)
	OP_PARAMETER_LED_BRIGHTNESS_PERCENT,	// 9 in percent (0 inhibits)
	OP_PARAMETER_CAMERA_ENABLED,	// 10 0 = disabled, 1 = enabled
	OP_PARAMETER_MD_INTERVAL,		// 11 Interval (ms) between frames in MD mode (0 inhibits)
	OP_PARAMETER_FLASH_DURATION,	// 12 Duration (ms) that LED Flash is on
	OP_PARAMETER_FLASH_LED,			// 13 LED bit mask: vis=1, IR=2, none=0)
	OP_PARAMETER_NUM_ENTRIES		// Count of entries above here
} OP_PARAMETERS_E;
```
OP_PARAMETER_NUM_ENTRIES is only used to establish the number of entries.

The `op_parameter[]` array is initialised with default values in the `fatfs_task.h` file.
If the `CONFIG.TXT` file is present the the file is read by the AI processor ever time it does a hard boot or a warm boot. Any values in that file will over-write the default values.

Some of the `op_parameter[]` values are likely to chnage while the AI processor is awake. All the values are re-written to the `CONFIG.TXT` file just before the AI processor enters deep power down (DPD).

The values are passed periodically from the AI processor to the BLE processor and can be used by that code. Therefore when chnages are made to the `OP_PARAMETERS_E` enumeration for the AI processor, the same change must be made in the BLE processor code. This is
done in the `aiProcessor.h` file.

## Operational Parameters as a Table

| Index | Name                                  | Default Value | Notes                                                |
| ----- | ------------------------------------- | ------------- | ---------------------------------------------------- |
|     0 | OP_PARAMETER_SEQUENCE_NUMBER          | 0             | Image file number                                    |
|     1 | OP_PARAMETER_NUM_NN_ANALYSES          | 0             | # times the NN model has run                         |
|     2 | OP_PARAMETER_NUM_POSITIVE_NN_ANALYSES | 0             | # times the NN model says "yes"                      |
|     3 | OP_PARAMETER_NUM_COLD_BOOTS           | 0             | # of AI processor cold boots                         |
|     4 | OP_PARAMETER_NUM_WARM_BOOTS           | 0             | # of AI processor warm boots                         |
|     5 | OP_PARAMETER_NUM_PICTURES             | 3             | Num pics when triggered                              |
|     6 | OP_PARAMETER_PICTURE_INTERVAL         | 1500          | Pic interval when triggered (ms)                     |
|     7 | OP_PARAMETER_TIMELAPSE_INTERVAL       | 60            | Interval (s) (0 inhibits)                            |
|     8 | OP_PARAMETER_INTERVAL_BEFORE_DPD      | 10000         | Delay before DPD (ms)                                |
|     9 | OP_PARAMETER_LED_BRIGHTNESS_PERCENT   | 5             | In percent (0 inhibits)                              |
|    10 | OP_PARAMETER_CAMERA_ENABLED           | 1             | 0 = disabled, 1 = enabled                            |
|    11 | OP_PARAMETER_MD_INTERVAL              | 1000          | Interval (ms) between frames in MD mode (0 inhibits) |
|    12 | OP_PARAMETER_FLASH_DURATION           | 100           | Duration (ms) that LED Flash is on                   |
|    13 | OP_PARAMETER_FLASH_LED                | 0             | LED bit mask: vis = 1, IR = 2, none = 0              |


## Operational Parameters Description

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


