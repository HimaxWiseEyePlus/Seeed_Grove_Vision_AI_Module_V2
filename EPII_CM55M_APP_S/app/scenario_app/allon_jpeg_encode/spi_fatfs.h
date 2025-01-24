/*
 * spi_fatfs.h
 *
 */


int fatfs_init(void);
int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename);
