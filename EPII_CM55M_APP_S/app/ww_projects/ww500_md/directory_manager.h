/**
 * @file directory_manager.h
 *
 *  Created: 31 Jul 2025
 *      Author: TBP
 *
 * @brief Header file for directory management functions.
 *
 * This file contains declarations for functions that manage directories,
 * including creating, deleting, and listing directories.
 *
 * CGP 13/11/25 - Need to think about all of these functions and how they are used.
 */

#ifndef APP_WW_PROJECTS_WW500_MD_DIRECTORY_MANAGER_H_
#define APP_WW_PROJECTS_WW500_MD_DIRECTORY_MANAGER_H_

/********************************** Includes ******************************************/

#include <stdbool.h>
#include "ww500_md.h"
#include "ff.h"
#include "image_task.h"

// Warning: if using 8.3 file names then this applies to directories also.
// Names are upper case.
// TBP - TODO: We need to review naming conventions for directories.
#if FF_USE_LFN
#define CAPTURE_DIR "Deployment"
#define STATE_FILE "configuration.txt"
#else
#define CAPTURE_DIR "/IMAGES"
#define STATE_FILE "CONFIG.TXT"
#endif // FF_USE_LFN

#define CONFIG_DIR "/CONFIG"

/**************************************** Type declarations  *************************************/

typedef struct
{
    FIL configFile;                             // File object for the config directory
    FIL imagesFile;                             // File object for the images directory
    FRESULT configRes;                          // Result code for config operations
    FRESULT imagesRes;                          // Result code for image operations
    bool configOpen;                            // Flag to indicate if config file is open
    bool imagesOpen;                            // Flag to indicate if images file is open
    int imagesDirIdx;                           // Index of the current images directory
    char current_config_dir[IMAGEFILENAMELEN];  // Current config directory path
    char current_capture_dir[IMAGEFILENAMELEN]; // Current capture directory path
    char base_dir[IMAGEFILENAMELEN];            // Current working directory path
} directoryManager_t;

/**************************************** Global Defines  *************************************/

// extern char current_dir[256];
extern directoryManager_t dirManager;


/**************************************** Global Function Declarations  *************************************/

FRESULT dir_mgr_init_directories(directoryManager_t *dirManager);
FRESULT dir_mgr_add_capture_folder(directoryManager_t *dirManager);
FRESULT dir_mgr_delete_capture_folder(const char *folder_name, directoryManager_t *dirManager);

#endif /* APP_WW_PROJECTS_WW500_MD_DIRECTORY_MANAGER_H_ */
