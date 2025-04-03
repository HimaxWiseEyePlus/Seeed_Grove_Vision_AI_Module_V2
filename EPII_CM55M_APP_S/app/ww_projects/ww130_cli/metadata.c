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

// #define APP1_MARKER 0xFFE1
#define TIFF_HEADER_SIZE 8
// #define IFD_ENTRY_SIZE 12
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
//  */
// int createAPP1Block(ImageMetadata *metadata, unsigned char *buffer, int bufferSize)
// {
//     uint8_t *ptr = buffer;
//     uint8_t *tiffHeaderStart;
//     uint8_t *valuePtr;
//     uint32_t ifd_offset = 8; // Standard offset to first IFD from TIFF header
//     uint32_t value_offset;   // For storing values that don't fit in 4 bytes
//     uint16_t num_tags = 4;   // Number of tags in IFD0

//     // Check buffer size - make a conservative estimate
//     if (bufferSize < 200)
//     {
//         xprintf("Buffer too small for APP1 block\n");
//         return 0;
//     }

//     // APP1 Marker (0xFFE1)
//     *ptr++ = 0xFF;
//     *ptr++ = 0xE1;

//     // Placeholder for length (2 bytes, will fill later)
//     uint8_t *lengthPtr = ptr;
//     ptr += 2;

//     // EXIF Header (6 bytes)
//     memcpy(ptr, "Exif\0\0", 6);
//     ptr += 6;

//     // Store the beginning of TIFF header for calculating offsets
//     tiffHeaderStart = ptr;

//     // TIFF Header - Byte order (II = little endian for Intel)
//     *ptr++ = 'I';
//     *ptr++ = 'I';

//     // TIFF signature (42)
//     *ptr++ = 42;
//     *ptr++ = 0;

//     // Offset to first IFD (8 bytes from start of TIFF header)
//     *ptr++ = 8;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0;

//     // Number of entries in IFD0
//     *ptr++ = (uint8_t)num_tags;
//     *ptr++ = 0;

//     // First value offset after the tags and next IFD pointer
//     value_offset = ifd_offset + (num_tags * 12) + 4;
//     valuePtr = tiffHeaderStart + value_offset;

//     // XResolution (Tag 0x011a) - RATIONAL type (5)
//     *ptr++ = 0x1a;
//     *ptr++ = 0x01; // Tag
//     *ptr++ = 5;
//     *ptr++ = 0; // Type: RATIONAL
//     *ptr++ = 1;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0; // Count: 1
//     *ptr++ = 2;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0; // Value: 2 (inches)

//     // YCbCrPositioning (Tag 0x0213) - SHORT type (3)
//     *ptr++ = 0x13;
//     *ptr++ = 0x02; // Tag
//     *ptr++ = 3;
//     *ptr++ = 0; // Type: SHORT
//     *ptr++ = 1;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0; // Count: 1
//     *ptr++ = 1;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0; // Value: 1 (centered)

//     // Next IFD offset (none, so 0)
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0;
//     *ptr++ = 0;

//     // Use valuePtr (which has been incremented) to add custom metadata
//     int remainingSize = bufferSize - (valuePtr - buffer);
//     xprintf("remainingSize: %d\n", remainingSize);
//     xprintf("bufferSize, - (valuePtr, buffer): %d, %d, %d\n", bufferSize, valuePtr, buffer);
//     if (remainingSize > 0)
//     {
//         int written = snprintf((char *)valuePtr, remainingSize,
//                                "MediaID: %s\nDeploymentID: %s\nCaptureMethod: %s\n"
//                                "Latitude: %.6f\nLongitude: %.6f\nTimestamp: %s\nFavourite: %d\n",
//                                metadata->mediaID, metadata->deploymentID, metadata->captureMethod,
//                                metadata->latitude, metadata->longitude, metadata->timestamp,
//                                metadata->favourite);

//         if (written > 0 && written < remainingSize)
//         {
//             valuePtr += written;
//         }
//     }

//     // Calculate and set APP1 length field (everything after the marker)
//     uint16_t app1Length = (uint16_t)(valuePtr - buffer - 2);
//     lengthPtr[0] = (app1Length >> 8) & 0xFF; // High byte
//     lengthPtr[1] = app1Length & 0xFF;        // Low byte

//     // Return total size of APP1 block
//     return (valuePtr - buffer);
// }

int createAPP1Block(ImageMetadata *metadata, unsigned char *buffer, int bufferSize)
{
    // Constants for EXIF structure
    const uint16_t APP1_MARKER = 0xFFE1;
    const uint16_t TIFF_MAGIC = 42;
    const uint32_t IFD_OFFSET = 8;
    const uint16_t NUM_ENTRIES = 4;
    const uint16_t IFD_ENTRY_SIZE = 12;
    // Minimum required size calculation
    const int MIN_SIZE = 2 +                              // APP1 marker
                         2 +                              // Length
                         6 +                              // EXIF header
                         8 +                              // TIFF header
                         2 +                              // Number of entries
                         (NUM_ENTRIES * IFD_ENTRY_SIZE) + // IFD entries
                         4 +                              // Next IFD pointer
                         100;                             // Minimum space for values
    if (bufferSize < MIN_SIZE)
    {
        xprintf("Buffer too small for APP1 block. Need %d bytes, got %d\n",
                MIN_SIZE, bufferSize);
        return 0;
    }
    int currentPos = 0;
    // 1. APP1 Marker
    buffer[currentPos++] = (APP1_MARKER >> 8) & 0xFF; // 0xFF
    buffer[currentPos++] = APP1_MARKER & 0xFF;        // 0xE1
    // 2. Save position for length (will fill later)
    int lengthPos = currentPos;
    currentPos += 2;
    // 3. EXIF Header
    const char exifHeader[] = {'E', 'x', 'i', 'f', 0, 0};
    memcpy(&buffer[currentPos], exifHeader, 6);
    currentPos += 6;
    // Save TIFF header start for offset calculations
    int tiffHeaderStart = currentPos;
    // 4. TIFF Header
    // Byte order (II = little endian)
    buffer[currentPos++] = 'I';
    buffer[currentPos++] = 'I';
    // TIFF magic number (42)
    buffer[currentPos++] = TIFF_MAGIC & 0xFF;
    buffer[currentPos++] = (TIFF_MAGIC >> 8) & 0xFF;
    // Offset to first IFD
    buffer[currentPos++] = IFD_OFFSET & 0xFF;
    buffer[currentPos++] = (IFD_OFFSET >> 8) & 0xFF;
    buffer[currentPos++] = (IFD_OFFSET >> 16) & 0xFF;
    buffer[currentPos++] = (IFD_OFFSET >> 24) & 0xFF;
    // 5. IFD Entry Count
    buffer[currentPos++] = NUM_ENTRIES & 0xFF;
    buffer[currentPos++] = (NUM_ENTRIES >> 8) & 0xFF;
    // Calculate value offset (after IFD entries and next IFD pointer)
    uint32_t valueOffset = IFD_OFFSET + (NUM_ENTRIES * IFD_ENTRY_SIZE) + 4;
    int valuePos = tiffHeaderStart + valueOffset;
    // 6. IFD Entries
    // XResolution (0x011A) - RATIONAL
    struct
    {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint32_t value;
    } xres = {0x011A, 5, 1, valuePos - tiffHeaderStart};
    memcpy(&buffer[currentPos], &xres, IFD_ENTRY_SIZE);
    currentPos += IFD_ENTRY_SIZE;
    // Store XResolution value (72/1 for 72 DPI)
    buffer[valuePos++] = 72; // Numerator
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 1; // Denominator
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    // YResolution (0x011B) - RATIONAL
    struct
    {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint32_t value;
    } yres = {0x011B, 5, 1, valuePos - tiffHeaderStart};
    memcpy(&buffer[currentPos], &yres, IFD_ENTRY_SIZE);
    currentPos += IFD_ENTRY_SIZE;
    // Store YResolution value (72/1 for 72 DPI)
    buffer[valuePos++] = 72; // Numerator
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 1; // Denominator
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    buffer[valuePos++] = 0;
    // ResolutionUnit (0x0128) - SHORT
    struct
    {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint16_t value;
        uint16_t pad;
    } resUnit = {0x0128, 3, 1, 2, 0}; // 2 = inches
    memcpy(&buffer[currentPos], &resUnit, IFD_ENTRY_SIZE);
    currentPos += IFD_ENTRY_SIZE;
    // YCbCrPositioning (0x0213) - SHORT
    struct
    {
        uint16_t tag;
        uint16_t type;
        uint32_t count;
        uint16_t value;
        uint16_t pad;
    } ycbcr = {0x0213, 3, 1, 1, 0}; // 1 = centered
    memcpy(&buffer[currentPos], &ycbcr, IFD_ENTRY_SIZE);
    currentPos += IFD_ENTRY_SIZE;
    // Next IFD offset (0 = no more IFDs)
    memset(&buffer[currentPos], 0, 4);
    currentPos += 4;
    currentPos = valuePos;

    // Add before writing the APP1 block
    xprintf("APP1 Block first 20 bytes: ");
    for (int i = 0; i < 20; i++)
    {
        xprintf("%02X ", buffer[i]);
    }
    xprintf("\n");

    // 7. Custom metadata as ASCII values
    int remainingSize = bufferSize - currentPos;
    if (remainingSize > 0)
    {
        int written = snprintf((char *)&buffer[currentPos], remainingSize,
                               "MediaID:%s\nDeploymentID:%s\nCaptureMethod:%s\n"
                               "Latitude:%.6f\nLongitude:%.6f\nTimestamp:%s\nFavourite:%d\n",
                               metadata->mediaID, metadata->deploymentID, metadata->captureMethod,
                               metadata->latitude, metadata->longitude, metadata->timestamp,
                               metadata->favourite);
        if (written > 0 && written < remainingSize)
        {
            currentPos += written;
        }
    }
    // 8. Calculate and set APP1 length (everything after marker)
    uint16_t app1Length = currentPos - 2;
    buffer[lengthPos] = (app1Length >> 8) & 0xFF;
    buffer[lengthPos + 1] = app1Length & 0xFF;

    // Add after calculating the APP1 length
    xprintf("BEDGCalculated APP1 length: %d (0x%04X)\n", app1Length, app1Length);
    xprintf("BEDGLength bytes: %02X %02X\n", buffer[lengthPos], buffer[lengthPos + 1]);
    return currentPos; // Return total size of APP1 block
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