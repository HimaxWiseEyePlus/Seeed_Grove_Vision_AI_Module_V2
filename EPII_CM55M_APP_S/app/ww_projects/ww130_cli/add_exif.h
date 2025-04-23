#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef ADD_EXIF_H
#define ADD_EXIF_H

// Function declarations
size_t addExifGPS(uint32_t SRAM_addr, size_t img_size, uint8_t *filename, double lat, double lon);
int create_exif_gps_block(unsigned char *buffer, double lat, double lon);

#endif // ADD_EXIF_H
