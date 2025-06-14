/*
 * fatfs_task.h
 *
 *  Created on: 19 Aug 2024
 *      Author: CGP
 *
 *      https://github.com/FreeRTOS/FreeRTOS/blob/520fc225eb2dd5e21c951ca325e1c51eed3a5c13/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/File-Related-CLI-commands.c
 *      https://github.com/FreeRTOS/FreeRTOS/blob/520fc225eb2dd5e21c951ca325e1c51eed3a5c13/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_FAT_SL_Demos/CreateExampleFiles/File-system-demo.c
 *      https://github.com/FreeRTOS/Lab-Project-FreeRTOS-FAT
 *      https://www.freertos.org/Documentation/03-Libraries/05-FreeRTOS-labs/04-FreeRTOS-plus-FAT/file_related_cli_commands
 *
 */

#ifndef APP_WW_PROJECTS_WW500_MD_FATFS_TASK_H_
#define APP_WW_PROJECTS_WW500_MD_FATFS_TASK_H_

/********************************** Includes ******************************************/

#include <stdbool.h>
#include "ff.h"
#include "ww500_md.h"

/**************************************** Global Defines  *************************************/

// TODO Experimental: set a limit on the name of files
#define	FNAMELEN 16

/**************************************** Type declarations  *************************************/

// Operational parameters to get/set.
// Typically the values are saved to SD card before entering DPD
// OP_PARAMETER_NUM_ENTRIES is only used to establish the number of entries
typedef enum {
	OP_PARAMETER_SEQUENCE_NUMBER,
	OP_PARAMETER_NUM_PICTURES,
	OP_PARAMETER_PICTURE_INTERVAL,
	OP_PARAMETER_INTERVAL_BEFORE_DPD,
	OP_PARAMETER_LED_FLASH_DUTY,
	OP_PARAMETER_GPS_LOCATION,
	OP_PARAMETER_NUM_ENTRIES
} OP_PARAMETERS_E;

// The states for the fatfs_task
// APP_FATFS_STATE_NUMSTATES is only used to establish the number of states
typedef enum {
	APP_FATFS_STATE_UNINIT						=0x0000,
	APP_FATFS_STATE_IDLE						=0x0001,
	APP_FATFS_STATE_BUSY						=0x0002,
	APP_FATFS_STATE_NUMSTATES					=0x0003
} APP_FATFS_STATE_E;

// Structure to use for file operations:
// Initially for reading and writing a file
typedef struct {
	char *		fileName;
	uint8_t *	buffer;		// Pointer to the buffer containing file contents
	uint32_t 	length;		// Number of bytes to write or read
	FRESULT 	res;		// Result code returned from fatFs
	bool		closeWhenDone;	// If true the file is closed when the operation completes
	bool		unmountWhenDone;	// If true the SD card is unmounted when the operation completed
	QueueHandle_t senderQueue;	// FreeRTOS queue that will get the response
} fileOperation_t;




/**************************************** Global routine declarations  *************************************/

TaskHandle_t fatfs_createTask(int8_t priority, APP_WAKE_REASON_E wakeReason);

uint16_t fatfs_getState(void);

const char * fatfs_getStateString(void);

// Get one of the Operational Parameters
int32_t fatfs_getOperationalParameter(OP_PARAMETERS_E parameter);

// Set one of teh operational parameters
void fatfs_setOperationalParameter(OP_PARAMETERS_E parameter, int32_t value);

uint16_t fatfs_getImageSequenceNumber(void);

void fatfs_incrementImageSequenceNumber(void);

uint16_t fatfs_saveSequenceNumber(uint16_t sequenceNumber);



#endif /* APP_WW_PROJECTS_WW500_MD_FATFS_TASK_H_ */
