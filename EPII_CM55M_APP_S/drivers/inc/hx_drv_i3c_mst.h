#ifndef _HX_DRV_I3C_MST_H_
#define _HX_DRV_I3C_MST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	I3C_MST_DRV	I3C_MST Hardware accelerator Driver
 * \ingroup		I3C_MST_DRV
 * \brief	I3C_MST Hardware accelerator Driver Declaration
 * \details Application can use I3C_MST Structure and library to control Hardware\n
 * <pre>
 * For Example
 *
 * </pre>
 */
/**
 * \defgroup	I3C_MST_DRV_ENUM	I3C_MST Hardware accelerator Driver Enumeration
 * \ingroup	I3C_MST_DRV
 * \brief	Defines some enumeration of I3C_MST Hardware accelerator Driver need.
 * @{
 */
/**
 * \enum I3C_MST_PROTOCOL_TYPE_E
 * \brief I3C_MST Prorocol Type
 */
typedef enum {
    I3C_LEGACY_MODE = 0x04, /**< legacy mode */
    I3C_SDR_MODE = 0x00, /**< SDR mode */
    I3C_BROADCAST_CCC_MODE = 0x01, /**< broadcast ccc mode */
    I3C_DIRECT_CCC_MODE = 0x02, /**< direct ccc mode */
    I3C_IPTP_MODE = 0x03, /**< IPTP mode */
    I3C_ENTDAA_CCC_MODE = 0x05, /**< ENTDAA mode */
    I3C_HDR_DDR_MODE = 0x08, /**< HDR DDR mode */
    I3C_HDR_DDR_BR_CCC_MODE = 0x09, /**< HDR DDR broadcast ccc mode */
    I3C_HDR_DDR_DIR_CCC_MODE = 0x0A, /**< HDR DDR direct ccc mode */
} I3C_MST_PROTOCOL_TYPE_E;
/**
 * \enum I3C_MST_DATA_MODE_E
 * \brief I3C_MST Data Transfer mode
 */
typedef enum {
    I3C_REG_MODE, /**< 4 byte reg mode */
	I3C_FIFO_MODE, /**< FIFO mode */
} I3C_MST_DATA_MODE_E;
/**
 * \enum I3C_MST_FIFO_STA_E
 * \brief I3C_MST FIFO Stat
 */
typedef enum {
    I3C_FIFO_EMPTY_STATUS = 0x01, /**< FIFO empty */
    I3C_FIFO_OCCUPY_STATUS = 0x02, /**< FIFO occupy */
    I3C_FIFO_UNDERRUN_STATUS = 0x04, /**< FIFO underrun */
    I3C_FIFO_OVERFLOW_STATUS = 0x08, /**< FIFO overflow */
    I3C_FIFO_HAVE_SPACE_STATUS = 0x10, /**< FIFO have space */
    I3C_FIFO_HAVE_DATA_STATUS = 0x20, /**< FIFO have data */
} I3C_MST_FIFO_STA_E;
/**
 * \enum I3C_MST_FIFO_IN_SEL_E
 * \brief I3C_MST FIFO Input Select
 */
typedef enum {
    I3C_FIFO_IN_HOST_INTERF = 0x00, /**< FIFO in from host */
    I3C_FIFO_IN_RESEVED = 0x01, /**< FIFO reserved */
    I3C_FIFO_IN_AHB_MASTER_DMA = 0x02, /**< FIFO in from AHB master */
    I3C_FIFO_IN_I3C = 0x03 /**< FIFO in from I3C */
} I3C_MST_FIFO_IN_SEL_E;

/**
 * \enum I3C_MST_FIFO_OUT_SEL_E
 * \brief I3C_MST FIFO Output Select
 */
typedef enum {
    I3C_FIFO_OUT_HOST_INTERF = 0X00, /**< FIFO out to host */
    I3C_FIFO_OUT_VIDEO_OUTPUT_INTERF = 0X01, /**< FIFO out to INP */
    I3C_FIFO_OUT_AHB_MASTER_DMA = 0X02, /**< FIFO out to AHB master */
    I3C_FIFO_OUT_I3C = 0X03 /**< FIFO out to I3C */
} I3C_MST_FIFO_OUT_SEL_E;

/**
 * \enum I3C_MST_CLK_SEL_E
 * \brief I3C_MST Clock Select
 */
typedef enum {
    I3C_CLK_HALF_RCLK = 0x00, /**< 1/2 reference clk */
    I3C_CLK_ONE_THIRD_RCLK = 0x01, /**< 1/3 reference clk */
    I3C_CLK_QUARTER = 0x02, /**< 1/4 reference clk */
    I3C_CLK_OTHER = 0x03 /**< other define */
} I3C_MST_CLK_SEL_E;

/**
 * \enum I3C_MST_SCL_CLK_DIV_SEL_E
 * \brief I3C_MST SCL Clock Select
 */
typedef enum{
    I3C_SCL_CLK_DIV_2, /**< 1/2 reference clk */
    I3C_SCL_CLK_DIV_3, /**< 1/3 reference clk */
    I3C_SCL_CLK_DIV_4, /**< 1/4 reference clk */
    I3C_SCL_CLK_DIV_8, /**< 1/8 reference clk */
    I3C_SCL_CLK_DIV_16, /**< 1/16 reference clk */
    I3C_SCL_CLK_DIV_32, /**< 1/32 reference clk */
    I3C_SCL_CLK_DIV_45, /**< 1/45 reference clk */
}I3C_MST_SCL_CLK_DIV_SEL_E;

/**
 * \enum I3C_MST_INT_STATE_E
 * \brief I3C_MST Interrupt Stat
 */
typedef enum{
    I3C_INT_IBI_REQ = 0x01,
    I3C_INT_MASTERSHIP_REQ = 0x02,
    I3C_INT_HOTJOIN_REQ = 0x04,
    I3C_INT_READ_ERR = 0x08,
    I3C_INT_M0_ERR = 0x10,
    I3C_INT_M1_REQ = 0x20,
    I3C_INT_M2_REQ = 0x40,
    I3C_INT_DMAC = 0x80,
    I3C_INT_AHB_DMA = 0x100,
    I3C_INT_NO_ACK = 0x200,
    I3C_INT_DDR_NO_ACK = 0x400,
    I3C_INT_DDR_END_REQ = 0x800,
}I3C_MST_INT_STATE_E;

/**
 * \enum I3CHC_ERROR_E
 * \brief I3CHC Errors Type
 */
typedef enum
{
    I3CHC_NO_ERROR                           = 0,    /**< No ERROR */
    I3CHC_ERROR_NOT_INIT                     = 1,    /**< ERROR MSG: not init */
    I3CHC_ERROR_ALREADY_INIT                 = 2,    /**< ERROR MSG: already init */
    I3CHC_ERROR_INVALID_PARAMETERS           = 3,    /**< ERROR MSG: Invalid parameters */
    I3CHC_UNKNOWN_ERROR                      = 4,    /**< ERROR MSG: UNKNOWN ERROR*/
    I3CHC_ENTDAA_FAIL                        = 5,    /**< ERROR MSG: ENTDAA FAIL*/
	I3CHC_FREQ_FAIL                        	 = 6,    /**< ERROR MSG: I3C FREQ FAIL*/
} I3CHC_ERROR_E;

/**
 * \enum I3CHC_STATUS_E
 * \brief I3C HC Interrupt Status
 */
typedef enum
{
    I3CHC_STATUS_NO                 = 0,    /**< No */
    I3CHC_STATUS_IBI_REQ            = 0x1,  /**< [0] : I3C slave issue IBI request */
    I3CHC_STATUS_MASTERSHIP_REQ     = 0x2,  /**< [1] : I3C slave issue mastership request */
    I3CHC_STATUS_HOTJOIN_REQ        = 0x4,   /**< [2] : I3C slave issue Hot-Join request */
    I3CHC_STATUS_RECV_NOMATCH       = 0x8,   /**< [3] : master receive total data byte number are not equal to register (REG_NUMBER_OF_DATA) setting*/
    I3CHC_STATUS_M0_ERROR           = 0x10, /**< [4] : M0 Error occur. Only for CCC command mode*/
    I3CHC_STATUS_M1_ERROR           = 0x20, /**< [5] : M1 Error occur. SDA bus data not equal to write data*/
    I3CHC_STATUS_M2_ERROR           = 0x40, /**< [6] : M2 Error occur. No slave reply ACK after master send broadcast address (7'h7E)*/
    I3CHC_STATUS_DMAC_FINISH        = 0x80, /**< [7] : DMAC Interrupt when data transfer finish*/
    I3CHC_STATUS_DMA_FINISH         = 0x100,    /**< [8] : AHB Master DMA interrupt when data transfer finish*/
    I3CHC_STATUS_NO_ACK             = 0x200,    /**< [9] : Slave Not reply ACK,*/
    I3CHC_STATUS_DDR_NO_ACK         = 0x400,    /**< [10] : Slave Not reply ACK after HDR-DDR CMD,*/
    I3CHC_STATUS_SLV_REQ_END        = 0x800,    /**< [11] : Slave Request End master write after HDR-DDR Write DATA,*/
} I3CHC_STATUS_E;
/** @} */


/**
 * \brief   I3C HC interrupt status callback function
 *
 * \param[in]   event    interrupt event
 * \return  void.
 */
typedef void (*I3CHC_ISREvent_t) (I3CHC_STATUS_E event);

/**
 * \brief   register I3C HC Interrupt Status callback
 *
 * \param[in]   cb_event     I3C HC interrupt callback function
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_register_cb(I3CHC_ISREvent_t  cb_event);

/**
 * \brief   clear I3C HC Interrupt Status
 *
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_clr_int_status();

/**
 * \brief   I3C HC init: init I3C controller and set I3C CLOCK
 *
 * \param[in]  i3c_clk    I3C controller clock frequency: ? KHz
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_init(uint32_t i3c_clk);

/**
 * \brief   set I3C controller to I3C SDR mode
 *
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_set_sdr_mode();

/**
 * \brief   set I3C controller to I3C Legacy(I2C) mode
 *
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_set_legacy_mode();

/**
 * \brief   Start ENTDAA process with input I3C slave ID
 *
 * * \param[in]  slv_addr    input I3C slave ID
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_entdaa(uint8_t slv_addr);

/**
 * \brief   Start I3C controller single write (data size <= 4)
 *
 * \param[in]  slave_addr    I3C slave ID
 * \param[in]  data     	 write data pointer
 * \param[in]  data_size     write data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_single_write(uint8_t slave_addr, uint8_t* data, uint32_t data_size);

/**
 * \brief   Start I3C controller single read (data size <= 4)
 *
 * \param[in]  slave_addr    I3C slave ID
 * \param[out]  data     	 read data pointer
 * \param[in]  data_size     read data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_single_read(uint8_t slave_addr, uint8_t* data, uint32_t data_size);

/**
 * \brief   Start I3C controller DMA write (data size > 4)
 *
 * \param[in]  slave_addr    I3C slave ID
 * \param[in]  data     	 write data pointer
 * \param[in]  data_size     write data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_dma_write(uint8_t slave_addr, uint8_t* data, uint32_t data_size);

/**
 * \brief   Start I3C controller DMA read (data size > 4)
 *
 * \param[in]  slave_addr    I3C slave ID
 * \param[out]  data     	 read data pointer
 * \param[in]  data_size     read data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_dma_read(uint8_t slave_addr, uint8_t* data, uint32_t data_size);

/**
 * \brief   Start I3C controller BROADCAST CCC
 *
 * \param[in]  ccc_code    		write CCC code
 * \param[in]  defining_byte    write defining_byte
 * \param[in]  data     		write data pointer
 * \param[in]  data_size        write data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_broadcast_ccc(uint8_t ccc_code, uint8_t defining_byte, uint8_t *data, uint32_t data_size);

/**
 * \brief   Start I3C controller DIRECT CCC WRITE
 *
 * \param[in]  slave_addr       I3C slave ID
 * \param[in]  ccc_code    		write CCC code
 * \param[in]  defining_byte    write defining_byte
 * \param[in]  data     		write data pointer
 * \param[in]  data_size        write data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_direct_ccc_write(uint8_t slave_addr, uint8_t ccc_code, uint8_t defining_byte, uint8_t *data, uint32_t data_size);

/**
 * \brief   Start I3C controller DIRECT CCC READ
 *
 * \param[in]  slave_addr       I3C slave ID
 * \param[in]  ccc_code    		write CCC code
 * \param[in]  defining_byte    write defining_byte
 * \param[in]  data     		read data pointer
 * \param[in]  data_size        read data size
 * \return  I3CHC_ERROR_E.
 */
I3CHC_ERROR_E hx_drv_i3chc_direct_ccc_read(uint8_t slave_addr, uint8_t ccc_code, uint8_t defining_byte, uint8_t *data, uint32_t data_size);

/**
 * \brief   Get IBI data
 *
 * \return  32 BIT IBI data
 */
uint32_t hx_drv_i3chc_get_ibi_data();
#endif
