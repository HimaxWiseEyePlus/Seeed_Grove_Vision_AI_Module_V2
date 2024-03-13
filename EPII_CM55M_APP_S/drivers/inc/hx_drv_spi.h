/**
 * @file hx_drv_spi.h
 * @author himax/902449
 * @brief
 * @version V1.0.3
 * @date 2020-03-25
 *
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *
 */
#ifndef INC_HX_DRV_SPI_M_H_
#define INC_HX_DRV_SPI_M_H_

#include "dev_common.h"
#include "WE2_core.h"
#if defined(IP_INST_DMA1) || defined(IP_INST_DMA2)
#include "hx_drv_dmac.h"
#endif

/* bootlaodr/bootRom flag '
    flash type:FLASH_TYPE_WINBOND */
#ifdef BOOT_USED
#define BL_USE
#endif

/**
 * \defgroup SPI
 * \brief  Serial Peripheral Interface
 * \details WE2 has three sets of SPI Master, Standard SPI(SSPI mst), Quad SPI(QSPI), and Octal SPI(OSPI).
 *          WE2 has one sets of SPI Slave, Standard SPI(SSPI slv).
 * <pre>
 *      ///SPI MASTER
 *      Usage-1: SSPIM pin mux configuration and initialization
 *      ///The output pin of SSPIM is defined by the user application.
 *      hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
 *      hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
 *      hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
 *      hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 1);
 *      
 *      ///initialize SSPIM
 *      uint32_t max_spi_freq;
 *      DEV_SPI_PTR dev_spi_m;
 *      hx_drv_spi_mst_init(USE_DW_SPI_MST_S, DW_SPI_S_RELBASE);
 *      dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
 *      //get max spi output freq.
 *      dev_spi_m->spi_control(SPI_CMD_MST_UPDATE_SYSCLK, (SPI_CTRL_PARAM)&max_spi_freq);
 *      dev_spi_m->spi_open(DEV_MASTER_MODE, max_spi_freq);
 * 
 *      ///get SPI output clock at current
 *      dev_spi_m->spi_control(SPI_CMD_MST_GET_CURRENT_FREQ, (SPI_CTRL_PARAM)&val32);
 *      
 *      Usage-2: Change reference clock of SSPI master.
 *      ///for example, with RC96M enabled, set SSPI output clock = 48HMZ. 
 *      ///ref. clock source is selected to RC96M and divider is 0
 *      hx_drv_scu_set_pdlsc_sspimclk_cfg(SCU_LSCSSPIMSRC_RC96M48M, 0);
 * 
 *      uint32_t max_clk
 *      DEV_SPI_PTR dev_spi_m;
 *      hx_drv_spi_mst_init(USE_DW_SPI_MST_S, DW_SPI_S_RELBASE);
 *      dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_Q);
 * 
 *      ///update information of reference clock, and get max output clock(max_clk = 48000000)
 *      dev_spi_m->spi_control(SPI_CMD_MST_UPDATE_SYSCLK, (SPI_CTRL_PARAM)&max_clk);
 *      dev_spi_m->spi_open(DEV_MASTER_MODE, max_clk);
 * 
 *      Usage-3: Transmit data using interrupt method with SSPI master
 *      void sspi_callback_fun_tx(void *status)
 *      {
 *          cprintf("SPI Master0 TX CB_FUN\n");
 *          return; 
 *      }
 * 
 *      int data_size = 32;
 *      unsigned char m_tx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      DEV_SPI_PTR dev_spi_m;
 *      dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
 *      dev_spi_m->spi_control(SPI_CMD_SET_TXCB, (SPI_CTRL_PARAM)sspi_callback_fun_tx);
 * 
 *      for(int data_count = 0; data_count < data_size; data_count++)
 *          m_tx_buf[data_count] = data_count % 256;
 * `    dev_spi_m->spi_write(m_tx_buf, data_size);
 * 
 *      Usage-4: Transmit data using DMA method with SSPI master
 *      void sspi_callback_fun_tx_dma(void *status)
 *      {
 *          cprintf("SPI Master0 TX DMA CB_FUN\n");
 *          return; 
 *      }
 * 
 *      int data_size = 32;
 *      unsigned char m_tx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      DEV_SPI_PTR dev_spi_m;
 *      dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
 *      dev_spi_m->spi_control(SPI_CMD_SET_TXCB, (SPI_CTRL_PARAM)sspi_callback_fun_tx);
 * 
 *      for(int data_count = 0; data_count < data_size; data_count++)
 *          m_tx_buf[data_count] = data_count % 256;
 * `    dev_spi_m->spi_write_dma(m_tx_buf, data_size, sspi_callback_fun_tx_dma);
 * 
 *      Usage-5: Transmit data then receive data using DMA mode with SSPI master
 *      uint32_t g_tx_addr, g_rx_addr;
 *      uint32_t rx_data_count = 3;
 *      uint32_t tx_data_count = 1;
 *      void spi_m1_callback_fun_TXRX(void * status)
 *      {
 *          uint8_t* tx_addr = (uint8_t*)g_tx_addr;
 *          uint8_t* rx_addr = (uint8_t*)g_rx_addr;
 *          cprintf("TX addr: %x\n", g_tx_addr);
 *          cprintf("TX data:[");
 *          for(int i = 0; i < tx_data_count ; ++i)
 *          {
 *              cprintf("%d,", tx_addr[i]);
 *          }
 *          cprintf("]\n\n");
 *          cprintf("RX addr: %x\n", g_rx_addr);
 *          cprintf("RX data=[\n");
 *          int j = 1;
 *          for(int i=0; i < rx_data_count; i++)
 *          {
 *              cprintf("%d,", rx_addr[i]);
 *              if(j%16 == 0)
 *                  cprintf("\n");    
 *              j++;
 *          }
 *          cprintf("]\n\n");
 *      }
 *      
 *      uint8_t tx_buf[1] = {0x9f};
 *      uint8_t rx_buf[3] = {0};
 * 
 *      DEV_SPI_PTR dev_spi_m;
 *      dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
 *  
 *      //after spi master write data out, it waits dedicated tick and then read data 
 *      DW_SPI_M_RXSD_S rx_delay_config;
 *      rx_delay_config.RSD = 2;
 *      rx_delay_config.SE = 0;
 *      dev_spi_m->spi_control(SPI_CMD_MST_SET_RXSD, (SPI_CTRL_PARAM)&rx_delay_config);
 *      dev_spi_m->spi_write_then_read_dma((void*)tx_buf, tx_size, (void*)tx_data_count, rx_data_count, spi_m1_callback_fun_TXRX);
 * 
 * 
 *      ///SPI slave
 *      Usage-1: SSPIs pin mux configuration and initialization
 *      ///The output pin of SSPI slave is defined by the user application.
 *      hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_S_DO, 1);
 *      hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_S_DI, 1);
 *      hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_S_CLK, 1);
 *      hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_S_CS, 1);
 * 
 *      ///initialize SSPI slave
 *      hx_drv_spi_slv_init(USE_DW_SPI_SLV_0, DW_SSPI_S_RELBASE);
 *      DEV_SPI_PTR dev_spi_s;
 *      dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);
 *      dev_spi_s->spi_open(DEV_SLAVE_MODE, SPI_CPOL_0_CPHA_0); 
 * 
 *      Usage-1: Transmit data using DMA method with SSPI slave
 *      void spi_s_callback_fun_tx(void *status)
 *      {
 *          cprintf("SPI Slave TX CB_FUN\n");
 *      }
 * 
 *      unsigned char s_tx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      uint32_t data_count = 0;
 *      uitn32_t data_size = 32;
 *      volatile int32_t *blocking;
 *      for (data_count = 0; data_count < data_size; data_count++)
 *         s_tx_buf[data_count] = data_count % 256;
 * 
 *      DEV_SPI_PTR dev_spi_s;
 *      dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);
 *      dev_spi_s->spi_open(DEV_SLAVE_MODE, SPI_CPOL_0_CPHA_0); 
 *      blocking = dev_spi_s->spi_write_dma(s_tx_buf, data_size, spi_s_callback_fun_tx);
 *      while (*blocking == -1) {};
 * 
 *      Usage-2: Receive data using DMA method with SSPI slave
 *      void spi_s_callback_fun_rx(void *status)
 *      {
 *          cprintf("SPI Slave RX CB_FUN\n");
 *          for (int i = 0; i < m_data_size; i++)
 *          {
 *          cprintf("data[%d]=%x\n", i, s_rx_buf[i]);
 *          }
 *      }
 * 
 *      unsigned char s_rx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      uint32_t data_size = 32;
 *      volatile int32_t *blocking;
 *      DEV_SPI_PTR dev_spi_s;
 *      dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);
 *      dev_spi_s->spi_open(DEV_SLAVE_MODE, SPI_CPOL_0_CPHA_0); 
 * 
 *      blocking = dev_spi_s->spi_read_dma(s_rx_buf, data_size, spi_s_callback_fun_rx);    
 *      while (*blocking == -1) {};
 * 
 *      Usage-3: Transfer and Receive data using interrupt method with SSPI slave
 *      unsigned char s_tx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      unsigned char s_rx_buf[64] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
 *      uint32_t m_data_size = 0;
 *      uint32_t 
 *      void spi_s_callback_fun_xfer(void *status)
 *      {
 *          cprintf("SPI Slave XFRE CB_FUN\n");
 *          for (int i = 0; i < m_data_size; i++)
 *          {
 *              cprintf("RX data[%d]=%x\n", i, s_rx_buf[i]);
 *          }
 *          for (int i = 0; i < m_data_size; i++)
 *          {
 *              cprintf("TX data[%d]=%x\n", i, s_tx_buf[i]);
 *          }    
 *          m_data_size = 0;
 *      }
 *  
 *      uint32_t rx_data_size = 32, tx_data_size = 32;
 *      DEV_SPI_TRANSFER data;
 *      DEV_SPI_PTR dev_spi_s;
 *      dev_spi_s = hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_0);
 *      dev_spi_s->spi_open(DEV_SLAVE_MODE, SPI_CPOL_0_CPHA_0); 
 *      dev_spi_s->spi_control(SPI_CMD_SET_XFERCB, (SPI_CTRL_PARAM)spi_s_callback_fun_xfer);
 *      m_data_size = rx_data_size>tx_data_size?rx_data_size:tx_data_size;
 *      memset(s_tx_buf, 0x0, m_data_size);
 *      memset(s_rx_buf, 0x0, m_data_size);
 * 
 *      for (i = 0; i < tx_data_size; i++)
 *          s_tx_buf[i] = (256 - i % 256);
 * 
 *      DEV_SPI_XFER_SET_TXBUF(&data, s_tx_buf, 0, tx_data_size);
 *      DEV_SPI_XFER_SET_RXBUF(&data, s_rx_buf, 0, rx_data_size);
 *      dev_spi_s->spi_control(SPI_CMD_TRANSFER_INT, (SPI_CTRL_PARAM)&data);
 * 
 * </pre>
 * @}
 */


/**
 * \defgroup SPI DMA information
 * \brief DMA default setting:
 * QSPI : use DMA1, TX channel = 0, RX channel = 1
 * SSPIm: use DMA2, TX channel = 0, RX channel = 1
 * OSPI : use DMA1, TX channel = 0, RX channel = 1
 * SSPIs: use DMA2, TX channel = 0, RX channel = 1
 * 
 * \note Each SPI corresponds to a specific DMA and cannot be changed.
 * \note QSPI and OSPI can't be used at the same time, because DMA1 only has 2 channels.
 * \note Change DMA channel used by SSPI master or slave, use \ref SPI_CMD_SET_DMA_CHANNELS to set
 */

/* In BOOTROM, if defined this flag,then only use DMA callback when TX/RX transfer done.*/
#ifdef BOOTROM
#define NO_SUPPORT_SPI_ISR
#endif

/**
 * \name	DesignWare SPI Object Number
 * @{
 */
#define DW_SPI_NUM (3)     /*!< DesignWare SPI valid number */
#define DW_SPI_SLV_NUM (1) /*!< Datafusion SPI slave valid number */
/** @} end of name */

/**
 * \name	Designware SPI Object ID Macros
 * @{
 */

/************************************************
 *  SPI Master ID Macros
 ***********************************************/
#define DW_SPI_Q_ID 0       /*!< SPI 0 id macro (mast mode) */
#define DW_SPI_S_ID 1       /*!< SPI 1 id macro (mast mode) */
#define DW_SPI_O_ID 2       /*!< SPI 2 id macro (mast mode) */

/************************************************
 *  SPI Slave ID Macros
 ***********************************************/
#define DW_SPI_SLV_0_ID 3   /*!< SPI 3 id macro (slave mode) */
#define DW_SPI_SLV_ALL_ID DW_SPI_SLV_NUM
/** @} end of name */

#if defined(IP_INST_SSPI_SLAVE) || defined(IP_INST_NS_SSPI_SLAVE)
#define USE_DW_SSPI_SLV 1
#endif
#if defined(IP_INST_QSPI_HOST)
#define USE_DW_QSPI 1 /*!< enable use designware SPI 0 */
#endif
#ifndef BL_USE
#if defined(IP_INST_QSPI_HOST)
#define USE_DW_OSPI 1
#endif
#if defined(IP_INST_SSPI_HOST) || defined(IP_INST_NS_SSPI_HOST)
#define USE_DW_SSPI 1 /*!< enable use designware SPI 1 */
#endif
#endif

/**
 * \note In non-security, user can't use QSPI & OSPI drivers directly
 *      user should use veneer SPI eeprom API or veneer SPI PSRAM API
 */
#ifdef TRUSTZONE_NS
#undef USE_DW_QSPI
#undef USE_DW_OSPI
#endif

/**
 * \enum USE_DW_SPI_MST_E
 * \brief SPI Master ID enum
 */
typedef enum use_dw_spi_mst_e
{
    USE_DW_SPI_MST_Q = DW_SPI_Q_ID, /*!< enable use dw spi master 0 */
    USE_DW_SPI_MST_S = DW_SPI_S_ID, /*!< enable use dw spi master 1 */
    USE_DW_SPI_MST_O = DW_SPI_O_ID,

    USE_DW_SPI_MST_ALL = DW_SPI_NUM, /*!< enable use dw spi master all */
    USE_DW_SPI_MST_MAX
} USE_DW_SPI_MST_E;

/**
 * \enum USE_DW_SPI_SLV_E
 * \brief SPI slave ID enum
 */
typedef enum use_dw_spi_slv_e
{
    USE_DW_SPI_SLV_0 = DW_SPI_SLV_0_ID, /*!< enable use datafusion subsystem spi slave 0 */
    USE_DW_SPI_SLV_ALL,                 /*!< enable use datafusion subsystem spi slave all */
    USE_DW_SPI_SLV_MAX
} USE_DW_SPI_SLV_E;

/**
 *  \struct DW_SPI_M_RXSD_S
 * 	\brief SPI Master RX Sample Delay
 * 	\param RSD: Receive Data (rxd) Sample Delay. this value is the delay tick based on reference clock. value range: 0~255
 * 	\param SE: Sampling Edge. To decide the sampling edge for RXD signal with reference {0:positive edge ,1:negative edge}
 *      
 */
typedef struct DW_SPI_M_RX_SMAPLE_DELAY_CONFIG_S
{
    uint32_t RSD;
    uint32_t SE;
} DW_SPI_M_RXSD_S;


/**
 * \enum SPI_TMOD_E
 * \brief SPI Transfer mode 
 */
typedef enum spi_trnasfer_mode_e
{
    SPI_TMOD_TX_AND_RX = 0x0,
    SPI_TMOD_TX_ONLY = 0x1,
    SPI_TMOD_RX_ONLY = 0x2,
    SPI_TMOD_TX_THEN_RX = 0x3, // TX_THEN_RX
} SPI_TMOD_E;


/**
 * \enum SPI_FRAME_FORMAT_E
 * \brief SPI Frame format 
 * \note SSPI: sopport single
 *       QSPI: sopport single, dual, quad
 *       OPSI: sopport single, daul, quad, octal
 */
typedef enum spi_frame_format_e
{
    SPI_FRF_SINGLE_FORMAT = 0x0,
    SPI_FRF_DUAL_FORMAT = 0x1,
    SPI_FRF_QUAD_FORMAT = 0x2,
    SPI_FRF_OCTAL_FORMAT = 0x3,
} SPI_FRAME_FORMAT_E;

typedef enum spi_dma_tanfer_cfg_e
{
    SPI_DMA_CFG_B_B = 0x0,
    SPI_DMA_CFG_H_H = 0x1,
    SPI_DMA_CFG_W_W = 0x2,
    SPI_DMA_CFG_W_B = 0x3,
} SPI_DMA_TANSFER_CFG_E;

/**
 * \enum SPI_INST_L_E
 * \brief instruction length
 */
typedef enum spi_instruction_length_e
{
    SPI_INST_L0 = 0x0,
    SPI_INST_L4 = 0x1,
    SPI_INST_L8 = 0x2,
    SPI_INST_L16 = 0x3,
} SPI_INST_L_E;

/**
 * \enum SPI_ADDR_L_E
 * \brief Length of Address to be transmitted.
 */
typedef enum spi_addr_length_e
{
    SPI_ADDR_L0 = 0x0,
    SPI_ADDR_L4 = 0x1,
    SPI_ADDR_L8 = 0x2,
    SPI_ADDR_L12 = 0x3,
    SPI_ADDR_L16 = 0x4,
    SPI_ADDR_L20 = 0x5,
    SPI_ADDR_L24 = 0x6,
    SPI_ADDR_l28 = 0x7,
    SPI_ADDR_L32 = 0x8,
    SPI_ADDR_L36 = 0x9,
    SPI_ADDR_L40 = 0xA,
    SPI_ADDR_L44 = 0xB,
    SPI_ADDR_L48 = 0xC,
    SPI_ADDR_L52 = 0xD,
    SPI_ADDR_L56 = 0xE,
    SPI_ADDR_L60 = 0xF,
} SPI_ADDR_L_E;

/**
 * \enum SPI_TRANSFER_TYPE_E
 * \brief Address and instruction transfer format.
 * TT0:Instruction and Address will be sent in Standard SPI Mode.
 * TT1:Instruction will be sent in Standard SPI Mode and Address will be sent in the mode specified by \ref SPI_FRAME_FORMAT_E SPI_FRF. 
 * TT2:Both Instruction and Address will be sent in the mode specified by \ref SPI_FRAME_FORMAT_E SPI_FRF. 
 * TT3:Reserved.
 */
typedef enum spi_transfer_type_e
{
    SPI_TRANSFER_TT0 = 0x0,
    SPI_TRANSFER_TT1 = 0x1,
    SPI_TRANSFER_TT2 = 0x2,
    SPI_TRANSFER_TT3 = 0x3,

} SPI_TRANSFER_TYPE_E;

/**
 * \struct SPI_TRANSFER_FORMAT_S
 * \brief for seting SPI Transfer configure in Dual/Quad/Octal mode 
 * \ref SPI_CMD_MST_SET_TRANSFER_FORMAT, SPI_CMD_MST_SET_XIP_R_FORMAT, SPI_CMD_MST_SET_XIP_W_FORMAT
 * 
 * \param spi_format: \ref SPI_FRAME_FORMAT_E
 * \param inst_len: \ref SPI_INST_L_E
 * \param addr_len: \ref SPI_ADDR_L_E
 * \param trans_type: \ref SPI_TRANSFER_TYPE_E
 * \param wait_cycle: uint32_t. vaild value range:0~31
 */
typedef struct spi_transfer_format_s
{
    uint32_t spi_format; /*SPI Frame Format*/
    uint32_t inst_len;   /*instruction length*/
    uint32_t addr_len;   /*addr length*/
    uint32_t trans_type; /*transfer type */
    uint32_t wait_cycle; /*Wait cycles in Dual/Quad/Octal mode*/
} SPI_TRANSFER_FORMAT_S;

/**
 * \struct SPI_DMA_TRANS_INFO_S
 * \brief Get DMA ID and DMA request of specific SPI
 * \ref SPI_CMD_GET_DMA_REQ
 */
typedef struct spi_dma_trans_info_s
{
    uint32_t spi_dma;   /*< dma_id */
    uint32_t tx_req;    /*< dma request for Tx */
    uint32_t rx_req;    /*< dma request for Rx */
} SPI_DMA_TRANS_INFO_S;

typedef struct spi_dma_channels_s
{
    uint8_t spi_id;     /*< SPI id */
    uint8_t tx_dma;     /*< dma channel of Tx */
    uint8_t rx_dma;     /*< dma channel of Rx */
} SPI_DMA_CHANNELS_S;

typedef SPI_DMA_CHANNELS_S DEV_SPI_CHANNEL;
/** Define SPI control commands for common usage */
#define DEV_SET_SPI_SYSCMD(cmd) DEV_SET_SYSCMD((cmd))
/** Define SPI control commands for master usage */
#define DEV_SET_SPI_MST_SYSCMD(cmd) DEV_SET_SYSCMD(0x00001000 | (cmd))
/** Define SPI control commands for slave usage */
#define DEV_SET_SPI_SLV_SYSCMD(cmd) DEV_SET_SYSCMD(0x00002000 | (cmd))

/************************************************
 *  ++++ Common commands for SPI Device ++++ 
 ************************************************/
/**
 * \brief Get \ref dev_spi_info::status "current device status"
 * \param type : uint32_t *
 * \param usage : store result of current status
 * \return value explanation :
 */
#define SPI_CMD_GET_STATUS DEV_SET_SPI_SYSCMD(0)
/**
 * set the \ref dev_spi_info::clk_mode "clock mode" of spi transfer
 * \param type : uint32_t
 * \param usage : spi clock mode to choose clock phase and clock polarity
 * \return value explanation :
 */
#define SPI_CMD_SET_CLK_MODE DEV_SET_SPI_SYSCMD(1)
/**
 * set spi \ref dev_spi_info::dfs "data frame size"
 * \param type : uint32_t
 * \param usage : should > 0
 * \return value explanation : If dfs is not supported, then return \ref E_SYS
 */
#define SPI_CMD_SET_DFS DEV_SET_SPI_SYSCMD(2)
/**
 * set the \ref dev_spi_info::dummy "dummy data" during spi transfer
 * \param type : uint32_t
 * \param usage : dummy data to transfer
 * \return value explanation :
 */
#define SPI_CMD_SET_DUMMY_DATA DEV_SET_SPI_SYSCMD(3)
/**
 * Set \ref dev_spi_cbs::tx_cb "spi transmit success callback" function
 * when all required bytes are transmitted for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transmit success callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_TXCB DEV_SET_SPI_SYSCMD(4)
/**
 * Set \ref dev_spi_cbs::rx_cb "spi receive success callback" function
 * when all required bytes are received for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : receive success callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_RXCB DEV_SET_SPI_SYSCMD(5)
/**
 * Set \ref dev_spi_cbs::xfer_cb "spi transfer success callback" function
 * when all required transfer are done for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transfer success callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_XFERCB DEV_SET_SPI_SYSCMD(6)
/**
 * Set \ref dev_spi_cbs::err_cb "spi transfer error callback" function
 * when something error happened for interrupt method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transfer error callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_ERRCB DEV_SET_SPI_SYSCMD(7)
/**
 * Set buffer in interrupt transmit, and it will set \ref dev_spi_info::xfer "spi tranfer".
 * - SPI master and slave mode use case  \n
 *    For both master and slave mode, if you set tx buffer to NULL, when tx interrupt is enabled and entered into tx
 * interrupt, it will automatically disable the tx interrupt, so when you want to transfer something, you need to set
 * the tx buffer to Non-NULL and enable tx interrupt, when the tx buffer is sent, it will disable the tx interrupt and
 * call tx callback function if available.
 * \param type : DEV_BUFFER * or NULL
 * \param usage : buffer structure pointer, if param is NULL, then it will set xfer to empty
 * \return value explanation :
 */
#define SPI_CMD_SET_TXINT_BUF DEV_SET_SPI_SYSCMD(8)
/**
 * Set buffer in interrupt receive, and it will set \ref dev_spi_info::xfer "spi tranfer".
 * - SPI master mode use case  \n
 *   Similar to \ref SPI_CMD_SET_TXINT_BUF
 * - SPI slave mode use case   \n
 *   Similiar to \ref SPI_CMD_SET_TXINT_BUF
 * \param type : DEV_BUFFER * or NULL
 * \param usage : buffer structure pointer, if param is NULL, then it will set xfer to empty
 * \return value explanation :
 */
#define SPI_CMD_SET_RXINT_BUF DEV_SET_SPI_SYSCMD(9)
/**
 * Enable or disable transmit interrupt,
 * for master mode, only one of tx and rx interrupt can be enabled,
 * if tx interrupt is enabled, then rx interrupt can't be enabled.
 * \param type : uint32_t
 * \param usage : enable(none-zero) or disable(zero) flag
 * \return value explanation :
 */
#define SPI_CMD_SET_TXINT DEV_SET_SPI_SYSCMD(10)
/**
 * Enable or disable receive interrupt,
 * for master mode, only one of tx and rx interrupt can be enabled,
 * if rx interrupt is enabled, then tx interrupt can't be enabled.
 * \param type : uint32_t
 * \param usage : enable(none-zero) or disable(zero) flag
 * \return value explanation :
 */
#define SPI_CMD_SET_RXINT DEV_SET_SPI_SYSCMD(11)
/**
 * start the transfer by polling
 * \param type : \ref DEV_SPI_TRANSFER *
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_TRANSFER_POLLING DEV_SET_SPI_SYSCMD(12)
/**
 * start the transfer by interrupt
 * \param type : \ref DEV_SPI_TRANSFER * or NULL
 * \param usage : If NULL, it will disable transfer interrupt, if not NULL, it will enable transfer interrupt
 * \return value explanation :
 */
#define SPI_CMD_TRANSFER_INT DEV_SET_SPI_SYSCMD(13)
/**
 * Abort current interrupt transmit operation if tx interrupt enabled,
 * it will disable transmit interrupt, and set \ref DEV_IN_TX_ABRT
 * in \ref dev_spi_info::status "status" variable,
 * and call the transmit callback function, when tx callback is finished,
 * it will clear \ref DEV_IN_TX_ABRT and return
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_ABORT_TX DEV_SET_SPI_SYSCMD(14)
/**
 * Abort current interrupt receive operation if rx interrupt enabled,
 * it will disable receive interrupt, and set \ref DEV_IN_TX_ABRT
 * in \ref dev_spi_info::status "status" variable,
 * and call the receive callback function, when rx callback is finished,
 * it will clear \ref DEV_IN_TX_ABRT and return
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_ABORT_RX DEV_SET_SPI_SYSCMD(15)
/**
 * Abort current interrupt transfer operation if transfer is issued,
 * it will disable transfer interrupt, and set \ref DEV_IN_XFER_ABRT
 * in \ref dev_spi_info::status "status" variable,
 * and call the transfer callback function, when xfer callback is finished,
 * it will clear \ref DEV_IN_XFER_ABRT and return
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_ABORT_XFER DEV_SET_SPI_SYSCMD(16)
/**
 * Do a software reset for SPI device, it will stop current transfer,
 * and clear error state and bring device to normal state, set next condition to STOP
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_RESET DEV_SET_SPI_SYSCMD(17)
/**
 * Flush spi tx fifo, this will clear the data in tx fifo
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_FLUSH_TX DEV_SET_SPI_SYSCMD(18)
/**
 * Flush spi rx fifo, this will clear the data in rx fifo
 * \param type : NULL
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_FLUSH_RX DEV_SET_SPI_SYSCMD(19)
/**
 * Enable spi device
 * \param type : NULL
 * \param usage : param is not required
 * \return value explanation :
 */
#define SPI_CMD_ENA_DEV DEV_SET_SPI_SYSCMD(20)
/**
 * Disable spi device, when device is disabled,
 * only \ref SPI_CMD_ENA_DEV, \ref SPI_CMD_DIS_DEV,
 * \ref SPI_CMD_GET_STATUS and \ref SPI_CMD_RESET
 * commands can be executed, other commands will return \ref E_SYS
 * \param type : NULL
 * \param usage : param is not required
 * \return value explanation :
 */
#define SPI_CMD_DIS_DEV DEV_SET_SPI_SYSCMD(21)
/**
 * Get how many bytes space in spi are available to transmit,
 * this can be used in interrupt callback functions,
 * cooperate with \ref dev_spi::spi_write "spi_write" API to realize non-blocked write
 * \param type : int32_t *
 * \param usage : store the write available bytes, > 0 for available bytes, 0 for not available
 * \return value explanation :
 */
#define SPI_CMD_GET_TXAVAIL DEV_SET_SPI_SYSCMD(22)
/**
 * Get how many bytes in spi are available to receive,
 * this can be used in interrupt callback functions,
 * cooperate with \ref dev_spi::spi_read "spi_read" API to realize non-blocked read
 * \param type : int32_t *
 * \param usage : store the read available bytes, > 0 for available bytes, 0 for not available
 * \return value explanation :
 */
#define SPI_CMD_GET_RXAVAIL DEV_SET_SPI_SYSCMD(23)

//DEV_SET_SPI_SYSCMD(24)

/**
 * Get the raw status of the DWC_ssi interrupts.
 * \param type : int32_t *
 * \param usage: store the interrupt flag status
 * \return value explanation :
 * [0] Transmit FIFO Empty Raw Interrupt Status.
 * [1] Transmit FIFO Overflow Raw Interrupt Status
 * [2] Receive FIFO Underflow Raw Interrupt Status
 * [3] Receive FIFO Overflow Raw Interrupt Status
 * [4] Receive FIFO Full Raw Interrupt Status
 * [5] Multi-Master Contention Raw Interrupt Status.
 * [6] XIP Receive FIFO Overflow Raw Interrupt Status
 * [7] Transmit FIFO Underflow Interrupt Raw Status
 * [8] AXI Error Interrupt Raw Status
 * [10] SPI Transmit Error Interrupt status.
 * [11] SSI Done Interrupt Raw Status
 */
#define SPI_CMD_GET_INT_STATUS DEV_SET_SPI_SYSCMD(25)

/**
 * Set \ref dev_spi_cbs::tx_cb "spi transmit success callback" function
 * when all required bytes are transmitted for DMA method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : transmit success callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_DMA_TXCB DEV_SET_SPI_SYSCMD(26)

/**
 * Set \ref dev_spi_cbs::rx_cb "spi receive success callback" function
 * when all required bytes are received for DMA method
 * \param type : \ref DEV_CALLBACK * or NULL
 * \param usage : receive success callback function for spi
 * \return value explanation :
 */
#define SPI_CMD_SET_DMA_RXCB DEV_SET_SPI_SYSCMD(27)

/**
 * Set the level at which DMA request is made by the transmit logic
 * \param type : uint32_t
 * \param usage : should > 0
 * \return value explanation :
 */
#define SPI_CMD_SET_DMA_TDLR DEV_SET_SPI_SYSCMD(28)

/**
 * Set the level at which DMA request is made by the receive logic
 * \param type : uint32_t
 * \param usage : should > 0
 * \return value explanation :
 */
#define SPI_CMD_SET_DMA_RDLR DEV_SET_SPI_SYSCMD(29)

/** Set DMA config for each SPI frame format
 * \param type : SPI_FRAME_FORMAT_E
 * \param usage :
 * \param value explanation :
 */
#define SPI_CMD_SET_DMA_CONFIG DEV_SET_SPI_SYSCMD(31)

/** Get SPI register address.
 * \param type : uint32_t*
 * \param usage :
 * \param value explanation :
 */
#define SPI_CMD_GET_SPI_BASE_REG DEV_SET_SPI_SYSCMD(32)

/** Set SPI transfer mode
 * \param type : SPI_TMOD_E
 * \param usage :
 * \param value explanation :
 */
#define SPI_CMD_SET_TMOD DEV_SET_SPI_SYSCMD(33)

/**
 * Set SPI DMA controller to enable transmit/receive DMA.
 * \param type : uint32_t
 * \param usage : 0: disable TX&RX, 1:enable RX, 2:enable TX, 3:enable TX&RX
 * \return value explanation :
 */
#define SPI_CMD_ENA_DMA_TXRX DEV_SET_SPI_SYSCMD(34)

/**
 * Get DMA request ID & DMA struct for SPI
 * \param type : SPI_DMA_TRANS_INFO_S*
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_GET_DMA_REQ DEV_SET_SPI_SYSCMD(35)

/**
 * Get flag value to check if spi busy.
 * \param type :
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_GET_BUSY_STATUS DEV_SET_SPI_SYSCMD(36)

/**
 * Enable clock stretching capability in SPI transfer.
 * \param type : uint32_t
 * \param usage : 0 or 1
 * \return value explanation :
 */
#define SPI_CMD_ENV_CLK_STRETCH DEV_SET_SPI_SYSCMD(37)

/**
 * Set transmit FIFO threshold to control the level of entries at which
 * the transmit  FIFO controller triggers an interrupt.
 * \param type : uint32_t
 * \param usage : should < TX FIFO lenght
 * \return value explanation :
 */
#define SPI_CMD_SET_TXFT DEV_SET_SPI_SYSCMD(38)

/**
 * Set receive FIFO threshold to control the level of entries at which
 * the receive  FIFO controller triggers an interrupt.
 * \param type : uint32_t
 * \param usage : should < RX FIFO lenght
 * \return value explanation :
 */
#define SPI_CMD_SET_RXFT DEV_SET_SPI_SYSCMD(39)

/**
 * Get done flag pointer of DMA for SPI TX.
 * \param type: int32_t*
 *  value = -1 -> DMA channel of SPI Tx is busy status
 *  value = 1  -> DMA channel of SPI Tx is IDEL status
 *  \return value explanation :
 */
#define SPI_CMD_GET_TX_DMA_DONE DEV_SET_SPI_SYSCMD(40)

/**
 * Get done flag pointer of DMA for SPI RX.
 * \param type: int32_t
 *  value = -1 -> DMA channel of SPI Rx is busy status
 *  value = 1  -> DMA channel of SPI Rxis IDEL status
 *  \return value explanation :
 */
#define SPI_CMD_GET_RX_DMA_DONE DEV_SET_SPI_SYSCMD(41)

/**YX 20230222 added. Polling DMA RX/TX and SPIM TX status without DMA and SPI IRQ
 * @{
 */

/**
 * Get the value of SPI Status Register
 * \param type: uint32_t*
 * \note  vaild for HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ is defined
 */
#define SPI_CMD_GET_STATUS_REG  DEV_SET_SPI_SYSCMD(42)

/**
 * Enable interrupt of SPI
 * \param type: bool 
 * \note  vaild for HX_LIB_SPIM_DMA_POLL_STATUS_NO_IRQ is defined
 */
#define SPI_CMD_ENA_IRQ         DEV_SET_SPI_SYSCMD(43)
/** @} YX 20230222 added*/
/**
 * Change DMA channel used by SPI TX/RX
 * \param type: SPI_DMA_CHANNELS_S*
 * \note vaild value range: 0~1 for QSPI/OSPI,  0~7 for SSPI master/SSPI slave
 */
#define SPI_CMD_SET_DMA_CHANNELS DEV_SET_SPI_SYSCMD(44)

/**
 * Get DMA channel used by SPI TX/RX
 * \param type: SPI_DMA_CHANNELS_S*
 */
#define SPI_CMD_GET_DMA_CHANNELS DEV_SET_SPI_SYSCMD(45)
/* ++++ Master only commands for SPI Device ++++ */
/**
 *  select spi slave device
 * \param type : uint32_t
 * \param usage : the number of spi slave device to select
 * \return value explanation : return \ref E_SYS when selection can't be done, return \ref E_CTX during transfer
 */
#define SPI_CMD_MST_SEL_DEV DEV_SET_SPI_MST_SYSCMD(0)
/**
 *  de-select spi slave device
 * \param type : uint32_t
 * \param usage : the number of spi slave device to de-select
 * \return value explanation : return \ref E_SYS when selection can't be done, return \ref E_CTX during transfer
 */
#define SPI_CMD_MST_DSEL_DEV DEV_SET_SPI_MST_SYSCMD(1)
/**
 * Set \ref dev_spi_info::freq "spi frequency".
 * \param type : uint32_t
 * \param usage : spi freq
 * \return value explanation : no return
 */
#define SPI_CMD_MST_SET_FREQ DEV_SET_SPI_MST_SYSCMD(2)

/**
 * Update system clock of SPI, and how fast can the output frequency in spi be transmitted.
 * \param type : uint32_t *
 * \param usage : the current output frequenct of spi
 * \return value explanation :
 */
#define SPI_CMD_MST_UPDATE_SYSCLK DEV_SET_SPI_MST_SYSCMD(3)

/**
 * Set number of data frames to be continuously received by the SPI.
 * \param type : uint32_t
 * \param usage : should < 65535
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_RX_COUNT DEV_SET_SPI_MST_SYSCMD(4)

/**
 * Set SPI transfer configure
 * \param type : SPI_TRANSFER_FORMAT_S*
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_TRANSFER_FORMAT DEV_SET_SPI_MST_SYSCMD(5)

/**
 * Enable continuous transfer in XIP mode.
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_XIP_CONT DEV_SET_SPI_MST_SYSCMD(6)

/**
 * Set XIP INCR & WRAP transfer opcode
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_XIP_INST DEV_SET_SPI_MST_SYSCMD(7)

/**
 * Get the current frequency of SPI
 * \param type : uint32_t*
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_GET_CURRENT_FREQ DEV_SET_SPI_MST_SYSCMD(8)

/**
 * Set flag to fix DFS for XIP transfer. If enable then the data frame
 * size for XIP transfer will be fixed to the value is CTRLR0.DFS
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_ENV_XIP_DFS_HC DEV_SET_SPI_MST_SYSCMD(9)

/**
 * Enable XIP instruction. If enable then XIP transfers will alse
 * have instruction phase.
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_ENV_XIP_INST DEV_SET_SPI_MST_SYSCMD(10)

/**
 * Set SPI XIP read configure
 * \param type : SPI_TRANSFER_FORMAT_S*
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_XIP_R_FORMAT DEV_SET_SPI_MST_SYSCMD(11)

/**
 * Set SPI XIP write configure
 * \param type : SPI_TRANSFER_FORMAT_S*
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_XIP_W_FORMAT DEV_SET_SPI_MST_SYSCMD(12)

/**
 * Set \ref DW_SPI_M_RXSD_S::  the number of ssi_clk cycles that are delayed before the actual sample of the rxd input
 * occurs.
 * \param type : DW_SPI_M_RXSD_S
 * \param usage : the current output frequenct of spi
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_RXSD DEV_SET_SPI_MST_SYSCMD(13)

/**Enable SPI Hyperbus frame format for XIP transfers
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_ENV_HYPERBUS DEV_SET_SPI_MST_SYSCMD(14)

/**Enable RXDS signling during address and command  phase of Hyperbus transfer.
 * if enable than SPI transmit (2 * wait_cycles - 1) wait cycles after the address phase is complete.
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_ENV_HB_RXDS_SIG DEV_SET_SPI_MST_SYSCMD(15)

/**Enable SPI Dual-data rate transfers in dual/quad /octal frame formats of SPI.
 * \param type : uint32_t
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_ENV_DDR DEV_SET_SPI_MST_SYSCMD(16)

/**
 * Set wait cycles in dual/quad/ocal mode. (not contain XIP mode)
 * \param type : uint32_t, vaild value range: 0~31
 * \param usage :
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_WAITCYCLES DEV_SET_SPI_MST_SYSCMD(17)

/**
 * \brief select spi xip slave device
 * \param type : uint32_t
 * \param usage : the number of spi slave device to select
 * \return value explanation : return \ref E_SYS when selection can't be done, return \ref E_CTX during transfer
 */
#define SPI_CMD_MST_SEL_XIP_DEV DEV_SET_SPI_MST_SYSCMD(18)

/**
 * \brief enable/disable Dual-data rate transfer for Instruction phase
 * \param type : uint32_t
 * \param usage: 0 or 1
 * \return value explanation : 
 */
#define SPI_CMD_MST_ENV_INST_DDR DEV_SET_SPI_MST_SYSCMD(19)

/**
 * Set transmit start FIFO threshold to control the level of entries in transmit
 * FIFO above which transfer will start on serial line.
 * \param type : uint32_t
 * \param usage : should < TX FIFO lenght
 * \return value explanation :
 */
#define SPI_CMD_MST_SET_TXFTHR DEV_SET_SPI_MST_SYSCMD(20)


/**
 * \defgroup	DEVICE_HAL_SPI_CALLBACK	SPI Interrupt callback functions
 * \ingroup	DEVICE_HAL_SPI
 * \brief	callback function structure for SPI device 
 * only for interrupt mode
 * @{
 */
typedef struct dev_spi_cbs
{
    DEV_CALLBACK tx_cb;   /*!< spi data transmit success required bytes callback */
    DEV_CALLBACK rx_cb;   /*!< spi data receive success required bytes callback */
    DEV_CALLBACK err_cb;  /*!< spi error callback */
    DEV_CALLBACK xfer_cb; /*!< transfer callback */
} DEV_SPI_CBS, *DEV_SPI_CBS_PTR;
/** @} */

/**
 * \defgroup	DEVICE_HAL_SPI_CLK_MODE	SPI Clock Mode
 * \ingroup	DEVICE_HAL_SPI
 * \brief	spi clock mode settings
 * @{
 */
/** SPI Clock Mode */
typedef enum spi_clk_mode
{
    SPI_CPOL_0_CPHA_0 =
        0, /*!< Inactive state of serial clock is low, serial clock toggles in middle of first data bit */
    SPI_CPOL_0_CPHA_1 =
        1, /*!< Inactive state of serial clock is low, serial clock toggles at start of first data bit  */
    SPI_CPOL_1_CPHA_0 =
        2, /*!< Inactive state of serial clock is high, serial clock toggles in middle of first data bit */
    SPI_CPOL_1_CPHA_1 =
        3, /*!< Inactive state of serial clock is high, serial clock toggles at start of first data bit */

    SPI_CLK_MODE_0 = SPI_CPOL_0_CPHA_0, /*!< Equal to \ref SPI_CPOL_0_CPHA_0 */
    SPI_CLK_MODE_1 = SPI_CPOL_0_CPHA_1, /*!< Equal to \ref SPI_CPOL_0_CPHA_1 */
    SPI_CLK_MODE_2 = SPI_CPOL_1_CPHA_0, /*!< Equal to \ref SPI_CPOL_1_CPHA_0 */
    SPI_CLK_MODE_3 = SPI_CPOL_1_CPHA_1  /*!< Equal to \ref SPI_CPOL_1_CPHA_1 */
} SPI_CLK_MODE;

typedef enum DEV_SPI_MAT_SPPED_S
{
    SPI_MAT_FULL_SPEED = 0, // Divider = 4
    SPI_MAT_HALF_SPEED = 1, // Divider = 8
    SPI_MAT_SLOW_SPEED = 2, // Divider = 12
} DEV_SPI_MAT_SPEED_E;

typedef SPI_CLK_MODE DEV_SPI_CLK_MODE;

/*!< Default SPI device clock mode */
#ifndef BL_use
#define SPI_CLK_MODE_DEFAULT SPI_CPOL_0_CPHA_0
#endif
// #define SPI_CLK_MODE_DEFAULT		SPI_CPOL_1_CPHA_0
/** @} */

/**
 * \defgroup	DEVICE_HAL_SPI_SLAVE_SELECT	SPI Slave Select Line
 * \ingroup	DEVICE_HAL_SPI
 * \brief	spi slave select line definition
 * @{
 */
/** SPI Slave Select Line, start from 0 */
#define DEV_SPI_CS_LINE_DEF(line) (line)
typedef enum dev_spi_cs_line
{
    DEV_SPI_CS_LINE_0 = 0, /*!< SPI slave select line 0  */
    DEV_SPI_CS_LINE_1,     /*!< SPI slave select line 1  */
    DEV_SPI_CS_LINE_2,     /*!< SPI slave select line 2  */
    DEV_SPI_CS_LINE_3,     /*!< SPI slave select line 3  */
} DEV_SPI_CS_LINE;
/** @} */

/**
 * \defgroup	DEVICE_HAL_SPI_DEVSTRUCT	SPI Device Structure
 * \ingroup	DEVICE_HAL_SPI
 * \brief	contains definitions of spi device structure.
 * \details	this structure will be used in user implemented code, which was called
 * 	Device Driver Implement Layer for spi to realize in user code.
 * @{
 */
typedef struct dev_spi_transfer DEV_SPI_TRANSFER, *DEV_SPI_TRANSFER_PTR;
/**
 * \brief	spi read and write data structure used by SPI_CMD_TRANSFER
 * 	spi write then read data
 *
 */
struct dev_spi_transfer
{
    DEV_SPI_TRANSFER *next;
    /* Calc by software */
    /** tot_len = (tx_totlen>rx_totlen)?tx_totlen:rx_totlen */
    uint32_t tot_len;
    /* Set by user */
    uint8_t *tx_buf;
    uint32_t tx_ofs;
    uint32_t tx_len;
    uint8_t *rx_buf;
    uint32_t rx_ofs;
    uint32_t rx_len;
    /* Should auto set to proper value during set buffer value */
    uint32_t tx_idx;
    uint32_t tx_totlen; /** tx_totlen = tx_len + tx_ofs */
    uint32_t rx_idx;
    uint32_t rx_totlen; /** rx_totlen = rx_len + rx_ofs */
};

/** Set tx buffer of device spi transfer */
#define DEV_SPI_XFER_SET_TXBUF(xfer, buf, ofs, len)                                                                    \
    {                                                                                                                  \
        (xfer)->tx_buf = (uint8_t *)(buf);                                                                             \
        (xfer)->tx_len = (uint32_t)(len);                                                                              \
        (xfer)->tx_ofs = (uint32_t)(ofs);                                                                              \
        (xfer)->tx_idx = 0;                                                                                            \
        (xfer)->tx_totlen = ((uint32_t)(len) + (uint32_t)(ofs));                                                       \
    }

/** Set rx buffer of device spi transfer */
#define DEV_SPI_XFER_SET_RXBUF(xfer, buf, ofs, len)                                                                    \
    {                                                                                                                  \
        (xfer)->rx_buf = (uint8_t *)(buf);                                                                             \
        (xfer)->rx_len = (uint32_t)(len);                                                                              \
        (xfer)->rx_ofs = (uint32_t)(ofs);                                                                              \
        (xfer)->rx_idx = 0;                                                                                            \
        (xfer)->rx_totlen = ((uint32_t)(len) + (uint32_t)(ofs));                                                       \
    }

/** Calculate total length of current transfer without next transfer */
#define DEV_SPI_XFER_CALC_TOTLEN(xfer)                                                                                 \
    (xfer)->tot_len = ((xfer)->tx_totlen > (xfer)->rx_totlen) ? (xfer)->tx_totlen : (xfer)->rx_totlen;

/** Set next SPI transfer */
#define DEV_SPI_XFER_SET_NEXT(xfer, next_xfer) (xfer)->next = (next_xfer);

/** Init spi transfer */
#define DEV_SPI_XFER_INIT(xfer)                                                                                        \
    {                                                                                                                  \
        (xfer)->tx_idx = 0;                                                                                            \
        (xfer)->rx_idx = 0;                                                                                            \
        (xfer)->tx_totlen = ((xfer)->tx_len + (xfer)->tx_ofs);                                                         \
        (xfer)->rx_totlen = ((xfer)->rx_len + (xfer)->rx_ofs);                                                         \
        DEV_SPI_XFER_CALC_TOTLEN(xfer);                                                                                \
    }
/** Flash Access Mode */
typedef enum
{
    FLASH_SINGLE = 1, /*!< Single mode  */
    FLASH_DUAL = 2,   /*!< Dual mode  */
    FLASH_QUAD = 4,   /*!< Quad mode */
} FLASH_ACCESS_MODE_E;

#define DW_SPI_OBJECT DEV_SPI *

/**
 * \defgroup	DEVICE_DW_SPI_REGSTRUCT		DesignWare SPI Register Structure
 * \ingroup	DEVICE_DW_SPI
 * \brief	contains definitions of DesignWare SPI register structure.
 * \details	detailed description of DesignWare SPI register information
 * @{
 */
/**
 * \brief	DesignWare SPI register structure
 * \details	Detailed struct description of DesignWare SPI
 * 	block register information, implementation of dev_spi_info::spi_regs
 */
typedef volatile struct dw_spi_reg
{
    /*!< Control Register */
    /*!< SPI Control Register 0  (0x0) */
    uint32_t CTRLR0;
    /*!< SPI Control Register 1  (0x4) */
    uint32_t CTRLR1;
    /*!< Enable Register */
    /*!< SPI Enable Register  (0x8) */
    uint32_t SSIENR;
    /*!< SPI Microwire Control Register  (0xC) */
    uint32_t MWCR;
    /*!< SPI Slave Enable Register  (0x10) */
    uint32_t SER;
    /*!< SPI Baud Rate Select Register  (0x14) */
    uint32_t BAUDR;
    /*!< TX and RX FIFO Control Register */
    /*!< SPI Transmit FIFO Threshold Level Register  (0x18) */
    uint32_t TXFTLR;
    /*!< SPI Receive  FIFO Threshold Level Register  (0x1C) */
    uint32_t RXFTLR;
    /*!< SPI Transmit FIFO Level Register  (0x20) */
    uint32_t TXFLR;
    /*!< SPI Receive  FIFO Level Register  (0x24) */
    uint32_t RXFLR;
    /*!< SPI Status   Register  (0x28) */
    uint32_t SR;
    /*!< Interrupt Enable/Disable/Control Registers */
    /*!< SPI Interrupt Mask Register  (0x2C) */
    uint32_t IMR;
    /*!< SPI Interrupt Status Register  (0x30) */
    uint32_t ISR;
    /*!< SPI Raw Interrupt Status Register (0x34) */
    uint32_t RISR;
    /*!< SPI Transmit FIFO Overflow Interrupt Clear Register  (0x38) */
    uint32_t TXOICR;
    /*!< SPI Receive  FIFO Overflow Interrupt Clear Register  (0x3C) */
    uint32_t RXOICR;
    /*!< SPI Receive FIFO Underflow Interrupt Clear Register  (0x40) */
    uint32_t RXUICR;
    /*!< SPI Multi-Master Interrupt Clear Register  (0x44) */
    uint32_t MSTICR;
    /*!< SPI Interrupt Clear Register  (0x48) */
    uint32_t ICR;
    /*!< DMA Control Register  (0x4C) */
    uint32_t DMACR;
    /*!< DMA Transmit Data Level  (0x50) */
    uint32_t DMATDLR;
    /*!< DMA Receive Data Level  (0x54) */
    uint32_t DMARDLR;
    /*!< SPI Identification Register  (0x58) */
    uint32_t IDR;
    /*!< SPI CoreKit ID Register (Value after Reset : 0x3332322A)  (0x5C) */
    uint32_t SSI_VER_ID;
    /*!< Data Register */
    /*!< SPI DATA Register for both Read and Write  (0x60) */
    uint32_t DATAREG;
    /*!< More SPI DATA Register for both Read and Write  (0x64-0xEC) */
    uint32_t DRS[35];
    /** 0xF0, RxD Sample Delay Register (0xf0) */
    uint32_t RX_SAMPLE_DLY;
    /** 0xF4, SPI_CTRLR0  Register (0xf4) */
    uint32_t SPI_CTRLR0;
    /** 0xF8, DDR_DRIVE_EDGE Register (0xf8) */
    uint32_t DDR_DRIVE_EDGE;
    /** 0xFC, XIP_MODE_BITS Register (0xfc) */
    uint32_t XIP_MODE_BITS;
    /** 0x100, XIP_INCR_INST Register (0x100) */
    uint32_t XIP_INCR_INST;
    /** 0x104, XIP_WRAP_INST  Register (0x104) */
    uint32_t XIP_WRAP_INST;
    /** 0x108, XIP_CTRL  Register (0x108) */
    uint32_t XIP_CTRL;
    /** 0x10C, XIP_SER  Register (0x10c) */
    uint32_t XIP_SER;
    /*for OSPI*/
    /** 0x110, XRXOICR  Register (0x110) */
    uint32_t XRXOICR;
    /** 0x114, XIP_CNT_TIME_OUT  Register (0x114) */
    uint32_t XIP_CNT_TIME_OUT;
    /*!< More SPI DATA Register for both Read and Write  (0x64-0xEC) */
    uint32_t RESERVED[10];
    /** 0x140, XIP_WRITE_INCR_INST  Register (0x140) */
    uint32_t XIP_WRITE_INCR_INST;
    /** 0x144, XIP_WRITE_WRAP_INST  Register (0x144) */
    uint32_t XIP_WRITE_WRAP_INST;
    /** 0x148, XIP_WRITE_CTRL  Register (0x148) */
    uint32_t XIP_WRITE_CTRL;

} DW_SPI_REG, *DW_SPI_REG_PTR;
/** @} */

/**
 * \brief	spi information struct definition
 * \details	informations about spi open state, working state,
 * 	frequency, spi registers, working method, interrupt number
 */
typedef struct dev_spi_info
{
    uint32_t dev_id; /*!< spi device Id */
    void *spi_ctrl;  /*!< spi control related */

    uint32_t status;  /*!< current working status, refer to \ref DEVICE_HAL_COMMON_DEVSTATUS, this should be \ref
                         DEV_ENABLED for first open */
    uint32_t freq;    /*!< spi working baudrate */
    uint8_t mode;     /*!< spi working mode (master/slave) */
    uint8_t clk_mode; /*!< spi clock phase and polarity, this should be \ref SPI_CLK_MODE_DEFAULT for first open */
    uint8_t opn_cnt; /*!< spi open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
    uint8_t slave;   /*!< current selected slave device no, start from 0, this should be \ref SPI_SLAVE_NOT_SELECTED for
                        first open */
    uint8_t dfs;     /*!< data frame size, this should be \ref SPI_DFS_DEFAULT for first open */

    uint32_t rx_sampledly;    /*!< RxD Sample Delay */
    uint32_t dw_apb_bus_freq; /*!< spi ip apb bus frequency */
    uint32_t intno;           /*!< interrupt no */
    uint32_t tx_fifo_len;     /*!< transmit fifo length */
    uint32_t rx_fifo_len;     /*!< receive fifo length */

    DEV_SPI_CBS spi_cbs; /*!< spi callbacks,  this should be all NULL for first open */
    DEV_SPI_CBS
    spi_dma_cbs; /*!< spi dma callbacks,  this should be all NULL for first open, xfer_cb & err_cb not use */

    DEV_SPI_CHANNEL spi_dma; /*< DMA channel NO. for SPI device */
    uint32_t dma_txdl; /*<DMA Transmit Data Level */
    uint32_t dma_rxdl; /*<DMA Receive Data Level */

    DEV_SPI_TRANSFER xfer; /*!< spi transfer, this should be set to all zero for first open */
    uint32_t dummy; /*!< dummy write data when send and receive, this should be \ref SPI_DUMMY_DEFAULT for first open */

} DEV_SPI_INFO, *DEV_SPI_INFO_PTR;

#define SPI_DFS_DEFAULT 8             /*!< Default spi data frame size */
#define SPI_SLAVE_NOT_SELECTED (0xFF) /*!< Slave is not selected */
#define SPI_DUMMY_DEFAULT (0xFF)      /*!< default dummy value for first open */

typedef void (*spimcomm_cb_t)(void);

/**
 * \brief	spi device interface definition
 * \details	define spi device interface, like spi information structure,
 * 	fuctions to get spi info, open/close/control spi, send/receive data by spi.
 * 
 * For master, max output freq. is divided by 2 based on the reference clock.
 *  If the setting of reference clock is changed, the driver updates the information through \ref SPI_CMD_MST_UPDATE_SYSCLK.
 * 
 * Hardware limit:
 * QSPI master: max output freq. is 100MHZ
 * SSPI master: max output freq. is 50MHZ
 * OSPI master: max outupt freq. is 100MHZ
 * SSPI slave : max input clock is 40MHZ
 * 
 * For change the setting of reference clock:
 * using \ref hx_drv_scu_set_pdhsc_qspisrc_cfg to set QSPI master 
 * using \ref hx_drv_scu_set_pdlsc_sspimclk_cfg to set SSPI master
 * using \ref hx_drv_scu_set_pdhsc_ospisrc_cfg to set OSPI master
 * using \ref hx_drv_scu_set_pdlsc_sspisclk_cfg to setting SPIS slave
 * 
 * \note The Clock Divider of SPI must be even, 
 *      SPI speed user-defined may be different from the actual SPI output speed.
 *      user can use \ref SPI_CMD_MST_GET_CURRENT_FREQ to get real SPI speed.
 * 
 * \note	all this details are implemented by user in user porting code
 *          [Master]
 *          QSPI: only supprot spi_open, spi_close, spi_control function
 *          OSPI: only supprot spi_open, spi_close, spi_control function
 *          SSPI: All functions support except spi_read_ptl.
 *          [Slave]
 *          SSPI: Most of the functions support except spi_read_ptl, spi_read_write_dma and spi_write_then_read_dma
 * 
 *          spi_read_write_dma:  only SSPI mst & SSPI slv support
 *          spi_write_then_read_dma:  only SSPI mst support
 */
typedef struct dev_spi
{
	/*!< spi device information */
    DEV_SPI_INFO spi_info;

    /**
     * open spi device in master/slave mode
     * 
     * \param mode [in] Set Master/Slave to SPI
     * \param param [in] when in master mode: param stands for frequency.
     *                  when in slave mode, param stands for clock mode.
     * \note The clock mode of master can be seted by control command "SPI_CMD_SET_CLK_MODE";
     */
    int32_t (*spi_open)(uint32_t mode, uint32_t param);

    /**
     * close spi device
     */
    int32_t (*spi_close)(void);

    /**
     * control spi device
     * 
     * \param[in] ctrl_cmd  \ref DEV_SET_SPI_SYSCMD "control command", to change or get some thing related to spi
     * \param[in, out] param parameters that maybe argument of the command,
     * 					or return values of the command, must not be NULL
     * \retval	E_OK	Control device successfully
     * \retval	E_CLSED	Device is not opened
     * \retval	E_OBJ	Device object is not valid or not exists
     * \retval	E_PAR	Parameter is not valid for current control command
     * \retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
     * \retval	E_CTX	Control device failed, due to different reasons like in transfer state
     * \retval	E_NOSPT	Control command is not supported or not valid
     */
    int32_t (*spi_control)(uint32_t ctrl_cmd, void *param);

    /**
     * send data to spi device (interrupt method)
     * 
     * \param[in] data Pointer to the data buffer to be written to SPI master/slave device
     * \param[in] len The length of the data that will be written to the SPI master/slave device.
     * 
     * \note Use \ref SPI_CMD_SET_TXCB to register callback function
     */
    int32_t (*spi_write)(const void *data, uint32_t len);

    /**
     * read data from spi device (interrupt method)
     * 
     * \param[in] data Pointer to the data buffer where the received data from SPI master/slave device
     * \param[in] len The length of the data that will be received from SPI master/slave device.
     * 
     * \note Use \ref SPI_CMD_SET_RXCB to register callback function
     */
    int32_t (*spi_read)(void *data, uint32_t len);

    /**
     * send data to spi device (dma method) , 
     * SSPI Mst max transfer size = 4095 bytes
     * SSPI Slv max transfer size = 256*4095 bytes
     * 
     * \param[in] data Pointer to the data buffer to be written to SPI master/slave device
     * \param[in] len The length of the data that will be written to the SPI master/slave device.
     * \param[out] cb  callback function after operation is done
     * 
     * \return An error code \headerfile "dev_commmon.h"
     * \retval E_OK	Control device successfully
     * \retval E_NORES SPI is busy
     * \retval E_NOSPT No support this feature
     */
    int32_t (*spi_write_dma)(const void *data, uint32_t len,
                              void *cb);

    /**
     * read data from spi device (dma method) , max receive size: 256*4095= 1023KB (about)
     * 
     * \param[in] data Pointer to the data buffer where the received data from SPI master/slave device
     * \param[in] len The length of the data that will be received from SPI master/slave device.
     * \param[out] cb  callback function after operation is done
     * 
     * \return An error code \headerfile "dev_commmon.h"
     * \retval E_OK	Control device successfully
     * \retval E_NORES SPI is busy
     * \retval E_NOSPT No support this feature
     */
    int32_t (*spi_read_dma)(void *data, uint32_t len, void *cb);

    /**
     * send data to spi device ( method with protocol), 
     * max header buffer size = 4095 bytes
     * max data size = 256*4095 = 1023KB (about)
     * \if not need header file 
     *      use like this spi_write_ptl(NULL, 0, data_array_ptr, data_size, callback function)
     * \endif
     * 
     * \param[in] header_buf Pointer to the header buffer will to be written to SPI master/slave device. (opt.)
     *                      if you don't need header, the param is set to NULL
     * \param[in] header_len The length of header wiil to be written to SPI master/slave device. (opt.)
     *                      if you don't need header, the param is set to 0.
     * \param[in] data_packet Pointer to the data buffer will to be written to SPI master/slave device.
     * \param[in] data_len The length of data wiil to be written to SPI master/slave device.
     * \param[out] cb  callback function after operation is done
     * 
     * \return An error code \headerfile "dev_commmon.h"
     * \retval E_OK	Control device successfully
     * \retval E_NORES SPI is busy
     * \retval E_NOSPT No support this feature
     */
    int32_t (*spi_write_ptl)(const void *header_buf, uint32_t header_len, const void *data_packet, uint32_t data_len,
                            void *cb);

    /**
     * halt all writing data (for halt dma method)
     */
    int32_t (*spi_write_ptl_halt)(void);

    /**
     * halt all read data (for halt dma method)
     * \note It has not been verified.
     */
    int32_t (*spi_read_halt)(void);

    /**
     * ONLY SSPI master support,
     * spi master will do read and write simultaneously
     * ex. the API int32_t *(*spi_read_write_dma)(void *tx_data, uint32_t tx_len, void *rx_data, uint32_t rx_len, void *cb);
     *
     * \note  "tx_len" should be equals to "rx_len"
     *
     *   ex. to send 10 bytes of data and receive 10 bytes of data,
     *       please allocate a 10 bytes of tx_data buffer and 10 bytes of rx_data
     *       set tx_len = 10 and rx_len = 10
     *
     * \param[out] tx_data  the write data buffer array pointer
     *
     * \param[out]  tx_len write data buffer length in bytes, cannot exceed 4095 bytes
     *
     * \param[in] rx_data  the read data buffer array pointer
     *
     * \param[in] rx_len  read data buffer length in bytes, cannot exceed 4095 bytes
     *
     * \param[out] cb  callback function after operation is done
     */
    int32_t (*spi_read_write_dma)(void *tx_data, uint32_t tx_len,
                            void *rx_data, uint32_t rx_len, void *cb);

    /**
     * ONLY SSPI master support,
     * spi master will do write first with target length and then read dedicated length of data
     *
     *   ex. to send 3 bytes of data and then receive 5000 bytes of data,
     *       please allocate a 3 bytes of tx_data buffer and 5000 bytes of rx_data
     *       set tx_len = 3 and rx_len = 5000. it will send 3 bytes of data out and then receive 5000 bytes of data
     *
     * \param[out] tx_data  the write data buffer array pointer
     *
     * \param[out] tx_len  write data buffer length in bytes, cannot exceed 500K bytes
     *
     * \param[in] rx_data  the read data buffer array pointer
     *
     * \param[in] rx_len  read data buffer length in bytes, cannot exceed 500K bytes
     *
     * \param[out] cb  callback function after operation is done
     */
    int32_t (*spi_write_then_read_dma)(void* tx_data, uint32_t tx_len, 
                            void* rx_data, uint32_t rx_len, void* cb);
} DEV_SPI, *DEV_SPI_PTR;

typedef void* SPI_CTRL_PARAM;

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \brief	get current spi master handle, initial device if needed
     *
     * \param[in]	spi_id	 send id as input to get handle
     * \return	handler of select spi master.
     */
    extern DEV_SPI_PTR hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_E spi_id);

    /**
     * \brief	initial select spi master
     *
     * \param[in]	spi_id	 select spi master
     * \param[in]   base_addr The regiseter address of SPI master.
     * \return	error id.
     */
    // extern int hx_drv_spi_mst_init(USE_DW_SPI_MST_E spi_id);
    extern int hx_drv_spi_mst_init(USE_DW_SPI_MST_E spi_id, uint32_t base_addr);

    /**
     * \brief	de-initial select spi master
     *
     * \param[in]	spi_id	 select spi master
     * \return	error id.
     */
    extern int hx_drv_spi_mst_deinit(USE_DW_SPI_MST_E spi_id);

#ifdef USE_DW_SSPI
    extern int32_t* hx_drv_spi_s_get_tx_done_flag(void);
    int hx_spimcomm_register_tx_cb(uint32_t spi_id, spimcomm_cb_t aWritecb);
    int hx_spimcomm_register_rx_cb(uint32_t spi_id, spimcomm_cb_t aReadcb);
#endif
    /** =======================SPI SLAVE ========================================*/
    /**
     * \brief	get an \ref dev_spi slave "spi device" by spi slave device id.
     * 	For how to use spi slave device hal refer to \ref dev_spi slave "Functions in spi slave device structure"
     * \param[in]	spi_id	id of spi, defined by user
     * \retval	!NULL	pointer to an \ref dev_spi slave "spi slave device structure"
     * \retval	NULL	failed to find the spi slave device by \ref spi_id
     * \note	need to implemented by user in user code
     */
    extern DEV_SPI_PTR hx_drv_spi_slv_get_dev(USE_DW_SPI_SLV_E spi_id);

    /**
     * \brief	initial select spi slave
     *
     * \param[in]	spi_id	 select spi slave
     * \param[in]   base_addr The regiseter address of SPI slave.
     * \return	error id.
     */
    extern int hx_drv_spi_slv_init(USE_DW_SPI_SLV_E spi_id, uint32_t base_addr);

    /**
     * \brief	de-initial select spi slave
     *
     * \param[in]	spi_id	 select spi slave
     * \return	error id.
     */
    extern int hx_drv_spi_slv_deinit(USE_DW_SPI_SLV_E spi_id);

    /**
     * \brief	get max output freq. of spi based on source clock.
     *
     * \param[in]	spi_id	 select spi master
     * \param[in]   freq   pointer of freq.
     * \return	error id.
     */
    extern int32_t hx_spim_update_clk_get_maxfreq(USE_DW_SPI_MST_E spi_id, uint32_t *freq);

    /**
     * \brief	set width and brust size of DMA for spi
     * \param[in]	dma_info include spi id, tx dma request, rx dam request
     * \param[in]   format frame format of spi transfer.
     * \return error id.
     * \note	only support signel, dual, quad format.
     */
    extern int32_t hx_drv_spi_dma_set_config(SPI_DMA_TRANS_INFO_S *dma_info, SPI_FRAME_FORMAT_E format);


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* INC_HX_DRV_SPI_S_H_ */
