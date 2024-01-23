#ifndef INC_SPI_S_PROTOCOL_H_
#define INC_SPI_S_PROTOCOL_H_

#include "dev_common.h"
#include "spi_protocol.h"

typedef void (*spi_slv_protocol_cb_t) (void );

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	initial spi slave device
 *
 * \param[in]	none
 * \return	status of operation, return 0 is pass
 */
extern int hx_drv_spi_slv_open();

/**
 * \brief	spi slave device read data with protocol. protocl header length = 7
 *
 * \param[in]	SRAM_addr	 System memory address for data storage
 * \param[in]	img_size	 data size pointer, which will save size of read data
 * \return	status of operation, return 0 is pass, -1 is fail, -2 is spi not yet enable by I2C command
 */
extern int hx_drv_spi_slv_protocol_read_simple(uint32_t SRAM_addr, uint32_t *img_size );

#if 0
/**
 * \brief	spi slave device read data with protocol(method with user-defined protocl)
 *
 * \param[in]	SRAM_addr	 System memory address for data storage
 * \param[in]	img_size	 data size pointer, which will save size of read data
 * \param[in]   header_len   header size pointer, which will save lenght of protocol header.
 * \return	status of operation, return 0 is pass, -1 is fail, -2 is spi not yet enable by I2C command
 */
extern int hx_drv_spi_slv_protocol_write_simple_cus(uint32_t SRAM_addr, uint32_t img_size, uint32_t header_len);
#endif 

/**
 * \brief	spi slave device write data with protocol (non-blocking)
 *
 * \param[in]	SRAM_addr	 System memory address for data write out
 * \param[in]	img_size	 data size to write out
 * \return	status of operation, return 0 is pass, -1 is fail, -2 is spi not yet enable by I2C command
 */
extern int hx_drv_spi_slv_protocol_write_simple(uint32_t SRAM_addr, uint32_t img_size);

/**
 * \brief	spi slave device write data with protocol (non-blocking). data type should be specified. 
 *
 * \param[in]	SRAM_addr	 System memory address for data write out
 * \param[in]	img_size	 data size to write out
 * \param[in]	data_type	data type to write , reference SPI_CMD_DATA_TYPE
 * \return	status of operation, return 0 is pass, -1 is fail, -2 is spi not yet enable by I2C command
 */
extern int hx_drv_spi_slv_protocol_write_simple_sp(uint32_t SRAM_addr, uint32_t img_size, SPI_CMD_DATA_TYPE data_type);

/**
 * \brief	spi slave device write data with protocol (non-blocking). data type should be specified. 
 * 			Additional information can be addad to the header.
 *
 * \param[in]	SRAM_addr		    address of image data to transfer
 * \param[in]	img_size	    	image data size in bytes
 * \param[in]	header_len	    	packet header length
 * \param[in]   extra_header        extern header   
 * \param[in]   extra_header_len    extern header length. should < 17.
 * \retval		0				    success
 * \retval		-1				    fail
 */
extern int hx_drv_spi_slv_protocol_write_simple_ex(uint32_t SRAM_addr, uint32_t img_size, SPI_CMD_DATA_TYPE data_type,
                                                uint8_t* extra_header, uint32_t extra_header_len);
/**
 * \brief	check spi slave device write status
 *
 * \param[in]	none
 * \return	status of operation, return 0 is okay to read/write, -1 is in busy status
 */
extern int hx_drv_spi_slv_protocol_write_busy_status();

/**
 * \brief	stop spi slave device from write operation
 *
 * \param[in]	none
 * \return	status of operation, return 0 is halt operation done, -1 is halt fail
 */
extern int hx_drv_spi_slv_protocol_write_halt();

/**
 * \brief	spi slave register callback function for send data finish.
 *
 * \param[in]	aRWritecb	 	callback function set to driver, driver will trigger this callback function when
 * send data finish. \retval		0				success \retval		-1				fail
 */
extern int hx_drv_spi_slv_protocol_register_tx_cb(spi_slv_protocol_cb_t aRWritecb);

/**
 * \brief	spi slave register callback function for read data finish.
 *
 * \param[in]	aReadcb	 	callback function set to driver, driver will trigger this callback function when
 * read data finish. \retval		0				success \retval		-1				fail
 */
extern int hx_drv_spi_slv_protocol_register_rx_cb(spi_slv_protocol_cb_t aReadcb);

#ifdef __cplusplus
}
#endif
#endif
