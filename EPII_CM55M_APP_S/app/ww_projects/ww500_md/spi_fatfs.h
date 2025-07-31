/*
 * spi_fatfs.h
 *
 */
#include "directory_manager.h"


//int fatfs_init(bool printDiskInfo);
int fastfs_write_image(uint32_t SRAM_addr, uint32_t img_size, uint8_t *filename, directoryManager_t *dirManager);
