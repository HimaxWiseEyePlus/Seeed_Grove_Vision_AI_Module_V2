/*
 * spi_fatfs.h
 *
 */

#include <stdint.h>

uint16_t fatfs_init(void);
int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename);
uint16_t fatfs_saveSequenceNumber(uint16_t sequenceNumber);


