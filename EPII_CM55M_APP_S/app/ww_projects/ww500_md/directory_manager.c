/**
 * @file directory_manager.c
 *
 *  Created: 31 Jul 2025
 *      Author: TBP
 *
 * This file manages directories for the WW500 MD project.
 * It initializes directories, creates capture folders,
 * and handles directory-related operations.
 * It primiliarly communicates with the FatFS task.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "xprintf.h"
#include "fatfs_task.h"
#include "directory_manager.h"
#include "image_task.h"
#include "ffconf.h"

/*************************************** Definitions *******************************************/

#define MAXIMAGEDIRECTORIES	999

/*************************************** Local variables *******************************************/

/*************************************** Global variables *******************************************/

directoryManager_t dirManager; // Added definition for dirManager

///*************************************** Global Function Definitions *****************************/

/**
 * Looks for a directory to save images.
 * If it exists, use it. Otherwise, create it.
 *
 * TBP - I've set FF_FS_LOCK to 2. This means that the directory
 * is locked when it is opened, and unlocked when it is closed.
 * This is to prevent other tasks from writing to the directory
 * while it is being used. 2 also means that it enables two directories
 * to be used simulantaneously.
 *
 * TODO - this should probably be the responsibility of the image task?
 * Or else: move generateImageFileName() to the fatfs task as well.
 */
FRESULT dir_mgr_init_directories(directoryManager_t *dirManager)
{
	FRESULT res;
	char path_buf[IMAGEFILENAMELEN];
	FILINFO fno;
	int len = sizeof(dirManager->base_dir);
	res = f_getcwd(dirManager->base_dir, len); /* Get current directory */
	dirManager->imagesDirIdx = 0;

	// === CONFIG DIRECTORY ===
	xsprintf(path_buf, CONFIG_DIR);
	res = f_stat(path_buf, &fno);
	if (res == FR_OK)
	{
		xprintf("Directory '%s' exists\r\n", path_buf);
	}
	else
	{
		xprintf("Creating directory '%s'\r\n", path_buf);
		res = f_mkdir(path_buf);
		if (res != FR_OK)
		{
			xprintf("f_mkdir(config) failed (%d)\r\n", res);
			dirManager->configRes = res;
			return dirManager->configRes;
		}
	}
	strcpy(dirManager->current_config_dir, path_buf); // Set initial result for config operations

	// === IMAGES DIRECTORY ===
#if FF_USE_LFN
	snprintf(path_buf, sizeof(path_buf), "%s_%04d", CAPTURE_DIR, dirManager->imagesDirIdx);
#else
	// Use 8.3 file name format
	snprintf(path_buf, sizeof(path_buf), "%s.%03d", CAPTURE_DIR, dirManager->imagesDirIdx);
#endif // FF_USE_LFN

	res = f_stat(path_buf, &fno);
	if (res == FR_OK)
	{
		xprintf("Directory '%s' exists\r\n", path_buf);
	}
	else
	{
		xprintf("Creating directory '%s'\r\n", path_buf);
		res = f_mkdir(path_buf);
		if (res != FR_OK)
		{
			xprintf("f_mkdir(images) failed (%d)\r\n", res);
			dirManager->imagesRes = res;
			return dirManager->imagesRes;
		}
		dirManager->imagesDirIdx = 1;
	}
	strcpy(dirManager->current_capture_dir, path_buf); // Set initial result for image operations
	dirManager->configOpen = false;
	dirManager->imagesOpen = false;

	return res;
}

/**
 * Creates a new folder for image captures when threshold met.
 * Realisic use case will be to check the number of captures within a folder,
 * when it reaches the threshold (fatfs_getImageSequenceNumber),
 * this function gets called to create a new folder
 * and points to this directory for new captures to be stored.
 *
 * @param dirManager Pointer to the directory manager structure to initialize.
 * @return FRESULT indicating the success or failure of the operation.
 */
FRESULT dir_mgr_add_capture_folder(directoryManager_t *dirManager)
{
	char path_buf[IMAGEFILENAMELEN];

	// 999 is an arbitrary number
	// TODO, what we set the limits to and when to shift to a new capture folder
	if (dirManager->imagesDirIdx < MAXIMAGEDIRECTORIES)
	{
		dirManager->imagesDirIdx++;
		uint16_t idx = dirManager->imagesDirIdx;

#if FF_USE_LFN
		snprintf(path_buf, sizeof(path_buf), "%s_%04d", CAPTURE_DIR, idx);
#else
		snprintf(path_buf, sizeof(path_buf), "%s.%03d", CAPTURE_DIR, idx);
#endif

		// Creates the new folder
		FRESULT res = f_mkdir(path_buf);
		if (res != FR_OK)
		{
			xprintf("Failed to create folder: %s, error: %d\n", path_buf, res);
			return res;
		}
		dirManager->imagesRes = FR_OK;
	}
	else
	{
		xprintf("Folder index too high: %d (max %d)\n", dirManager->imagesDirIdx, MAXIMAGEDIRECTORIES);
		dirManager->imagesRes = FR_INVALID_NAME; // Set error in directory manager
		return dirManager->imagesRes;
	}
	strcpy(dirManager->current_capture_dir, path_buf); // Update current directory
	return dirManager->imagesRes;
}

/**
 * Deletes a folder for image captures.
 * @param path_buf Name of the folder to be deleted.
 * @param dirManager Pointer to the directory manager structure.
 * @return FRESULT indicating the success or failure of the operation.
 */
FRESULT dir_mgr_delete_capture_folder(const char *path_buf, directoryManager_t *dirManager)
{
	FRESULT res = f_rmdir(path_buf);
	if (res != FR_OK)
	{
		xprintf("Failed to delete folder: %s, error: %d\n", path_buf, res);
		dirManager->imagesRes = res; // Update directory manager with error
	}
	else
	{
		xprintf("Successfully deleted folder: %s\n", path_buf);
		dirManager->imagesRes = FR_OK; // Update directory manager on success
		dirManager->imagesDirIdx--;	   // Decrease folder count
	}
	return res;
}
