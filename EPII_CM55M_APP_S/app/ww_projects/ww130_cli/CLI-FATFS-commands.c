/*
 * CLI-FATFS-commands.c
 *
 * Implements CLI commands for the fatFs
 *
 *  Created on: 20 Aug 2024
 *      Author: CGP
 *
 * Only a few commands are implemented, but this can be expanded as and when required.
 * Some functions are optional and need to be enabled by editing ffconf.h.
 * See here for details of this: http://elm-chan.org/fsw/ff/doc/appnote.html
 *
 * See here for the inspiration for adding file system commands,
 * but note the API is for the FreeRTOS-Plus_FATFS which differs from the fatFS API we are using:
 * 		https://github.com/FreeRTOS/FreeRTOS/blob/520fc225eb2dd5e21c951ca325e1c51eed3a5c13/FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/File-Related-CLI-commands.c
 *
 * FatFs commands are documented here:
 * 		http://elm-chan.org/fsw/ff/
 *
 * IMPORTANT NOTE: At the time of writing the CLI commands implemented here don't use
 * the asynchronous method implemented in fatfs_task.c & .h
 *
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
#include "CLI-commands.h"
#include "CLI-FATFS-commands.h"
#include "printf_x.h"

#include "ff.h"
#include "ffconf.h"		// This may need to be adjusted

/*************************************** Definitions *******************************************/

/*************************************** External variables *******************************************/

/*************************************** Local variables *******************************************/

/*************************************** Local routine prototypes  *************************************/

static void vRegisterCLICommands( void );
static void strip_newline(char *str, uint16_t maxlen);

/*
 * Defines a command that returns a table showing the state of each task at the
 * time the command is called.
 */
static BaseType_t prvInfoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvDirCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvPwdCommand( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );
static BaseType_t prvChdirCommand( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );
static BaseType_t prvMkdirCommand( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );
static BaseType_t prvTypeCommand( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );
static BaseType_t prvUnmountCommand( char * pcWriteBuffer, size_t xWriteBufferLen, const char * pcCommandString );


/********************************** Structures that define CLI commands  *************************************/


// Structure that defines the ls command line command, which lists all the files in the current directory.
static const CLI_Command_Definition_t xInfo = {
    "info",         /* The command string to type. */
    "info:\r\n Print some information about the disk\r\n",
    prvInfoCommand, /* The function to run. */
    0              /* No parameters are expected. */
};

// Structure that defines the ls command line command, which lists all the files in the current directory.
static const CLI_Command_Definition_t xDir = {
    "dir",         /* The command string to type. */
    "dir:\r\n Lists the files in the current directory\r\n",
    prvDirCommand, /* The function to run. */
    0              /* No parameters are expected. */
};

// Structure that defines the pwd command line command, which prints the current directory.
static const CLI_Command_Definition_t xPwd = {
    "pwd",         /* The command string to type. */
    "pwd:\r\n Displays current directory\r\n",
    prvPwdCommand, /* The function to run. */
    0              /* No parameters are expected. */
};

// Structure that defines the pwd command line command, which prints the current directory.
static const CLI_Command_Definition_t xChdir = {
    "cd",         /* The command string to type. */
    "cd <new_dir>:\r\n Change directory to <new_dir>\r\n",
    prvChdirCommand, /* The function to run. */
    1              /* No parameters are expected. */
};

// Structure that defines the pwd command line command, which prints the current directory.
static const CLI_Command_Definition_t xMkdir = {
    "mkdir",         /* The command string to type. */
    "mkdir <new_dir>:\r\n Make new directory <new_dir>\r\n",
    prvMkdirCommand, /* The function to run. */
    1              /* No parameters are expected. */
};

// Structure that defines the pwd command line command, which prints the current directory.
static const CLI_Command_Definition_t xType = {
    "type",         /* The command string to type. */
    "type <file>:\r\n Prints contents of <file>\r\n",
    prvTypeCommand, /* The function to run. */
    1              /* No parameters are expected. */
};

// Structure that defines the pwd command line command, which prints the current directory.
static const CLI_Command_Definition_t xUnmount = {
    "unmount",         /* The command string to type. */
    "unmount:\r\n Unmount (save writes?)\r\n",
    prvUnmountCommand, /* The function to run. */
    0              /* No parameters are expected. */
};


/********************************** Private Function Definitions - for CLI commands ****************************/

// One of these commands for each activity invoked by the CLI

/**
 * Prints some status.
 *
 * Uses f_getfree - see http://elm-chan.org/fsw/ff/doc/getfree.html
 *
 */
static BaseType_t prvInfoCommand( char * pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char * pcCommandString ) {
    FRESULT res;
    FATFS *getFreeFs; 	  // Read information
    DWORD free_clusters;  // Free Clusters
    DWORD free_sectors;	  // Free Sectors
    DWORD total_sectors;  // Total Sectors
    TCHAR label[12]; 	  // Buffer for the volume label
    DWORD vsn; 			  // Volume serial number

	memset( pcWriteBuffer, 0x00, xWriteBufferLen );

	f_getlabel("", label, &vsn);
	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Label: %s\nSerial No: 0x%08x\n", (char *) label, (int) vsn);

	// Get some statistics from the SD card
	res = f_getfree("", &free_clusters, &getFreeFs);

	if (res) {
    	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_getfree() failed (%u)\r\n", res);
		return pdFALSE;
	}

	// Formula comes from ChaN's documentation
	total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
	free_sectors = free_clusters * getFreeFs->csize;

	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen,
			"%10lu K total drive space.\r\n%10lu K available.\r\n",
			total_sectors / 2, free_sectors / 2);

	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/**
 * Directory listing.
 *
 * This uses the CLI mechanism that allows you to return a partial result and expect the function to be called
 * again. That way I am printing one entry each time.
 */
static BaseType_t prvDirCommand( char * pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char * pcCommandString ) {
    FRESULT res;
    FILINFO fno;
    char cur_dir[CLI_FATFS_FILE_NAME_BUF_SIZE];
    UINT len = CLI_FATFS_FILE_NAME_BUF_SIZE;

    // These are static for processing of multiple entries,one at a time
    static DIR dir;					// directory structure
    static uint16_t nfile = 0;		// count files
    static uint16_t ndir = 0;		// count directories
    static bool listing = false;	// True after we have processed the first entry

    memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    if (!listing) {
    	// This is stuff to do the first time we enter this function.
    	res = f_getcwd(cur_dir, len);      /* Get current directory */

    	if (res) {
    		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_getcwd res = %d\r\n", res);
    		return pdFALSE;
    	}
//    	else  {
//    		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "cur_dir = %s\r\n", cur_dir);
//    	}

    	res = f_opendir(&dir, cur_dir);    /* Open the directory */

    	if (res) {
        	pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Failed to open '%s'. (%u)\r\n", cur_dir, res);
        	listing = false;
        	nfile = 0;
        	ndir = 0;
        	return pdFALSE;
    	}
    	else {
        	listing = true;
    	}
    }

    // Here when the directory  has been opened successfully.
    // This might be the first attempt to read for the directory, or not.
    res = f_readdir(&dir, &fno);                   /* Read a directory item */

    if (res != FR_OK || fno.fname[0] == 0) {
    	// Error or end of dir
        pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%d dirs, %d files.\r\n", ndir, nfile);

    	f_closedir(&dir);
    	listing = false;
    	nfile = 0;
    	ndir = 0;
    	return pdFALSE;
    }

    pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen,
    		"%c%c%c%c%c %u-%02u-%02u, %02u:%02u %10d %s",
    					((fno.fattrib & AM_DIR) ? 'D' : '-'),
    					((fno.fattrib & AM_RDO) ? 'R' : '-'),
    					((fno.fattrib & AM_SYS) ? 'S' : '-'),
    					((fno.fattrib & AM_HID) ? 'H' : '-'),
    					((fno.fattrib & AM_ARC) ? 'A' : '-'),
    					((fno.fdate >> 9) + 1980), (fno.fdate >> 5 & 15),
    					(fno.fdate & 31), (fno.ftime >> 11), (fno.ftime >> 5 & 63),
    					(int) fno.fsize, fno.fname);
    if (fno.fattrib & AM_DIR) {
    	// Directory
    	//pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "   <DIR>   %s", fno.fname);
    	ndir++;
    }
    else {
    	// File
    	//pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%10u %s", (int) fno.fsize, fno.fname);
    	nfile++;
    }
    // Assume there is more to come
	return pdTRUE;
}

/**
 * Print current directory
 *
 * See
 */

static BaseType_t prvPwdCommand( char * pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char * pcCommandString ) {
    FRESULT res;
    char cur_dir[CLI_FATFS_FILE_NAME_BUF_SIZE];
    UINT len = CLI_FATFS_FILE_NAME_BUF_SIZE;

	memset( pcWriteBuffer, 0x00, xWriteBufferLen );

    res = f_getcwd(cur_dir, len);      /* Get current directory */

	if (res)  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_getcwd res = %d\r\n", res);
	}
	else  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "cur_dir = %s\r\n", cur_dir);
	}
	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/**
 * Change directory
 * See http://elm-chan.org/fsw/ff/doc/chdir.html
 *
 */
static BaseType_t prvChdirCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
    FRESULT res;
	const char *pcParameter;
	BaseType_t lParameterStringLength;

	/* Get parameter */
	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	if (pcParameter != NULL) {
		//Maybe some checking here?
		res = f_chdir(pcParameter);
	}
	else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply directory name.\r\n");
		return pdFALSE;
	}

    if (res == FR_OK) {
    	// TODO remove this when done.
		//pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "OK\r\n");
    }
    else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_chdir %s failed: %d\r\n", pcParameter, res);
    }

	return pdFALSE;
}

/**
 * Make directory
 * See http://elm-chan.org/fsw/ff/doc/mkdir.html
 *
 */
static BaseType_t prvMkdirCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
    FRESULT res;
	const char *pcParameter;
	BaseType_t lParameterStringLength;

	/* Get parameter */
	pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
	if (pcParameter != NULL) {
		//Maybe some checking here?
		res = f_mkdir(pcParameter);
	}
	else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply directory name.\r\n");
		return pdFALSE;
	}

    if (res == FR_OK) {
    	// TODO remove this when done.
		//pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "OK\r\n");
    }
    else {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_mkdir %s failed: %d\r\n", pcParameter,res);
    }

	return pdFALSE;
}


/**
 * Type file
 *
 * Assumes it is a text file. Prints one line at a time using the CLI facility to call multiple times.
 *
 * See http://elm-chan.org/fsw/ff/doc/gets.html
 *
 */
static BaseType_t prvTypeCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
	FRESULT res;
	const char *pcParameter;
	BaseType_t lParameterStringLength;
	char line[CLI_OUTPUT_BUF_SIZE]; /* Line buffer */

	static bool listing  = false;
	static FIL fil;

	if (!listing) {
		// This is stuff to do the first time we enter this function.
		pcParameter = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParameterStringLength);
		if (pcParameter != NULL) {
			//Maybe some checking here?
			res = f_open(&fil, pcParameter, FA_READ);
		}
		else {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Must supply file name.\r\n");
			return pdFALSE;
		}

		if (res) {
			pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Failed to open '%s'. (%u)\r\n", pcParameter, res);
			return pdFALSE;
		}
	}

	// Here if the file is open
	listing = true;

	if (f_gets(line, sizeof line, &fil)) {
		// strip trailing \n or \r since these will be added when the pcWriteBuffer is printed.
		strip_newline(line, CLI_OUTPUT_BUF_SIZE);
//		uint16_t lineLength = strnlen(line, CLI_OUTPUT_BUF_SIZE);
//
//		printf_x_printBuffer((uint8_t *) line, strnlen(line, CLI_OUTPUT_BUF_SIZE));
//		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%d %d ", count++, lineLength);	// test how many times we go through
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "%s", line);

		// Assume there is more to read
		return pdTRUE;
	}
	else {
		// No data left
		f_close(&fil);
		listing = false;
		return pdFALSE;
	}
}

/**
 * Unmount
 *
 * See http://elm-chan.org/fsw/ff/doc/mount.html
 */

static BaseType_t prvUnmountCommand( char * pcWriteBuffer,
                                 size_t xWriteBufferLen,
                                 const char * pcCommandString ) {
    FRESULT res;


    res = f_unmount("");

	if (res)  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "f_unmont failed. res = %d\r\n", res);
	}
	else  {
		pcWriteBuffer += snprintf(pcWriteBuffer, xWriteBufferLen, "Unmounted OK.NOW WHAT?\r\n");
	}
	/* There is no more data to return after this single string, so return pdFALSE. */
	return pdFALSE;
}

/********************************** Private Function Definitions - Other **************************/

/**
 * Register CLI commands
 */
static void vRegisterCLICommands( void ) {
	FreeRTOS_CLIRegisterCommand( &xInfo );
	FreeRTOS_CLIRegisterCommand( &xDir );
	FreeRTOS_CLIRegisterCommand( &xPwd );
	FreeRTOS_CLIRegisterCommand( &xChdir );
	FreeRTOS_CLIRegisterCommand( &xMkdir );
	FreeRTOS_CLIRegisterCommand( &xType );
	FreeRTOS_CLIRegisterCommand( &xUnmount );
}

/**
 * Remove \r\n from strings when using type command, since they will be added back later
 */
static void strip_newline(char *str, uint16_t maxlen) {
    size_t len = strnlen(str, maxlen);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
    }
}

/********************************** Public Function Definitions  *************************************/

void cli_fatfs_init(void) {

	/* Register available CLI commands */
	vRegisterCLICommands();
}
