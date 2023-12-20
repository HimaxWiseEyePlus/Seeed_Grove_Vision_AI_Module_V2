/*
 * hx_drv_ppc.h
 *
 *  Created on: 2021�~5��30��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_PPC_H_
#define DRIVERS_INC_HX_DRV_PPC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "hx_drv_scu.h"
/**
 * \defgroup	PPC_DRV	PPC  Driver
 * \ingroup		PPC_DRV
 */
/**
 * \defgroup	PPC_DRV_ENUM	PPC  Driver Enumeration
 * \ingroup	PPC_DRV
 * \brief	Defines some enumeration of PPC  Driver need.
 * @{
 */
/**
 * \enum PPC_ERROR_E
 * \brief PPC Errors Type
 */
typedef enum {
	PPC_NO_ERROR = 0, /**< No ERROR */
	PPC_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	PPC_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} PPC_ERROR_E;
/** @} */

/**
 * \brief	PPC interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \return	void.
 */
typedef void (*PPC_ISREvent_t) (uint32_t event);


/**
 * \defgroup	PPC_DRV_REGSTRUCT	PPC  Driver Structure
 * \ingroup	PPC_DRV
 * \brief	contains definitions of PPC  Driver structure.
 * @{
 */
/**
 * \struct PPC_FlashSys_DWC_CFG_T
 * \brief PPC DWC Configuration
 */
typedef struct {
	PPC_ISREvent_t callback;
	SCU_PPC_CFG_T dwc_cfg;
} PPC_FlashSys_DWC_CFG_T;

/**
 * \struct PPC_FlashSys_ISP_CFG_T
 * \brief PPC ISP Configuration
 */
typedef struct {
	PPC_ISREvent_t callback;
	SCU_PPC_CFG_T isp_cfg;
} PPC_FlashSys_ISP_CFG_T;

/**
 * \struct PPC_AHB3_CFG_T
 * \brief PPC AHB3 Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E dma2_state; /*!< DMA2 state*/
	SCU_PPC_NONSEC_E dma3_state; /*!< DMA3 */
	SCU_PPC_NONSEC_E sspi_host_state; /*!< SSPI Host State */
	SCU_PPC_NONSEC_E sspi_slave_state; /*!< SSPI Slave State */
	SCU_PPC_AP_E dma2_ap; /*!< DMA2 AP */
	SCU_PPC_AP_E dma3_ap; /*!< DMA2 AP */
	SCU_PPC_AP_E sspi_host_ap; /*!< SSPI Host AP */
	SCU_PPC_AP_E sspi_slave_ap; /*!< SSPI Slave AP */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_AHB3_CFG_T;

/**
 * \struct PPC_AHB1M4_CFG_T
 * \brief PPC AHB1M4 Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E dma0_state; /*!< DMA0 state*/
	SCU_PPC_NONSEC_E dma1_state; /*!< DMA1 state */
	SCU_PPC_NONSEC_E isp_state; /*!< ISP state */
	SCU_PPC_AP_E dma0_ap; /*!< DMA0 AP */
	SCU_PPC_AP_E dma1_ap; /*!< DMA1 AP */
	SCU_PPC_AP_E isp_ap; /*!< ISP AP */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_AHB1M4_CFG_T;

/**
 * \struct PPC_APB0NS_CFG_T
 * \brief PPC AHB0NS Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E uart1_state; /*!< uart1_state RS485*/
	SCU_PPC_NONSEC_E sculsc_state; /*!< scu lsc ctrl state*/
	SCU_PPC_NONSEC_E i3c_slave1_state; /*!< I3C Slave1 state */
	SCU_PPC_NONSEC_E pwm_state; /*!< PWM state */
	SCU_PPC_NONSEC_E i2cm_sensor_state; /*!< I2CM Sensor state */
	SCU_PPC_NONSEC_E sndwire_apb_state; /*!< Sound Wire APB state */
	SCU_PPC_NONSEC_E sndwire_pdi0_state; /*!< Sound Wire PDI0 state */
	SCU_PPC_NONSEC_E sndwire_pdi1_state; /*!< Sound Wire PDI1 state */
	SCU_PPC_NONSEC_E sndwire_pdi2_state; /*!< Sound Wire PDI1 state */
	SCU_PPC_NONSEC_E vad_d_state; /*!< VAD D state */
	SCU_PPC_NONSEC_E vad_a_state; /*!< VAD A state */
	SCU_PPC_NONSEC_E rop_d_state; /*!< ROP D state */
	SCU_PPC_NONSEC_E adcctrl_lv_state; /*!< ADCC LV state */
	SCU_PPC_NONSEC_E img_sc_status_state; /*!< Image Senctrl status state */
	SCU_PPC_NONSEC_E sw_reg_lsc_state; /*!< SW Reg LSC state */

	SCU_PPC_AP_E uart1_ap; /*!< uart1 AP RS485*/
	SCU_PPC_AP_E sculsc_ap; /*!< sculsc AP */
	SCU_PPC_AP_E i3c_slave1_ap; /*!< i3 cslave1 AP */
	SCU_PPC_AP_E pwm_ap; /*!< pwm AP */
	SCU_PPC_AP_E i2cm_sensor_ap; /*!< spi_host2 AP */
	SCU_PPC_AP_E sndwire_apb_ap; /*!< Sound Wire APB AP */
	SCU_PPC_AP_E sndwire_pdi0_ap; /*!< Sound Wire PDI0 AP */
	SCU_PPC_AP_E sndwire_pdi1_ap; /*!< Sound Wire PDI1 AP */
	SCU_PPC_AP_E sndwire_pdi2_ap; /*!< Sound Wire PDI1 AP */
	SCU_PPC_AP_E vad_d_ap; /*!< VAD D AP */
	SCU_PPC_AP_E vad_a_ap; /*!< VAD A AP */
	SCU_PPC_AP_E rop_d_ap; /*!< ROP D AP */
	SCU_PPC_AP_E adcctrl_lv_ap; /*!< ADCC LV */
	SCU_PPC_AP_E img_sc_status_ap; /*!< Image Senctrl status ap */
	SCU_PPC_AP_E sw_reg_lsc_ap; /*!< SW Reg LSC ap */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB0NS_CFG_T;

/**
 * \struct PPC_APB0PS_CFG_T
 * \brief PPC AHB0PS Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E i2cslave_state; /*!< I2C Slave (full/flash Wr) state*/
	SCU_PPC_NONSEC_E i2chost_state; /*!< I2C Host state */
	SCU_PPC_NONSEC_E i2shost_state; /*!< I2S_Host state */
	SCU_PPC_NONSEC_E uart0_state; /*!< UART0 state */
	SCU_PPC_NONSEC_E uart2_state; /*!< UART2 state */
	SCU_PPC_NONSEC_E pdm_state; /*!< PDM state */
	SCU_PPC_NONSEC_E gpio0_state; /*!< gpio0 state */
	SCU_PPC_NONSEC_E gpio1_state; /*!< gpio1 state */
	SCU_PPC_NONSEC_E gpio2_state; /*!< gpio2 state */
	SCU_PPC_NONSEC_E gpio3_state; /*!< gpio3 state */
	SCU_PPC_NONSEC_E mailbox_state; /*!< mailbox state */
	SCU_PPC_NONSEC_E i2sslave_state; /*!< i2s slave state */
	SCU_PPC_NONSEC_E i3cslave_state; /*!< I3C Slave state */
	SCU_PPC_NONSEC_E i2cslave1_state; /*!< I2C Slave1 state */
	SCU_PPC_NONSEC_E himaxpdm_state; /*!< Himax PDM state */
	SCU_PPC_AP_E i2cslave_ap; /*!< I2C Slave (full/flash Wr) ap*/
	SCU_PPC_AP_E i2chost_ap; /*!< I2C Host ap */
	SCU_PPC_AP_E i2shost_ap; /*!< I2S_Host ap */
	SCU_PPC_AP_E uart0_ap; /*!< UART0 ap */
	SCU_PPC_AP_E uart2_ap; /*!< UART1 ap */
	SCU_PPC_AP_E pdm_ap; /*!< PDM ap */
	SCU_PPC_AP_E gpio0_ap; /*!< gpio0 ap */
	SCU_PPC_AP_E gpio1_ap; /*!< gpio1 ap */
	SCU_PPC_AP_E gpio2_ap; /*!< gpio2 ap */
	SCU_PPC_AP_E gpio3_ap; /*!< gpio3 ap */
	SCU_PPC_AP_E mailbox_ap; /*!< mailbox ap */
	SCU_PPC_AP_E i2sslave_ap; /*!< i2s slave ap */
	SCU_PPC_AP_E i3cslave_ap; /*!< I3C Slave ap */
	SCU_PPC_AP_E i2cslave1_ap; /*!< I2C Slave1 ap */
	SCU_PPC_AP_E himaxpdm_ap; /*!< Himax PDM ap */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB0PS_CFG_T;

/**
 * \struct PPC_APB1NS_CFG_T
 * \brief PPC APB1NS Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E wdt0_state; /*!< wdt0 state*/
	SCU_PPC_NONSEC_E wdt1_state; /*!< wdt1 state */
	SCU_PPC_NONSEC_E apbtestreg_state; /*!< APB_TestReg state */
	SCU_PPC_NONSEC_E mipitx_aip_phy_state; /*!< MIPI TX AIP Phy state */
	SCU_PPC_NONSEC_E inpdp_state; /*!< INP DP state */
	SCU_PPC_NONSEC_E senctrl_state; /*!< Sensor Ctrl state */
	SCU_PPC_NONSEC_E mipirx_phy_state; /*!< MIPI RX Phy state */

	SCU_PPC_NONSEC_E timer0_state; /*!< TIMER0 state */
	SCU_PPC_NONSEC_E timer1_state; /*!< TIMER1 state */
	SCU_PPC_NONSEC_E timer2_state; /*!< TIMER2 state */
	SCU_PPC_NONSEC_E timer3_state; /*!< TIMER3 state */
	SCU_PPC_NONSEC_E timer4_state; /*!< TIMER4 state */
	SCU_PPC_NONSEC_E timer5_state; /*!< TIMER5 state */

	SCU_PPC_AP_E wdt0_ap; /*!< wdt0 ap*/
	SCU_PPC_AP_E wdt1_ap; /*!< wdt1 ap */
	SCU_PPC_AP_E apbtestreg_ap; /*!< APB_TestReg ap */
	SCU_PPC_AP_E mipitx_aip_phy_ap; /*!< MIPI TX AIP Phy ap */
	SCU_PPC_AP_E inpdp_ap; /*!< INP DP ap */
	SCU_PPC_AP_E senctrl_ap; /*!< Sensor Ctrl ap */
	SCU_PPC_AP_E mipirx_phy_ap; /*!< MIPI RX Phy ap */

	SCU_PPC_AP_E timer0_ap; /*!< TIMER0 ap */
	SCU_PPC_AP_E timer1_ap; /*!< TIMER1 ap */
	SCU_PPC_AP_E timer2_ap; /*!< TIMER2 ap */
	SCU_PPC_AP_E timer3_ap; /*!< TIMER3 ap */
	SCU_PPC_AP_E timer4_ap; /*!< TIMER4 ap */
	SCU_PPC_AP_E timer5_ap; /*!< TIMER5 ap */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB1NS_CFG_T;

/**
 * \struct PPC_APB1PS_CFG_T
 * \brief PPC APB1PS Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E sb_gpio_state; /*!< SB GPIO state*/
	SCU_PPC_NONSEC_E ts_ctrl_state; /*!< TS Ctrl state*/
	SCU_PPC_NONSEC_E adc_ctrl_hv_state; /*!< ADC CTRL HV state*/
	SCU_PPC_NONSEC_E scu_sb_state; /*!< SCU SB state*/
	SCU_PPC_NONSEC_E i2cm_mipi_state; /*!< I2C Master For MIPI state */
	SCU_PPC_NONSEC_E timer6_state; /*!< Timer6 state*/
	SCU_PPC_NONSEC_E timer7_state; /*!< Timer7 state*/
	SCU_PPC_NONSEC_E timer8_state; /*!< Timer8 state*/
	SCU_PPC_NONSEC_E apb_testreg_state; /*!< APB Test Reg state */

	SCU_PPC_AP_E sb_gpio_ap; /*!< SB GPIO AP*/
	SCU_PPC_AP_E ts_ctrl_ap; /*!< TS Ctrl ap*/
	SCU_PPC_AP_E adc_ctrl_hv_ap; /*!< ADC CTRL HV ap*/
	SCU_PPC_AP_E scu_sb_ap; /*!< SCU SB ap*/
	SCU_PPC_AP_E i2cm_mipi_ap; /*!< I2C Master For MIPI LV */
	SCU_PPC_AP_E timer6_ap; /*!< Timer6 ap*/
	SCU_PPC_AP_E timer7_ap; /*!< Timer7 ap*/
	SCU_PPC_AP_E timer8_ap; /*!< Timer8 ap*/
	SCU_PPC_AP_E apb_testreg_ap; /*!< APB Test Reg AP */
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB1PS_CFG_T;

/**
 * \struct PPC_APB2_CFG_T
 * \brief PPC APB2 Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E sdio_state; /*!< [0] SDIO*/
	SCU_PPC_NONSEC_E u55ctrl_state; /*!< [2] U55 Ctrl*/
	SCU_PPC_NONSEC_E cc312apbs_state; /*!< [3] CC-312 CTRL (APBS)*/
	SCU_PPC_NONSEC_E cc312apbsc_state; /*!< [4] CC-312 CTRL (APBSC)*/
	SCU_PPC_NONSEC_E pufrt_state; /*!< [5] PUFrt debug Port*/
	SCU_PPC_NONSEC_E mipitxctrl_state; /*!< [6] MIPITX CTRL */
	SCU_PPC_NONSEC_E scu_hsc_state; /*!< [7] SCU HSC Register*/
	SCU_PPC_NONSEC_E cs_dbg_state; /*!< [8] Debug authentication*/
	SCU_PPC_NONSEC_E cs_sdc_state; /*!< [9] SDC int*/
	SCU_PPC_NONSEC_E cs_apb_async_state; /*!< [10] SYS APB async bridge slave*/
	SCU_PPC_NONSEC_E mipirx_ctrl_state; /*!< [11] MPIP RX CTRL*/
	SCU_PPC_NONSEC_E i3c_hc_state; /*!< [15] I3C_HC*/
	SCU_PPC_NONSEC_E img_status_state; /*!< [16] Image_Path Status*/
	SCU_PPC_AP_E sdio_ap; /*!< [0] SDIO*/
	SCU_PPC_AP_E u55ctrl_ap; /*!< [2] U55 Ctrl*/
	SCU_PPC_AP_E cc312apbs_ap; /*!< [3] CC-312 CTRL (APBS)*/
	SCU_PPC_AP_E cc312apbsc_ap; /*!< [4] CC-312 CTRL (APBSC)*/
	SCU_PPC_AP_E pufrt_ap; /*!< [5] PUFrt debug Port*/
	SCU_PPC_AP_E mipitxctrl_ap; /*!< [6] MIPITX CTRL */
	SCU_PPC_AP_E scu_hsc_ap; /*!< [7] SCU HSC Register*/
	SCU_PPC_AP_E cs_dbg_ap; /*!< [8] Debug authentication*/
	SCU_PPC_AP_E cs_sdc_ap; /*!< [9] SDC int*/
	SCU_PPC_AP_E cs_apb_async_ap; /*!< [10] SYS APB async bridge slave*/
	SCU_PPC_AP_E mipirx_ctrl_ap; /*!< [11] MPIP RX CTRL*/
	SCU_PPC_AP_E i3c_hc_ap; /*!< [15] I3C_HC*/
	SCU_PPC_AP_E img_status_ap; /*!< [16] Image_Path Status*/
	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB2_CFG_T;

/**
 * \struct PPC_BOOTROM_CFG_T
 * \brief PPC Bootrom Configuration
 */
typedef struct {
	SCU_AXI_PPC_CFG_T bootrom_cfg;
	PPC_ISREvent_t callback;
} PPC_BOOTROM_CFG_T;

/**
 * \struct PPC_APB1AON_CFG_T
 * \brief PPC APB1AON Configuration
 */
typedef struct {
	SCU_PPC_NONSEC_E scu_aon_state; /*!< SCU AON state*/
	SCU_PPC_NONSEC_E pmu_state; /*!< PMU state*/
	SCU_PPC_NONSEC_E aon_gpio_state; /*!< AON GPIO state*/
	SCU_PPC_NONSEC_E rtc0_state; /*!< RTC0 state*/
	SCU_PPC_NONSEC_E rtc1_state; /*!< RTC1 state*/
	SCU_PPC_NONSEC_E rtc2_state; /*!< RTC2 state*/
	SCU_PPC_NONSEC_E sw_reg_aon_state; /*!< SW Reg AON state*/

	SCU_PPC_AP_E scu_aon_ap; /*!< scu AP */
	SCU_PPC_AP_E pmu_ap; /*!< PMU state*/
	SCU_PPC_AP_E aon_gpio_ap; /*!< AON GPIO AP*/
	SCU_PPC_AP_E rtc0_ap; /*!< RTC0 AP*/
	SCU_PPC_AP_E rtc1_ap; /*!< RTC1 AP*/
	SCU_PPC_AP_E rtc2_ap; /*!< RTC2 AP*/
	SCU_PPC_AP_E sw_reg_aon_ap; /*!< SW Reg AON ap*/

	SCU_PPC_RESP_E resp; /*!< PPC Resp */
	uint8_t irq_enable; /*!< PPC_IRQ_ENABLE */
	PPC_ISREvent_t callback;
} PPC_APB1AON_CFG_T;

/** @} */

/**
 * \defgroup	PPC_DRV_MACRO	PPC  Driver MACRO
 * \ingroup	PPC_DRV
 * \brief	contains definitions of PPC  Driver MACRO.
 * @{
 */

/** @} */

/**
 * \defgroup	PPC_DRV_FUNCDLR	 PPC Driver Function Declaration
 * \ingroup	PPC_DRV
 * \brief	Contains declarations of  PPC Driver functions.
 * @{
 */
/**
 * \brief	set DWC PPC configuration
 *
 * \param[in]	cfg	DWC PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_dwc(PPC_FlashSys_DWC_CFG_T cfg);
/**
 * \brief	set DWC PPC configuration
 *
 * \param[out]	cfg	DWC PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_dwc(PPC_FlashSys_DWC_CFG_T *cfg);

/**
 * \brief	set ISP PPC configuration
 *
 * \param[in]	cfg	ISP PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_isp(PPC_FlashSys_ISP_CFG_T cfg);
/**
 * \brief	set ISP PPC configuration
 *
 * \param[out]	cfg	ISP PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_isp(PPC_FlashSys_ISP_CFG_T *cfg);

/**
 * \brief	set AHB3 PPC configuration
 *
 * \param[in]	cfg	AHB3 PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_ahb3(PPC_AHB3_CFG_T cfg);
/**
 * \brief	set AHB3 PPC configuration
 *
 * \param[out]	cfg	AHB3 PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_ahb3(PPC_AHB3_CFG_T *cfg);

/**
 * \brief	set AHB1M4 PPC configuration
 *
 * \param[in]	cfg	AHB1M4 PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_ahb1m4(PPC_AHB1M4_CFG_T cfg);
/**
 * \brief	set AHB1M4 PPC configuration
 *
 * \param[out]	cfg	AHB1M4 PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_ahb1m4(PPC_AHB1M4_CFG_T *cfg);

/**
 * \brief	set APB0NS PPC configuration
 *
 * \param[in]	cfg	APB0NS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb0ns(PPC_APB0NS_CFG_T cfg);
/**
 * \brief	set APB0NS PPC configuration
 *
 * \param[out]	cfg	APB0NS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb0ns(PPC_APB0NS_CFG_T *cfg);

/**
 * \brief	set APB0PS PPC configuration
 *
 * \param[in]	cfg	APB0PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb0ps(PPC_APB0PS_CFG_T cfg);
/**
 * \brief	set APB0PS PPC configuration
 *
 * \param[out]	cfg	APB0PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb0ps(PPC_APB0PS_CFG_T *cfg);

/**
 * \brief	set APB1NS PPC configuration
 *
 * \param[in]	cfg	APB1NS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb1ns(PPC_APB1NS_CFG_T cfg);
/**
 * \brief	set APB1NS PPC configuration
 *
 * \param[out]	cfg	APB1NS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb1ns(PPC_APB1NS_CFG_T *cfg);

/**
 * \brief	set APB1PS PPC configuration
 *
 * \param[in]	cfg	APB1PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb1ps(PPC_APB1PS_CFG_T cfg);
/**
 * \brief	set APB1PS PPC configuration
 *
 * \param[out]	cfg	APB1PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb1ps(PPC_APB1PS_CFG_T *cfg);

/**
 * \brief	set APB1PS PPC configuration
 *
 * \param[in]	cfg	APB1PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb2(PPC_APB2_CFG_T cfg);
/**
 * \brief	set APB1PS PPC configuration
 *
 * \param[out]	cfg	APB1PS PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb2(PPC_APB2_CFG_T *cfg);

/**
 * \brief	set apb1_aon PPC configuration
 *
 * \param[in]	cfg	apb1_aon PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_apb1_aon(PPC_APB1AON_CFG_T cfg);
/**
 * \brief	set apb1_aon PPC configuration
 *
 * \param[out]	cfg	apb1_aon PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_apb1_aon(PPC_APB1AON_CFG_T *cfg);

/**
 * \brief	set BootROM PPC configuration
 *
 * \param[in]	cfg	BootROM PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_set_bootrom(PPC_BOOTROM_CFG_T cfg);
/**
 * \brief	set BootROM PPC configuration
 *
 * \param[out]	cfg	BootROM PPC configuration
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_get_bootrom(PPC_BOOTROM_CFG_T *cfg);

/**
 * \brief	register PPC DWC Interrupt Status callback
 *
 * \param[in]	cb_event	PPC DWC Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_dwc_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC ISP Interrupt Status callback
 *
 * \param[in]	cb_event	PPC ISP Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_isp_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC AHB3 Interrupt Status callback
 *
 * \param[in]	cb_event	PPC AHB3 Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_ahb3_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC AHB1M4 Interrupt Status callback
 *
 * \param[in]	cb_event	PPC AHB1M4 Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_ahb1m4_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC APB0NS Interrupt Status callback
 *
 * \param[in]	cb_event	PPC APB0NS Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb0ns_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC APB0PS Interrupt Status callback
 *
 * \param[in]	cb_event	PPC APB0PS Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb0ps_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC APB1NS Interrupt Status callback
 *
 * \param[in]	cb_event	PPC APB1NS Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb1ns_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC APB1PS Interrupt Status callback
 *
 * \param[in]	cb_event	PPC APB1PS Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb1ps_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC APB2 Interrupt Status callback
 *
 * \param[in]	cb_event	PPC APB2 Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb2_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC BOOTROM Interrupt Status callback
 *
 * \param[in]	cb_event	PPC BOOTROM Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_bootrom_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	register PPC apb1_aon Interrupt Status callback
 *
 * \param[in]	cb_event	PPC apb1_aon Interrupt callback function
 * \return	PPC_ERROR_E.
 */
PPC_ERROR_E hx_drv_ppc_register_apb1_aon_cb(PPC_ISREvent_t  cb_event);

/**
 * \brief	PPC ISP IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_isp_irq_handler(void);

/**
 * \brief	PPC AHB3 IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_ahb3_irq_handler(void);

/**
 * \brief	PPC AHB1M4 IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_ahb1m4_irq_handler(void);

/**
 * \brief	PPC AHB0NS IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_ahb0ns_irq_handler(void);

/**
 * \brief	PPC APB0PS IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_apb0ps_irq_handler(void);

/**
 * \brief	PPC APB1NS IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_apb1ns_irq_handler(void);

/**
 * \brief	PPC APB1PS IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_apb1ps_irq_handler(void);

/**
 * \brief	PPC APB2 IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_apb2_irq_handler(void);

/**
 * \brief	PPC BOOTROM IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_bootrom_irq_handler(void);

/**
 * \brief	PPC APB_AON IRQ handler
 *
 * \return	none.
 */
void hx_drv_ppc_apb_aon_irq_handler(void);

#endif /* DRIVERS_INC_HX_DRV_PPC_H_ */
