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

#ifndef APP_WW_PROJECTS_WW130_CLI_FATFS_TASK_H_
#define APP_WW_PROJECTS_WW130_CLI_FATFS_TASK_H_

// The states for the fatfs_task
typedef enum {
	APP_FATFS_STATE_UNINIT						=0x0000,
	APP_FATFS_STATE_IDLE						=0x0001,
	APP_FATFS_STATE_I2C_RX						=0x0002,
	APP_FATFS_STATE_I2C_TX						=0x0003,
	APP_FATFS_STATE_PA0						=0x0004,
	APP_FATFS_STATE_ERROR						,
} APP_FATFS_STATE_E;


void fatfs_createTask(void);

#endif /* APP_WW_PROJECTS_WW130_CLI_FATFS_TASK_H_ */
