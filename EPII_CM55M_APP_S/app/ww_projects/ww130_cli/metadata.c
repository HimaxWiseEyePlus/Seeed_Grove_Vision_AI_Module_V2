/**
 * metadata.c
 * created on: 24.12.2024
 * author: 	TBP
 *
 * @brief Metadata functions to add custom metadata to JPEG images via manually inserting data into the APP1 block
 * Checking to see if the images are successful, I was using the exiftool on the command line to check the metadata.
 * "exiftool -g1 -a -s -warning -validate image0001.jpg"
 * "exiftool image0001.jpg -v3"
 * "jpeginfo -c 2025-01-01image*.jpg"
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "metadata.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define APP1_MARKER 0xE1
#define TIFF_HEADER_SIZE 8
#define IFD_ENTRY_SIZE 12
#define EXIF_HEADER "Exif\0\0"
#define BYTE_ORDER "II" // Little-endian format for Intel systems
#define TIFF_TAG_VERSION 42
#define TAG_OFFSET_IFD0 8
#define EXIF_TAG_COUNT 6 // Adjust this for the number of tags you are adding

/* The ExifTag struct is wrapped in #pragma pack(push, 1) and #pragma pack(pop)
 * directives to ensure that the struct members are packed tightly without any padding bytes.
 * The 1 argument in #pragma pack(push, 1) specifies that the alignment should be set to 1 byte.
 */
#pragma pack(push, 1)
typedef struct
{
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t valueOffset;
} ExifTag;
#pragma pack(pop)

/*
 * Writes an EXIF tag to the buffer
 *
 * @param ptr: Pointer to buffer
 * @param tag: Tag ID
 * @param type: Data type
 * @param count: Number of values
 * @param valueOffset: Offset to value
 * @return: Pointer to next tag
 */
uint8_t *writeExifTag(uint8_t *ptr, uint16_t tag, uint16_t type, uint32_t count, uint32_t valueOffset)
{
    ExifTag *exifTag = (ExifTag *)ptr;
    exifTag->tag = tag;
    exifTag->type = type;
    exifTag->count = count;
    exifTag->valueOffset = valueOffset;
    return ptr + sizeof(ExifTag);
}

/*
 * Creates an APP1 block with custom metadata
 *
 * @param metadata: ImageMetadata struct with custom metadata
 * @param buffer: Pointer to buffer to store APP1 block
 * @param bufferSize: Size of buffer
 * @return: Size of APP1 block
 */
int createAPP1Block(ImageMetadata *metadata, unsigned char *buffer, int bufferSize)
{
    uint8_t *ptr = buffer;
    uint8_t *tiffHeaderStart;
    uint8_t *valuePtr;
    uint32_t ifd_offset = 8; // Standard offset to first IFD from TIFF header
    uint32_t value_offset;   // For storing values that don't fit in 4 bytes
    uint16_t num_tags = 4;   // Number of tags in IFD0

    // Check buffer size - make a conservative estimate
    if (bufferSize < 200)
    {
        xprintf("Buffer too small for APP1 block\n");
        return 0;
    }

    // APP1 Marker (0xFFE1)
    *ptr++ = 0xFF;
    *ptr++ = 0xE1;

    // Placeholder for length (2 bytes, will fill later)
    uint8_t *lengthPtr = ptr;
    ptr += 2;

    // EXIF Header (6 bytes)
    memcpy(ptr, "Exif\0\0", 6);
    ptr += 6;

    // Store the beginning of TIFF header for calculating offsets
    tiffHeaderStart = ptr;

    // TIFF Header - Byte order (II = little endian for Intel)
    *ptr++ = 'I';
    *ptr++ = 'I';

    // TIFF signature (42)
    *ptr++ = 42;
    *ptr++ = 0;

    // Offset to first IFD (8 bytes from start of TIFF header)
    *ptr++ = 8;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0;

    // Number of entries in IFD0
    *ptr++ = (uint8_t)num_tags;
    *ptr++ = 0;

    // First value offset after the tags and next IFD pointer
    value_offset = ifd_offset + (num_tags * 12) + 4;
    valuePtr = tiffHeaderStart + value_offset;

    // XResolution (Tag 0x011a) - RATIONAL type (5)
    *ptr++ = 0x1a;
    *ptr++ = 0x01; // Tag
    *ptr++ = 5;
    *ptr++ = 0; // Type: RATIONAL
    *ptr++ = 1;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0; // Count: 1
    *ptr++ = 2;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0; // Value: 2 (inches)

    // YCbCrPositioning (Tag 0x0213) - SHORT type (3)
    *ptr++ = 0x13;
    *ptr++ = 0x02; // Tag
    *ptr++ = 3;
    *ptr++ = 0; // Type: SHORT
    *ptr++ = 1;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0; // Count: 1
    *ptr++ = 1;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0; // Value: 1 (centered)

    // Next IFD offset (none, so 0)
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0;
    *ptr++ = 0;

    // Use valuePtr (which has been incremented) to add custom metadata
    int remainingSize = bufferSize - (valuePtr - buffer);
    xprintf("remainingSize: %d\n", remainingSize);
    xprintf("bufferSize, - (valuePtr, buffer): %d, %d, %d\n", bufferSize, valuePtr, buffer);
    if (remainingSize > 0)
    {
        int written = snprintf((char *)valuePtr, remainingSize,
                               "MediaID: %s\nDeploymentID: %s\nCaptureMethod: %s\n"
                               "Latitude: %.6f\nLongitude: %.6f\nTimestamp: %s\nFavourite: %d\n",
                               metadata->mediaID, metadata->deploymentID, metadata->captureMethod,
                               metadata->latitude, metadata->longitude, metadata->timestamp,
                               metadata->favourite);

        if (written > 0 && written < remainingSize)
        {
            valuePtr += written;
        }
    }

    // // Write IFD0 tags
    // // XResolution (Tag 0x011A) - Rational type (5)
    // *ptr++ = 0x1A;
    // *ptr++ = 0x01; // Tag
    // *ptr++ = 5;
    // *ptr++ = 0; // Type: RATIONAL
    // *ptr++ = 1;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0;                   // Count: 1
    // *ptr++ = value_offset & 0xFF; // Value Offset (pointer to data)
    // *ptr++ = (value_offset >> 8) & 0xFF;
    // *ptr++ = (value_offset >> 16) & 0xFF;
    // *ptr++ = (value_offset >> 24) & 0xFF;

    // // Write XResolution value (72/1)
    // *valuePtr++ = 72;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0; // Numerator
    // *valuePtr++ = 1;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0; // Denominator

    // // Update value_offset for next tag
    // value_offset += 8; // Size of RATIONAL

    // // YResolution (Tag 0x011B) - Rational type (5)
    // *ptr++ = 0x1B;
    // *ptr++ = 0x01; // Tag
    // *ptr++ = 5;
    // *ptr++ = 0; // Type: RATIONAL
    // *ptr++ = 1;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0;                   // Count: 1
    // *ptr++ = value_offset & 0xFF; // Value Offset (pointer to data)
    // *ptr++ = (value_offset >> 8) & 0xFF;
    // *ptr++ = (value_offset >> 16) & 0xFF;
    // *ptr++ = (value_offset >> 24) & 0xFF;

    // // Write YResolution value (72/1)
    // *valuePtr++ = 72;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0; // Numerator
    // *valuePtr++ = 1;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0;
    // *valuePtr++ = 0; // Denominator

    // // ResolutionUnit (Tag 0x0128) - SHORT type (3)
    // *ptr++ = 0x28;
    // *ptr++ = 0x01; // Tag
    // *ptr++ = 3;
    // *ptr++ = 0; // Type: SHORT
    // *ptr++ = 1;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0; // Count: 1
    // *ptr++ = 2;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0; // Value: 2 (inches)

    // // YCbCrPositioning (Tag 0x0213) - SHORT type (3)
    // *ptr++ = 0x13;
    // *ptr++ = 0x02; // Tag
    // *ptr++ = 3;
    // *ptr++ = 0; // Type: SHORT
    // *ptr++ = 1;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0; // Count: 1
    // *ptr++ = 1;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0; // Value: 1 (centered)

    // // Next IFD offset (none, so 0)
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0;
    // *ptr++ = 0;

    // // Use valuePtr (which has been incremented) to add custom metadata
    // int remainingSize = bufferSize - (valuePtr - buffer);
    // xprintf("remainingSize: %d/n", remainingSize);
    // xprintf("bufferSize, - (valuePtr, buffer): %d, %d, %d\n", bufferSize, valuePtr, buffer);
    // if (remainingSize > 0)
    // {
    //     int written = snprintf((char *)valuePtr, remainingSize,
    //                            "MediaID: %s\nDeploymentID: %s\nCaptureMethod: %s\n"
    //                            "Latitude: %.6f\nLongitude: %.6f\nTimestamp: %s\nFavourite: %d\n",
    //                            metadata->mediaID, metadata->deploymentID, metadata->captureMethod,
    //                            metadata->latitude, metadata->longitude, metadata->timestamp,
    //                            metadata->favourite);

    //     if (written > 0 && written < remainingSize)
    //     {
    //         valuePtr += written;
    //     }
    // }

    // Calculate and set APP1 length field (everything after the marker)
    uint16_t app1Length = (uint16_t)(valuePtr - buffer - 2);
    lengthPtr[0] = (app1Length >> 8) & 0xFF; // High byte
    lengthPtr[1] = app1Length & 0xFF;        // Low byte

    // Return total size of APP1 block
    return (valuePtr - buffer);
}

/*
 * For debugging purposes, prints the APP1 block content to CMD
 * @param app1Size: Size of APP1 block
 * @param app1Block: Pointer to APP1 block
 * @return: void
 */
void printEXIF(size_t app1Size, char *app1Block)
{
    // Debug output - show the APP1 block content
    xprintf("EXIF Block (Hex): ");
    for (size_t i = 0; i < app1Size; i++)
    {
        xprintf("%02X ", app1Block[i]);
        if ((i + 1) % 16 == 0)
            xprintf("\n"); // Print a newline every 16 bytes
    }
    xprintf("\n");

    xprintf("EXIF Block (Decoded):\n");
    for (size_t i = 0; i < app1Size; i++)
    {
        if (app1Block[i] >= 32 && app1Block[i] <= 126)
        { // Printable ASCII
            xprintf("%c", app1Block[i]);
        }
        else if (app1Block[i] == '\n')
        {
            xprintf("\n");
        }
        else
        {
            xprintf(".");
        }
    }
    xprintf("\n");
}