#ifndef _HX_DRV_CSITX_H_
#define _HX_DRV_CSITX_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
/**
 * \defgroup	CSITX_DRV	CSITX Hardware accelerator Driver
 * \ingroup		CSITX_DRV
 * \brief	CSITX Hardware accelerator Driver Declaration
 * \details Application can use CSITX Structure and library to control Hardware\n
 * <pre>
 * For Example
 *
 * </pre>
 */
/**
 * \defgroup	CSITX_DRV_ENUM	CSITX Hardware accelerator Driver Enumeration
 * \ingroup	CSITX_DRV
 * \brief	Defines some enumeration of CSITX Hardware accelerator Driver need.
 * @{
 */

/**
 * \enum CSITX_ERROR_E
 * \brief CSITX Errors Type
 */
typedef enum
{
    CSITX_NO_ERROR                           = 0,    /**< No ERROR */
    CSITX_ERROR_NOT_INIT                     = -1,    /**< ERROR MSG: not init */
} CSITX_ERROR_E;

/**
 * \enum CSITX_DPHYCLKMODE_E
 * \brief CSITX DPHYCLKMODE Type
 */
typedef enum
{
	CSITX_DPHYCLOCK_CONT                    = 0,    /**< No ERROR */
	CSITX_DPHYCLOCK_NON_CONT               	= 1,    /**< ERROR MSG: not init */
} CSITX_DPHYCLKMODE_E;
/** @} */

/**
 * \defgroup	MIPITX_DRV_REGSTRUCT	MIPITX Driver Structure
 * \ingroup	MIPITX_DRV
 * \brief	contains definitions of MIPITX Driver structure.
 * @{
 */
/**
 * \struct MIPITX_DPHYAREG_CFG_T
 * \brief MIPITX DPHY AREG configuration by MIPITX Driver
 */
typedef struct {
	uint8_t mipitx_dphy_areg0;						/**< MIPITX DPHY ARGE0 */
	uint8_t mipitx_dphy_areg1;						/**< MIPITX DPHY ARGE1 */
	uint8_t mipitx_dphy_areg2;						/**< MIPITX DPHY ARGE2 */
	uint8_t mipitx_dphy_areg3;						/**< MIPITX DPHY ARGE3 */
	uint8_t mipitx_dphy_areg4;						/**< MIPITX DPHY ARGE4 */
	uint8_t mipitx_dphy_areg5;						/**< MIPITX DPHY ARGE5 */
	uint8_t mipitx_dphy_areg6;						/**< MIPITX DPHY ARGE6 */
	uint8_t mipitx_dphy_areg7;						/**< MIPITX DPHY ARGE7 */
	uint8_t mipitx_dphy_areg8;						/**< MIPITX DPHY ARGE8 */
	uint8_t mipitx_dphy_areg9;						/**< MIPITX DPHY ARGE9 */
	uint8_t mipitx_dphy_areg10;						/**< MIPITX DPHY ARGE10 */
	uint8_t mipitx_dphy_areg11;						/**< MIPITX DPHY ARGE11 */
	uint8_t mipitx_dphy_areg12;						/**< MIPITX DPHY ARGE12 */
	uint8_t mipitx_dphy_areg100;					/**< MIPITX DPHY ARGE100 */
} MIPITX_DPHYAREG_CFG_T;
/** @} */

/**
 * \brief   CSITX Init
 *
 * \param[in]	reg_baseaddr_csi		CSITX reg base address
 * \param[in]	reg_baseaddr_dphy		CSITX DPHY reg base address
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_init(uint32_t reg_baseaddr_csi, uint32_t reg_baseaddr_dphy);

/**
 * \brief   CSITX enable
 *
 * \param[in]	lane_nb		CSITX lane number
 * \param[in]	line_len	CSITX line length
 * \param[in]	frame_len	CSITX frame length
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_enable(uint8_t lane_nb, uint16_t line_len, uint16_t frame_len);

/**
 * \brief   CSITX disable
 *
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_disable();

/**
 * \brief   CSITX dphy power on
 *
 * \param[in]	bit_rate	CSITX bit rate
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_dphy_pwron(uint16_t bit_rate);

/**
 * \brief   CSITX dphy WITH XTAL24M power on
 *
 * \param[in]	bit_rate	CSITX bit rate
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_dphy_wi_xtal24_pwron(uint16_t bit_rate);

/**
 * \brief   CSITX dphy power off
 *
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_dphy_pwroff();

/**
 * \brief   CSITX set fifo fill
 *
 * \param[in]	fifo_fill	CSITX fifo fill
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_set_fifo_fill(uint16_t fifo_fill);

/**
 * \brief   CSITX get fifo depth
 *
 * \param[out]	fifo_fill	CSITX fifo fill
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_get_fifo_fill(uint16_t *fifo_fill);

/**
 * \brief   CSITX set CLK MODE
 *
 * \param[in]	clkmode		CSITX CLK mode
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_set_dphy_clkmode(CSITX_DPHYCLKMODE_E clkmode);

/**
 * \brief   CSITX get CLK MODE
 *
 * \param[out]	clkmode		CSITX CLK mode
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_get_dphy_clkmode(CSITX_DPHYCLKMODE_E *clkmode);

/**
 * \brief   CSITX CLR IRQ state
 *
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_clr_irq_state();

/**
 * \brief   CSITX get IRQ State
 *
 * \param[out]	irq_state	CSITX IRQ state
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_get_irq_state(uint32_t *irq_state);

/**
 * \brief   CSITX set DESKEW enable
 *
 * \param[in]	enable	DESKEW enable
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_set_deskew(uint8_t enable);

/**
 * \brief   CSITX get DESKEW enable
 *
 * \param[out]	enable	DESKEW enable
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_get_deskew(uint8_t *enable);

/**
 * \brief   CSITX get DEPTH per pixel
 *
 * \param[out]	depth	dpeth per pixel
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_get_pixel_depth(uint8_t *depth);

/**
 * \brief   CSITX set DEPTH per pixel
 *
 * \param[in]	depth	depth per pixel
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_set_pixel_depth(uint8_t depth);

/**
 * \brief   CSITX set Continuous CLK out
 *
 * \param[in]	lane_nb	lane number
 * \return  CSITX_ERROR_E.
 */
CSITX_ERROR_E hx_drv_csitx_set_continuous_clk(uint8_t lane_nb);

#endif
