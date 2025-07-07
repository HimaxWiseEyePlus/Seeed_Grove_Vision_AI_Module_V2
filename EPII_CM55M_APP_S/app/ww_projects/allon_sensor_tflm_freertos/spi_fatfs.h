/*
 * spi_fatfs.h
 *
 */


int fatfs_init(void);
int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename);
int insert_exif_in_memory(uint32_t input_image, uint32_t output_image, size_t max_output_size, size_t* output_size);
