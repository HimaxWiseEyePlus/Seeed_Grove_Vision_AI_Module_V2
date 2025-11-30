# Description of the CONFIG.TXT File
#### CGP - 25 November 2025

The CONFIG.TXT file contains "Operational Parameters" for the WW500.

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

Operational Parameters which are not present in CONFIG.TXT are given their default values.

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

## More Details

For more details of how the Operational Parameters are used, see [Operational_Parameters.md](https://github.com/wildlifeai/Seeed_Grove_Vision_AI_Module_V2/blob/ledflash2/_Documentation/Operational_Parameters.md)
NOTE - correct the URL when the file is in the 'main' branch.