/*
 * metadata.h
 *
 * Created on: 17/03/2024
 * Author: TBP
 *
 * Header file for EXIF and metadata handling
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "FreeRTOS.h"

#ifndef METADATA_H
#define METADATA_H

#define EXIF_HEADER_SIZE 10
#define TIFF_HEADER_SIZE 8
#define IFD_ENTRY_SIZE 12
#define NUM_IFD_ENTRIES 3

extern const uint8_t exif_header[];

// EXIF Metadata Struct
typedef struct
{
    char mediaID[10];
    char deploymentID[10];
    char captureMethod[10];
    float latitude;
    float longitude;
    char timestamp[20];
    bool favourite;
} imageMetadata;

#endif // METADATA_H