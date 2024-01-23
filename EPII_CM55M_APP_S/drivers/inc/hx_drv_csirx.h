#ifndef _HX_DRV_CSIRX_H_
#define _HX_DRV_CSIRX_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"

/**
 * \defgroup	CSIRX_DRV	CSIRX Hardware accelerator Driver
 * \ingroup		CSIRX_DRV
 * \brief	CSIRX Hardware accelerator Driver Declaration
 * \details Application can use CSIRX Structure and library to control Hardware\n
 * <pre>
 * For Example
 *
 * </pre>
 */
/**
 * \defgroup	CSIRX_DRV_ENUM	CSIRX Hardware accelerator Driver Enumeration
 * \ingroup	CSIRX_DRV
 * \brief	Defines some enumeration of CSIRX Hardware accelerator Driver need.
 * @{
 */

/**
 * \enum CSIRX_ERROR_E
 * \brief CSIRX Errors Type
 */
typedef enum
{
    CSIRX_NO_ERROR                           = 0,    /**< No ERROR */
    CSIRX_ERROR_NOT_INIT                     = -1,    /**< ERROR MSG: not init */
} CSIRX_ERROR_E;
/** @} */

/**
 * \defgroup	MIPIRX_DRV_REGSTRUCT	MIPIRX Driver Structure
 * \ingroup	MIPIRX_DRV
 * \brief	contains definitions of MIPIRX Driver structure.
 * @{
 */
/**
 * \struct MIPIRX_DPHYAREG_CFG_T
 * \brief MIPIRX DPHY AREG configuration by MIPIRX Driver
 */
typedef struct {
	uint8_t mipirx_dphy_areg00;						/**< MIPIRX DPHY ARGE00 */
	uint8_t mipirx_dphy_areg01;						/**< MIPIRX DPHY ARGE01 */
	uint8_t mipirx_dphy_areg02;						/**< MIPIRX DPHY ARGE02 */
} MIPIRX_DPHYAREG_CFG_T;

/**
 * \struct MIPIRX_DPHYHSCNT_CFG_T
 * \brief MIPIRX DPHY HS Counter configuration by MIPIRX Driver
 */
typedef struct {
	uint8_t mipirx_dphy_hscnt_clk_en;				/**< MIPIRX DPHY HS-Counter CLK enable */
	uint8_t mipirx_dphy_hscnt_ln0_en;				/**< MIPIRX DPHY HS-Counter LN0 enable */
	uint8_t mipirx_dphy_hscnt_ln1_en;				/**< MIPIRX DPHY HS-Counter LN1 enable */
	uint8_t mipirx_dphy_hscnt_clk_val;				/**< MIPIRX DPHY HS-Counter CLK value */
	uint8_t mipirx_dphy_hscnt_ln0_val;				/**< MIPIRX DPHY HS-Counter LN0 value */
	uint8_t mipirx_dphy_hscnt_ln1_val;				/**< MIPIRX DPHY HS-Counter LN1 value */
} MIPIRX_DPHYHSCNT_CFG_T;
/** @} */

/**
 * \brief   CSIRX enable
 *
 * \param[in]	reg_baseaddr_csi	   CSIRX base address
 * \param[in]	reg_baseaddr_dphy	   CSIRX DPHY base address
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_init(uint32_t reg_baseaddr_csi, uint32_t reg_baseaddr_dphy);

/**
 * \brief   CSIRX enable
 *
 * \param[in]	lane_nb	   CSIRX lane number
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_enable(uint8_t lane_nb);

/**
 * \brief   CSIRX disable
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_disable();

/**
 * \brief   CSIRX DPHY POWER ON
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_pwron();

/**
 * \brief   CSIRX DPHY POWER OFF
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_pwroff();

/**
 * \brief	get CSIRX DPHY AREG VALUE
 *
 * \param[out]	areg	DPHY AREG value
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_get_areg(MIPIRX_DPHYAREG_CFG_T *areg);

/**
 * \brief	set CSIRX DPHY AREG VALUE
 *
 * \param[in]	areg	DPHY AREG value
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_set_areg(MIPIRX_DPHYAREG_CFG_T areg);

/**
 * \brief	set CSIRX DPHY Virtual Channel Number
 *
 * \param[in]	vc_num	Selected VC number
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_set_vc_num(uint8_t vc_num);

/**
 * \brief	get CSIRX DPHY Virtual Channel Number
 *
 * \param[out]	vc_num	Selected VC number
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_vc_num(uint8_t* vc_num);

/**
 * \brief	get CSIRX DPHY HS-Count Config
 *
 * \param[out]	hs_cnt	DPHY HS-Count Config
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_get_hscnt(MIPIRX_DPHYHSCNT_CFG_T *hs_cnt);

/**
 * \brief	set CSIRX DPHY HS-Count Config
 *
 * \param[in]	hs_cnt	DPHY HS-Count Config
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_dphy_set_hscnt(MIPIRX_DPHYHSCNT_CFG_T hs_cnt);

/**
 * \brief	get CSIRX fifo fill
 *
 * \param[out]	fifo_fill	CSIRX fifo fill
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_fifo_fill(uint16_t *fifo_fill);

/**
 * \brief	set CSIRX fifo fill
 *
 * \param[in]	fifo_fill	CSIRX fifo fill
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_set_fifo_fill(uint16_t fifo_fill);

/**
 * \brief	get CSIRX INFO IRQ
 *
 * \param[in]	infoirq		CSIRX get infor irq
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_infoirq_state(uint32_t *infoirq);

/**
 * \brief	clr CSIRX infor IRQ
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_clr_infoirq_state();

/**
 * \brief	get CSIRX ERROR IRQ
 *
 * \param[in]	errirq		CSIRX get ERROR irq
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_errirq_state(uint32_t *errirq);

/**
 * \brief	clr CSIRX Error IRQ
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_clr_errirq_state();

/**
 * \brief	get CSIRX DPHY ERROR IRQ
 *
 * \param[in]	dphyerrirq		CSIRX get DPHY ERROR irq
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_dphyerrirq_state(uint32_t *dphyerrirq);

/**
 * \brief	clr CSIRX DPHY ERROR IRQ
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_clr_dphyerrirq_state();

/**
 * \brief	set DEFALT MIPI TUNCATE CFG
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_set_default_tuncatecfg();

/**
 * \brief	enable CSIRX dphy lane ctrl
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_enable_dphy_ctrl();

/**
 * \brief	get CSIRX DPHY clock lane stop state
 *
 * \return  clock stop state
 */
uint32_t hx_drv_csirx_get_clkln_stopstate();

/**
 * \brief	get CSIRX DPHY lane0 stop state
 *
 * \return  lane0 stop state
 */
uint32_t hx_drv_csirx_get_ln0_stopstate();

/**
 * \brief	get CSIRX DPHY lane1 stop state
 *
 * \return  lane stop state
 */
uint32_t hx_drv_csirx_get_ln1_stopstate();

/**
 * \brief	Get CSIRX pixel depth
 *
 * \param[out]	depth	pixel depth
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_pixel_depth(uint8_t *depth);

/**
 * \brief	set CSIRX pixel depth
 *
 * \param[in]	depth	pixel depth
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_set_pixel_depth(uint8_t depth);

/**
 * \brief	Get CSIRX LANE SWAP enable
 *
 * \param[out]	enable	LANE SWAP enable
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_get_lnswap_enable(uint8_t *enable);

/**
 * \brief	Set CSIRX LANE SWAP enable
 *
 * \param[in]	enable	LANE SWAP enable
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_set_lnswap_enable(uint8_t enable);

/**
 * \brief	Reset MIPI RX DPHY WRAPPER
 *
 * \return  CSIRX_ERROR_E.
 */
CSIRX_ERROR_E hx_drv_csirx_rst_mipi_dphy_wrapper();
#endif
