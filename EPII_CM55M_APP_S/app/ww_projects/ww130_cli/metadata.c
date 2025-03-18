/*
 * metadata.c
 *
 * Created on: 18/03/2024
 * Author: TBP
 *
 * Source file for metadata handling
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "metadata.h"

const uint8_t exif_header[] = {
    0xFF, 0xE1, 0x00, 0x2E,                         // APP1 Marker, Length 0x002E (46 bytes)
    'E', 'x', 'i', 'f', 0x00, 0x00,                 // EXIF Identifier
    0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00, // TIFF Header
    0x01, 0x00,                                     // Number of IFD Entries
    0x01, 0x0F,                                     // Tag: Make (0x010F)
    0x02, 0x00,                                     // Type: ASCII String
    0x0A, 0x00, 0x00, 0x00,                         // Data Length: 10 bytes
    0x26, 0x00, 0x00, 0x00,                         // Offset to Data
    'H', 'i', 'm', 'a', 'x', 0x00, 0x00, 0x00       // "Himax\0"
};

/*
 * Inserts the EXIF data into the image buffer
 */
uint8_t *add_exif_to_buffer(uint8_t *jpeg_buffer, size_t jpeg_size, size_t *new_size, imageMetadata *metadata)
{
    if (!jpeg_buffer || jpeg_size < 2)
        return NULL;

    // Ensure buffer starts with JPEG SOI (0xFFD8)
    if (jpeg_buffer[0] != 0xFF || jpeg_buffer[1] != 0xD8)
    {
        return NULL; // Not a valid JPEG
    }

    // Calculate new buffer
    size_t exif_size = sizeof(exif_header) + sizeof(imageMetadata);
    *new_size = jpeg_size + exif_size;
    uint8_t *new_buffer = (uint8_t *)pvPortMalloc(*new_size);
    if (!new_buffer)
        return NULL;

    // Insert SOI marker (FFD8)
    new_buffer[0] = 0xFF;
    new_buffer[1] = 0xD8;

    // Insert EXIF data
    memcpy(new_buffer + 2, exif_header, sizeof(exif_header));
    memcpy(new_buffer + 2 + sizeof(exif_header), metadata, sizeof(imageMetadata));

    // Copy original JPEG data after EXIF
    memcpy(new_buffer + 2 + exif_size, jpeg_buffer + 2, jpeg_size - 2);

    return new_buffer;
}
