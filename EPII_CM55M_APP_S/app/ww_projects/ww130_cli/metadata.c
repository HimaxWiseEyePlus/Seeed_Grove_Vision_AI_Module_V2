// /**
//  * metadata.c
//  * created on: 24.12.2024
//  * author: 	TBP
//  *
//  * @brief Metadata functions to add custom metadata to JPEG images via manually inserting data into the APP1 block
//  * Checking to see if the images are successful, I was using the exiftool on the command line to check the metadata.
//  * "exiftool -g1 -a -s -warning -validate image0001.jpg"
//  * "exiftool image0001.jpg -v3"
//  * "jpeginfo -c 2025-01-01image*.jpg"
//  * chmod +x ./we2_local_image_gen
//  * chmod +x ./secureboot_tool/*
//  * chmod +x ./arm_none_eabi/*
// exiftool -config ../Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -G1 -a -validate image_0001_2025-01-02.jpg
// exiftool -config ../Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/ww_projects/ww130_cli/exif_config/exiftool_validation_hook.config -g1 -a -s -warning -validate *.jpg
// exiftool - config../ Seeed_Grove_Vision_AI_Module_V2 / EPII_CM55M_APP_S / app / ww_projects / ww130_cli / exif_config / exiftool_complete.config *.jpg
//  */

// #include <stdint.h>
// #include <string.h>
// #include <stdio.h>
// #include <stdbool.h>
// #include "metadata.h"

// #include <stddef.h>
// #include <stdio.h>
// #include <stdlib.h>

// // #define APP1_MARKER 0xFFE1
// #define TIFF_HEADER_SIZE 8
// // #define IFD_ENTRY_SIZE 12
// #define EXIF_HEADER "Exif\0\0"
// #define BYTE_ORDER "II" // Little-endian format for Intel systems
// #define TIFF_TAG_VERSION 42
// #define TAG_OFFSET_IFD0 8
// #define EXIF_TAG_COUNT 6 // Adjust this for the number of tags you are adding

// /* The ExifTag struct is wrapped in #pragma pack(push, 1) and #pragma pack(pop)
//  * directives to ensure that the struct members are packed tightly without any padding bytes.
//  * The 1 argument in #pragma pack(push, 1) specifies that the alignment should be set to 1 byte.
//  */
// #pragma pack(push, 1)
// typedef struct
// {
//     uint16_t tag;
//     uint16_t type;
//     uint32_t count;
//     uint32_t valueOffset;
// } ExifTag;
// #pragma pack(pop)

// /*
//  * Writes an EXIF tag to the buffer
//  *
//  * @param ptr: Pointer to buffer
//  * @param tag: Tag ID
//  * @param type: Data type
//  * @param count: Number of values
//  * @param valueOffset: Offset to value
//  * @return: Pointer to next tag
//  */
// uint8_t *writeExifTag(uint8_t *ptr, uint16_t tag, uint16_t type, uint32_t count, uint32_t valueOffset)
// {
//     ExifTag *exifTag = (ExifTag *)ptr;
//     exifTag->tag = tag;
//     exifTag->type = type;
//     exifTag->count = count;
//     exifTag->valueOffset = valueOffset;
//     return ptr + sizeof(ExifTag);
// }

// int createAPP1Block(ImageMetadata *metadata, unsigned char *buffer, int bufferSize)
// {
//     const uint16_t APP1_MARKER = 0xFFE1;
//     const uint16_t TIFF_MAGIC = 42;
//     const uint32_t IFD_OFFSET = 8;
//     const uint16_t NUM_ENTRIES = 4; // Sorted, no Compression
//     const uint16_t IFD_ENTRY_SIZE = 12;

//     const int MIN_SIZE = 2 + 2 + 6 + 8 + 2 + (NUM_ENTRIES * IFD_ENTRY_SIZE) + 4 + 100;
//     if (bufferSize < MIN_SIZE)
//     {
//         xprintf("Buffer too small for APP1 block. Need %d bytes, got %d\n", MIN_SIZE, bufferSize);
//         return 0;
//     }

//     int currentPos = 0;
//     buffer[currentPos++] = (APP1_MARKER >> 8) & 0xFF;
//     buffer[currentPos++] = APP1_MARKER & 0xFF;

//     int lengthPos = currentPos;
//     currentPos += 2;

//     const char exifHeader[] = {'E', 'x', 'i', 'f', 0, 0};
//     memcpy(&buffer[currentPos], exifHeader, 6);
//     currentPos += 6;

//     int tiffHeaderStart = currentPos;

//     buffer[currentPos++] = 'I'; // Byte order: Little endian
//     buffer[currentPos++] = 'I';
//     buffer[currentPos++] = TIFF_MAGIC & 0xFF;
//     buffer[currentPos++] = (TIFF_MAGIC >> 8) & 0xFF;
//     buffer[currentPos++] = IFD_OFFSET & 0xFF;
//     buffer[currentPos++] = (IFD_OFFSET >> 8) & 0xFF;
//     buffer[currentPos++] = (IFD_OFFSET >> 16) & 0xFF;
//     buffer[currentPos++] = (IFD_OFFSET >> 24) & 0xFF;

//     buffer[currentPos++] = NUM_ENTRIES & 0xFF;
//     buffer[currentPos++] = (NUM_ENTRIES >> 8) & 0xFF;

//     uint32_t valuesStartOffset = IFD_OFFSET + 2 + (NUM_ENTRIES * IFD_ENTRY_SIZE) + 4;
//     uint32_t xresOffset = valuesStartOffset;
//     uint32_t yresOffset = xresOffset + 8;

//     // === Sorted IFD Entries ===

//     // 0x011A XResolution
//     struct
//     {
//         uint16_t tag;
//         uint16_t type;
//         uint32_t count;
//         uint32_t value;
//     } xres = {
//         0x011A, 5, 1, xresOffset};
//     memcpy(&buffer[currentPos], &xres, IFD_ENTRY_SIZE);
//     currentPos += IFD_ENTRY_SIZE;

//     // 0x011B YResolution
//     struct
//     {
//         uint16_t tag;
//         uint16_t type;
//         uint32_t count;
//         uint32_t value;
//     } yres = {
//         0x011B, 5, 1, yresOffset};
//     memcpy(&buffer[currentPos], &yres, IFD_ENTRY_SIZE);
//     currentPos += IFD_ENTRY_SIZE;

//     // 0x0128 ResolutionUnit
//     struct
//     {
//         uint16_t tag;
//         uint16_t type;
//         uint32_t count;
//         uint16_t value;
//         uint16_t pad;
//     } resUnit = {
//         0x0128, 3, 1, 2, 0};
//     memcpy(&buffer[currentPos], &resUnit, IFD_ENTRY_SIZE);
//     currentPos += IFD_ENTRY_SIZE;

//     // 0x0213 YCbCrPositioning
//     struct
//     {
//         uint16_t tag;
//         uint16_t type;
//         uint32_t count;
//         uint16_t value;
//         uint16_t pad;
//     } ycbcr = {
//         0x0213, 3, 1, 1, 0};
//     memcpy(&buffer[currentPos], &ycbcr, IFD_ENTRY_SIZE);
//     currentPos += IFD_ENTRY_SIZE;

//     // No IFD1
//     memset(&buffer[currentPos], 0, 4);
//     currentPos += 4;

//     // === RATIONAL values ===
//     int valuePos = tiffHeaderStart + xresOffset;

//     // XResolution = 72/1
//     buffer[valuePos++] = 72;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 1;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;

//     // YResolution = 72/1
//     buffer[valuePos++] = 72;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 1;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;
//     buffer[valuePos++] = 0;

//     currentPos = valuePos;
//     xprintf("DEBUG Metadata:\nMediaID: %s\nDeploymentID: %s\nCaptureMethod: %s\nLat: %f\nLon: %f\nTimestamp: %s\nFavourite: %s\n",
//             metadata->mediaID,
//             metadata->deploymentID,
//             metadata->captureMethod,
//             metadata->latitude,
//             metadata->longitude,
//             metadata->timestamp,
//             metadata->favourite ? "true" : "false");

//     // Optional metadata
//     int remainingSize = bufferSize - currentPos;
//     if (remainingSize > 0)
//     {
//         int written = snprintf((char *)&buffer[currentPos], remainingSize,
//                                "MediaID:%s\nDeploymentID:%s\nCaptureMethod:%s\n"
//                                "Latitude:%.6f\nLongitude:%.6f\nTimestamp:%s\nFavourite:%d\n",
//                                metadata->mediaID, metadata->deploymentID, metadata->captureMethod,
//                                metadata->latitude, metadata->longitude, metadata->timestamp,
//                                metadata->favourite);
//         if (written > 0 && written < remainingSize)
//             currentPos += written;
//     }

//     // Finalize APP1 length
//     uint16_t app1Length = currentPos - 2;
//     buffer[lengthPos] = (app1Length >> 8) & 0xFF;
//     buffer[lengthPos + 1] = app1Length & 0xFF;

//     xprintf("APP1 Block length: %d (0x%04X)\n", app1Length, app1Length);
//     return currentPos;
// }

// /*
//  * For debugging purposes, prints the APP1 block content to CMD
//  * @param app1Size: Size of APP1 block
//  * @param app1Block: Pointer to APP1 block
//  * @return: void
//  */
// void printEXIF(size_t app1Size, char *app1Block)
// {
//     // Debug output - show the APP1 block content
//     xprintf("EXIF Block (Hex): ");
//     for (size_t i = 0; i < app1Size; i++)
//     {
//         xprintf("%02X ", app1Block[i]);
//         if ((i + 1) % 16 == 0)
//             xprintf("\n"); // Print a newline every 16 bytes
//     }
//     xprintf("\n");

//     xprintf("EXIF Block (Decoded):\n");
//     for (size_t i = 0; i < app1Size; i++)
//     {
//         if (app1Block[i] >= 32 && app1Block[i] <= 126)
//         { // Printable ASCII
//             xprintf("%c", app1Block[i]);
//         }
//         else if (app1Block[i] == '\n')
//         {
//             xprintf("\n");
//         }
//         else
//         {
//             xprintf(".");
//         }
//     }
//     xprintf("\n");
// }