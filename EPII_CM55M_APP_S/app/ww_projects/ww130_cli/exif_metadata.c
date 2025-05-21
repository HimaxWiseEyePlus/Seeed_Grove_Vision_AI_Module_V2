/**
 * exif_metadata.c
 * created on: 15.03.2025
 * author: 	TBP
 *
 * @brief Metadata functions to add custom metadata to JPEG images via manually inserting data into the APP1 block
 * */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "exif_metadata.h"
#include "printf_x.h"
#include "xprintf.h"
#include <math.h>
#include <stdbool.h>
#include "spi_fatfs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "cvapp.h"

// Align the offset to the next 4-byte boundary
#define ALIGN_OFFSET(offset) (((offset) + 3) & ~3)

/**
 * This function is called to set the EXIF metadata for the image
 */
int initialize_metadata(ImageMetadata *metadata, ModelResults model_scores, UINT file_dir_idx)
{
    int ret = 0;
    // Convert file_dir_idx to string
    char file_dir_idx_str[16];
    snprintf(file_dir_idx_str, sizeof(file_dir_idx_str), "%04d", file_dir_idx);
    memset(metadata->deploymentId, 0, sizeof(metadata->deploymentId));
    strncpy(metadata->deploymentId, file_dir_idx_str, sizeof(metadata->deploymentId) - 1);

    // "Wildlife Watcher" is the static EXIF project name for now
    memset(metadata->deploymentProject, 0, sizeof(metadata->deploymentProject));
    strncpy(metadata->deploymentProject, "Wildlife Watcher", sizeof(metadata->deploymentProject) - 1);

    // rat_score
    char rat_score_str[5]; // Currently for result values between -100 and 100
    snprintf(rat_score_str, sizeof(rat_score_str), "%4d", model_scores.rat_score);
    memset(metadata->modelCategoryPositive, 0, sizeof(metadata->modelCategoryPositive));
    strncpy(metadata->modelCategoryPositive, rat_score_str, sizeof(metadata->modelCategoryPositive) - 1);

    // no_rat_score
    char no_rat_score_str[5];
    snprintf(no_rat_score_str, sizeof(no_rat_score_str), "%4d", model_scores.no_rat_score);
    memset(metadata->modelCategoryNegative, 0, sizeof(metadata->modelCategoryNegative));
    strncpy(metadata->modelCategoryNegative, no_rat_score_str, sizeof(metadata->modelCategoryNegative) - 1);

    // GPS
    metadata->latitude = -39.3538;
    metadata->longitude = 174.4383;

    // Error checking. Is this necessary for these values?
    // We could probably just carry on even if any of these aren't set
    if (strlen(metadata->deploymentId) == 0 || strlen(metadata->deploymentProject) == 0)
    {
        return -1;
    }

    return ret;
}
// Function to write a rational value (2 uint32 values: numerator and denominator)
static uint32_t write_rational(unsigned char *buffer, uint32_t offset, uint32_t num, uint32_t denom)
{
    WRITE32LE(buffer + offset, num);
    WRITE32LE(buffer + offset + 4, denom);
    return offset + 8;
}

// Create EXIF block with GPS and custom data
static uint32_t create_exif_block(unsigned char *buffer, ImageMetadata *data)
{
    uint32_t offset = 0;
    uint16_t entryCount = 0;
    uint32_t ifd0Offset = 0;
    uint32_t gpsIfdOffset = 0;
    uint32_t valueOffset = 0;

    // EXIF header marker
    memcpy(buffer + offset, "Exif\0\0", 6);
    offset += 6;

    // TIFF header
    uint32_t tiffHeaderStart = offset;
    memcpy(buffer + offset, "II", 2); // Little endian
    offset += 2;
    WRITE16LE(buffer + offset, 0x002A); // TIFF magic number
    offset += 2;
    WRITE32LE(buffer + offset, 8); // Offset to first IFD
    offset += 4;

    // IFD0 will start at offset 8 from TIFF header
    ifd0Offset = tiffHeaderStart + 8;

    // Calculate where values will be stored (after IFD entries)
    // We'll have 6 entries in IFD0: Orientation, XResolution, YResolution, YCbCrPositioning, ExifIFD, and GPS IFD
    entryCount = 7;
    valueOffset = ifd0Offset + 2 + (entryCount * IFD_ENTRY_SIZE) + 4;

    // Write IFD0 entry count
    WRITE16LE(buffer + ifd0Offset, entryCount);
    offset = ifd0Offset + 2;

    // Entry 1: Orientation (short - 0x0112), value = 1 (normal)
    WRITE16LE(buffer + offset, 0x0112); // Tag
    WRITE16LE(buffer + offset + 2, 3);  // Type: SHORT
    WRITE32LE(buffer + offset + 4, 1);  // Count
    WRITE16LE(buffer + offset + 8, 1);  // Value (normal orientation)
    WRITE16LE(buffer + offset + 10, 0); // Padding
    offset += IFD_ENTRY_SIZE;

    // Entry 2: XResolution (rational - 0x011A), default 72 dpi
    WRITE16LE(buffer + offset, 0x011A);                            // Tag
    WRITE16LE(buffer + offset + 2, 5);                             // Type: RATIONAL
    WRITE32LE(buffer + offset + 4, 1);                             // Count
    WRITE32LE(buffer + offset + 8, valueOffset - tiffHeaderStart); // Pointer
    offset += IFD_ENTRY_SIZE;
    write_rational(buffer, valueOffset, 72, 1); // 72/1 dpi
    valueOffset += 8;

    // Entry 3: YResolution (rational - 0x011B), default 72 dpi
    WRITE16LE(buffer + offset, 0x011B);                            // Tag
    WRITE16LE(buffer + offset + 2, 5);                             // Type: RATIONAL
    WRITE32LE(buffer + offset + 4, 1);                             // Count
    WRITE32LE(buffer + offset + 8, valueOffset - tiffHeaderStart); // Pointer
    offset += IFD_ENTRY_SIZE;
    write_rational(buffer, valueOffset, 72, 1); // 72/1 dpi
    valueOffset += 8;

    // Entry 4: ResolutionUnit (0x0128), value = 2 (inches)
    WRITE16LE(buffer + offset, 0x0128); // Tag
    WRITE16LE(buffer + offset + 2, 3);  // Type: SHORT
    WRITE32LE(buffer + offset + 4, 1);  // Count
    WRITE16LE(buffer + offset + 8, 2);  // Value (2 = inches)
    WRITE16LE(buffer + offset + 10, 0); // Padding
    offset += IFD_ENTRY_SIZE;

    // Entry 4: YCbCrPositioning (0x0213), value = 1 (centered)
    WRITE16LE(buffer + offset, 0x0213); // Tag
    WRITE16LE(buffer + offset + 2, 3);  // Type: SHORT
    WRITE32LE(buffer + offset + 4, 1);  // Count
    WRITE16LE(buffer + offset + 8, 1);  // Value (centered)
    WRITE16LE(buffer + offset + 10, 0); // Padding
    offset += IFD_ENTRY_SIZE;

    // Entry 5: ExifIFD Pointer (must come before GPS IFD for proper tag order)
    uint32_t privateIfdOffset = valueOffset + 200;                      // Allocate space for next IFD
    WRITE16LE(buffer + offset, 0x8769);                                 // ExifIFD tag
    WRITE16LE(buffer + offset + 2, 4);                                  // Type: LONG
    WRITE32LE(buffer + offset + 4, 1);                                  // Count
    WRITE32LE(buffer + offset + 8, privateIfdOffset - tiffHeaderStart); // Pointer
    offset += IFD_ENTRY_SIZE;

    // Entry 6: GPS IFD Pointer
    gpsIfdOffset = valueOffset;
    WRITE16LE(buffer + offset, 0x8825);                             // Tag (0x8825)
    WRITE16LE(buffer + offset + 2, 4);                              // Type: LONG
    WRITE32LE(buffer + offset + 4, 1);                              // Count
    WRITE32LE(buffer + offset + 8, gpsIfdOffset - tiffHeaderStart); // Pointer
    offset += IFD_ENTRY_SIZE;

    // Next IFD offset (0 = no more IFDs)
    WRITE32LE(buffer + offset, 0);
    offset += 4;

    // GPS IFD
    if (data->latitude != 0 || data->longitude != 0)
    {
        // Convert decimal degrees to degrees, minutes, seconds
        double lat_abs = fabs(data->latitude);
        int lat_deg = (int)lat_abs;
        double lat_min_all = (lat_abs - lat_deg) * 60;
        int lat_min = (int)lat_min_all;
        double lat_sec = (lat_min_all - lat_min) * 60;

        double lon_abs = fabs(data->longitude);
        int lon_deg = (int)lon_abs;
        double lon_min_all = (lon_abs - lon_deg) * 60;
        int lon_min = (int)lon_min_all;
        double lon_sec = (lon_min_all - lon_min) * 60;

        // Latitude reference: N or S
        char lat_ref = data->latitude >= 0 ? 'N' : 'S';
        // Longitude reference: E or W
        char lon_ref = data->longitude >= 0 ? 'E' : 'W';

        // Write GPS IFD
        offset = gpsIfdOffset;
        uint16_t gpsEntryCount = 5; // GPS version, lat ref, lat, lon ref, lon
        WRITE16LE(buffer + offset, gpsEntryCount);
        offset += 2;

        // Allocate space for values after all entries
        uint32_t gpsValueOffset = gpsIfdOffset + 2 + (gpsEntryCount * IFD_ENTRY_SIZE) + 4;

        // GPS Version ID (0x0000): 2.2.0.0
        WRITE16LE(buffer + offset, 0x0000); // Tag
        WRITE16LE(buffer + offset + 2, 1);  // Type: BYTE
        WRITE32LE(buffer + offset + 4, 4);  // Count
        // Store directly since it fits in 4 bytes
        buffer[offset + 8] = 2;  // Major version
        buffer[offset + 9] = 2;  // Minor version
        buffer[offset + 10] = 0; // Release
        buffer[offset + 11] = 0; // Reserved
        offset += IFD_ENTRY_SIZE;

        // GPS Latitude Ref (0x0001): 'N' or 'S'
        WRITE16LE(buffer + offset, 0x0001); // Tag
        WRITE16LE(buffer + offset + 2, 2);  // Type: ASCII
        WRITE32LE(buffer + offset + 4, 2);  // Count (including null)
        // Store directly since it fits in 4 bytes
        buffer[offset + 8] = lat_ref;
        buffer[offset + 9] = 0;  // Null terminator
        buffer[offset + 10] = 0; // Padding
        buffer[offset + 11] = 0; // Padding
        offset += IFD_ENTRY_SIZE;

        // GPS Latitude (0x0002): [degrees, minutes, seconds]
        WRITE16LE(buffer + offset, 0x0002); // Tag
        WRITE16LE(buffer + offset + 2, 5);  // Type: RATIONAL
        WRITE32LE(buffer + offset + 4, 3);  // Count
        WRITE32LE(buffer + offset + 8, gpsValueOffset - tiffHeaderStart);
        offset += IFD_ENTRY_SIZE;

        // Write latitude values
        gpsValueOffset = write_rational(buffer, gpsValueOffset, lat_deg, 1);
        gpsValueOffset = write_rational(buffer, gpsValueOffset, lat_min, 1);
        gpsValueOffset = write_rational(buffer, gpsValueOffset, (uint32_t)(lat_sec * 1000), 1000);

        // GPS Longitude Ref (0x0003): 'E' or 'W'
        WRITE16LE(buffer + offset, 0x0003); // Tag
        WRITE16LE(buffer + offset + 2, 2);  // Type: ASCII
        WRITE32LE(buffer + offset + 4, 2);  // Count (including null)
        // Store directly since it fits in 4 bytes
        buffer[offset + 8] = lon_ref;
        buffer[offset + 9] = 0;  // Null terminator
        buffer[offset + 10] = 0; // Padding
        buffer[offset + 11] = 0; // Padding
        offset += IFD_ENTRY_SIZE;

        // GPS Longitude (0x0004): [degrees, minutes, seconds]
        WRITE16LE(buffer + offset, 0x0004); // Tag
        WRITE16LE(buffer + offset + 2, 5);  // Type: RATIONAL
        WRITE32LE(buffer + offset + 4, 3);  // Count
        WRITE32LE(buffer + offset + 8, gpsValueOffset - tiffHeaderStart);
        offset += IFD_ENTRY_SIZE;

        // Write longitude values
        gpsValueOffset = write_rational(buffer, gpsValueOffset, lon_deg, 1);
        gpsValueOffset = write_rational(buffer, gpsValueOffset, lon_min, 1);
        gpsValueOffset = write_rational(buffer, gpsValueOffset, (uint32_t)(lon_sec * 1000), 1000);

        // Write zero for next IFD offset
        WRITE32LE(buffer + offset, 0);
        offset += 4;

        // Update offset to the next section after GPS values
        offset = gpsValueOffset;
    }

    // Private IFD for custom tags
    uint32_t customTagOffset = privateIfdOffset;
    uint16_t customEntryCount = 4 + 6; // 4 custom tags + 5 required EXIF tags
    WRITE16LE(buffer + customTagOffset, customEntryCount);
    customTagOffset += 2;

    // Value offsets come after all IFD entries
    uint32_t customValueOffset = privateIfdOffset + 2 + (customEntryCount * IFD_ENTRY_SIZE) + 4;

    // Required EXIF tags (Should be added before custom tags, in ascending order)

    // ExifVersion (0x9000) = "0231"
    WRITE16LE(buffer + customTagOffset, 0x9000);
    WRITE16LE(buffer + customTagOffset + 2, 7); // UNDEFINED
    WRITE32LE(buffer + customTagOffset + 4, 4); // Count
    // Store directly since it fits in 4 bytes
    buffer[customTagOffset + 8] = '0';
    buffer[customTagOffset + 9] = '2';
    buffer[customTagOffset + 10] = '3';
    buffer[customTagOffset + 11] = '1';
    customTagOffset += IFD_ENTRY_SIZE;

    // ComponentsConfiguration (0x9101) = 1 2 3 0 (YCBCR)
    WRITE16LE(buffer + customTagOffset, 0x9101);
    WRITE16LE(buffer + customTagOffset + 2, 7); // UNDEFINED
    WRITE32LE(buffer + customTagOffset + 4, 4); // Count
    // Store directly since it fits in 4 bytes
    buffer[customTagOffset + 8] = 1;  // Y
    buffer[customTagOffset + 9] = 2;  // Cb
    buffer[customTagOffset + 10] = 3; // Cr
    buffer[customTagOffset + 11] = 0; // Padding
    customTagOffset += IFD_ENTRY_SIZE;

    // FlashpixVersion (0xA000) = "0100"
    WRITE16LE(buffer + customTagOffset, 0xA000);
    WRITE16LE(buffer + customTagOffset + 2, 7); // UNDEFINED
    WRITE32LE(buffer + customTagOffset + 4, 4); // Count
    // Store directly since it fits in 4 bytes
    buffer[customTagOffset + 8] = '0';
    buffer[customTagOffset + 9] = '1';
    buffer[customTagOffset + 10] = '0';
    buffer[customTagOffset + 11] = '0';
    customTagOffset += IFD_ENTRY_SIZE;

    // ColorSpace (0xA001) = 1 (sRGB)
    WRITE16LE(buffer + customTagOffset, 0xA001);
    WRITE16LE(buffer + customTagOffset + 2, 3);  // SHORT
    WRITE32LE(buffer + customTagOffset + 4, 1);  // Count
    WRITE16LE(buffer + customTagOffset + 8, 1);  // sRGB value
    WRITE16LE(buffer + customTagOffset + 10, 0); // Padding
    customTagOffset += IFD_ENTRY_SIZE;

    // ExifImageWidth (0xA002) = 640 (from the JPEG)
    WRITE16LE(buffer + customTagOffset, 0xA002);
    WRITE16LE(buffer + customTagOffset + 2, 3);   // SHORT
    WRITE32LE(buffer + customTagOffset + 4, 1);   // Count
    WRITE16LE(buffer + customTagOffset + 8, 640); // Image width
    WRITE16LE(buffer + customTagOffset + 10, 0);  // Padding
    customTagOffset += IFD_ENTRY_SIZE;

    // ExifImageHeight (0xA003) = 480 (from the JPEG)
    WRITE16LE(buffer + customTagOffset, 0xA003);
    WRITE16LE(buffer + customTagOffset + 2, 3);   // SHORT
    WRITE32LE(buffer + customTagOffset + 4, 1);   // Count
    WRITE16LE(buffer + customTagOffset + 8, 480); // Image height
    WRITE16LE(buffer + customTagOffset + 10, 0);  // Padding
    customTagOffset += IFD_ENTRY_SIZE;

    // Deployment ID
    uint32_t aligned_offset = ALIGN_OFFSET(customValueOffset);
    memset(buffer + customValueOffset, 0, aligned_offset - customValueOffset); // Pad with zeros
    customValueOffset = aligned_offset;
    uint32_t deploymentIdLen = strlen(data->deploymentId) + 1; // +1 for null
    WRITE16LE(buffer + customTagOffset, TAG_DEPLOYMENT_ID);
    WRITE16LE(buffer + customTagOffset + 2, 2); // ASCII
    WRITE32LE(buffer + customTagOffset + 4, deploymentIdLen);
    WRITE32LE(buffer + customTagOffset + 8, customValueOffset - tiffHeaderStart);
    customTagOffset += IFD_ENTRY_SIZE;
    // Write string value
    memcpy(buffer + customValueOffset, data->deploymentId, deploymentIdLen);
    customValueOffset += deploymentIdLen;

    // Ensure even alignment
    if (customValueOffset % 2 != 0)
    {
        buffer[customValueOffset] = 0; // Add padding byte
        customValueOffset++;
    }

    // Deployment Project
    aligned_offset = ALIGN_OFFSET(customValueOffset);
    memset(buffer + customValueOffset, 0, aligned_offset - customValueOffset); // Pad with zeros
    customValueOffset = aligned_offset;
    uint32_t projectLen = strlen(data->deploymentProject) + 1;
    WRITE16LE(buffer + customTagOffset, TAG_DEPLOYMENT_PROJECT);
    WRITE16LE(buffer + customTagOffset + 2, 2); // ASCII
    WRITE32LE(buffer + customTagOffset + 4, projectLen);
    WRITE32LE(buffer + customTagOffset + 8, customValueOffset - tiffHeaderStart);
    customTagOffset += IFD_ENTRY_SIZE;
    // Write string value
    memcpy(buffer + customValueOffset, data->deploymentProject, projectLen);
    customValueOffset += projectLen;

    // NN results "for"
    aligned_offset = ALIGN_OFFSET(customValueOffset);
    memset(buffer + customValueOffset, 0, aligned_offset - customValueOffset); // Pad with zeros
    customValueOffset = aligned_offset;
    uint32_t obsIdLen = strlen(data->modelCategoryPositive) + 1;
    WRITE16LE(buffer + customTagOffset, TAG_MODEL_FOR);
    WRITE16LE(buffer + customTagOffset + 2, 2); // ASCII
    WRITE32LE(buffer + customTagOffset + 4, obsIdLen);
    WRITE32LE(buffer + customTagOffset + 8, customValueOffset - tiffHeaderStart);
    customTagOffset += IFD_ENTRY_SIZE;
    // Write string value
    memcpy(buffer + customValueOffset, data->modelCategoryPositive, obsIdLen);
    customValueOffset += obsIdLen;

    // NN results "against"
    aligned_offset = ALIGN_OFFSET(customValueOffset);
    memset(buffer + customValueOffset, 0, aligned_offset - customValueOffset); // Pad with zeros
    customValueOffset = aligned_offset;
    uint32_t obsTypeLen = strlen(data->modelCategoryNegative) + 1;
    WRITE16LE(buffer + customTagOffset, TAG_MODEL_AGAINST);
    WRITE16LE(buffer + customTagOffset + 2, 2); // ASCII
    WRITE32LE(buffer + customTagOffset + 4, obsTypeLen);
    WRITE32LE(buffer + customTagOffset + 8, customValueOffset - tiffHeaderStart);
    customTagOffset += IFD_ENTRY_SIZE;
    // Write string value
    memcpy(buffer + customValueOffset, data->modelCategoryNegative, obsTypeLen);
    customValueOffset += obsTypeLen;

    // Write zero for next IFD offset
    WRITE32LE(buffer + customTagOffset, 0);
    customTagOffset += 4;

    // Return total size of the EXIF block
    return customValueOffset > offset ? customValueOffset : offset;
}

int insert_exif(const char *fileName, ImageMetadata *exifData)
{
    FRESULT res;
    FIL srcFile, tmpFile;
    UINT bytesRead, bytesWritten;
    char tmpFileName[64];
    uint8_t buffer[WRITE_BUFFER_SIZE];
    uint8_t exifBlock[MAX_METADATA_SIZE];
    uint32_t exifSize;

    // Prepare temp file name
    snprintf(tmpFileName, sizeof(tmpFileName), "%s.tmp", fileName);

    // Open original file
    res = f_open(&srcFile, fileName, FA_READ);
    if (res != FR_OK)
    {
        xprintf("Failed to open source file: %s\n", fileName);
        return res;
    }

    // Open temp file
    res = f_open(&tmpFile, tmpFileName, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        xprintf("Failed to open temp file: %s\n", tmpFileName);
        f_close(&srcFile);
        return res;
    }

    // Step 1: Copy SOI marker (first 2 bytes)
    res = f_read(&srcFile, buffer, 2, &bytesRead);
    if (res != FR_OK || bytesRead != 2 || memcmp(buffer, "\xFF\xD8", 2) != 0)
    {
        xprintf("Invalid JPEG SOI marker\n");
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName); // delete temp file
        return FR_INVALID_OBJECT;
    }

    // Write SOI to temp
    res = f_write(&tmpFile, buffer, 2, &bytesWritten);
    if (res != FR_OK || bytesWritten != 2)
    {
        xprintf("Failed writing SOI marker\n");
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName);
        return res;
    }

    // Step 2: Write new APP1 (EXIF) block
    exifSize = create_exif_block(exifBlock, exifData);
    if (exifSize == 0 || exifSize > MAX_METADATA_SIZE)
    {
        xprintf("Invalid EXIF block size: %lu\n", exifSize);
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName);
        return FR_INVALID_OBJECT;
    }

    // Write APP1 marker 0xFFE1
    uint8_t app1Marker[2] = {0xFF, 0xE1};
    res = f_write(&tmpFile, app1Marker, 2, &bytesWritten);
    if (res != FR_OK || bytesWritten != 2)
    {
        xprintf("Failed writing APP1 marker\n");
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName);
        return res;
    }

    // Write APP1 block length (exifSize + 2 bytes for length itself)
    uint16_t app1Length = exifSize + 2;
    uint8_t app1LengthBytes[2] = {(uint8_t)(app1Length >> 8), (uint8_t)(app1Length & 0xFF)};
    res = f_write(&tmpFile, app1LengthBytes, 2, &bytesWritten);
    if (res != FR_OK || bytesWritten != 2)
    {
        xprintf("Failed writing APP1 length\n");
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName);
        return res;
    }

    // Write EXIF block
    res = f_write(&tmpFile, exifBlock, exifSize, &bytesWritten);
    if (res != FR_OK || bytesWritten != exifSize)
    {
        xprintf("Failed writing EXIF block\n");
        f_close(&srcFile);
        f_close(&tmpFile);
        f_unlink(tmpFileName);
        return res;
    }

    // Step 3: Copy rest of original JPEG
    do
    {
        res = f_read(&srcFile, buffer, WRITE_BUFFER_SIZE, &bytesRead);
        if (res != FR_OK)
        {
            xprintf("Failed reading source file\n");
            break;
        }
        if (bytesRead > 0)
        {
            res = f_write(&tmpFile, buffer, bytesRead, &bytesWritten);
            if (res != FR_OK || bytesWritten != bytesRead)
            {
                xprintf("Failed writing temp file\n");
                break;
            }
        }
    } while (bytesRead > 0);

    f_close(&srcFile);
    f_close(&tmpFile);

    if (res == FR_OK)
    {
        // Replace original file
        f_unlink(fileName);                    // Delete original
        res = f_rename(tmpFileName, fileName); // Rename temp to original
        if (res != FR_OK)
        {
            xprintf("Failed renaming temp file to original\n");
        }
    }
    else
    {
        f_unlink(tmpFileName); // Clean up temp file on error
    }

    return res;
}