/**
 * exif_metadata.h
 * created on: 15.03.2025
 * author: 	TBP
 *
 * @brief Metadata functions to add custom metadata to JPEG images via manually inserting data into the APP1 block
 * */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef exif_metadata_H
#define exif_metadata_H

// Custom tag IDs - using private tag range
#define TAG_DEPLOYMENT_ID 0xF200
#define TAG_DEPLOYMENT_PROJECT 0xF201
#define TAG_OBSERVATION_ID 0xF300
#define TAG_OBSERVATION_TYPE 0xF301
#define IFD_ENTRY_SIZE 12
#define WRITE_BUFFER_SIZE 512 // Size of the buffer for reading/writing
#define MAX_METADATA_SIZE 512 // Maximum size of the EXIF block

// Function to write a 16-bit value in little-endian format
#define WRITE16LE(p, v)               \
    do                                \
    {                                 \
        uint8_t *_p = (uint8_t *)(p); \
        uint16_t _v = (v);            \
        _p[0] = _v & 0xFF;            \
        _p[1] = (_v >> 8) & 0xFF;     \
    } while (0)
// Function to write a 32-bit value in little-endian format
#define WRITE32LE(p, v)               \
    do                                \
    {                                 \
        uint8_t *_p = (uint8_t *)(p); \
        uint32_t _v = (v);            \
        _p[0] = _v & 0xFF;            \
        _p[1] = (_v >> 8) & 0xFF;     \
        _p[2] = (_v >> 16) & 0xFF;    \
        _p[3] = (_v >> 24) & 0xFF;    \
    } while (0)

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
int insert_exif(const char *fileName, ImageMetadata *metadata);

#endif // exif_metadata_H
