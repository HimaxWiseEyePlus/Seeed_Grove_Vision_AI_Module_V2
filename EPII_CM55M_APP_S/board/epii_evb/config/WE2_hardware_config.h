/*
 * WE2_hardware_config.h
 *
 *  Created on: 2021�~3��28��
 *      Author: 902447
 */

#ifndef BOARD_EPII_EVB_CONFIG_EPII_HARDWARE_CONFIG_H_
#define BOARD_EPII_EVB_CONFIG_EPII_HARDWARE_CONFIG_H_

/*----------------------------------------------------------------------------
 Define clocks
 *-------CM55M_NS_APP_ROM_BASE--------------------------------------------------*/
#define LSC_CLK                 (24000000U)
#define HSC_CLK                 (24000000U)
#define SB_CLK                  (24000000U)
#define AON_CLK                 (32000U)

#define AHB_M_HCLK              LSC_CLK
#define CM55S_CLK               LSC_CLK
#define AHB_2_HCLK    			LSC_CLK
#define AHB_3_HCLK              LSC_CLK
#define APB_0_PCLK              LSC_CLK/4

#define CM55M_CLK               HSC_CLK
#define NPU_CLK                 HSC_CLK
#define AXI_ACLK                HSC_CLK
#define AHB_0_HCLK              HSC_CLK/2
#define AHB_1_HCLK              HSC_CLK/2


#define AHB_4_CLK               SB_CLK

#define I3C_HC_REF_CLK          HSC_CLK
#define APB_1_PCLK              SB_CLK/4
#define pdm_clk                 LSC_CLK
#define i2s_host_sclk           LSC_CLK

#define  SYSTEM_CLOCK    (CM55M_CLK)


#endif /* BOARD_EPII_EVB_CONFIG_EPII_HARDWARE_CONFIG_H_ */
