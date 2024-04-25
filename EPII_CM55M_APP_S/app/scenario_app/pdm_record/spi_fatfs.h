/*
 * spi_fatfs.h
 *
 */


int wav_header_init(int32_t srate, int16_t num_chans);
int fatfs_init(void);
int fastfs_write_audio(uint32_t SRAM_addr, uint32_t pcm_size, uint8_t *filename);
