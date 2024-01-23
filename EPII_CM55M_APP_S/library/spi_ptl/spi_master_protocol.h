
#ifndef INC_SPI_M_PROTOCOL_H_
#define INC_SPI_M_PROTOCOL_H_

#include "dev_common.h"
#include "spi_protocol.h"

#define SPI_FLASH_PTL (0)
typedef void (*spi_mst_protocol_cb_t)(void);

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \brief	spi master 1 open device, speed with set to 2Mhz
     *
     * \retval	0					success
     * \retval	-1					fail
     */
    extern int hx_drv_spi_mst_open();

    /**
     * \brief	spi master 1 open device with target speed.
     *			Ex.
     *				//set spi master 1 speed to 50Mhz
     *				if(hx_drv_spi_mst_open_speed(50000000) != 0)
     *					return ;
     *
     * \param[in]	clk_hz			clock speed value between 0 to 50000000Hz
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_open_speed(uint32_t clk_hz);

    /**
     * \brief	spi master 1 use protocol to send image out. data type in the header will be "JPG = 0x01".
     *
     * \param[in]	SRAM_addr	 	address of image data to transfer
     * \param[in]	img_size	 	image data size in bytes
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_write(uint32_t SRAM_addr, uint32_t img_size);
#if 0
/**
 * \brief	spi master 1 use protocol to send image out. header_len should be described at upper layer.
 * 			This is a custom API for send data
 *
 * \param[in]	SRAM_addr		address of image data to transfer
 * \param[in]	img_size		image data size in bytes
 * \param[in]	header_len	 	packet header length
 * \retval		0				success
 * \retval		-1				fail
 */
extern int hx_drv_spi_mst_protocol_write_cus(uint32_t SRAM_addr, uint32_t img_size, uint32_t header_len);
#endif
    /**
     * \brief	spi master 1 use protocol to send data out. data type should be specified.
     *
     * \param[in]	SRAM_addr	 	address of data to transfer
     * \param[in]	img_size	 	data size
     * \param[in]	data_type	 	data type
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_write_sp(uint32_t SRAM_addr, uint32_t img_size, SPI_CMD_DATA_TYPE data_type);

    /**
     * \brief	spi master 1 use protocol to send data out. data type should be specified. 
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
    extern int hx_drv_spi_mst_protocol_write_ex(uint32_t SRAM_addr, uint32_t img_size, SPI_CMD_DATA_TYPE data_type, 
                                                uint8_t* extra_header, uint32_t exter_heahe_len);
    /**
     * \brief	spi master 1 force to stop current transmission.
     *
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_write_halt();

    /**
     * \brief	spi master 1 use protocol to read data. this is a blocking function which maximum wait 3 sec for spi
     * slave ready and read data back. during the transmission, spi master wait spi slave ready flag via GPIO handshake
     * pin. spi master read header packet back, this should includes data length information given by spi slave. spi
     * master then read target size of data back.
     *
     * \param[in]	SRAM_addr		address of data to read back
     * \param[out]	img_size		read back data size
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_read(uint32_t SRAM_addr, uint32_t *img_size);

    /**
     * \brief	spi master 1 register callback function for send data finish.
     *
     * \param[in]	aRWritecb	 	callback function set to driver, driver will trigger this callback function when
     * send data finish. \retval		0				success \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_register_tx_cb(spi_mst_protocol_cb_t aRWritecb);

    /**
     * \brief	spi master 1 register callback function for read data finish.
     *
     * \param[in]	aReadcb	 	callback function set to driver, driver will trigger this callback function when
     * read data finish. \retval		0				success \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_register_rx_cb(spi_mst_protocol_cb_t aReadcb);

    /**
     * \brief	spi master 1 use protocol to send data out. data type should be specified.
     * img_size must be multiples of 4.
     * use this function to send data from FLash 
     * 
     * \param[in]	SRAM_addr	 	address of data to transfer
     * \param[in]	img_size	 	data size
     * \param[in]	data_type	 	data type
     * \param[in]	word_switch	 	data endian switch. 
     * \retval		0				success
     * \retval		-1				fail
     */
    extern int hx_drv_spi_mst_protocol_write_word_sp(uint32_t SRAM_addr, uint32_t img_size, SPI_CMD_DATA_TYPE data_type, uint8_t word_switch);
#ifdef __cplusplus
}
#endif
#endif
