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
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

// Define specific tags
#define TAG_MAKE 0x010F           // Standard EXIF tag for manufacturer
#define TAG_MODEL 0x0110          // Standard EXIF tag for model
#define TAG_DATETIME 0x0132       // Standard EXIF tag for date/time
#define TAG_MEDIA_ID 0x8769       // Custom tag for media ID (using EXIF IFD pointer)
#define TAG_DEPLOYMENT_ID 0x927C  // Custom tag for deployment ID (using Maker Note)
#define TAG_CAPTURE_METHOD 0xA005 // Custom tag (using Interop IFD pointer)
#define TAG_FAVOURITE 0xA010      // Custom tag for favourite flag

// GPS related tags
#define TAG_GPS_IFD 0x8825     // GPS IFD pointer
#define TAG_GPS_LAT_REF 0x0001 // GPS latitude reference (N/S)
#define TAG_GPS_LAT 0x0002     // GPS latitude
#define TAG_GPS_LON_REF 0x0003 // GPS longitude reference (E/W)
#define TAG_GPS_LON 0x0004     // GPS longitude

const uint8_t exif_header[] = {
    0xFF, 0xE1,                     // APP1 Marker
    0x00, 0x2A,                     // Length (will need adjustment based on final size)
    'E', 'x', 'i', 'f', 0x00, 0x00, // Exif identifier
    0x49, 0x49, 0x2A, 0x00,         // TIFF header (little endian)
    0x08, 0x00, 0x00, 0x00,         // Offset to first IFD
    0x01, 0x00,                     // Number of directory entries

    // IFD entry for custom field (using 0x0F01 as in your example)
    0x01, 0x0F,             // Tag (0x0F01)
    0x02, 0x00,             // Type: ASCII String
    0x0A, 0x00, 0x00, 0x00, // Count: 10 bytes
    0x26, 0x00, 0x00, 0x00, // Offset to data

    // Value for field (considering 'Himax\0' padded to 10 bytes)
    'H', 'i', 'm', 'a', 'x', 0x00, 0x00, 0x00, 0x00, 0x00};

// uint8_t *add_exif_to_buffer(uint8_t *jpeg_buffer, size_t jpeg_size, size_t *new_size, imageMetadata *metadata)
// {
//     if (!jpeg_buffer || jpeg_size < 2 || !metadata)
//         return NULL;

//     // Ensure buffer starts with JPEG SOI (0xFFD8)
//     if (jpeg_buffer[0] != 0xFF || jpeg_buffer[1] != 0xD8)
//         return NULL; // Not a valid JPEG

//     // Calculate string lengths (including null terminator)
//     size_t media_id_len = strnlen(metadata->mediaID, 9) + 1;
//     size_t deployment_id_len = strnlen(metadata->deploymentID, 9) + 1;
//     size_t capture_method_len = strnlen(metadata->captureMethod, 9) + 1;
//     size_t timestamp_len = strnlen(metadata->timestamp, 19) + 1;

//     // Size of TIFF header + IFD entry count
//     const size_t tiff_header_size = 8;
//     const size_t ifd_entry_size = 12;

//     // Calculate number of IFD entries
//     const int num_ifd_entries = 6; // Make, Model, DateTime, MediaID, Favourite, GPS IFD pointer
//     const int num_gps_entries = 4; // GPS Lat Ref, GPS Lat, GPS Lon Ref, GPS Lon

//     // Calculate offsets
//     uint32_t ifd_offset = 8; // Offset to first IFD from TIFF header
//     uint32_t gps_ifd_offset = ifd_offset + 2 + (num_ifd_entries * ifd_entry_size) + 4;
//     uint32_t string_data_offset = gps_ifd_offset + 2 + (num_gps_entries * ifd_entry_size) + 4;

//     // Calculate total EXIF data size
//     size_t exif_data_size = string_data_offset +
//                             8 + // "Himax\0\0\0"
//                             7 + // "Camera\0"
//                             timestamp_len +
//                             media_id_len +
//                             deployment_id_len +
//                             capture_method_len +
//                             24; // GPS data (4 values, 6 bytes each)

//     // Allocate buffer for EXIF data
//     uint8_t *exif_data = (uint8_t *)pvPortMalloc(exif_data_size);
//     if (!exif_data)
//         return NULL;

//     uint8_t *p = exif_data;

//     // Write TIFF header (little endian)
//     *p++ = 0x49;
//     *p++ = 0x49; // "II" (Intel byte order)
//     *p++ = 0x2A;
//     *p++ = 0x00; // TIFF identifier
//     *p++ = 0x08;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Offset to first IFD

//     // Write main IFD entry count
//     *p++ = num_ifd_entries & 0xFF;
//     *p++ = (num_ifd_entries >> 8) & 0xFF;

//     // Current position for string data
//     uint32_t current_str_offset = string_data_offset;

//     // Write Make tag (0x010F)
//     *p++ = TAG_MAKE & 0xFF;
//     *p++ = (TAG_MAKE >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII string
//     *p++ = 0x06;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 6 bytes
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += 8; // "Himax\0\0\0" (padded to 8 bytes)

//     // Write Model tag (0x0110)
//     *p++ = TAG_MODEL & 0xFF;
//     *p++ = (TAG_MODEL >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII string
//     *p++ = 0x07;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 7 bytes
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += 7; // "Camera\0"

//     // Write DateTime tag (0x0132)
//     *p++ = TAG_DATETIME & 0xFF;
//     *p++ = (TAG_DATETIME >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII string
//     *p++ = timestamp_len & 0xFF;
//     *p++ = (timestamp_len >> 8) & 0xFF;
//     *p++ = (timestamp_len >> 16) & 0xFF;
//     *p++ = (timestamp_len >> 24) & 0xFF;
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += timestamp_len;

//     // Write MediaID tag (using EXIF IFD pointer tag)
//     *p++ = TAG_MEDIA_ID & 0xFF;
//     *p++ = (TAG_MEDIA_ID >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII string
//     *p++ = media_id_len & 0xFF;
//     *p++ = (media_id_len >> 8) & 0xFF;
//     *p++ = (media_id_len >> 16) & 0xFF;
//     *p++ = (media_id_len >> 24) & 0xFF;
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += media_id_len;

//     // Write DeploymentID tag (using Maker Note tag)
//     *p++ = TAG_DEPLOYMENT_ID & 0xFF;
//     *p++ = (TAG_DEPLOYMENT_ID >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII string
//     *p++ = deployment_id_len & 0xFF;
//     *p++ = (deployment_id_len >> 8) & 0xFF;
//     *p++ = (deployment_id_len >> 16) & 0xFF;
//     *p++ = (deployment_id_len >> 24) & 0xFF;
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += deployment_id_len;

//     // Write Favourite tag
//     *p++ = TAG_FAVOURITE & 0xFF;
//     *p++ = (TAG_FAVOURITE >> 8) & 0xFF;
//     *p++ = 0x01;
//     *p++ = 0x00; // Type: BYTE
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 1
//     uint8_t fav_value = metadata->favourite ? 1 : 0;
//     *p++ = fav_value;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Inline data (value padded to 4 bytes)

//     // Write GPS IFD pointer
//     *p++ = TAG_GPS_IFD & 0xFF;
//     *p++ = (TAG_GPS_IFD >> 8) & 0xFF;
//     *p++ = 0x04;
//     *p++ = 0x00; // Type: LONG
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 1
//     *p++ = gps_ifd_offset & 0xFF;
//     *p++ = (gps_ifd_offset >> 8) & 0xFF;
//     *p++ = (gps_ifd_offset >> 16) & 0xFF;
//     *p++ = (gps_ifd_offset >> 24) & 0xFF;

//     // Write next IFD offset (0 = no more IFDs)
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00;

//     // Write GPS IFD
//     *p++ = num_gps_entries & 0xFF;
//     *p++ = (num_gps_entries >> 8) & 0xFF;

//     // Write GPS latitude reference
//     *p++ = TAG_GPS_LAT_REF & 0xFF;
//     *p++ = (TAG_GPS_LAT_REF >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII
//     *p++ = 0x02;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 2
//     char lat_ref = metadata->latitude >= 0 ? 'N' : 'S';
//     *p++ = lat_ref;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Inline data: 'N' or 'S'

//     // Write GPS latitude
//     *p++ = TAG_GPS_LAT & 0xFF;
//     *p++ = (TAG_GPS_LAT >> 8) & 0xFF;
//     *p++ = 0x05;
//     *p++ = 0x00; // Type: RATIONAL
//     *p++ = 0x03;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 3
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;
//     current_str_offset += 24; // 3 rationals (8 bytes each)

//     // Write GPS longitude reference
//     *p++ = TAG_GPS_LON_REF & 0xFF;
//     *p++ = (TAG_GPS_LON_REF >> 8) & 0xFF;
//     *p++ = 0x02;
//     *p++ = 0x00; // Type: ASCII
//     *p++ = 0x02;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 2
//     char lon_ref = metadata->longitude >= 0 ? 'E' : 'W';
//     *p++ = lon_ref;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Inline data: 'E' or 'W'

//     // Write GPS longitude
//     *p++ = TAG_GPS_LON & 0xFF;
//     *p++ = (TAG_GPS_LON >> 8) & 0xFF;
//     *p++ = 0x05;
//     *p++ = 0x00; // Type: RATIONAL
//     *p++ = 0x03;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Count: 3
//     *p++ = current_str_offset & 0xFF;
//     *p++ = (current_str_offset >> 8) & 0xFF;
//     *p++ = (current_str_offset >> 16) & 0xFF;
//     *p++ = (current_str_offset >> 24) & 0xFF;

//     // Write next IFD offset (0 = no more IFDs)
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00;

//     // Write string data
//     memcpy(p, "Himax\0\0\0", 8);
//     p += 8;

//     memcpy(p, "Camera\0", 7);
//     p += 7;

//     memcpy(p, metadata->timestamp, timestamp_len);
//     p += timestamp_len;

//     memcpy(p, metadata->mediaID, media_id_len);
//     p += media_id_len;

//     memcpy(p, metadata->deploymentID, deployment_id_len);
//     p += deployment_id_len;

//     memcpy(p, metadata->captureMethod, capture_method_len);
//     p += capture_method_len;

//     // Write GPS data (convert to rationals)
//     // For latitude
//     float lat = fabs(metadata->latitude);
//     int lat_deg = (int)lat;
//     float lat_min = (lat - lat_deg) * 60.0f;
//     int lat_min_int = (int)lat_min;
//     float lat_sec = (lat_min - lat_min_int) * 60.0f;

//     // Degrees as rational
//     *p++ = (lat_deg) & 0xFF;
//     *p++ = (lat_deg >> 8) & 0xFF;
//     *p++ = (lat_deg >> 16) & 0xFF;
//     *p++ = (lat_deg >> 24) & 0xFF;
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 1

//     // Minutes as rational
//     *p++ = (lat_min_int) & 0xFF;
//     *p++ = (lat_min_int >> 8) & 0xFF;
//     *p++ = (lat_min_int >> 16) & 0xFF;
//     *p++ = (lat_min_int >> 24) & 0xFF;
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 1

//     // Seconds as rational (multiply by 100 for precision)
//     uint32_t lat_sec_int = (uint32_t)(lat_sec * 100);
//     *p++ = (lat_sec_int) & 0xFF;
//     *p++ = (lat_sec_int >> 8) & 0xFF;
//     *p++ = (lat_sec_int >> 16) & 0xFF;
//     *p++ = (lat_sec_int >> 24) & 0xFF;
//     *p++ = 0x64;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 100

//     // For longitude
//     float lon = fabs(metadata->longitude);
//     int lon_deg = (int)lon;
//     float lon_min = (lon - lon_deg) * 60.0f;
//     int lon_min_int = (int)lon_min;
//     float lon_sec = (lon_min - lon_min_int) * 60.0f;

//     // Degrees as rational
//     *p++ = (lon_deg) & 0xFF;
//     *p++ = (lon_deg >> 8) & 0xFF;
//     *p++ = (lon_deg >> 16) & 0xFF;
//     *p++ = (lon_deg >> 24) & 0xFF;
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 1

//     // Minutes as rational
//     *p++ = (lon_min_int) & 0xFF;
//     *p++ = (lon_min_int >> 8) & 0xFF;
//     *p++ = (lon_min_int >> 16) & 0xFF;
//     *p++ = (lon_min_int >> 24) & 0xFF;
//     *p++ = 0x01;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 1

//     // Seconds as rational (multiply by 100 for precision)
//     uint32_t lon_sec_int = (uint32_t)(lon_sec * 100);
//     *p++ = (lon_sec_int) & 0xFF;
//     *p++ = (lon_sec_int >> 8) & 0xFF;
//     *p++ = (lon_sec_int >> 16) & 0xFF;
//     *p++ = (lon_sec_int >> 24) & 0xFF;
//     *p++ = 0x64;
//     *p++ = 0x00;
//     *p++ = 0x00;
//     *p++ = 0x00; // Denominator: 100

//     // Now create the JPEG APP1 segment
//     uint16_t app1_size = 2 + 6 + exif_data_size; // 2 bytes for length, 6 for "Exif\0\0"
//     xprintf("JPEGKPEGjpeg_size: %d, app1_size: %d\n", jpeg_size, app1_size);
//     // Calculate new total buffer size
//     *new_size = jpeg_size + app1_size;

//     // Allocate new buffer
//     uint8_t *new_buffer = (uint8_t *)pvPortMalloc(*new_size);
//     if (!new_buffer)
//     {
//         free(exif_data);
//         return NULL;
//     }

//     // Copy SOI marker
//     new_buffer[0] = jpeg_buffer[0]; // 0xFF
//     new_buffer[1] = jpeg_buffer[1]; // 0xD8

//     // Write APP1 marker
//     new_buffer[2] = 0xFF;
//     new_buffer[3] = 0xE1;

//     // Write APP1 length (exclude marker itself)
//     uint16_t app1_length = app1_size - 2;
//     new_buffer[4] = (app1_length >> 8) & 0xFF;
//     new_buffer[5] = app1_length & 0xFF;

//     // Write Exif identifier
//     memcpy(new_buffer + 6, "Exif\0\0", 6);

//     // Copy EXIF data
//     memcpy(new_buffer + 12, exif_data, exif_data_size);

//     // Copy the rest of the JPEG data (skipping original SOI)
//     memcpy(new_buffer + 12 + exif_data_size, jpeg_buffer + 2, jpeg_size - 2);
//     xprintf("MEOWfirst");
//     // Free the temporary buffer
//     vPortFree(exif_data);

//     xprintf("MEOOOWnew_size: %d\n", *new_size);

//     return new_buffer;
// }

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

    // Copy the rest of the original JPEG data
    memcpy(new_buffer + 2 + sizeof(exif_header) + sizeof(imageMetadata), jpeg_buffer + 2, jpeg_size - 2);

    return new_buffer;
}

// int insert_exif_metadata(uint8_t *jpeg_buffer, uint32_t jpeg_size, uint32_t *new_size, imageMetadata *metadata)
// {
//     if (!jpeg_buffer || !metadata)
//     {
//         return -1; // Invalid input
//     }

//     // JPEG SOI marker check
//     if (jpeg_buffer[0] != 0xFF || jpeg_buffer[1] != 0xD8)
//     {
//         return -2; // Not a valid JPEG file
//     }

//     // EXIF header
//     uint8_t exif_header[EXIF_HEADER_SIZE] = {
//         0xFF, 0xE1,                    // APP1 Marker
//         0x00, 0x2C,                    // Segment size (44 bytes, can be updated later)
//         'E', 'x', 'i', 'f', 0x00, 0x00 // EXIF header
//     };

//     // TIFF header (Intel little-endian format)
//     uint8_t tiff_header[TIFF_HEADER_SIZE] = {
//         0x49, 0x49,            // Little-endian
//         0x2A, 0x00,            // TIFF Magic Number
//         0x08, 0x00, 0x00, 0x00 // Offset to first IFD (8 bytes after TIFF header)
//     };

//     // IFD entries (minimal example with 3 entries)
//     uint8_t ifd_entries[NUM_IFD_ENTRIES * IFD_ENTRY_SIZE] = {
//         0x01,
//         0x0F,
//         0x02,
//         0x00, // Custom Tag 0x0F01 (ASCII string)
//         0x0A,
//         0x00,
//         0x00,
//         0x00, // Data size: 10 bytes
//         0x26,
//         0x00,
//         0x00,
//         0x00, // Offset to string data (relative to TIFF)

//         0x02,
//         0x0F,
//         0x02,
//         0x00, // Custom Tag 0x0F02 (ASCII string)
//         0x0A,
//         0x00,
//         0x00,
//         0x00,
//         0x30,
//         0x00,
//         0x00,
//         0x00,

//         0x03,
//         0x0F,
//         0x02,
//         0x00, // Custom Tag 0x0F03 (ASCII string)
//         0x0A,
//         0x00,
//         0x00,
//         0x00,
//         0x3A,
//         0x00,
//         0x00,
//         0x00,
//     };

//     // Metadata string values
//     char tag_data[30];
//     snprintf(tag_data, sizeof(tag_data), "%.10s%.10s%.10s", metadata->mediaID, metadata->deploymentID, metadata->captureMethod);

//     xprintf("woofjpeg_size: %d\n", jpeg_size);
//     xprintf("Woofjpeg_buffer size: %d\n", sizeof(jpeg_buffer));

//     // Create a new buffer for the updated JPEG
//     xprintf(sizeof(exif_header), sizeof(tiff_header), sizeof(ifd_entries), sizeof(tag_data));
//     uint32_t new_jpeg_size = jpeg_size + sizeof(exif_header) + sizeof(tiff_header) + sizeof(ifd_entries) + sizeof(tag_data);
//     uint8_t *new_jpeg_buffer = (uint8_t *)pvPortMalloc(new_jpeg_size);
//     if (!new_jpeg_buffer)
//     {
//         return -3; // Memory allocation failed
//     }

//     // Construct the new JPEG with EXIF metadata
//     uint8_t *ptr = new_jpeg_buffer;
//     memcpy(ptr, jpeg_buffer, 2); // Copy SOI marker
//     ptr += 2;
//     memcpy(ptr, exif_header, sizeof(exif_header));
//     ptr += sizeof(exif_header);
//     memcpy(ptr, tiff_header, sizeof(tiff_header));
//     ptr += sizeof(tiff_header);
//     memcpy(ptr, ifd_entries, sizeof(ifd_entries));
//     ptr += sizeof(ifd_entries);
//     memcpy(ptr, tag_data, sizeof(tag_data));
//     ptr += sizeof(tag_data);
//     memcpy(ptr, jpeg_buffer + 2, jpeg_size - 2); // Copy the rest of the JPEG

//     xprintf("MEOWjpeg_size: %d\n", jpeg_size);
//     xprintf("MEOWjpeg_buffer size: %d\n", sizeof(jpeg_buffer));

//     // Replace old buffer with the new one
//     jpeg_buffer = NULL;
//     jpeg_buffer = new_jpeg_buffer;
//     new_size = new_jpeg_size;

//     xprintf("jpeg_size: %d\n", new_size);
//     xprintf("jpeg_buffer size: %d\n", sizeof(jpeg_buffer));
//     return jpeg_buffer; // Success
// }
