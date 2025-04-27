#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "metadata.h"

#ifndef ADD_EXIF_H
#define ADD_EXIF_H

// Custom tag IDs - using private tag range
#define TAG_DEPLOYMENT_ID 0xF200
#define TAG_DEPLOYMENT_PROJECT 0xF201
#define TAG_OBSERVATION_ID 0xF300
#define TAG_OBSERVATION_TYPE 0xF301

// Structure to hold custom EXIF data
typedef struct
{
    char deploymentId[32];      // Project deployment ID (string)
    char deploymentProject[64]; // Project name (string)
    char observationId[32];     // Observation ID (string)
    char observationType[32];   // Type of observation (string)
    double latitude;            // GPS latitude (degrees, positive = North)
    double longitude;           // GPS longitude (degrees, positive = East)
} ImageMetadata;

// Function declarations
size_t addExifGPS(uint32_t SRAM_addr, size_t img_size, uint8_t *filename, double lat, double lon);
int create_exif_gps_block(unsigned char *buffer, double lat, double lon);
int insert_exif(const char *fileName, ImageMetadata *metadata);

#endif // ADD_EXIF_H
