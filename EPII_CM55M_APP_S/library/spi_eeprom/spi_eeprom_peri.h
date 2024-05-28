#ifndef _LIB_SPI_SEEPROM_PERI_H
#define _LIB_SPI_SEEPROM_PERI_H
#include "vendor_eeprom_param.h"
#include "hx_drv_spi.h"

#define SPI_EEPROM_USE_DMA

#define SPI_REG_RANGE (0x200)

#define SPI_M_DMACTRLx_LL_MAX (251) // 6 + 250 x 4K ~= 1M
#define DMACTRLx_SIZE_MAX 0xFFF     // 4092
#define DMA_REQUEST_QSPI_TX 0
#define DMA_REQUEST_QSPI_RX 1
#define DMA_REQUEST_OSPI_TX 2
#define DMA_REQUEST_OSPI_RX 3

#define DW_SPI_SPI_EEPROM_WINBOND_CH0 0xEF
#define DW_SPI_SPI_EEPROM_MXIC_CH0 0xC2
#define DW_SPI_SPI_EEPROM_GIGADEV_CH0 0xC8

//0:QSPI; 2:OSPI
extern volatile int32_t spi_flash_TX_busy_flag[__SCB_DCACHE_LINE_SIZE/4];
//0:QSPI; 2:OSPI
extern volatile int32_t spi_flash_RX_busy_flag[__SCB_DCACHE_LINE_SIZE/4];

void spi_eeprom_transfer_error_cb();
int32_t word_switch_func(uint8_t *data, uint32_t len);

#ifdef SPI_EEPROM_XIP_RELATED
// int32_t hx_lib_spi_eeprom_SetXIP(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, FLASH_ACCESS_MODE_E xip_mode,
//                                  bool enable, bool xip_cont);
// void hx_lib_spi_eeprom_setXIP_para(DEV_SPI_PTR spi_obj_ptr, uint32_t mode);        
int32_t hx_lib_spi_eeprom_SetXIP(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, FLASH_ACCESS_MODE_E xip_mode,
                                 bool enable, bool xip_cont, SPI_EEPROM_PARAM_S* flash_config);
void hx_lib_spi_eeprom_setXIP_para(DEV_SPI_PTR spi_obj_ptr, uint32_t mode, SPI_EEPROM_PARAM_S* flash_config);
#endif 

#ifdef LIB_SPI_EEPROM_SET_QUAD_MODE_BIT
// void hx_lib_spi_eeprom_set_quad_mode(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, bool enable, CUR_FLASH_TYPE_E current_flash_type);
void hx_lib_spi_eeprom_set_quad_mode(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, bool enable, SPI_EEPROM_PARAM_S* flash_config);
#endif

#if defined(SPI_EEPROM_WRITE_RELATED) || defined(SPI_EEPROM_WORD_W_R_RELATED) || defined(LIB_SPI_EEPROM_SET_QUAD_MODE_BIT)
// int32_t hx_lib_spi_eeprom_setWriteEnable(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr);
// int32_t hx_lib_spi_eeprom_waitWIP(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr);
int32_t hx_lib_spi_eeprom_setWriteEnable(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, SPI_EEPROM_PARAM_S* flash_config);
int32_t hx_lib_spi_eeprom_waitWIP(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, SPI_EEPROM_PARAM_S* flash_config);
uint8_t hx_lib_spi_eeprom_readWEL(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, SPI_EEPROM_PARAM_S* flash_config);
#endif 

#if defined(SPI_EEPROM_CHIP_ERASE_ERLATED) || defined(SPI_EEPROM_SECTOR_ERASE_RELATED)
#if defined(LIB_SPI_EEPROM_SET_W_PROTECT_BIT)
// int32_t hx_lib_spi_eeprom_clear_write_protect(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, CUR_FLASH_TYPE_E current_flash_type);
int32_t hx_lib_spi_eeprom_clear_write_protect(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, SPI_EEPROM_PARAM_S* flash_config);
#endif
#endif

#ifdef LIB_SPI_EEPROM_USE_DMA
void hx_lib_spi_eeprom_get_dma_req(USE_DW_SPI_MST_E spi_id, uint32_t *tx_req, uint32_t *rx_req);
void hx_lib_spi_eeprom_set_DMA_config(DEV_DMAC_PTR dma_obj_ptr, uint32_t spi_id, SPI_FRAME_FORMAT_E format);
void hx_lib_spi_eeprom_DMA_send_recv(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, void *src,
                                            uint32_t src_size, void *dest, uint32_t dest_size);
void hx_lib_spi_eeprom_DMA_send(DEV_SPI_PTR spi_obj_ptr, DEV_DMAC_PTR dma_obj_ptr, void *src, uint32_t size);   
#endif

#if defined(HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ)
uint32_t hx_lib_spi_eeprom_polling_xfer_status(DEV_SPI_PTR dev_spi_eeprom, 
                                               DEV_DMAC_PTR dev_dma_eeprom, 
                                               const int status_bits, const int rx_dma_chan_id, const int tx_dma_chan_id);
#endif                                            
#endif /*_LIB_SPI_SEEPROM_PERI_H*/
