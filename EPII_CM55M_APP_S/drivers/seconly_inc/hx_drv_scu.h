/*
 * hx_drv_scu.h
 *
 *  Created on: 2021/4/21
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_SCU_H_
#define DRIVERS_INC_HX_DRV_SCU_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "hx_drv_timer.h"
#ifndef BOOT_USED
#include "hx_drv_rtc.h"
#include "hx_drv_watchdog.h"
#endif
#include "hx_drv_scu_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	SCU_DRV	SCU Driver
 * \ingroup		SCU_DRV
 * \brief	SCU Driver Declaration
 * \details Application can use SCU Structure to control Hardware\n
 * <pre>
 * For Example
 * Ex1: SCU_CFG_T: All SCU configuration for application to control SCU
 * </pre>
 */
/**
 * \defgroup	SCU_DRV_ENUM	SCU Driver Enumeration
 * \ingroup	SCU_DRV
 * \brief	Defines some enumeration of SCU Driver need.
 * @{
 */

/**
 * \enum SCU_CM55M_STATE_E
 * \brief SCU CM55M State
 */
typedef enum {
	SCU_CM55M_STATE_RESET = 0, /**< CM55M State Reset */
	SCU_CM55M_STATE_NORMAL, /**< CM55M State Normal */
} SCU_CM55M_STATE_E;

/**
 * \enum SCU_Coresight_nSRST_E
 * \brief SCU SCU_Coresight_nSRST_E (system reset request, low-active)
 */
typedef enum {
	SCU_Coresight_nSRST_RESET = 0, /**< Coresight system State Reset */
	SCU_Coresight_nSRST_NORMAL, /**< Coresight system state Normal */
} SCU_Coresight_nSRST_E;

/**
 * \enum SCU_CC312_SECMODE_E
 * \brief SCU CC312 Secure Mode
 */
typedef enum {
	SCU_CC312_SECMODE_SECURE = 0, /**< Secure Mode Secure */
	SCU_CC312_SECMODE_NS, /**< Secure Mode Non Secure */
} SCU_CC312_SECMODE_E;

/**
 * \enum SCU_U55_PORSL_E
 * \brief SCU U55 The Power-On-Reset Security Level (PORSL).
 */
typedef enum {
	SCU_U55_PORSL_SECURE = 0, /**< U55 Secure Level*/
	SCU_U55_PORSL_NSECURE, /**< U55 NS level */
} SCU_U55_PORSL_E;


/**
 * \enum SCU_MSC_MASTER_E
 * \brief SCU MSC Master.
 */
typedef enum {
	SCU_MSC_MASTER_SECURE = 0, /**< Secure Master*/
	SCU_MSC_MASTER_NSECURE, /**< NS Master */
} SCU_MSC_MASTER_E;

/**
 * \enum SCU_MSC_RESP_E
 * \brief SCU MSC Response.
 */
typedef enum {
	SCU_MSC_RESP_RAZ_WI = 0, /**<  reads return zero and it ignores writes.*/
	SCU_MSC_RESP_ERR, /**< MSC responds with an ERROR response. */
} SCU_MSC_RESP_E;

/**
 * \enum SCU_MSC_IRQ_RD_E
 * \brief SCU MSC IRQ RD Enable/Disable.
 */
typedef enum {
	SCU_MSC_IRQ_RD_DISABLE = 0, /**<  For read transactions, disable the generation of interrupts for security violations and faulty attribute conversions.*/
	SCU_MSC_IRQ_RD_ENABLE, /**<  For read transactions, enable the generation of interrupts for security violations and faulty attribute conversions. */
} SCU_MSC_IRQ_RD_E;

/**
 * \enum SCU_MSC_IRQ_WR_E
 * \brief SCU MSC IRQ WR Enable/Disable.
 */
typedef enum {
	SCU_MSC_IRQ_WR_DISABLE = 0, /**<  0 = For write transactions, disable the generation of interrupts for security violations and faulty attribute conversions.*/
	SCU_MSC_IRQ_WR_ENABLE, /**<   For write transactions, enable the generation of interrupts for security violations and faulty attribute conversions.*/
} SCU_MSC_IRQ_WR_E;

/**
 * \enum SCU_AHB3_PPC_POS_E
 * \brief SCU PPC AHB3 Pos
 */
typedef enum {
	SCU_AHB3_PPC_POS_DMA2 = 1, /**<  DMA 2 control*/
	SCU_AHB3_PPC_POS_DMA3 = 2, /**< DMA 3 control*/
	SCU_AHB3_PPC_POS_SSPI_HOST = 3, /**< SSPI Host control*/
	SCU_AHB3_PPC_POS_SSPI_SLAVE = 4, /**< SSPI Slave control*/
} SCU_AHB3_PPC_POS_E;

/**
 * \enum SCU_AHB1M4_PPC_POS_E
 * \brief SCU PPC AHB1M4 Pos
 */
typedef enum {
	SCU_AHB1M4_PPC_POS_DMA0 = 1, /**<  DMA 0 control*/
	SCU_AHB1M4_PPC_POS_DMA1, /**< DMA 1 control*/
	SCU_AHB1M4_PPC_POS_ISP, /**< ISP control*/
} SCU_AHB1M4_PPC_POS_E;

/**
 * \enum SCU_APB0NS_PPC_POS_E
 * \brief SCU PPC APB0NS Pos
 */
typedef enum {
	SCU_APB0NS_PPC_POS_UART1 = 0, /**<  UART1 RS485*/
	SCU_APB0NS_PPC_POS_SCU_LSC = 1, /**<  SCU LSC*/
	SCU_APB0NS_PPC_POS_I3C_SLAVE1 = 2, /**<  I3C Slave 1*/
	SCU_APB0NS_PPC_POS_PWM = 3, /**< PWM*/
	SCU_APB0NS_PPC_POS_I2CMSENSOR = 4, /**< I2CM Sensor*/
	SCU_APB0NS_PPC_POS_SNDWIRE_APB = 5, /**< SOUNDWIRE_APB*/
	SCU_APB0NS_PPC_POS_SNDWIRE_PDI0 = 6, /**< SOUNDWIRE_PDI0*/
	SCU_APB0NS_PPC_POS_SNDWIRE_PDI1 = 7, /**< SOUNDWIRE_PDI1*/
	SCU_APB0NS_PPC_POS_SNDWIRE_PDI2 = 8, /**< SOUNDWIRE_PDI2*/
	SCU_APB0NS_PPC_POS_VAD_D = 9, /**< VAD_D*/
	SCU_APB0NS_PPC_POS_VAD_A = 10, /**< VAD_A*/
	SCU_APB0NS_PPC_POS_ROP_D = 11, /**< ROP_D*/
	SCU_APB0NS_PPC_POS_ADCCTRL_LV = 12, /**< ADC_CTRL_LV*/
	SCU_APB0NS_PPC_POS_IMG_SC_STATUS = 14, /**< Image Senctrl status*/
	SCU_APB0NS_PPC_POS_SW_REG_LSC = 15, /**< SW_REG_LSC*/
} SCU_APB0NS_PPC_POS_E;

/**
 * \enum SCU_APB0PS_PPC_POS_E
 * \brief SCU PPC APB0PS Pos
 */
typedef enum {
	SCU_APB0PS_PPC_POS_I2C_Slave = 0, /**<  I2C Slave (full/flash Wr)*/
	SCU_APB0PS_PPC_POS_I2C_Host = 1, /**< I2C Host*/
	SCU_APB0PS_PPC_POS_I2S_Host = 2, /**< I2S_Host*/
	SCU_APB0PS_PPC_POS_UART_0 = 3, /**< UART_0*/
	SCU_APB0PS_PPC_POS_UART_2 = 4, /**< UART_2*/
	SCU_APB0PS_PPC_POS_PDM = 5, /**< PDM*/
	SCU_APB0PS_PPC_POS_GPIO_0 = 6, /**< GPIO_0*/
	SCU_APB0PS_PPC_POS_GPIO_1 = 7, /**< GPIO_1*/
	SCU_APB0PS_PPC_POS_GPIO_2 = 8 , /**< GPIO_2*/
	SCU_APB0PS_PPC_POS_GPIO_3 = 9, /**< GPIO_3*/
	SCU_APB0PS_PPC_POS_CPU_MAILBOX = 10, /**< CPU_MAILBOX*/
	SCU_APB0PS_PPC_POS_I2S_SLAVE = 11, /**< I2S_SLAVE*/
	SCU_APB0PS_PPC_POS_I3C_Slave = 13, /**< I3C Slave*/
	SCU_APB0PS_PPC_POS_I2C_Slave1 = 14, /**< I2C Slave1*/
	SCU_APB0PS_PPC_POS_HimaxPDM = 15, /**< Himax PDM*/
} SCU_APB0PS_PPC_POS_E;

/**
 * \enum SCU_APB1NS_PPC_POS_E
 * \brief SCU PPC APB1NS Pos
 */
typedef enum {
	SCU_APB1NS_PPC_POS_WDT0 = 0, /**<  WDT0*/
	SCU_APB1NS_PPC_POS_WDT1 = 1, /**< WDT1*/
	SCU_APB1NS_PPC_POS_APB_Test_Reg = 2, /**< APB Test Register*/
	SCU_APB1NS_PPC_POS_MIPI_TX_AIP_PHY = 3, /**< MIPI-TX PHY*/
	SCU_APB1NS_PPC_POS_INP_DP = 4, /**< INPDPP*/
	SCU_APB1NS_PPC_POS_Sensor_Ctrl = 5, /**< Sensor Ctrl*/
	SCU_APB1NS_PPC_POS_MIPI_RX_PHY = 6, /**< MIPI-RX PHY*/

	SCU_APB1NS_PPC_POS_Timer0 = 10, /**< Timer0*/
	SCU_APB1NS_PPC_POS_Timer1 = 11, /**< Timer1*/
	SCU_APB1NS_PPC_POS_Timer2 = 12, /**< Timer2*/
	SCU_APB1NS_PPC_POS_Timer3 = 13, /**< Timer3*/
	SCU_APB1NS_PPC_POS_Timer4 = 14, /**< Timer4*/
	SCU_APB1NS_PPC_POS_Timer5 = 15, /**< Timer5*/
} SCU_APB1NS_PPC_POS_E;


/**
 * \enum SCU_APB1PS_PPC_POS_E
 * \brief SCU PPC APB1PS Pos
 */
typedef enum {
	SCU_APB1PS_PPC_POS_SB_GPIO = 2, /**<  [2] SB_GPIO (0~7)*/
	SCU_APB1PS_PPC_POS_TS_CTRL = 4, /**<  [4] TS_CTRL*/
	SCU_APB1PS_PPC_POS_ADC_CTRL_HV = 5, /**<  [5] ADC_CTRL_HV*/
	SCU_APB1PS_PPC_POS_SCU_SB = 6, /**<  [6] SCU_SB*/
	SCU_APB1PS_PPC_POS_I2CM_MIPI = 8, /**< [8] I2CM MIPI*/
	SCU_APB1PS_PPC_POS_TIMER6 = 9, /**< [9] I2CM MIPI*/
	SCU_APB1PS_PPC_POS_TIMER7 = 10, /**< [10] I2CM MIPI*/
	SCU_APB1PS_PPC_POS_TIMER8 = 11, /**< [11] I2CM MIPI*/
	SCU_APB1PS_PPC_POS_APB_TEST_REG = 15, /**< APB Test Register*/
} SCU_APB1PS_PPC_POS_E;


/**
 * \enum SCU_APB2_PPC_POS_E
 * \brief SCU PPC APB2 Pos
 */
typedef enum {
	SCU_APB2_PPC_POS_SDIO = 0, /**<  [0] SDIO*/
	SCU_APB2_PPC_POS_MPC_OF_OSPI = 1, /**< [1] MPC of OSPI*/
	SCU_APB2_PPC_POS_U55_CTRL = 2, /**< [2] U55 CTRL*/
	SCU_APB2_PPC_POS_CC312_CTRL_APBS = 3, /**< [3] CC-312 CTRL (APBS)*/
	SCU_APB2_PPC_POS_CC312_CTRL_APBSC = 4, /**< [4] CC-312 CTRL (APBSC)*/
	SCU_APB2_PPC_POS_PUFRT_DEBUG_PORT = 5, /**< [5] PUFrt debug Port*/
	SCU_APB2_PPC_POS_MIPITX_CTRL = 6, /**< [6] MIPI TX CTRL*/
	SCU_APB2_PPC_POS_SCU_HSC = 7, /**< [7] SCU HSC*/
	SCU_APB2_PPC_POS_CS_DBG_AUTHEN = 8, /**< [8] Debug authentication*/
	SCU_APB2_PPC_POS_CS_SDC = 9, /**< [9] SDC int*/
	SCU_APB2_PPC_POS_CS_SYS_APB_ASYNC = 10, /**< [10] SYS APB async bridge slave*/
	SCU_APB2_PPC_POS_MIPI_RX_CTRL = 11,/**< [11] MPIP RX CTRL*/
	SCU_APB2_PPC_POS_MPC_OF_SRAM0 = 12,/**< [12] MPC of SRAM0*/
	SCU_APB2_PPC_POS_MPC_OF_SRAM1 = 13,/**< [13] MPC of SRAM1*/
	SCU_APB2_PPC_POS_MPC_OF_QSPI = 14,/**< [14] QSPI MPC*/
	SCU_APB2_PPC_POS_I3C_HC = 15,/**< [15] I3C_HC*/
	SCU_APB2_PPC_POS_IMG_STATUS = 16,/**< [16] Image_Path Status*/
} SCU_APB2_PPC_POS_E;

/**
 * \enum SCU_APB1_AON_PPC_POS_E
 * \brief SCU PPC APB1_AON Pos
 */
typedef enum {
	SCU_APB1_AON_PPC_POS_SCU_AON = 0, /**<  [0] SCU_AON*/
	SCU_APB1_AON_PPC_POS_PMU = 1, /**< [1] PMU*/
	SCU_APB1_AON_PPC_POS_AON_GPIO = 3, /**< [3] AON_GPIO*/
	SCU_APB1_AON_PPC_POS_RTC_0 = 7, /**< [7] RTC 0*/
	SCU_APB1_AON_PPC_POS_RTC_1 = 8, /**< [8] RTC 1*/
	SCU_APB1_AON_PPC_POS_RTC_2 = 9, /**< [9] RTC 2*/
	SCU_APB1_AON_PPC_POS_SWREG = 15,/**< [15] SWREG*/
} SCU_APB1_AON_PPC_POS_E;

/**
 * \enum SCU_PPC_AP_E
 * \brief
 * 0 : Non-privileged access disabled on selected port.
 * 1 : Non-privileged access enabled on selected port.
 * Non-privileged transfers are blocked if not enabled by cfg_ap for the given port
 */
typedef enum {
	SCU_PPC_AP_DISABLE = 0, /**<  0 : Non-privileged access disabled on selected port.*/
	SCU_PPC_AP_ENABLE, /**< 1 : Non-privileged access enabled on selected port.*/
} SCU_PPC_AP_E;

/**
 * \enum SCU_PPC_NONSEC_E
 * \brief
 * 0 : Secure on selected port.
 * 1 : Non-Secure on selected port.
 */
typedef enum {
	SCU_PPC_NONSEC_SECURE = 0, /**<  0 : Secure on selected port.*/
	SCU_PPC_NONSEC_NS, /**< 1 : Non-Secure on selected port.*/
} SCU_PPC_NONSEC_E;

/**
 * \enum SCU_PPC_RESP_E
 * \brief SCU PPC Response.
 */
typedef enum {
	SCU_PPC_RESP_RAZ_WI = 0, /**<  reads return zero and it ignores writes.*/
	SCU_PPC_RESP_ERR, /**< PPC responds with an ERROR response. */
} SCU_PPC_RESP_E;

/**
 * \enum SCU_MPC_NONSEC_E
 * \brief
 * 0 : Secure on selected port.
 * 1 : Non-Secure on selected port.
 */
typedef enum {
	SCU_MPC_NONSEC_SECURE = 0, /**<  0 : Secure on selected port.*/
	SCU_MPC_NONSEC_NS, /**< 1 : Non-Secure on selected port.*/
} SCU_MPC_NONSEC_E;

/**
 * \enum SCU_MPC_RESP_E
 * \brief SCU MPC Response.
 */
typedef enum {
	SCU_MPC_RESP_RAZ_WI = 0, /**<  reads return zero and it ignores writes.*/
	SCU_MPC_RESP_ERR, /**< PPC responds with an ERROR response. */
} SCU_MPC_RESP_E;

/** @} */

/**
 * \brief	SCU MSC interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*SCU_MSC_ISREvent_t) (uint32_t event);

/**
 * \defgroup	SCU_DRV_MACRO	SCU Driver MACRO
 * \ingroup	SCU_DRV
 * \brief	contains definitions of SCU Driver MACRO.
 * @{
 */
/**
 * \struct SCU_U55_AXI_MSC_CFG_T
 * \brief U55 AXI MSC Configuration
 */
typedef struct {
	SCU_MSC_MASTER_E state; /*!< REG_U55M0_MSC_CFG_NONSEC */
	SCU_MSC_RESP_E resp; /*!< REG_U55M0_MSC_CFG_SEC_RESP */
	uint8_t irq_rd_enable; /*!< REG_U55M0_MSC_IRQ_ENABLE_RD */
	uint8_t irq_wr_enable; /*!< REG_U55M0_MSC_IRQ_ENABLE_WR */
	SCU_MSC_ISREvent_t callback;
} SCU_U55_AXI_MSC_CFG_T;

/**
 * \struct SCU_AHB_MSC_CFG_T
 * \brief AHB MSC configuration Header
 */
typedef struct {
	SCU_MSC_MASTER_E state; /*!< AHB_MSC_CFG_NONSEC */
	SCU_MSC_RESP_E resp; /*!< AHB_MSC_CFG_SEC_RESP */
	uint8_t irq_enable; /*!< AHB_MSC_IRQ_ENABLE */
	SCU_MSC_ISREvent_t callback;
} SCU_AHB_MSC_CFG_T;

/**
 * \struct SCU_PPC_CFG_T
 * \brief PPC configuration Header
 */
typedef struct {
	uint32_t ap; /*!< PPC AP */
	uint32_t state; /*!< PPC NonSEC */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
} SCU_PPC_CFG_T;

/**
 * \struct SCU_AXI_PPC_CFG_T
 * \brief AXI PPC configuration Header
 */
typedef struct {
	uint32_t ap; /*!< PPC AP */
	uint32_t state; /*!< PPC NonSEC */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_rd_enable; /*!< IRQ_ENABLE_RD */
	uint8_t irq_wr_enable; /*!<IRQ_ENABLE_WR */
} SCU_AXI_PPC_CFG_T;

/**
 * \struct SCU_AXI_MPC_CFG_T
 * \brief MPC configuration Header
 */
typedef struct {
	uint32_t init_state; /*!< MPC Initial state */
	SCU_MPC_RESP_E resp; /*!< MPC resp */
	uint8_t irq_rd_enable; /*!< IRQ_ENABLE_RD */
	uint8_t irq_wr_enable; /*!< IRQ_ENABLE_WR */
} SCU_AXI_MPC_CFG_T;

/**
 * \struct SCU_AHB_MPC_CFG_T
 * \brief MPC configuration Header
 */
typedef struct {
	SCU_MPC_NONSEC_E init_state; /*!< MPC Initial state */
	uint8_t irq_enable; /*!< IRQ_ENABLE*/
} SCU_AHB_MPC_CFG_T;
/** @} */

/**
 * \defgroup	SCU_DRV_FUNCDLR	SCU Driver Function Declaration
 * \ingroup	SCU_DRV
 * \brief	Contains declarations of SCU Driver functions.
 * @{
 */
/**
 * \brief	initial SCU Driver
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_init();

/**
 * \brief	set Timer Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_timer_clk_en(TIMER_ID_E id, uint8_t clk_en);

/**
 * \brief	get Timer Clock Enable
 *
 * \param[out]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_timer_clk_en(TIMER_ID_E id, uint8_t *clk_en);

/**
 * \brief	set Timer Clock Div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_timer_clkdiv(TIMER_ID_E id, uint32_t clk_div);

/**
 * \brief	get Timer clock div
 *
 * \param[out]	clk_div	 clock div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_timer_clkdiv(TIMER_ID_E id, uint32_t *clk_div);

#ifndef BOOT_USED
/**
 * \brief	set WDG Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_wdg_clk_en(WATCHDOG_ID_E id, uint8_t clk_en);

/**
 * \brief	get WDG Clock Enable
 *
 * \param[out]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_wdg_clk_en(WATCHDOG_ID_E id, uint8_t *clk_en);

/**
 * \brief	set WDG clk div
 *
 * \param[in]	clk_div	 clock div (0: divide by 1, 1: divide by 2)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t clk_div);

/**
 * \brief	get WDG clock div
 *
 * \param[out]	clk_div	 clock div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_wdg_clkdiv(WATCHDOG_ID_E id, uint32_t *clk_div);



/**
 * \brief	set RTC State
 *
 * \param[in]	state	 RTC state
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_rtc_state(RTC_ID_E id, SCU_RTC_STATE_E state);

/**
 * \brief	get RTC State
 *
 * \param[out]	state	 RTC state
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_rtc_state(RTC_ID_E id, SCU_RTC_STATE_E *state);
/**
 * \brief	set RTC Clock Enable
 *
 * \param[in]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_rtc_clk_en(RTC_ID_E id, uint8_t clk_en);

/**
 * \brief	get RTC Clock Enable
 *
 * \param[out]	clk_en	 Clock Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_rtc_clk_en(RTC_ID_E id, uint8_t *clk_en);

/**
 * \brief	set RTC cycle width
 *
 * \param[in]	cycle_width	 cycle width (if clock is 24Mhz, cycle_width 24Mhz is 1 HZ for RTC)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_rtc_cycle(RTC_ID_E id, uint32_t cycle_width);
/**
 * \brief	get RTC cycle width
 *
 * \param[out]	cycle_width	 cycle width
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_rtc_cycle(RTC_ID_E id, uint32_t *cycle_width);
#endif


/**
 * \brief	set CM55S State
 *
 * \param[in]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_E state);

/**
 * \brief	get CM55S State
 *
 * \param[out]	state	 CM55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cm55s_state(SCU_CM55S_STATE_E *state);

/**
 * \brief	set CM55S POR State
 *
 * \param[in]	state	 CM55S POR State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cm55s_por_state(SCU_CM55S_STATE_E state);

/**
 * \brief	get CM55S POR State
 *
 * \param[out]	state	 CM55S POR State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cm55s_por_state(SCU_CM55S_STATE_E *state);
#ifndef BOOT_USED
/**
 * \brief	set U55 State
 *
 * \param[in]	state	 U55 State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_u55_state(SCU_U55_STATE_E state);

/**
 * \brief	get U55 State
 *
 * \param[out]	state	 U55S State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_u55_state(SCU_U55_STATE_E *state);

/**
 * \brief	set CM55M State
 *
 * \param[in]	state	 CM55M State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cm55m_state(SCU_CM55M_STATE_E state);

/**
 * \brief	get CM55M State
 *
 * \param[out]	state	 CM55M State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cm55m_state(SCU_CM55M_STATE_E *state);

/**
 * \brief	set CM55M POR State
 *
 * \param[in]	state	 CM55M POR State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cm55m_por_state(SCU_CM55M_STATE_E state);

/**
 * \brief	get CM55M POR State
 *
 * \param[out]	state	 CM55M POR State
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cm55m_por_state(SCU_CM55M_STATE_E *state);


/**
 * \brief	set CC312 SYS_RST_N
 *
 * \param[in]	data	 SYS_RST_N
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cc312_SYS_RST_N(uint8_t data);

/**
 * \brief	set CC312 AO_RST_N
 *
 * \param[in]	data	 AO_RST_N
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cc312_AO_RST_N(uint8_t data);

/**
 * \brief	set CC312 HOST_INT_REQ
 *
 * \param[out]	data	 HOST_INT_REQ
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cc312_HOST_INT_REQ(uint8_t *data);


/**
 * \brief	set I2S mode
 *
 * \param[in]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_I2S_mode(SCU_I2S_MODE_E mode);

/**
 * \brief	get I2S mode
 *
 * \param[out]	mode	 I2S mode
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_I2S_mode(SCU_I2S_MODE_E *mode);

/**
 * \brief	set PDM IP Selection
 *
 * \param[in]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PDM_IP(SCU_PDM_IP_E mode);

/**
 * \brief	get PDM IP Selection
 *
 * \param[out]	mode	 PDM Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_PDM_IP(SCU_PDM_IP_E *mode);

#endif//#ifndef BOOT_USED


/**
 * \brief	set ISP Write Enable
 *
 * \param[in]	data	ISP Write Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_isp_write_en(uint8_t data);

#ifndef BOOT_USED
/**
 * \brief	get  ISP Write Enable
 *
 * \param[out]	data	ISP Write Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_isp_write_en(uint8_t *data);
#endif

/**
 * \brief	set XIP Enable
 *
 * \param[in]	data	XIP Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_xip_en(uint8_t data);

/**
 * \brief	set Coresight Debug all enable
 *
 * \param[in]	data	Coresight Debug all enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_debug_all_enable(uint8_t data);

/**
 * \brief	get  CM55S_CPUWAIT
 *
 * \param[out]	data	CM55S_CPUWAIT
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55S_CPUWAIT(SCU_CM55_CPUWAIT_E *data);

#ifndef BOOT_USED
/**
 * \brief	get  XIP Enable
 *
 * \param[out]	data	XIP Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_xip_en(uint8_t *data);

/**
 * \brief	get  Coresight Debug all enable
 *
 * \param[out]	data	Coresight Debug all enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_debug_all_enable(uint8_t *data);

/**
 * \brief	set CM55M_INITSVTOR
 *
 * \param[in]	address	CM55M_INITSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55M_INITSVTOR(uint32_t address);

/**
 * \brief	get  CM55M_INITSVTOR
 *
 * \param[out]	address	CM55M_INITSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55M_INITSVTOR(uint32_t *address);

/**
 * \brief	set CM55M_INITNSVTOR
 *
 * \param[in]	address	CM55M_INITNSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55M_INITNSVTOR(uint32_t address);

/**
 * \brief	get  CM55M_INITNSVTOR
 *
 * \param[out]	address	CM55M_INITNSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55M_INITNSVTOR(uint32_t *address);

/**
 * \brief	set CM55M_CPUWAIT
 *
 * \param[in]	data	CM55M_CPUWAIT
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55M_CPUWAIT(SCU_CM55_CPUWAIT_E data);



/**
 * \brief	set CM55S_INITSVTOR
 *
 * \param[in]	address	CM55S_INITSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55S_INITSVTOR(uint32_t address);

/**
 * \brief	get  CM55S_INITSVTOR
 *
 * \param[out]	address	CM55S_INITSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55S_INITSVTOR(uint32_t *address);

/**
 * \brief	set CM55S_INITNSVTOR
 *
 * \param[in]	address	CM55S_INITNSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55S_INITNSVTOR(uint32_t address);

/**
 * \brief	get  CM55S_INITNSVTOR
 *
 * \param[out]	address	CM55S_INITNSVTOR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55S_INITNSVTOR(uint32_t *address);

/**
 * \brief	get  CM55M_CPUWAIT
 *
 * \param[out]	data	CM55M_CPUWAIT
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55M_CPUWAIT(SCU_CM55_CPUWAIT_E *data);

/**
 * \brief	set CM55S_CPUWAIT
 *
 * \param[in]	data	CM55S_CPUWAIT
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPUWAIT_E data);




/**
 * \brief	set U55_PORPL The Power-On-Reset Privilege Level (PORPL)
 *
 * \param[in]	data	U55_PORPL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_U55_PORPL(SCU_U55_PORPL_E data);

/**
 * \brief	get  U55_PORPL The Power-On-Reset Privilege Level (PORPL)
 *
 * \param[out]	data	U55_PORPL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_U55_PORPL(SCU_U55_PORPL_E *data);

/**
 * \brief	set U55_PORPL The Power-On-Reset Security Level
 *
 * \param[in]	data	U55_PORSL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_U55_PORSL(SCU_U55_PORSL_E data);

/**
 * \brief	get  U55_PORSL The Power-On-Reset Security Level
 *
 * \param[out]	data	U55_PORSL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_U55_PORSL(SCU_U55_PORSL_E *data);
#endif
/**
 * \brief	get  Version
 *
 * \param[out]	version1	version
 * \param[out]	version2	version
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_version(uint32_t *version1, uint32_t *version2);
#ifndef BOOT_USED
/**
 * \brief	get  Chip ID
 *
 * \param[out]	chipid	chip ID
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_chipid(uint32_t *chipid);
#endif
#ifndef BOOT_USED
/**
 * \brief	set CM55M IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_nsc	IDAU NSC
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55M_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_nsc, uint8_t idau_ns);

/**
 * \brief	get  CM55M IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_nsc	IDAU NSC
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55M_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_nsc, uint8_t *idau_ns);

/**
 * \brief	set CM55S IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_nsc	IDAU NSC
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_CM55S_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_nsc, uint8_t idau_ns);

/**
 * \brief	get  CM55S IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_nsc	IDAU NSC
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_CM55S_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_nsc, uint8_t *idau_ns);

/**
 * \brief	set U55 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_U55_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  U55 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_U55_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set DMA0 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA0_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DMA0 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA0_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);
#endif
/**
 * \brief	set DMA1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA1_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DMA1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA1_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);
#ifndef BOOTLOADER
/**
 * \brief	set DMA2M1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA2M1_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DMA2M1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA2M1_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set DMA2M2 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA2M2_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);
#endif
/**
 * \brief	get  DMA2M2 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA2M2_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);
#ifndef BOOT_USED
/**
 * \brief	set DMA3M1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA3M1_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DMA3M1 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA3M1_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set DMA3M2 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DMA3M2_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DMA3M2 IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DMA3M2_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set DPDMA IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DPDMA_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  DPDMA IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DPDMA_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set I3CHC IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_I3CHC_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  I3CHC IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_I3CHC_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);

/**
 * \brief	set SDIO IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[in]	idau_unchk	IDAU Uncheck
 * \param[in]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_IDAU(uint8_t region, uint8_t idau_unchk, uint8_t idau_ns);

/**
 * \brief	get  SDIO IDAU
 *
 * \param[in]	region	region number <= 5
 * \param[out]	idau_unchk	IDAU Uncheck
 * \param[out]	idau_ns	IDAU NS
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_SDIO_IDAU(uint8_t region, uint8_t *idau_unchk, uint8_t *idau_ns);


/**
 * \brief	set M55 AXI M0 MSC Configuration
 *
 * \param[in]	cfg	 u55_m0_msc_cfg
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_u55_m0_msc_cfg(SCU_U55_AXI_MSC_CFG_T cfg);

/**
 * \brief	get  M55 AXI M0 MSC Configuration
 *
 * \param[out]	cfg	u55_m0_msc_cfg
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_u55_m0_msc_cfg(SCU_U55_AXI_MSC_CFG_T *cfg);

/**
 * \brief	clear M55 AXI M0 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_u55_m0_msc_irq_clear();

/**
 * \brief	set M55 AXI M1 MSC Configuration
 *
 * \param[in]	cfg	 u55_m0_msc_cfg
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_u55_m1_msc_cfg(SCU_U55_AXI_MSC_CFG_T cfg);

/**
 * \brief	get  M55 AXI M1 MSC Configuration
 *
 * \param[out]	cfg	u55_m0_msc_cfg
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_u55_m1_msc_cfg(SCU_U55_AXI_MSC_CFG_T *cfg);

/**
 * \brief	clear M55 AXI M1 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_u55_m1_msc_irq_clear();


/**
 * \brief	set DMA0 MSC Configuration
 *
 * \param[in]	cfg	 DMA0 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma0_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DMA0 MSC Configuration
 *
 * \param[out]	cfg	 DMA0 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma0_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DMA0 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma0_msc_irq_clear();
#endif
/**
 * \brief	set DMA1 MSC Configuration
 *
 * \param[in]	cfg	 DMA1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma1_msc_cfg(SCU_AHB_MSC_CFG_T cfg);
#ifndef BOOT_USED
/**
 * \brief	get  DMA1 MSC Configuration
 *
 * \param[out]	cfg	 DMA1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma1_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);
#endif
/**
 * \brief	clear DMA1 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma1_msc_irq_clear();


/**
 * \brief	set DMA2 M1 MSC Configuration
 *
 * \param[in]	cfg	 DMA2 M1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma2m1_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DMA2 M1 MSC Configuration
 *
 * \param[out]	cfg	 DMA2 M1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma2m1_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DMA2 M1 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma2m1_msc_irq_clear();

/**
 * \brief	set DMA2 M2 MSC Configuration
 *
 * \param[in]	cfg	 DMA2 M2 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma2m2_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DMA2 M2 MSC Configuration
 *
 * \param[out]	cfg	 DMA2 M2 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma2m2_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DMA2 M2 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma2m2_msc_irq_clear();

#ifndef BOOT_USED
/**
 * \brief	set DMA3 M1 MSC Configuration
 *
 * \param[in]	cfg	 DMA3 M1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma3m1_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DMA3 M1 MSC Configuration
 *
 * \param[out]	cfg	 DMA3 M1 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma3m1_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DMA3 M1 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma3m1_msc_irq_clear();

/**
 * \brief	set DMA3 M2 MSC Configuration
 *
 * \param[in]	cfg	 DMA3 M2 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma3m2_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DMA3 M2 MSC Configuration
 *
 * \param[out]	cfg	 DMA3 M2 MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dma3m2_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DMA3 M2 MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dma3m2_msc_irq_clear();

/**
 * \brief	set DP DMA MSC Configuration
 *
 * \param[in]	cfg	 DP DMA MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dpdma_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  DP DMA MSC Configuration
 *
 * \param[out]	cfg	 DP DMA MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dpdma_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear DP DMA MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dpdma_msc_irq_clear();

/**
 * \brief	set I3C Host MSC Configuration
 *
 * \param[in]	cfg	 I3C Host MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i3ch_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  I3C Host MSC Configuration
 *
 * \param[out]	cfg	 I3C Host MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_i3ch_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear I3C Host MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i3ch_msc_irq_clear();

/**
 * \brief	set SDIO MSC Configuration
 *
 * \param[in]	cfg	 SDIO MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sdio_msc_cfg(SCU_AHB_MSC_CFG_T cfg);

/**
 * \brief	get  SDIO MSC Configuration
 *
 * \param[out]	cfg	 SDIO MSC config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_sdio_msc_cfg(SCU_AHB_MSC_CFG_T *cfg);

/**
 * \brief	clear SDIO MSC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sdio_msc_irq_clear();

/**
 * \brief	register MSC U55M0 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC U55M0 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_u55m0_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC U55M1 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC U55M1 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_u55m1_msc_cb(SCU_MSC_ISREvent_t  cb_event);
#endif
#ifndef BOOT_USED
/**
 * \brief	register MSC DMA0 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA0 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma0_msc_cb(SCU_MSC_ISREvent_t  cb_event);
#endif
/**
 * \brief	register MSC DMA1 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA1 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma1_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC DMA2M1 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA2M1 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma2m1_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC DMA2M2 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA2M2 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma2m2_msc_cb(SCU_MSC_ISREvent_t  cb_event);

#ifndef BOOT_USED
/**
 * \brief	register MSC DMA3M1 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA3M1 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma3m1_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC DMA3M2 Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DMA3M2 Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dma3m2_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC DPDMA Interrupt Status callback
 *
 * \param[in]	cb_event	MSC DPDMA Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_dpdma_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC I3CH Interrupt Status callback
 *
 * \param[in]	cb_event	MSC I3CH Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_i3ch_msc_cb(SCU_MSC_ISREvent_t  cb_event);

/**
 * \brief	register MSC SDIO Interrupt Status callback
 *
 * \param[in]	cb_event	MSC SDIO Interrupt callback function
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_register_sdio_msc_cb(SCU_MSC_ISREvent_t  cb_event);
#endif

/**
 * \brief	set apb1_aon PPC Configuration
 *
 * \param[in]	cfg	 apb1_aon PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1_aon_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  apb1_aon PPC Configuration
 *
 * \param[out]	cfg	 apb1_aon PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb1_aon_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear apb1_aon PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1_aon_ppc_irq_clear();

/**
 * \brief	set apb2_aon_mipirxctrl PPC Configuration
 *
 * \param[in]	ap	 apb2_aon_mipirxctrl Privileged
 * \param[in]	nonsec	 apb2_aon_mipirxctrl sec/nonsec
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb2_aon_mipirxctrl_ppc_cfg(uint32_t ap, uint32_t nonsec);

/**
 * \brief	get  apb2_aon_mipirxctrl PPC Configuration
 *
 * \param[out]	ap	 apb2_aon_mipirxctrl Privileged
 * \param[out]	nonsec	 apb2_aon_mipirxctrl sec/nonsec
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb2_aon_mipirxctrl_ppc_cfg(uint32_t *ap, uint32_t *nonsec);

/**
 * \brief	clear apb2_aon_mipirxctrl PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb2_aon_mipirxctrl_ppc_irq_clear();

/**
 * \brief	set dwc PPC Configuration
 *
 * \param[in]	cfg	 dwc PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dwc_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  dwc PPC Configuration
 *
 * \param[out]	cfg	 dwc PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dwc_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear dwc PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dwc_ppc_irq_clear();

/**
 * \brief	set ISP PPC Configuration
 *
 * \param[in]	cfg	 ISP PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_isp_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  ISP PPC Configuration
 *
 * \param[out]	cfg	 ISP PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_isp_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear ISP PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_isp_ppc_irq_clear();


/**
 * \brief	set AHB3 PPC Configuration
 *
 * \param[in]	cfg	 AHB3 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ahb3_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  AHB3 PPC Configuration
 *
 * \param[out]	cfg	 AHB3 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_ahb3_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear AHB3 PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ahb3_ppc_irq_clear();

/**
 * \brief	set AHB1M4 PPC Configuration
 *
 * \param[in]	cfg	 AHB1M4 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ahb1m4_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  AHB1M4 PPC Configuration
 *
 * \param[out]	cfg	 AHB1M4 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_ahb1m4_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear AHB1M4 PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ahb1m4_ppc_irq_clear();

/**
 * \brief	set APB0_NS PPC Configuration
 *
 * \param[in]	cfg	 APB0_NS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb0ns_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  APB0_NS PPC Configuration
 *
 * \param[out]	cfg	 APB0_NS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb0ns_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear APB0_NS PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb0ns_ppc_irq_clear();

/**
 * \brief	set APB0_PS PPC Configuration
 *
 * \param[in]	cfg	 APB0_PS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb0ps_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  APB0_PS PPC Configuration
 *
 * \param[out]	cfg	 APB0_PS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb0ps_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear APB0_PS PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb0ps_ppc_irq_clear();

/**
 * \brief	set APB1_NS PPC Configuration
 *
 * \param[in]	cfg	 APB1_NS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1ns_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  APB1_NS PPC Configuration
 *
 * \param[out]	cfg	 APB1_NS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb1ns_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear APB1_NS PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1ns_ppc_irq_clear();

/**
 * \brief	set APB1_PS PPC Configuration
 *
 * \param[in]	cfg	 APB1_PS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1ps_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  APB1_PS PPC Configuration
 *
 * \param[out]	cfg	 APB1_PS PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb1ps_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear APB1_PS PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb1ps_ppc_irq_clear();

/**
 * \brief	set APB2 PPC Configuration
 *
 * \param[in]	cfg	 APB2 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb2_ppc_cfg(SCU_PPC_CFG_T cfg);

/**
 * \brief	get  APB2 PPC Configuration
 *
 * \param[out]	cfg	 APB2 PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_apb2_ppc_cfg(SCU_PPC_CFG_T *cfg);

/**
 * \brief	clear APB2 PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_apb2_ppc_irq_clear();

/**
 * \brief	set ROM PPC Configuration
 *
 * \param[in]	cfg	 ROM PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_rom_ppc_cfg(SCU_AXI_PPC_CFG_T cfg);

/**
 * \brief	get  ROM PPC Configuration
 *
 * \param[out]	cfg	 ROM PPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_rom_ppc_cfg(SCU_AXI_PPC_CFG_T *cfg);

/**
 * \brief	clear ROM PPC IRQ
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_rom_ppc_irq_clear();

/**
 * \brief	set SRAM0 MPC Configuration
 *
 * \param[in]	cfg	 SRAM0 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram0_mpc_cfg(SCU_AXI_MPC_CFG_T cfg);

/**
 * \brief	get  SRAM0 MPC Configuration
 *
 * \param[out]	cfg	 SRAM0 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_sram0_mpc_cfg(SCU_AXI_MPC_CFG_T *cfg);

/**
 * \brief	set SRAM0 MPC init state
 *
 * \param[in]	cfg	 SRAM0 MPC init state
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram0_mpc_init(uint32_t init_state);

/**
 * \brief	set SRAM1 MPC Configuration
 *
 * \param[in]	cfg	 SRAM1 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram1_mpc_cfg(SCU_AXI_MPC_CFG_T cfg);

/**
 * \brief	get  SRAM1 MPC Configuration
 *
 * \param[out]	cfg	 SRAM1 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_sram1_mpc_cfg(SCU_AXI_MPC_CFG_T *cfg);

/**
 * \brief	set SRAM1 MPC init state
 *
 * \param[in]	cfg	 SRAM1 MPC init state
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram1_mpc_init(uint32_t init_state);
/**
 * \brief	set SRAM2 MPC Configuration
 *
 * \param[in]	cfg	 SRAM2 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram2_mpc_cfg(SCU_AHB_MPC_CFG_T cfg);

/**
 * \brief	get  SRAM2 MPC Configuration
 *
 * \param[out]	cfg	 SRAM2 MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_sram2_mpc_cfg(SCU_AHB_MPC_CFG_T *cfg);

/**
 * \brief	set QSPICACHE MPC Configuration
 *
 * \param[in]	cfg	 QSPICACHE MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_qspicache_mpc_cfg(SCU_AHB_MPC_CFG_T cfg);

/**
 * \brief	get  QSPICACHE MPC Configuration
 *
 * \param[out]	cfg	 QSPICACHE MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_qspicache_mpc_cfg(SCU_AHB_MPC_CFG_T *cfg);

/**
 * \brief	set OSPI MPC Configuration
 *
 * \param[in]	cfg	 OSPI MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ospi_mpc_cfg(SCU_AHB_MPC_CFG_T cfg);

/**
 * \brief	get  OSPI MPC Configuration
 *
 * \param[out]	cfg	 OSPI MPC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_ospi_mpc_cfg(SCU_AHB_MPC_CFG_T *cfg);


/**
 * \brief	get  BOOT OPT
 *
 * \param[out]	opt boot option
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_boot_opt(SCU_BOOT_OPT_E *opt);

/**
 * \brief	set OSPI XIP enable
 *
 * \param[in]	enable	 OSPI XIP enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ospi_xip_en(uint8_t enable);

#ifndef BOOT_USED
/**
 * \brief	set OSPI slave
 *
 * \param[in]	data	 OSPI slave
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ospi_slave(uint8_t data);
/**
 * \brief	set OSPI ResetN
 *
 * \param[in]	normal	 1: normal, 0: reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_ospi_resetn(uint8_t normal);

/**
 * \brief	set VAD D PCM Selection
 *
 * \param[in]	data	 VAD D PCM Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_vad_d_pcm_sel(SCU_VAD_PCM_DATA_SEL_E data);

/**
 * \brief	set SDIO_ITCFVAL
 *
 * \param[in]	data	 SDIO_ITCFVAL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sdio_itcfval(uint32_t data);

/**
 * \brief	set SDIO_ITCFMUL
 *
 * \param[in]	data	 SDIO_ITCFMUL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sdio_itcfmul(uint32_t data);

/**
 * \brief	set SDIO_ITCFSELL
 *
 * \param[in]	data	 SDIO_ITCFSEL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sdio_itcfsel(uint32_t data);
#endif

#ifdef SUPPORT_CKGEN
#ifndef BOOTROM
/**
 * \brief	set VAD D SRC Selection
 *
 * \param[in]	data	 VAD D SRC Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_vad_d_src_sel(SCU_LSCVADDCLKSRC_E data);

/**
 * \brief	get VAD D SRC Selection
 *
 * \param[out]	data	 VAD D SRC Sel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_vad_d_src_sel(SCU_LSCVADDCLKSRC_E *data);

/**
 * \brief	set I2S Clk Div
 *
 * \param[in]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2s_clkdiv(uint8_t clk_div);

/**
 * \brief	get I2S Clk Div
 *
 * \param[out]	clk_div	 I2S Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_i2s_clkdiv(uint8_t *clk_div);

/**
 * \brief	set PDM Clk Div
 *
 * \param[in]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdm_clkdiv(uint8_t clk_div);

/**
 * \brief	get PDM Clk Div
 *
 * \param[out]	clk_div	 PDM Clock div (0:div1, 1:div2, 2:div3,....)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdm_clkdiv(uint8_t *clk_div);
#endif

/**
 * \brief	set XTAL32K enable
 *
 * \param[in]	enable	 xtal32K enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_xtal32k_en(uint8_t enable);

/**
 * \brief	get xtal32k enable
 *
 * \param[out]	enable	 xtal32K enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_xtal32k_en(uint8_t *enable);

/**
 * \brief	set XTAL24M enable
 *
 * \param[in]	enable	 xtal24m enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_xtal24m_en(uint8_t enable);

/**
 * \brief	get xtal24M enable
 *
 * \param[out]	enable	 xtal24m enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_xtal24m_en(uint8_t *enable);

/**
 * \brief	set XTAL24M frequency selection
 *
 * \param[in]	sel	 xtal24m frequency selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_xtal24m_sel(SCU_XTAL24MSEL_E sel);

/**
 * \brief	get xtal24M frequency selection
 *
 * \param[out]	sel	 xtal24m frequency selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_xtal24m_sel(SCU_XTAL24MSEL_E *sel);

/**
 * \brief	set RC24M1M Selection and enable
 *
 * \param[in]	rc24m1msel	 RC24M1M CLK Sel
 * \param[in]	rc24m1menable	 RC24M1M CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC24M1M_selen(SCU_RC24M1MSEL_E rc24m1msel, uint8_t rc24m1menable);

/**
 * \brief	get RC24M1M Selection
 *
 * \param[out]	rc24m1msel	 RC24M1M CLK Sel and enable
 * \param[out]	rc24m1menable	 RC24M1M CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC24M1M_selen(SCU_RC24M1MSEL_E *rc24m1msel, uint8_t *rc24m1menable);

/**
 * \brief	set RC24M1M trim
 *
 * \param[in]	rc24m1mtrim	 RC24M1M CLK Trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC24M1M_trim(uint8_t rc24m1mtrim);

/**
 * \brief	get RC24M1M trim
 *
 * \param[]	rc24m1mtrim	 RC24M1M CLK Trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC24M1M_trim(uint8_t *rc24m1mtrim);


/**
 * \brief	set RC96M48M Selection
 *
 * \param[in]	rc96m48msel	 RC96M48M CLK Sel
 * \param[in]	rc96m48men	 RC96M48M CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC96M48M_selen(SCU_RC96M48MSEL_E rc96m48msel, uint8_t rc96m48menable);

/**
 * \brief	get RC96M48M Selection
 *
 * \param[out]	rc96m48msel	 RC96M48M CLK Sel
 * \param[out]	rc96m48men	 RC96M48M CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC96M48M_selen(SCU_RC96M48MSEL_E *rc96m48msel, uint8_t *rc96m48menable);

/**
 * \brief	set RC96M48M Trim
 *
 * \param[in]	rc96m48mtrim	 RC96M48M CLK Trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC96M48M_trim(uint8_t rc96m48mtrim);

/**
 * \brief	get RC96M48M Trim
 *
 * \param[out]	rc96m48mtrim	 RC96M48M CLK Trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC96M48M_trim(uint8_t *rc96m48mtrim);


/**
 * \brief	set RC32K1K Selection
 *
 * \param[in]	rc32k1ksel	 RC32K1K CLK Sel
 * \param[in]	rc32k1kenable	 RC32K1K CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC32K1K_selen(SCU_RC32K1KSEL_E rc32k1ksel, uint8_t rc32k1kenable);

/**
 * \brief	get RC32K1K Selection
 *
 * \param[out]	rc32k1ksel	 RC32K1K CLK Sel
 * \param[out]	rc32k1kenable	 RC32K1K CLK enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC32K1K_selen(SCU_RC32K1KSEL_E *rc32k1ksel, uint8_t *rc32k1kenable);

/**
 * \brief	set RC32K1K trim
 *
 * \param[in]	rc32k1ktrim	 RC32K1K trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_RC32K1K_trim(uint8_t rc32k1ktrim);

/**
 * \brief	get RC32K1K trim
 *
 * \param[out]	rc32k1ktrim	 RC32K1K trim
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_RC32K1K_trim(uint8_t *rc32k1ktrim);


/**
 * \brief	set clk source selection and enable by system clk use case (not disable RC24M if you need disable RC24M call API directly because you must confirm you don't need)
 *
 * \param[in]	sysclksrc_sel	 sysclksrc_sel
 * \param[in]	disable_notreq	 disable not required clk source
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_clksrcselen_bysyscase(SCU_SYSCLKSRC_HW_E sysclksrc_sel, uint8_t disable_notreq);

/**
 * \brief	set PLL Clk source DIV
 *
 * \param[in]	clk_div	 clock division (0-->div1, 1-->div2, 2--> div3...)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pll_srcdiv(uint8_t clk_div);

/**
 * \brief	get PLL Clk source DIV
 *
 * \param[out]	clk_div	 clock division (0-->div1, 1-->div2, 2--> div3...)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pll_srcdiv(uint8_t *clk_div);



/**
 * \brief	set PLL Configuration and enable
 *
 * \param[in]	pll_cfg	pll related config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pll_cfg(SCU_PLL_CLK_CFG_T pll_cfg);


/**
 * \brief	get PLL Configuration
 *
 * \param[out]	pll_cfg	 System CLK Src
 * \param[out]	freq	 PLL CLK Frequency
 * \param[out]	pllsrccfg	 PLL CLK SRC
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pll_cfg(SCU_PLL_CLK_CFG_T *pll_cfg);
/**
 * \brief	get PLL Src
 *
 * \param[out]	pllclksrc	 PLL CLK Src
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pll_src(SCU_PLLCLKSRC_E *pllclksrc);


/**
 * \brief	set PLL enable and power on/off
 *
 * \param[in]	enable	pll enable(1)/disable(0)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pll_enable(uint8_t enable);

/**
 * \brief	get PLL lock status
 *
 * \param[out]	lock	 PLL Lock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pll_lock_status(uint8_t *lock);

#ifndef BOOTROM
/**
 * \brief	set PD HSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T cfg, uint8_t change_dprx, uint8_t change_dp);

/**
 * \brief	get PD HSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_dpclk_cfg(SCU_PDHSC_DPCLK_CFG_T *cfg);
#endif


/**
 * \brief	set PD HSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T cfg);
#ifndef BOOTROM
/**
 * \brief	get PD HSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_dpclken_cfg(SCU_PDHSC_DPCLKEN_CFG_T *cfg);
#endif
/**
 * \brief	set PD HSC HSC_CLK related Configuration
 *
 * \param[in]	cfg	 PD HSC HSC_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_hscclk_cfg(SCU_PDHSC_HSCCLK_CFG_T cfg);

/**
 * \brief	get PD HSC HSC_CLK related Configuration
 *
 * \param[out]	cfg	 PD HSC HSC_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_hscclk_cfg(SCU_PDHSC_HSCCLK_CFG_T *cfg);

/**
 * \brief	set PD HSC HSC_CLK related Configuration
 * QSPI ssi_clk = src / (div+1)
 * \param[in]	src	 PD HSC QSPI Clock Src
 * \param[in]	div	 PD HSC QSPI Clock Div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_qspisrc_cfg(SCU_QSPICLKSRC_E src, uint8_t div);

/**
 * \brief	get PD HSC QSPI CLK related Configuration
 *
 * \param[out]	src	 PD HSC QSPI Clock Src
 * \param[out]	div	 PD HSC QSPI Clock Div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_qspisrc_cfg(SCU_QSPICLKSRC_E *src, uint8_t *div);

#ifndef BOOTROM
/**
 * \brief	set PD HSC HSC_CLK related Configuration
 * OSPI ssi_clk = src / (div+1)
 * \param[in]	src	 PD HSC OSPI Clock Src
 * \param[in]	div	 PD HSC OSPI Clock Div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_ospisrc_cfg(SCU_OSPICLKSRC_E src, uint8_t div);

/**
 * \brief	get PD HSC OSPI CLK related Configuration
 *
 * \param[out]	src	 PD HSC OSPI Clock Src
 * \param[out]	div	 PD HSC OSPI Clock Div
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_ospisrc_cfg(SCU_OSPICLKSRC_E *src, uint8_t *div);

/**
 * \brief	set PD HSC HSC32K_CLK related Configuration
 *
 * \param[in]	cfg	 PD HSC HSC32K_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_hsc32kclk_cfg(SCU_HSC32KCLKSRC_E cfg);

/**
 * \brief	get PD HSC HSC32K_CLK related Configuration
 *
 * \param[out]	cfg	 PD HSC HSC32K_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_hsc32kclk_cfg(SCU_HSC32KCLKSRC_E *cfg);
#endif
#ifndef BOOTROM
/**
 * \brief	set PD HSC puf_CLK related Configuration
 *
 * \param[in]	cfg	 PD HSC puf_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_pufclk_cfg(SCU_HSCPUFCLKSRC_E cfg);
#endif
/**
 * \brief	get PD HSC puf_CLK related Configuration
 *
 * \param[out]	cfg	 PD HSC puf_CLK related Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_pufclk_cfg(SCU_HSCPUFCLKSRC_E *cfg);



/**
 * \brief	set PD HSC CLK Configuration
 *
 * \param[in]	cfg	 PD HSC CLK Configuration
 * \param[in]	change_dprx	 change DP RX or not
 * \param[in]	change_dp	 change DP or not
 * \param[in]	hsc32k_src_change	 HSC 32K SRC or not(1: change, 0: not)
 * \param[in]	purf_src_change	 PURF SRC or not(1: change, 0: not)
 * \param[in]	qspi_change	 QSPI (1: change, 0: not)
 * \param[in]	ospi_change	 OSPI (1: change, 0: not)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_clk_cfg(SCU_PDHSC_CLK_CFG_T cfg, uint8_t change_dprx, uint8_t change_dp, uint8_t hsc32k_src_change, uint8_t purf_src_change, uint8_t qspi_change, uint8_t ospi_change);

/**
 * \brief	get PD HSC CLK Configuration
 *
 * \param[out]	cfg	 PD HSC CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_clk_cfg(SCU_PDHSC_CLK_CFG_T *cfg);


/**
 * \brief	set PD HSC CLK enable Configuration
 *
 * \param[in]	cfg	 PD HSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T cfg);
#ifndef BOOTROM
/**
 * \brief	get PD HSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD HSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdhsc_clken_cfg(SCU_PDHSC_CLKEN_CFG_T *cfg);
#endif
#ifndef BOOTROM
/**
 * \brief	set PD LSC 32k CLK Configuration
 *
 * \param[in]	cfg	 PD LSC 32k CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_32k_cfg(SCU_LSC32KCLKSRC_E cfg);

/**
 * \brief	get PD LSC 32k CLK Configuration
 *
 * \param[out]	cfg	 PD LSC 32k CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_32k_cfg(SCU_LSC32KCLKSRC_E *cfg);
/**
 * \brief	set PD LSC DP CLK Configuration
 *
 * \param[in]	cfg	 PD LSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_dpclk_cfg(SCU_PDLSC_DPCLK_CFG_T cfg);

/**
 * \brief	get PD LSC DP CLK Configuration
 *
 * \param[out]	cfg	 PD LSC DP CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_dpclk_cfg(SCU_PDLSC_DPCLK_CFG_T *cfg);
#endif


/**
 * \brief	set PD LSC DP CLK enable Configuration
 *
 * \param[in]	cfg	 PD LSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_dpclken_cfg(SCU_PDLSC_DPCLKEN_CFG_T cfg);
#ifndef BOOTROM
/**
 * \brief	get PD LSC DP CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC DP CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_dpclken_cfg(SCU_PDLSC_DPCLKEN_CFG_T *cfg);
#endif
/**
 * \brief	set PD LSC lsc CLK Configuration
 *
 * \param[in]	cfg	 PD LSC lsc CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_lscclk_cfg(SCU_LSC_CLK_CFG_T cfg);

/**
 * \brief	get PD LSC lsc CLK Configuration
 *
 * \param[out]	cfg	 PD LSC lsc CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_lscclk_cfg(SCU_LSC_CLK_CFG_T *cfg);
#ifndef BOOTROM
/**
 * \brief	set PD LSC Reference CLK Configuration
 *
 * \param[in]	cfg	 PD LSC Reference CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_lscrefclk_cfg(SCU_PDLSC_LSCREF_CFG_T cfg);
#endif
/**
 * \brief	get PD LSC Reference CLK Configuration
 *
 * \param[out]	cfg	 PD LSC Reference CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_lscrefclk_cfg(SCU_PDLSC_LSCREF_CFG_T *cfg);
#ifndef BOOTROM
/**
 * \brief	set PD LSC adcckin CLK Configuration
 *
 * \param[in]	cfg	 PD LSC adcckin CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T cfg);

/**
 * \brief	get PD LSC adcckin CLK Configuration
 *
 * \param[out]	cfg	 PD LSC adcckin CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_adcckinclk_cfg(SCU_PDLSC_ADCCKIN_CFG_T *cfg);


/**
 * \brief	set PD LSC CLK Monitor CLK Configuration
 *
 * \param[in]	cfg	 PD LSC CLK Monitor CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_ckmonsrcclk_cfg(SCU_LSCCKMONSRC_E ckmonsrc_clk);

/**
 * \brief	get PD LSC CLK Monitor CLK Configuration
 *
 * \param[out]	cfg	 PD LSC CLK Monitor CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_ckmonsrcclk_cfg(SCU_LSCCKMONSRC_E *ckmonsrc_clk);

/**
 * \brief	set PD LSC SSPI Master CLK Configuration
 * SSPI Master ssi_clk clock frequency = sspimsrc_clk / (sspim_div + 1f)
 * Max. SSPIM Frequency is 100 MHz
 * \param[in]	cfg	 PD LSC SSPI Master CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_sspimclk_cfg(SCU_LSCSSPIMSRC_E sspimsrc_clk, uint8_t sspim_div);

/**
 * \brief	get PD LSC SSPI Master CLK Configuration
 *
 * \param[out]	cfg	 PD LSC SSPI Master CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_sspimclk_cfg(SCU_LSCSSPIMSRC_E *sspimsrc_clk, uint8_t *sspim_div);


#if(IC_VERSION >= 30)
/**
 * \brief	set PD LSC PDM CLK Source
 *
 * \param[in]	cfg	 PD LSC PDM CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E pdmsrc_clk);

/**
 * \brief	get PD LSC PDM CLK Source
 *
 * \param[out]	cfg	 PD LSC PDM CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_pdmck_src(SCU_LSCPDMCLKSRC_E *pdmsrc_clk);

#endif
#endif

/**
 * \brief	set PD LSC SSPI Slave CLK Configuration  
 * SSPI Slave ssi_clk clock frequency = sspissrc_clk / (sspis_div + 1)
 * Max. SSPIM Frequency is 400 MHz
 * \param[in]	cfg	 PD LSC SSPI Slave CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_sspisclk_cfg(SCU_LSCSSPISSRC_E sspissrc_clk, uint8_t sspis_div);

/**
 * \brief	get PD LSC SSPI Slave CLK Configuration
 *
 * \param[out]	cfg	 PD LSC SSPI Slave CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_sspisclk_cfg(SCU_LSCSSPISSRC_E *sspissrc_clk, uint8_t *sspis_div);


/**
 * \brief	set PD LSC CLK Configuration
 *
 * \param[in]	cfg	 PD LSC CLK Configuration
 * \param[in]	change_dp	 PD LSC Change DP related
 * \param[in]	vad_src_change	 PD LSC VAD Source Change
 * \param[in]	sspim_src_change	  SSPIM Source Change
 * \param[in]	sspis_src_change	  SSPIS Source Change
 * \param[in]	adc_src_change	  ADC Source Change
 * \return	SCU_ERROR_E.
 */
#if(IC_VERSION >= 30)
SCU_ERROR_E hx_drv_scu_set_pdlsc_clk_cfg(SCU_PDLSC_CLK_CFG_T cfg, uint8_t change_dp, uint8_t vad_src_change, uint8_t sspim_src_change, uint8_t sspis_src_change, uint8_t adc_src_change, uint8_t pdm_src_change);
#else
SCU_ERROR_E hx_drv_scu_set_pdlsc_clk_cfg(SCU_PDLSC_CLK_CFG_T cfg, uint8_t change_dp, uint8_t vad_src_change, uint8_t sspim_src_change, uint8_t sspis_src_change, uint8_t adc_src_change);
#endif
/**
 * \brief	get PD LSC CLK Configuration
 *
 * \param[out]	cfg	 PD LSC CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_clk_cfg(SCU_PDLSC_CLK_CFG_T *cfg);

/**
 * \brief	set PD LSC CLK enable Configuration
 *
 * \param[in]	cfg	 PD LSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T cfg);
#ifndef BOOTROM
/**
 * \brief	get PD LSC CLK enable Configuration
 *
 * \param[out]	cfg	 PD LSC CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdlsc_clken_cfg(SCU_PDLSC_CLKEN_CFG_T *cfg);
#endif
#ifndef BOOTROM
/**
 * \brief	set PD SB CLK Configuration
 *
 * \param[in]	cfg	 PD SB CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdsb_clk_cfg(SCU_PDSB_CLK_CFG_T cfg);
#endif
/**
 * \brief	get PD SB CLK Configuration
 *
 * \param[out]	cfg	 PD SB CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdsb_clk_cfg(SCU_PDSB_CLK_CFG_T *cfg);


/**
 * \brief	set PD SB CLK enable Configuration
 *
 * \param[in]	cfg	 PD SB CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T cfg);
#ifndef BOOTROM
/**
 * \brief	get PD SB CLK enable Configuration
 *
 * \param[out]	cfg	 PD SB CLK enable Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdsb_clken_cfg(SCU_PDSB_CLKEN_CFG_T *cfg);
#endif
#ifndef BOOTROM
/**
 * \brief	set PD AON CLK Configuration
 *
 * \param[in]	cfg	 PD AON CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdaon_clk_cfg(SCU_PDAON_CLK_CFG_T cfg);
#endif
/**
 * \brief	get PD AON CLK Configuration
 *
 * \param[out]	cfg	 PD AON CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdaon_clk_cfg(SCU_PDAON_CLK_CFG_T *cfg);


/**
 * \brief	set PD AON CLK Enable
 *
 * \param[in]	cfg	 PD AON CLK Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T cfg);
/**
 * \brief	get PD AON CLK Enable
 *
 * \param[out]	cfg	 PD AON CLK Enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdaon_clken_cfg(SCU_PDAON_CLKEN_CFG_T *cfg);

/**
 * \brief	set ALL CLK Enable
 *
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_all_clken_cfg();


/**
 * \brief	set Simple CLK Configuration without PLL
 *
 * \param[in]	cfg	 Simple CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_simple_clk_cfg(SCU_SIMPLE_CLK_CFG_T cfg);

/**
 * \brief	get Simple CLK Configuration
 *
 * \param[out]	cfg	 Simple CLK Configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_simple_clk_cfg(SCU_SIMPLE_CLK_CFG_T *cfg);

/**
 * \brief	get current Frequency
 *
 * \param[in]	type	 Frequency Type
 * \param[out]	freq	 Freq
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_freq(SCU_CLK_FREQ_TYPE_E type, uint32_t *freq);

/**
 * \brief	clksrc top enable control
 *
 * \return	SCU_ERROR_E.
 */
void hx_drv_scu_clksrc_top_enable(uint32_t enable);


#endif

/**
 * \brief	set disable PD_LSC i2c2ahbdbg access
 *
 * \param[in]	disable	 Disable I2C2AHBDBG access
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2c2ahbdbg_limit_access(uint8_t disable);

/**
 * \brief	set disable i2c2ahbdbg
 *
 * \param[in]	disable	 Disable I2C2AHBDBG access
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2c2ahbdbg_access(uint8_t disable);

#ifndef BOOTROM
/**
 * \brief	set MCLK Clock selection
 *
 * \param[in]	ctrl	 MCLK Clock selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_mclk_ctrl(SCU_MCLKCTRL_SEL_E ctrl);

/**
 * \brief	set MCLK Clock enable
 *
 * \param[in]	ctrl	 MCLK Clock enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_mclk_enable(uint8_t enable);
#endif

#ifndef BOOT_USED
/**
 * \brief	set disable JTAG access
 *
 * \param[in]	disable	 disable JTAG access
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_JTAG_access(uint8_t disable);

/**
 * \brief	set I2CDBG Slave ID
 *
 * \param[in]	slave_id	 i2cdbg slave ID
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2cdbg_slaveid(uint8_t slave_id);

/**
 * \brief	set I2CDBG Slave ID
 *
 * \param[in]	slave_id	 i2cdbg slave ID
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2cflash_slaveid(uint8_t slave_id);

/**
 * \brief	set DP Wakeup Control Selection
 *
 * \param[in]	ctrl	 DP Wakeup Control Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dp_wakeupctrl_sel(SCU_DP_WAKEUPCTRL_SEL_E ctrl);


/**
 * \brief	get DP Wakeup Control Selection
 *
 * \param[out]	ctrl	 DP Wakeup Control Selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_dp_wakeupctrl_sel(SCU_DP_WAKEUPCTRL_SEL_E *ctrl);




/**
 * \brief	get MCLK Clock selection
 *
 * \param[out]	ctrl	 MCLK Clock selection
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_mclk_ctrl(SCU_MCLKCTRL_SEL_E *ctrl);

/**
 * \brief	get MCLK Clock enable
 *
 * \param[out]	ctrl	 MCLK Clock enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_mclk_enable(uint8_t *enable);

/**
 * \brief	set CM55S Reset Mask
 *
 * \param[in]	mask	CM55S Reset Mask
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_cm55sreset_mask(uint8_t mask);


/**
 * \brief	get CM55S Reset Mask
 *
 * \param[out]	mask	 CM55S Reset Mask
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_cm55sreset_mask(uint8_t *mask);

/**
 * \brief	set timer auto clear enable
 *
 * \param[in]	enable	[8:0] REG_TIMER_INT_AUTO_CLR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_timerautoclear(uint32_t enable);

/**
 * \brief	set APBBRG_AUD_DMA
 *
 * \param[in]	REG_APBBRG_AUD_DMA_EN
 * \param[in]	REG_AUD_CH_SEL
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_APBBRG_AUD_DMA(SCU_REG_APBBRG_AUD_DMA_E dma_en, SCU_AUD_CH_SEL_E ch_sel);
/**
 * \brief	set PA1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver Pull Disable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver I2CM,UART Pull enable/High. Others Pull Disable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PA3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (UART, I2CM pull enable/pull high. others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SEN_I2C_MST_SDA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_I2C_MST_SDA_pinmux(SCU_SEN_I2CM_SDA_PINMUX_E pinmux);

/**
 * \brief	set SEN_I2C_MST_SCL pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_I2C_MST_SCL_pinmux(SCU_SEN_I2CM_SCL_PINMUX_E pinmux);

/**
 * \brief	set SEN_FAE pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_FAE_pinmux(SCU_SEN_FAE_PINMUX_E pinmux);

/**
 * \brief	set SEN_TRIG pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_TRIG_pinmux(SCU_SEN_TRIG_PINMUX_E pinmux);

/**
 * \brief	set SEN_INT pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_INT_pinmux(SCU_SEN_INT_PINMUX_E pinmux);

/**
 * \brief	set SEN_XSLEEP pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_XSLEEP_pinmux(SCU_SEN_XSLEEP_PINMUX_E pinmux);

/**
 * \brief	set SEN_MCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_MCLK_pinmux(SCU_SEN_MCLK_PINMUX_E pinmux);

/**
 * \brief	set SEN_GPIO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_GPIO_pinmux(SCU_SEN_GPIO_PINMUX_E pinmux);

/**
 * \brief	set SEN_CSW0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_CSW0_pinmux(SCU_SEN_CSW0_PINMUX_E pinmux);

/**
 * \brief	set SEN_CSW1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_CSW1_pinmux(SCU_SEN_CSW1_PINMUX_E pinmux);

/**
 * \brief	set SEN_PCLKO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_PCLKO_pinmux(SCU_SEN_PCLK_PINMUX_E pinmux);

/**
 * \brief	set SEN_D0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D0_pinmux(SCU_SEN_D0_PINMUX_E pinmux);

/**
 * \brief	set SEN_D1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D1_pinmux(SCU_SEN_D1_PINMUX_E pinmux);

/**
 * \brief	set SEN_D2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D2_pinmux(SCU_SEN_D2_PINMUX_E pinmux);

/**
 * \brief	set SEN_D3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D3_pinmux(SCU_SEN_D3_PINMUX_E pinmux);

/**
 * \brief	set SEN_D4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D4_pinmux(SCU_SEN_D4_PINMUX_E pinmux);

/**
 * \brief	set SEN_D5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D5_pinmux(SCU_SEN_D5_PINMUX_E pinmux);

/**
 * \brief	set SEN_D6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D6_pinmux(SCU_SEN_D6_PINMUX_E pinmux);

/**
 * \brief	set SEN_D7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_D7_pinmux(SCU_SEN_D7_PINMUX_E pinmux);

/**
 * \brief	set SEN_FVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_FVALID_pinmux(SCU_SEN_FVALID_PINMUX_E pinmux);

/**
 * \brief	set SEN_LVALID pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SEN_LVALID_pinmux(SCU_SEN_LVALID_PINMUX_E pinmux);

/**
 * \brief	set SDIO_SDCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_SDCLK_pinmux(SCU_SDIO_SDCLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_CMD pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_CMD_pinmux(SCU_SDIO_CMD_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_DAT0_pinmux(SCU_SDIO_DATA0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_DAT1_pinmux(SCU_SDIO_DATA1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_DAT2_pinmux(SCU_SDIO_DATA2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SDIO_DAT3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SDIO_DAT3_pinmux(SCU_SDIO_DATA3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set SW_DATA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SW_DATA_pinmux(SCU_SW_DATA_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SW_CLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SW_CLK_pinmux(SCU_SW_CLK_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set PB2 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB5 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB6 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB7 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB8 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_E pinmux, uint8_t autocfg_pullcfg);


/**
 * \brief	set PB9 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB10 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB11 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PC3 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PC3_pinmux(SCU_PC3_PINMUX_E pinmux);

/**
 * \brief	set PC4 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PC4_pinmux(SCU_PC4_PINMUX_E pinmux);

#if 0//remove SCU 0524
/**
 * \brief	set OSPI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_OSPI_pinmux(SCU_OSPI_PINMUX_E pinmux);
#endif
/**
 * \brief	set I2CM_SCLSDA pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_I2CM_SCLSDA_pinmux(SCU_I2CM_SCLSDA_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

#endif

/**
 * \brief	set PB0 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SDIO, UART, I2CM pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB0_pinmux(SCU_PB0_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set PB1 pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable) 
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_PB1_pinmux(SCU_PB1_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set UART pin mux
 *
 * \param[in]	pinmux	pin mux
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (UART RX/TX pull enable/pull high. others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_UART_RX_TX_CTS_RTS_pinmux(SCU_UART_PINMUX_E pinmux, uint8_t autocfg_pullcfg);

/**
 * \brief	set SPIS_DO pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SPIS_DO_pinmux(SCU_SPIS_DO_PINMUX_E pinmux);

/**
 * \brief	set SPIS_DI pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SPIS_DI_pinmux(SCU_SPIS_DI_PINMUX_E pinmux);

/**
 * \brief	set SPIS_SCLK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SPIS_SCLK_pinmux(SCU_SPIS_SCLK_PINMUX_E pinmux);

/**
 * \brief	set SPIS_CS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SPIS_CS_pinmux(SCU_SPIS_CS_PINMUX_E pinmux);

#ifndef BOOT_USED
/**
 * \brief	set SRSTN pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SRSTN_pinmux(SCU_PAD_SRSTN_PINMUX_E pinmux);

/**
 * \brief	set SWCLK TCK pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SWCLKTCK_pinmux(SCU_PAD_SWCLK_TCK_PINMUX_E pinmux);

/**
 * \brief	set SWDIO TMS pin mux
 *
 * \param[in]	pinmux	pin mux
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SWDIO_TMS_pinmux(SCU_PAD_SWDIO_TMS_PINMUX_E pinmux);
#endif

#ifndef BOOT_USED
/**
 * \brief	set I2C_SLV Share Enable
 *
 * \param[in]	enable	disable or enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_I2C_SLV_share_enable(SCU_I2CSLV_SHARE_E enable);

/**
 * \brief	set I3C_SLV Share Enable
 *
 * \param[in]	enable	disable or enable
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_I3C_SLV_share_enable(SCU_I3CSLV_SHARE_E enable);
#endif//#ifndef BOOT_USED

/**
 * \brief	set QSPI related SW reset
 *
 * \param[in]	cfg	QSPI Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_QSPI_SWReset(SCU_SWRESET_E cfg);
#ifndef BOOTROM
/**
 * \brief	set AON related SW reset
 *
 * \param[in]	cfg	AON Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_AON_SWReset(SCU_AON_SWRESET_T cfg);

/**
 * \brief	set SB related SW reset
 *
 * \param[in]	cfg	SB Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_SB_SWReset(SCU_SB_SWRESET_T cfg);

/**
 * \brief	set HSC related SW reset
 *
 * \param[in]	cfg	HSC Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_HSC_SWReset(SCU_HSC_SWRESET_T cfg);
#endif
/**
 * \brief	set LSC related SW reset
 *
 * \param[in]	cfg	LSC Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_LSC_SWReset(SCU_LSC_SWRESET_T cfg);


#ifndef BOOT_USED
/**
 * \brief	set DP related SW reset
 *
 * \param[in]	cfg	DP Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_DP_SWReset(SCU_DP_SWRESET_T cfg);

/**
 * \brief	get DP related SW reset
 *
 * \param[out]	cfg	DP Related SW Reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_DP_SWReset(SCU_DP_SWRESET_T *cfg);
#endif

#ifndef BOOTROM
/**
 * \brief	set WDT control owner
 *
 * \param[in]	id 0 or 1
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_wdt_ctrl(uint32_t id, SCU_WDTCTRL_E ctrl);
#endif
/**
 * \brief	set Timer control owner
 *
 * \param[in]	id 0 ~ 8
 * \param[in]	ctrl enable by CPU or PMU
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_timer_ctrl(uint32_t id, SCU_TIMERCTRL_E ctrl);

#ifndef BOOT_USED
/**
 * \brief	set Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[in]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E channel);

/**
 * \brief	get Audio capture channel
 *
 * \param[in]	audio_type	 audio hardware selection
 * \param[out]	channel	 capture channel
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_audio_capture_channel(SCU_AUDIO_HWTYPE_E audio_type, SCU_AUDIO_CAPTURE_CHANNEL_E *channel);
#endif

#ifndef BOOT_USED
/**
 * \brief	set DC SB Clock
 *
 * \param[in]	cfg	 clock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdsb_dc_clk(SCU_SB_APB1AHB4_CLK_SEL_E cfg);

/**
 * \brief	set DC SB Clock
 *
 * \param[out]	cfg	 clock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdsb_dc_clk(SCU_SB_APB1AHB4_CLK_SEL_E *cfg);

/**
 * \brief	set Non DC SB Clock
 *
 * \param[in]	cfg	 clock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_pdsb_pmu_clk(SCU_SB_APB1AHB4_CLK_SEL_E cfg);

/**
 * \brief	set Non DC SB Clock
 *
 * \param[out]	cfg	 clock
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_pdsb_pmu_clk(SCU_SB_APB1AHB4_CLK_SEL_E *cfg);
#endif

#ifndef BOOT_USED
/**
 * \brief	set ADC LV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_lsc_adclv_reset(SCU_SWRESET_E cfg);
/**
 * \brief	set ADC HV Reset
 *
 * \param[in]	cfg	 reset
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_lsc_adchv_reset(SCU_SWRESET_E cfg);
#endif

#ifndef BOOT_USED
/**
 * \brief	get All Pin Mux Configuration
 *
 * \param[out]	pinmux_cfg	 all pin mux config
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg);
/**
 * \brief	set All Pin Mux Configuration
 *
 * \param[in]	pinmux_cfg	 reset
 * \param[in]	autocfg_pullcfg	 auto configuration PULL setting by driver (SWD, SDIO, UART, I2CM, SPIM_CS pull enable/pull high. Others pull disable)
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_all_pinmux_cfg(SCU_PINMUX_CFG_T *pinmux_cfg, uint8_t autocfg_pullcfg);
#endif

/**
 * \brief	CPU Do NOP Delay
 *
 * \param[in]	period_us	 nop period
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_cpu_nop_us(uint32_t period_us);


#ifndef BOOT_USED
#if(IC_VERSION >= 30)
/**
 * \brief	Sen MCLK and xShutdown control by register or ISP pin
 *
 * \param[in]	ctrl_src	 control by register or ISP pin
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sen_pad_ctrl_src(SCU_REG_SEN_SEL_SRC_E ctrl_src);

/**
 * \brief	Sen MCLK and xShutdown from internal or external pin(PB9 and PB10)
 *
 * \param[in]	input_sel	 input from PB9,PB10 or internal
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sen_pad_reg_input(SCU_REG_SEN_SEL_E input_sel);
#endif
#endif

#ifndef BOOT_USED
/**
 * \brief	Set VMUTE releated control
 *
 * \param[in]	ctrl	 VMUTE releated control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_vmute(SCU_VMUTE_CFG_T *ctrl);
#endif

#ifndef BOOT_USED
/**
 * \brief	Set SRAM Voltage Control
 *
 * \param[in]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_sram_volt(SCU_COREVOLT_SRAM_E ctrl);
/**
 * \brief	Get SRAM Voltage Control
 *
 * \param[out]	ctrl	 SRAM Voltage control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_sram_volt(SCU_COREVOLT_SRAM_E *ctrl);
#endif

/**
 * \brief	Get PAD pull configuration
 *
 * \param[in]	pull_cfg    all pad pull configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_all_pull_cfg(SCU_PAD_PULL_LIST_T *pull_cfg);

/**
 * \brief	Set PAD pull configuration
 *
 * \param[in]	pull_cfg    all pad pull configuration
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_all_pull_cfg(SCU_PAD_PULL_LIST_T *pull_cfg);


#ifndef BOOT_USED
/**
 * \brief	MSC U55M0 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_u55m0_irq_handler(void);

/**
 * \brief	MSC U55M1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_u55m1_irq_handler(void);

/**
 * \brief	MSC DMA0 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma0_irq_handler(void);

/**
 * \brief	MSC DMA1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma1_irq_handler(void);

/**
 * \brief	MSC DMA2M1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma2m1_irq_handler(void);

/**
 * \brief	MSC DMA2M2 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma2m2_irq_handler(void);

/**
 * \brief	MSC DMA3M1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma3m1_irq_handler(void);

/**
 * \brief	MSC DMA3M2 IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dma3m2_irq_handler(void);

/**
 * \brief	MSC DPDMA IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_dpdma_irq_handler(void);

/**
 * \brief	MSC I3CH IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_i3ch_irq_handler(void);

/**
 * \brief	MSC SDIO IRQ handler
 *
 * \return	none.
 */
void hx_drv_scu_msc_sdio_irq_handler(void);

/**
 * \brief	Check Memory Space attribute
 *
 * \param[in]	start_addr    start address for check memory attribute
 * \param[in]	size    memory size for check memory attribute
 * \param[in]	attr    memory attribute that app expected
 * \return	SCU_ERROR_E. memory space is as app expected or not
 */
SCU_ERROR_E hx_drv_scu_check_mem_attr(uint32_t start_addr, uint32_t size, SCU_MEM_ATTR_E attr);

/**
 * \brief	Set DP Clock source
 *
 * \param[in]	clksrc    set DP Clock source
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_dpclk(SCU_HSCDPCLKSRC_E clksrc);

/**
 * \brief	set HSC_CLK Premux as PLL or RC96/RC48
 *
 * \param[in]	pllmux	 1: PLL, 0:RC96/RC48
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_hscclk_pllrc96premux(uint8_t pllmux);

/**
 * \brief	set LSC_CLK Premux as PLL or RC96/RC48
 *
 * \param[in]	pllmux	 1: PLL, 0:RC96/RC48
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_lscclk_pllrc96premux(uint8_t pllmux);

/**
 * \brief	Set MIPI Control
 *
 * \param[in]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_mipi_ctrl(SCU_MIPI_CTRL_E ctrl);
/**
 * \brief	Get MIPI Control
 *
 * \param[out]	ctrl	 MIPI control
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_get_mipi_ctrl(SCU_MIPI_CTRL_E *ctrl);
#endif

/**
 * \brief	set disable I2C Interface to SPI Flash WR
 *
 * \param[in]	disable	 Disable I2C Interface to SPI Flash WR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_i2ctoflashwr_access(uint8_t disable);

/**
 * \brief	set disable SPI Interface to SPI Flash WR
 *
 * \param[in]	disable	 Disable SPI Interface to SPI Flash WR
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_spitoflashwr_access(uint8_t disable);

/**
 * \brief	set MIPI TX Phy RX clock source
 *
 * \param[in]	txphyrxclksrc	 MIPI TX Phy RX Clock source
 * \return	SCU_ERROR_E.
 */
SCU_ERROR_E hx_drv_scu_set_MIPITXPHY_RXClkSRC(SCU_TXPHY_RCLK_SRC_SEL_E txphyrxclksrc);

/** @} */
#ifdef __cplusplus
}
#endif
#endif /* DRIVERS_INC_HX_DRV_SCU_H_ */
