/**************************************************************************//**
 * @file     ARMCM55.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           ARMCM55 Device Series (configured for ARMCM55 with double precision FPU,
 *                                  DSP extension, MVE, TrustZone)
 * @version  V1.0.0
 * @date     27. March 2020
 ******************************************************************************/
/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WE2_ARMCM55_H
#define WE2_ARMCM55_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
	/* -------------------  Processor Exceptions Numbers  ----------------------------- */
	NonMaskableInt_IRQn = -14, /*  2 Non Maskable Interrupt */
	HardFault_IRQn = -13, /*  3 HardFault Interrupt */
	MemoryManagement_IRQn = -12, /*  4 Memory Management Interrupt */
	BusFault_IRQn = -11, /*  5 Bus Fault Interrupt */
	UsageFault_IRQn = -10, /*  6 Usage Fault Interrupt */
	SecureFault_IRQn = -9, /*  7 Secure Fault Interrupt */
	SVCall_IRQn = -5, /* 11 SV Call Interrupt */
	DebugMonitor_IRQn = -4, /* 12 Debug Monitor Interrupt */
	PendSV_IRQn = -2, /* 14 Pend SV Interrupt */
	SysTick_IRQn = -1, /* 15 System Tick Interrupt */

	/* -------------------  Processor Interrupt Numbers  ------------------------------ */
	CTIIRQ0_IRQn = 0,
	CTIIRQ1_IRQn = 1,
	sdcint_irq_IRQn = 2,
	Interrupt3_IRQn = 3,
	Interrupt4_IRQn = 4,
	Interrupt5_IRQn = 5,
	Interrupt6_IRQn = 6,
	MBOX_IRQn = 7,
	FPIXC_IRQn = 8,
	FPIDC_IRQn = 9,
	FPOFC_IRQn = 10,
	FPUFC_IRQn = 11,
	FPDZC_IRQn = 12,
	FPIOC_IRQn = 13,
	Interrupt14_IRQn = 14,
	Interrupt15_IRQn = 15,
	RTC0INTR_IRQn = 16,
	RTC1INTR_IRQn = 17,
	RTC2INTR_IRQn = 18,
	GPIO_GROUP_5_AON_IRQn = 19,
	SC_sen_int_IRQn = 20,
	APB1_AON_PPC_IRQn = 21,
	pmu_wdg_timeout_int_IRQn = 22,
	anti_tamp_int_IRQn = 23,
	Interrupt24_IRQn = 24,
	Interrupt25_IRQn = 25,
	Interrupt26_IRQn = 26,
	Interrupt27_IRQn = 27,
	Interrupt28_IRQn = 28,
	Interrupt29_IRQn = 29,
	Interrupt30_IRQn = 30,
	Interrupt31_IRQn = 31,
	WDOG0INT_IRQn = 32,
	WDOG1INT_IRQn = 33,
	TIMER0INT_IRQn = 34,
	TIMER1INT_IRQn = 35,
	TIMER2INT_IRQn = 36,
	TIMER3INT_IRQn = 37,
	TIMER4INT_IRQn = 38,
	TIMER5INT_IRQn = 39,
	APB_1_NS_PPC_IRQn = 40,
	APB_1_PS_PPC_IRQn = 41,
	GPIO_GROUP_4_SB_IRQn = 42,
	TS_CTRL_IRQn = 43,
	ADC_CTRL_HV_IRQn = 44,
	mipi_i2c_mst_trig_int_IRQn = 45,
	mipi_i2c_mst_stop_int_IRQn = 46,
	TIMER6INT_IRQn = 47,
	TIMER7INT_IRQn = 48,
	TIMER8INT_IRQn = 49,
	mipi_i2c_mst_noack_int_IRQn = 50,
	Interrupt51_IRQn = 51,
	Interrupt52_IRQn = 52,
	Interrupt53_IRQn = 53,
	Interrupt54_IRQn = 54,
	Interrupt55_IRQn = 55,
	Interrupt56_IRQn = 56,
	Interrupt57_IRQn = 57,
	Interrupt58_IRQn = 58,
	Interrupt59_IRQn = 59,
	Interrupt60_IRQn = 60,
	Interrupt61_IRQn = 61,
	Interrupt62_IRQn = 62,
	Interrupt63_IRQn = 63,
	DMAC2_DMACINTERR_IRQn = 64,
	DMAC2_DMACINTTC_IRQn = 65,
	DMAC2_DMACINTR_IRQn = 66,
	DMAC3_DMACINTERR_IRQn = 67,
	DMAC3_DMACINTTC_IRQn = 68,
	DMAC3_DMACINTR_IRQn = 69,
	adcc_lv_int_out_IRQn = 70,
	Interrupt71_IRQn = 71,
	I2S_HOST_rx_da_0_intr_IRQn = 72,
	I2S_HOST_rx_or_0_intr_IRQn = 73,
	I2S_HOST_tx_emp_0_intr_IRQn = 74,
	I2S_HOST_tx_or_0_intr_IRQn = 75,
	I2S_SLAVE_rx_da_0_intr_IRQn = 76,
	I2S_SLAVE_rx_or_0_intr_IRQn = 77,
	I2S_SLAVE_tx_emp_0_intr_IRQn = 78,
	I2S_SLAVE_tx_or_0_intr_IRQn = 79,
	PDM_int_rx_avail_IRQn = 80,
	PDM_int_err_IRQn = 81,
	PDM_int_clp_IRQn = 82,
	inpparser_error_int_out_IRQn = 83,
	SC_trigger_timeout_IRQn = 84,
	SC_sen_nframe_end_IRQn = 85,
	Soundwire_intp_IRQn = 86,
	pwm0_int_out_IRQn = 87,
	pwm1_int_out_IRQn = 88,
	pwm2_int_out_IRQn = 89,
	UART0_intr_IRQn = 90,
	UART1_intr_IRQn = 91,
	I2C_SLAVE_intr_IRQn = 92,
	I2C_MST_0_intr_IRQn = 93,
	I3C_SLAVE_intr_IRQn = 94,
	UART2_intr_IRQn = 95,
	I2C_MST_1_intr_IRQn = 96,
	AHB_SRAM2_MPC_IRQn = 97,
	AHB_3_PPC_IRQn = 98,
	APB0_NS_PPC_IRQn = 99,
	APB0_PS_PPC_IRQn = 100,
	DMA2_M1_MSC_IRQn = 101,
	DMA2_M2_MSC_IRQn = 102,
	DMA3_M1_MSC_IRQn = 103,
	DMA3_M2_MSC_IRQn = 104,
	SSPI_HOST_IRQn = 105,
	SSPI_SLAVE_IRQn = 106,
	GPIO_GROUP_0_IRQn = 107,
	GPIO_GROUP_1_IRQn = 108,
	GPIO_GROUP_2_IRQn = 109,
	GPIO_GROUP_3_IRQn = 110,
	VAD_D_IRQn = 111,
	I2C_Slave_1_IRQn = 112,
	I3C_Slave_1_IRQn = 113,
	HimaxPDM_int_rx_avail_IRQn = 114,
	HimaxPDM_int_err_IRQn = 115,
	HimaxPDM_int_clp_IRQn = 116,
	sw_pd0_int_IRQn = 117,
	sw_pd1_int_IRQn = 118,
	sw_pd2_int_IRQn = 119,
	Interrupt120_IRQn = 120,
	Interrupt121_IRQn = 121,
	Interrupt122_IRQn = 122,
	Interrupt123_IRQn = 123,
	Interrupt124_IRQn = 124,
	Interrupt125_IRQn = 125,
	Interrupt126_IRQn = 126,
	Interrupt127_IRQn = 127,
	DMAC0_DMACINTERR_IRQn = 128,
	DMAC0_DMACINTTC_IRQn = 129,
	DMAC0_DMACINTR_IRQn = 130,
	DMAC1_DMACINTERR_IRQn = 131,
	DMAC1_DMACINTTC_IRQn = 132,
	DMAC1_DMACINTR_IRQn = 133,
	Interrupt134_IRQn = 134,
	Interrupt135_IRQn = 135,
	inp_vsync_in_IRQn = 136,
	inp_hsync_in_IRQn = 137,
	inp_vsync_out_IRQn = 138,
	inp_hsync_out_IRQn = 139,
	REG_INP_S2P_ERROR_INT_IRQn = 140,
	DP_CDM_MOTION_INT_IRQn = 141,
	DP_ABNORMAL_INT_IRQn = 142,
	edm_int_out_IRQn = 143,
	wdma1_vsync_out_IRQn = 144,
	wdma1_hsync_out_IRQn = 145,
	wdma2_vsync_out_IRQn = 146,
	wdma2_hsync_out_IRQn = 147,
	wdma3_vsync_out_IRQn = 148,
	wdma3_hsync_out_IRQn = 149,
	INP_RX_DE_HCNT_ERROR_INT_IRQn = 150,
	INP_RX2OUT_DE_HCNT_ERROR_INT_IRQn = 151,
	WDMA1_int_IRQn = 152,
	WDMA1_abnormal_int_IRQn = 153,
	WDMA2_int_IRQn = 154,
	WDMA2_abnormal_int_IRQn = 155,
	WDMA3_int_IRQn = 156,
	WDMA3_abnormal_int_IRQn = 157,
	RDMA_int_IRQn = 158,
	RDMA_abnormal_int_IRQn = 159,
	I3C_INT_IRQn = 160,
	CC312_cc_host_int_req_IRQn = 161,
	QSPI_spi_w_intp_IRQn = 162,
	OSPI_MPC_IRQn = 163,
	QSPI_ssi_intp_IRQn = 164,
	OSPI_IRQn = 165,
	SDIO_IRQn = 166,
	SDIO_WAKEUP_IRQn = 167,
	AXI_ROM_AXI_PPC_IRQn = 168,
	AXI_SRAM0_AXI_MPC_IRQn = 169,
	AXI_SRAM1_AXI_MPC_IRQn = 170,
	DMA_1_AHB_MSC_IRQn = 171,
	APB_2_APB_PPC_IRQn = 172,
	qspi_cache_Cache_MPC_IRQn = 173,
	qspi_cache_DWC_PPC_IRQn = 174,
	qspi_cache_ISP_PPC_IRQn = 175,
	AHB1M4_PPC_IRQn = 176,
	DPDMA_MSC_IRQn = 177,
	DMA0_MSC_IRQn = 178,
	WDMA3_MSC_IRQn = 179,
	MIPI_RX_Interrupt0_IRQn = 180,
	MIPI_RX_Interrupt1_IRQn = 181,
	MIPI_RX_Interrupt2_IRQn = 182,
	MIPI_RX_Interrupt3_IRQn = 183,
	MIPI_TX_IRQn = 184,
	I3C_HC_MSC_IRQ_IRQn = 185,
	SDIO_MSC_IRQ_IRQn = 186,
	pufrt_top_IRQn = 187,
	Interrupt188_IRQn = 188,
	Interrupt189_IRQn = 189,
	Interrupt190_IRQn = 190,
	Interrupt191_IRQn = 191,
	U55_IRQn = 192,
	U55M0_MSC_IRQn = 193,
	U55M1_MSC_IRQn = 194,
	Interrupt195_IRQn = 195,
	Interrupt196_IRQn = 196,
	Interrupt197_IRQn = 197,
	Interrupt198_IRQn = 198,
	Interrupt199_IRQn = 199,
	Interrupt200_IRQn = 200,
	Interrupt201_IRQn = 201,
	Interrupt202_IRQn = 202,
	Interrupt203_IRQn = 203,
	Interrupt204_IRQn = 204,
	Interrupt205_IRQn = 205,
	Interrupt206_IRQn = 206,
	Interrupt207_IRQn = 207,
	Interrupt208_IRQn = 208,
	Interrupt209_IRQn = 209,
	Interrupt210_IRQn = 210,
	Interrupt211_IRQn = 211,
	Interrupt212_IRQn = 212,
	Interrupt213_IRQn = 213,
	Interrupt214_IRQn = 214,
	Interrupt215_IRQn = 215,
	Interrupt216_IRQn = 216,
	Interrupt217_IRQn = 217,
	Interrupt218_IRQn = 218,
	Interrupt219_IRQn = 219,
	Interrupt220_IRQn = 220,
	Interrupt221_IRQn = 221,
	Interrupt222_IRQn = 222,
	Interrupt223_IRQn = 223,
	Interrupt224_IRQn = 224,
	Interrupt225_IRQn = 225,
	Interrupt226_IRQn = 226,
	Interrupt227_IRQn = 227,
	Interrupt228_IRQn = 228,
	Interrupt229_IRQn = 229,
	Interrupt230_IRQn = 230,
	Interrupt231_IRQn = 231,
	Interrupt232_IRQn = 232,
	Interrupt233_IRQn = 233,
	Interrupt234_IRQn = 234,
	Interrupt235_IRQn = 235,
	Interrupt236_IRQn = 236,
	Interrupt237_IRQn = 237,
	Interrupt238_IRQn = 238,
	Interrupt239_IRQn = 239,
	Interrupt240_IRQn = 240,
	Interrupt241_IRQn = 241,
	Interrupt242_IRQn = 242,
	Interrupt243_IRQn = 243,
	Interrupt244_IRQn = 244,
	Interrupt245_IRQn = 245,
	Interrupt246_IRQn = 246,
	Interrupt247_IRQn = 247,
	Interrupt248_IRQn = 248,
	Interrupt249_IRQn = 249,
	Interrupt250_IRQn = 250,
	Interrupt251_IRQn = 251,
	Interrupt252_IRQn = 252,
	Interrupt253_IRQn = 253,
	Interrupt254_IRQn = 254,
	Interrupt255_IRQn = 255,
	Interrupt256_IRQn = 256,
	Interrupt257_IRQn = 257,
	Interrupt258_IRQn = 258,
	Interrupt259_IRQn = 259,
	Interrupt260_IRQn = 260,
	Interrupt261_IRQn = 261,
	Interrupt262_IRQn = 262,
	Interrupt263_IRQn = 263,
	Interrupt264_IRQn = 264,
	Interrupt265_IRQn = 265,
	Interrupt266_IRQn = 266,
	Interrupt267_IRQn = 267,
	Interrupt268_IRQn = 268,
	Interrupt269_IRQn = 269,
	Interrupt270_IRQn = 270,
	Interrupt271_IRQn = 271,
	Interrupt272_IRQn = 272,
	Interrupt273_IRQn = 273,
	Interrupt274_IRQn = 274,
	Interrupt275_IRQn = 275,
	Interrupt276_IRQn = 276,
	Interrupt277_IRQn = 277,
	Interrupt278_IRQn = 278,
	Interrupt279_IRQn = 279,
	Interrupt280_IRQn = 280,
	Interrupt281_IRQn = 281,
	Interrupt282_IRQn = 282,
	Interrupt283_IRQn = 283,
	Interrupt284_IRQn = 284,
	Interrupt285_IRQn = 285,
	Interrupt286_IRQn = 286,
	Interrupt287_IRQn = 287,
	Interrupt288_IRQn = 288,
	Interrupt289_IRQn = 289,
	Interrupt290_IRQn = 290,
	Interrupt291_IRQn = 291,
	Interrupt292_IRQn = 292,
	Interrupt293_IRQn = 293,
	Interrupt294_IRQn = 294,
	Interrupt295_IRQn = 295,
	Interrupt296_IRQn = 296,
	Interrupt297_IRQn = 297,
	Interrupt298_IRQn = 298,
	Interrupt299_IRQn = 299,
	Interrupt300_IRQn = 300,
	Interrupt301_IRQn = 301,
	Interrupt302_IRQn = 302,
	Interrupt303_IRQn = 303,
	Interrupt304_IRQn = 304,
	Interrupt305_IRQn = 305,
	Interrupt306_IRQn = 306,
	Interrupt307_IRQn = 307,
	Interrupt308_IRQn = 308,
	Interrupt309_IRQn = 309,
	Interrupt310_IRQn = 310,
	Interrupt311_IRQn = 311,
	Interrupt312_IRQn = 312,
	Interrupt313_IRQn = 313,
	Interrupt314_IRQn = 314,
	Interrupt315_IRQn = 315,
	Interrupt316_IRQn = 316,
	Interrupt317_IRQn = 317,
	Interrupt318_IRQn = 318,
	Interrupt319_IRQn = 319,
	Interrupt320_IRQn = 320,
	Interrupt321_IRQn = 321,
	Interrupt322_IRQn = 322,
	Interrupt323_IRQn = 323,
	Interrupt324_IRQn = 324,
	Interrupt325_IRQn = 325,
	Interrupt326_IRQn = 326,
	Interrupt327_IRQn = 327,
	Interrupt328_IRQn = 328,
	Interrupt329_IRQn = 329,
	Interrupt330_IRQn = 330,
	Interrupt331_IRQn = 331,
	Interrupt332_IRQn = 332,
	Interrupt333_IRQn = 333,
	Interrupt334_IRQn = 334,
	Interrupt335_IRQn = 335,
	Interrupt336_IRQn = 336,
	Interrupt337_IRQn = 337,
	Interrupt338_IRQn = 338,
	Interrupt339_IRQn = 339,
	Interrupt340_IRQn = 340,
	Interrupt341_IRQn = 341,
	Interrupt342_IRQn = 342,
	Interrupt343_IRQn = 343,
	Interrupt344_IRQn = 344,
	Interrupt345_IRQn = 345,
	Interrupt346_IRQn = 346,
	Interrupt347_IRQn = 347,
	Interrupt348_IRQn = 348,
	Interrupt349_IRQn = 349,
	Interrupt350_IRQn = 350,
	Interrupt351_IRQn = 351,
	Interrupt352_IRQn = 352,
	Interrupt353_IRQn = 353,
	Interrupt354_IRQn = 354,
	Interrupt355_IRQn = 355,
	Interrupt356_IRQn = 356,
	Interrupt357_IRQn = 357,
	Interrupt358_IRQn = 358,
	Interrupt359_IRQn = 359,
	Interrupt360_IRQn = 360,
	Interrupt361_IRQn = 361,
	Interrupt362_IRQn = 362,
	Interrupt363_IRQn = 363,
	Interrupt364_IRQn = 364,
	Interrupt365_IRQn = 365,
	Interrupt366_IRQn = 366,
	Interrupt367_IRQn = 367,
	Interrupt368_IRQn = 368,
	Interrupt369_IRQn = 369,
	Interrupt370_IRQn = 370,
	Interrupt371_IRQn = 371,
	Interrupt372_IRQn = 372,
	Interrupt373_IRQn = 373,
	Interrupt374_IRQn = 374,
	Interrupt375_IRQn = 375,
	Interrupt376_IRQn = 376,
	Interrupt377_IRQn = 377,
	Interrupt378_IRQn = 378,
	Interrupt379_IRQn = 379,
	Interrupt380_IRQn = 380,
	Interrupt381_IRQn = 381,
	Interrupt382_IRQn = 382,
	Interrupt383_IRQn = 383,
	Interrupt384_IRQn = 384,
	Interrupt385_IRQn = 385,
	Interrupt386_IRQn = 386,
	Interrupt387_IRQn = 387,
	Interrupt388_IRQn = 388,
	Interrupt389_IRQn = 389,
	Interrupt390_IRQn = 390,
	Interrupt391_IRQn = 391,
	Interrupt392_IRQn = 392,
	Interrupt393_IRQn = 393,
	Interrupt394_IRQn = 394,
	Interrupt395_IRQn = 395,
	Interrupt396_IRQn = 396,
	Interrupt397_IRQn = 397,
	Interrupt398_IRQn = 398,
	Interrupt399_IRQn = 399,
	Interrupt400_IRQn = 400,
	Interrupt401_IRQn = 401,
	Interrupt402_IRQn = 402,
	Interrupt403_IRQn = 403,
	Interrupt404_IRQn = 404,
	Interrupt405_IRQn = 405,
	Interrupt406_IRQn = 406,
	Interrupt407_IRQn = 407,
	Interrupt408_IRQn = 408,
	Interrupt409_IRQn = 409,
	Interrupt410_IRQn = 410,
	Interrupt411_IRQn = 411,
	Interrupt412_IRQn = 412,
	Interrupt413_IRQn = 413,
	Interrupt414_IRQn = 414,
	Interrupt415_IRQn = 415,
	Interrupt416_IRQn = 416,
	Interrupt417_IRQn = 417,
	Interrupt418_IRQn = 418,
	Interrupt419_IRQn = 419,
	Interrupt420_IRQn = 420,
	Interrupt421_IRQn = 421,
	Interrupt422_IRQn = 422,
	Interrupt423_IRQn = 423,
	Interrupt424_IRQn = 424,
	Interrupt425_IRQn = 425,
	Interrupt426_IRQn = 426,
	Interrupt427_IRQn = 427,
	Interrupt428_IRQn = 428,
	Interrupt429_IRQn = 429,
	Interrupt430_IRQn = 430,
	Interrupt431_IRQn = 431,
	Interrupt432_IRQn = 432,
	Interrupt433_IRQn = 433,
	Interrupt434_IRQn = 434,
	Interrupt435_IRQn = 435,
	Interrupt436_IRQn = 436,
	Interrupt437_IRQn = 437,
	Interrupt438_IRQn = 438,
	Interrupt439_IRQn = 439,
	Interrupt440_IRQn = 440,
	Interrupt441_IRQn = 441,
	Interrupt442_IRQn = 442,
	Interrupt443_IRQn = 443,
	Interrupt444_IRQn = 444,
	Interrupt445_IRQn = 445,
	Interrupt446_IRQn = 446,
	Interrupt447_IRQn = 447,
	Interrupt448_IRQn = 448,
	Interrupt449_IRQn = 449,
	Interrupt450_IRQn = 450,
	Interrupt451_IRQn = 451,
	Interrupt452_IRQn = 452,
	Interrupt453_IRQn = 453,
	Interrupt454_IRQn = 454,
	Interrupt455_IRQn = 455,
	Interrupt456_IRQn = 456,
	Interrupt457_IRQn = 457,
	Interrupt458_IRQn = 458,
	Interrupt459_IRQn = 459,
	Interrupt460_IRQn = 460,
	Interrupt461_IRQn = 461,
	Interrupt462_IRQn = 462,
	Interrupt463_IRQn = 463,
	Interrupt464_IRQn = 464,
	Interrupt465_IRQn = 465,
	Interrupt466_IRQn = 466,
	Interrupt467_IRQn = 467,
	Interrupt468_IRQn = 468,
	Interrupt469_IRQn = 469,
	Interrupt470_IRQn = 470,
	Interrupt471_IRQn = 471,
	Interrupt472_IRQn = 472,
	Interrupt473_IRQn = 473,
	Interrupt474_IRQn = 474,
	Interrupt475_IRQn = 475,
	Interrupt476_IRQn = 476,
	Interrupt477_IRQn = 477,
	Interrupt478_IRQn = 478,
	Interrupt479_IRQn = 479,
	Interrupt480_IRQn = 480,
/* Interrupts 10 .. 480 are left out */
} IRQn_Type;

/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
#warning Not supported compiler type
#endif

/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM55_REV                0x0001U   /* Core revision r0p1 */
#define __NVIC_PRIO_BITS          3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */
#define __VTOR_PRESENT            1U        /* VTOR present */
#define __MPU_PRESENT             1U        /* MPU present */
#define __FPU_PRESENT             1U        /* FPU present */
#define __FPU_DP                  1U        /* double precision FPU */
#define __DSP_PRESENT             1U        /* DSP extension present */
#define __SAUREGION_PRESENT       1U        /* SAU regions present */
#define __PMU_PRESENT             1U        /* PMU present */
#define __PMU_NUM_EVENTCNT        8U        /* PMU Event Counters */
#define __ICACHE_PRESENT          1U        /* Instruction Cache present */
#define __DCACHE_PRESENT          1U        /* Data Cache present */

#include "core_cm55.h"                      /* Processor and core peripherals */
#include "system_WE2_ARMCM55.h"                 /* System Header */

/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
#warning Not supported compiler type
#endif

#ifdef __cplusplus
}
#endif

#endif  /* WE2_ARMCM55_H */
