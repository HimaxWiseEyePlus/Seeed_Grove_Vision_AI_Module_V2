#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "add_exif.h"
#include "printf_x.h"
#include "xprintf.h"
#include <stdbool.h>
#include "spi_fatfs.h"
#include "ff.h"

#define IFD_ENTRY_SIZE 12
#define EXIF_BLOCK_SIZE 512
#define WRITE16LE(buf, val)             \
    do                                  \
    {                                   \
        (buf)[0] = (val) & 0xFF;        \
        (buf)[1] = ((val) >> 8) & 0xFF; \
    } while (0)
#define WRITE32LE(buf, val)              \
    do                                   \
    {                                    \
        (buf)[0] = (val) & 0xFF;         \
        (buf)[1] = ((val) >> 8) & 0xFF;  \
        (buf)[2] = ((val) >> 16) & 0xFF; \
        (buf)[3] = ((val) >> 24) & 0xFF; \
    } while (0)

// Helper to write a rational (numerator/denominator) as 8 bytes
void write_rational(uint8_t *buf, uint32_t num, uint32_t denom)
{
    WRITE32LE(buf, num);
    WRITE32LE(buf + 4, denom);
}

// Returns the size of the EXIF block
int create_exif_gps_block(unsigned char *buffer, double lat, double lon)
{
    // Zero-initialize the buffer to avoid garbage data
    memset(buffer, 0, EXIF_BLOCK_SIZE);

    // Offsets (relative to TIFF header start)
    const int tiffHeaderStart = 10;
    const int exifHeaderSize = 6;
    const int ifd0Offset = 8;      // after TIFF header
    const int xresOffset = 100;    // arbitrary, after IFDs
    const int yresOffset = 108;    // arbitrary, after xres
    const int gpsIfdOffset = 116;  // after yres
    const int gpsDataOffset = 200; // after GPS IFD

    // --- APP1 marker and length ---
    buffer[0] = 0xFF;
    buffer[1] = 0xE1;
    buffer[2] = 0;
    buffer[3] = 0; // length placeholder

    // --- EXIF header ---
    memcpy(buffer + 4, "Exif\0\0", exifHeaderSize);

    // --- TIFF header ---
    int pos = tiffHeaderStart;
    buffer[pos++] = 'I';
    buffer[pos++] = 'I';
    buffer[pos++] = 0x2A;
    buffer[pos++] = 0x00;
    WRITE32LE(buffer + pos, ifd0Offset);
    pos += 4;

    // --- IFD0 entries ---
    int currentPos = tiffHeaderStart + ifd0Offset;
    uint16_t numEntries = 5;
    WRITE16LE(buffer + currentPos, numEntries);
    currentPos += 2;

    // 0x011A XResolution (RATIONAL)
    WRITE16LE(buffer + currentPos, 0x011A);         // Tag
    WRITE16LE(buffer + currentPos + 2, 5);          // Type: RATIONAL
    WRITE32LE(buffer + currentPos + 4, 1);          // Count: 1
    WRITE32LE(buffer + currentPos + 8, xresOffset); // Value: offset to XResolution
    currentPos += 12;
    // 0x011B YResolution (RATIONAL)
    WRITE16LE(buffer + currentPos, 0x011B);
    WRITE16LE(buffer + currentPos + 2, 5);
    WRITE32LE(buffer + currentPos + 4, 1);
    WRITE32LE(buffer + currentPos + 8, yresOffset);
    currentPos += 12;
    // 0x0128 ResolutionUnit (SHORT, value=2)
    WRITE16LE(buffer + currentPos, 0x0128);
    WRITE16LE(buffer + currentPos + 2, 3);
    WRITE32LE(buffer + currentPos + 4, 1);
    WRITE16LE(buffer + currentPos + 8, 2); // Value: 2 (inches)
    buffer[currentPos + 10] = 0;
    buffer[currentPos + 11] = 0;
    currentPos += 12;
    // 0x0213 YCbCrPositioning (SHORT, value=1)
    WRITE16LE(buffer + currentPos, 0x0213);
    WRITE16LE(buffer + currentPos + 2, 3);
    WRITE32LE(buffer + currentPos + 4, 1);
    WRITE16LE(buffer + currentPos + 8, 1); // Value: 1 (centered)
    buffer[currentPos + 10] = 0;
    buffer[currentPos + 11] = 0;
    currentPos += 12;
    // 0x8825 GPSInfo (LONG, value=offset to GPS IFD)
    WRITE16LE(buffer + currentPos, 0x8825);
    WRITE16LE(buffer + currentPos + 2, 4);
    WRITE32LE(buffer + currentPos + 4, 1);
    WRITE32LE(buffer + currentPos + 8, gpsIfdOffset);
    currentPos += 12;
    // No IFD1
    memset(&buffer[currentPos], 0, 4);
    currentPos += 4;

    // --- RATIONAL values for XResolution and YResolution ---
    int valuePos = tiffHeaderStart + xresOffset;
    if (valuePos + 16 > EXIF_BLOCK_SIZE)
    {
        fprintf(stderr, "EXIF block overrun at X/YResolution!\n");
        return -1;
    }

    write_rational(buffer + valuePos, 72, 1); // XResolution = 72/1
    valuePos += 8;
    write_rational(buffer + valuePos, 72, 1); // YResolution = 72/1

    // --- GPS IFD entries ---
    int gpsIfdPos = tiffHeaderStart + gpsIfdOffset;
    if (gpsIfdPos + 2 + 5 * 12 + 4 > EXIF_BLOCK_SIZE)
    {
        fprintf(stderr, "EXIF block overrun at GPS IFD!\n");
        return -1;
    }
    uint16_t gpsNumEntries = 5;
    WRITE16LE(buffer + gpsIfdPos, gpsNumEntries);
    gpsIfdPos += 2;
    // 0x0000 GPSVersionID (BYTE[4])
    WRITE16LE(buffer + gpsIfdPos, 0x0000);
    WRITE16LE(buffer + gpsIfdPos + 2, 1);
    WRITE32LE(buffer + gpsIfdPos + 4, 4);
    buffer[gpsIfdPos + 8] = 2;
    buffer[gpsIfdPos + 9] = 3;
    buffer[gpsIfdPos + 10] = 0;
    buffer[gpsIfdPos + 11] = 0;
    gpsIfdPos += 12;
    // 0x0001 GPSLatitudeRef (ASCII, 2, offset)
    WRITE16LE(buffer + gpsIfdPos, 0x0001);
    WRITE16LE(buffer + gpsIfdPos + 2, 2); // Type: ASCII
    WRITE32LE(buffer + gpsIfdPos + 4, 2); // Count: 2
    // Store the value directly in the IFD entry instead of using an offset
    buffer[gpsIfdPos + 8] = (lat >= 0) ? 'N' : 'S';
    buffer[gpsIfdPos + 9] = '\0';
    buffer[gpsIfdPos + 10] = 0;
    buffer[gpsIfdPos + 11] = 0;
    gpsIfdPos += 12;
    // 0x0002 GPSLatitude (RATIONAL, 3, offset)
    WRITE16LE(buffer + gpsIfdPos, 0x0002);
    WRITE16LE(buffer + gpsIfdPos + 2, 5);
    WRITE32LE(buffer + gpsIfdPos + 4, 3);
    WRITE32LE(buffer + gpsIfdPos + 8, gpsDataOffset);
    gpsIfdPos += 12;
    // 0x0003 GPSLongitudeRef (ASCII, 2, offset)
    WRITE16LE(buffer + gpsIfdPos, 0x0003);
    WRITE16LE(buffer + gpsIfdPos + 2, 2); // Type: ASCII
    WRITE32LE(buffer + gpsIfdPos + 4, 2); // Count: 2
    // Store the value directly in the IFD entry instead of using an offset
    buffer[gpsIfdPos + 8] = (lon >= 0) ? 'E' : 'W';
    buffer[gpsIfdPos + 9] = '\0';
    buffer[gpsIfdPos + 10] = 0;
    buffer[gpsIfdPos + 11] = 0;
    gpsIfdPos += 12;
    // 0x0004 GPSLongitude (RATIONAL, 3, offset)
    WRITE16LE(buffer + gpsIfdPos, 0x0004);
    WRITE16LE(buffer + gpsIfdPos + 2, 5);
    WRITE32LE(buffer + gpsIfdPos + 4, 3);
    WRITE32LE(buffer + gpsIfdPos + 8, gpsDataOffset + 24);
    gpsIfdPos += 12;
    // No next IFD
    memset(&buffer[gpsIfdPos], 0, 4);

    // --- GPS data ---
    int gpsDataPos = tiffHeaderStart + gpsDataOffset;
    if (gpsDataPos + 52 > EXIF_BLOCK_SIZE)
    {
        fprintf(stderr, "EXIF block overrun at GPS data!\n");
        return -1;
    }

    // Print the GPS reference values that are now stored in the IFD entries
    printf("LatRef string: %c\n", (lat >= 0) ? 'N' : 'S');
    printf("LonRef string: %c\n", (lon >= 0) ? 'E' : 'W');

    // Latitude [deg, min, sec]
    double abs_lat = (lat >= 0) ? lat : -lat;
    int lat_deg = (int)abs_lat;
    int lat_min = (int)((abs_lat - lat_deg) * 60);
    double lat_sec = (abs_lat - lat_deg - lat_min / 60.0) * 3600.0;
    write_rational(buffer + gpsDataPos, lat_deg, 1);
    write_rational(buffer + gpsDataPos + 8, lat_min, 1);
    write_rational(buffer + gpsDataPos + 16, (uint32_t)(lat_sec * 10000), 10000);

    // Longitude [deg, min, sec]
    double abs_lon = (lon >= 0) ? lon : -lon;
    int lon_deg = (int)abs_lon;
    int lon_min = (int)((abs_lon - lon_deg) * 60);
    double lon_sec = (abs_lon - lon_deg - lon_min / 60.0) * 3600.0;
    write_rational(buffer + gpsDataPos + 24, lon_deg, 1);
    write_rational(buffer + gpsDataPos + 32, lon_min, 1);
    write_rational(buffer + gpsDataPos + 40, (uint32_t)(lon_sec * 10000), 10000);

    // --- Calculate total block size and fill in length ---
    int total_size = gpsDataPos + 52;
    if (total_size > EXIF_BLOCK_SIZE)
    {
        fprintf(stderr, "EXIF block overrun at final size!\n");
        return -1;
    }
    int app1_len = total_size - 2;
    buffer[2] = (app1_len >> 8) & 0xFF;
    buffer[3] = app1_len & 0xFF;
    return total_size;
}

// size_t addExifGPS(uint32_t SRAM_addr, size_t img_size, uint8_t *filename, double lat, double lon)
// {
//     FRESULT res; // FatFs function common result code
//     xprintf("Entered EXIFGPS");
//     uint32_t *in_buf = SRAM_addr;
//     size_t exif_block[EXIF_BLOCK_SIZE] = {0};
//     // int exif_size = create_exif_gps_block(exif_block, lat, lon);
//     size_t exif_size = 22;
//     if (exif_size < 0)
//     {
//         xprintf(stderr, "Failed to create EXIF block.\n");
//         return 1;
//     }
//     if (exif_size > EXIF_BLOCK_SIZE)
//     {
//         xprintf(stderr, "EXIF block size exceeds buffer!\n");
//         return 1;
//     }
//     xprintf("Heading into outbuf bby\n");
//     uint32_t out_buf[img_size + exif_size];
//     memcpy(out_buf, in_buf, 2);                                // Copy SOI
//     memcpy(out_buf + 2, exif_block, exif_size);                // Copy EXIF block
//     memcpy(out_buf + 2 + exif_size, in_buf + 2, img_size - 2); // Copy remaining image data
//                                                                // Output the buffer to stdout
//     res = fastfs_write_image(out_buf, img_size + exif_size, filename);
//     if (res != FR_OK)
//     {
//         fprintf(stderr, "Error writing file %s\n", filename);
//         return 1;
//     }
//     // res = fwrite(out_buf, 1, img_size + exif_size, stdout);
//     res = 0;
//     printf("Wrote %s with EXIF GPS (lat=%.6f, lon=%.6f)\n", filename, lat, lon);
//     return res;
// }