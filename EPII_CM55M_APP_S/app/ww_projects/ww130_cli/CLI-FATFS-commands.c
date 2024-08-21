/*
 * CLI-FATFS-commands.c
 *
 * Implements CLI commands for the fatFs
 *
 * See https://github.com/FreeRTOS/FreeRTOS/blob/520fc225eb2dd5e21c951ca325e1c51eed3a5c13/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/File-Related-CLI-commands.c
 *
 *  Created on: 20 Aug 2024
 *      Author: CGP
 */


/*************************************** Includes *******************************************/

/* Modified by Maxim Integrated 26-Jun-2015 to quiet compiler warnings */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"


#include "ff.h"

/*************************************** Definitions *******************************************/


/*************************************** External variables *******************************************/

/*************************************** Local variables *******************************************/

/*************************************** Local routine prototypes  *************************************/

static void vRegisterCLICommands( void );

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvDIRCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );


/********************************** Structures that define CLI commands  *************************************/


/* Structure that defines the DIR command line command, which lists all the
 * files in the current directory. */
static const CLI_Command_Definition_t xDIR =
{
    "dir",         /* The command string to type. */
    "\r\ndir:\r\n Lists the files in the current directory\r\n",
    prvDIRCommand, /* The function to run. */
    0              /* No parameters are expected. */
};


/********************************** Private Function Definitions - for CLI commands ****************************/

// One of these commands for each activity invoked by the CLI

static BaseType_t prvDIRCommand( char * pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char * pcCommandString ) {
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );

    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;
    char cur_dir[128];
    UINT len = 128;

	memset( pcWriteBuffer, 0x00, xWriteBufferLen );

	res = f_chdir("0:/");
    res = f_getcwd(cur_dir, len);      /* Get current directory */
	if (res)  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_getcwd res = %d\r\n", res);
		return pdFALSE;

	}
	else  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "cur_dir = %s\r\n", cur_dir);
	}

    res = f_opendir(&dir, cur_dir);    /* Open the directory */
	if (res)  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "list_dir res = %d\r\n", res);
		return pdFALSE;
	}


    if (res == FR_OK) {
        nfile = ndir = 0;
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Error or end of dir */
            if (fno.fattrib & AM_DIR) {
            	// Directory
    			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "   <DIR>   %s\r\n", fno.fname);
                ndir++;
            }
            else {
            	// File
    			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%10u %s\r\n", (int) fno.fsize, fno.fname);
                nfile++;
            }
        }
        f_closedir(&dir);

        pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%d dirs, %d files.\r\n", ndir, nfile);
    }
    else {
    	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Failed to open \"%s\". (%u)\r\n", cur_dir, res);
    }

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}


/********************************** Private Function Definitions - Other **************************/

/**
 * Register CLI commands
 */
static void vRegisterCLICommands( void ) {
	FreeRTOS_CLIRegisterCommand( &xDIR );
}

/********************************** Public Function Definitions  *************************************/

void cli_fatfs_init(void) {

	/* Register available CLI commands */
	vRegisterCLICommands();
}
