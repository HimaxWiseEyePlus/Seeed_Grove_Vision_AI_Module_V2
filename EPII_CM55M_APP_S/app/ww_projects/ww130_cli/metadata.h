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

#ifndef METADATA_H
#define METADATA_H

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