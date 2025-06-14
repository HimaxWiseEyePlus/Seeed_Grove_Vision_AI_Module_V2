/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2013-2024 OpenMV, LLC.
 *
 * CGP downloaded from https://github.com/openmv/openmv/blob/c8a7f3fc5f9590d948b25f02163875ad930085c7/src/omv/sensors/hm0360_regs.h#L4
 * on 16/3/25
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * HM0360 register definitions.
 */
#ifndef __REG_REGS_H__
#define __REG_REGS_H__

// Select streaming mode by writing to 0x0100.
// See data sheet 6.1 & 10.2
typedef enum {
    MODE_SLEEP,				// 0 Hardware sleep
	MODE_SW_CONTINUOUS,		// 1 SW triggers continuous streaming
	MODE_SW_NFRAMES_SLEEP,	// 2 SW trigger, output N frames then sleep for a set interval
	MODE_SW_NFRAMES_STANDBY,// 3 SW trigger, output N frames then s/w standby (needs a reset to restart???)
	MODE_HW_TRIGGER,		// 4 TRIGGER pin starts streaming
	MODE_RFU,				// 5 Not defined
	MODE_HW_NFRAMES_STANDBY,// 6 HW trigger, output N frames then s/w standby (needs a reset to restart???)
	MODE_HW_NFRAMES_SLEEP,	// 7 HW trigger, output N frames then sleep
} mode_select_t;

// Context select bits for 0x3034
#define CONTEXT_A	0
#define CONTEXT_B	1

// Bits in the INT_INDIC register
#define 		ALC_INT							0x01
#define 		STAT_INT						0x02
#define 		AENC_INT						0x04
#define 		MD_INT							0x08
#define 		MD_FLICKER_INT					0x10
#define 		EARLY_VSYNC_INT					0x20
#define 		AE_CONVERGE_INT					0x40

// Read only registers
#define         MODEL_ID_H                      0x0000
#define         MODEL_ID_L                      0x0001
#define         SILICON_REV                     0x0002
#define         FRAME_COUNT_H                   0x0005
#define         FRAME_COUNT_L                   0x0006
#define         PIXEL_ORDER                     0x0007
// Sensor mode control
#define         MODE_SELECT                     0x0100
#define         IMG_ORIENTATION                 0x0101
#define         EMBEDDED_LINE_EN                0x0102
#define         SW_RESET                        0x0103
#define         COMMAND_UPDATE                  0x0104
// Sensor exposure gain control
#define         INTEGRATION_H                   0x0202
#define         INTEGRATION_L                   0x0203
#define         ANALOG_GAIN                     0x0205
#define         DIGITAL_GAIN_H                  0x020E
#define         DIGITAL_GAIN_L                  0x020F
// Clock control
#define         PLL1_CONFIG                     0x0300
#define         PLL2_CONFIG                     0x0301
#define         PLL3_CONFIG                     0x0302
// Frame timing control
#define         FRAME_LEN_LINES_H               0x0340
#define         FRAME_LEN_LINES_L               0x0341
#define         LINE_LEN_PCK_H                  0x0342
#define         LINE_LEN_PCK_L                  0x0343
// Monochrome programming
#define         MONO_MODE                       0x0370
#define         MONO_MODE_ISP                   0x0371
#define         MONO_MODE_SEL                   0x0372
// Binning mode control
#define         H_SUBSAMPLE                     0x0380
#define         V_SUBSAMPLE                     0x0381
#define         BINNING_MODE                    0x0382
// Test pattern control
#define         TEST_PATTERN_MODE               0x0601
// Black level control
#define         BLC_TGT                         0x1004
#define         BLC2_TGT                        0x1009
#define         MONO_CTRL                       0x100A
// VSYNC / HSYNC / pixel shift registers
#define         OPFM_CTRL                       0x1014
// Tone mapping registers
#define         CMPRS_CTRL                      0x102F
#define         CMPRS_01                        0x1030
#define         CMPRS_02                        0x1031
#define         CMPRS_03                        0x1032
#define         CMPRS_04                        0x1033
#define         CMPRS_05                        0x1034
#define         CMPRS_06                        0x1035
#define         CMPRS_07                        0x1036
#define         CMPRS_08                        0x1037
#define         CMPRS_09                        0x1038
#define         CMPRS_10                        0x1039
#define         CMPRS_11                        0x103A
#define         CMPRS_12                        0x103B
#define         CMPRS_13                        0x103C
#define         CMPRS_14                        0x103D
#define         CMPRS_15                        0x103E
#define         CMPRS_16                        0x103F
// Automatic exposure control
#define         AE_CTRL                         0x2000
#define         AE_CTRL1                        0x2001
#define         CNT_ORGH_H                      0x2002
#define         CNT_ORGH_L                      0x2003
#define         CNT_ORGV_H                      0x2004
#define         CNT_ORGV_L                      0x2005
#define         CNT_STH_H                       0x2006
#define         CNT_STH_L                       0x2007
#define         CNT_STV_H                       0x2008
#define         CNT_STV_L                       0x2009
#define         CTRL_PG_SKIPCNT                 0x200A
#define         BV_WIN_WEIGHT_EN                0x200D
#define         MAX_INTG_H                      0x2029
#define         MAX_INTG_L                      0x202A
#define         MAX_AGAIN                       0x202B
#define         MAX_DGAIN_H                     0x202C
#define         MAX_DGAIN_L                     0x202D
#define         MIN_INTG                        0x202E
#define         MIN_AGAIN                       0x202F
#define         MIN_DGAIN                       0x2030
#define         T_DAMPING                       0x2031
#define         N_DAMPING                       0x2032
#define         ALC_TH                          0x2033
#define         AE_TARGET_MEAN                  0x2034
#define         AE_MIN_MEAN                     0x2035
#define         AE_TARGET_ZONE                  0x2036
#define         CONVERGE_IN_TH                  0x2037
#define         CONVERGE_OUT_TH                 0x2038
#define         FS_CTRL                         0x203B
#define         FS_60HZ_H                       0x203C
#define         FS_60HZ_L                       0x203D
#define         FS_50HZ_H                       0x203E
#define         FS_50HZ_L                       0x203F
#define         FRAME_CNT_TH                    0x205B
#define         AE_MEAN                         0x205D
#define         AE_CONVERGE                     0x2060
#define         AE_BLI_TGT                      0x2070
// Interrupt control
#define         PULSE_MODE                      0x2061
#define         PULSE_TH_H                      0x2062
#define         PULSE_TH_L                      0x2063
#define         INT_INDIC                       0x2064
#define         INT_CLEAR                       0x2065
//#define			MD_INT_BIT						0x08		// MD interrupt bit

// Motion detection control
#define         MD_CTRL                         0x2080		// Main MD control
#define         MD_CTRL_A                       0x354B		// Context A MD control
#define         MD_CTRL_B                       0x35A5		// Context B MD control
#define         ROI_START_END_V                 0x2081		// Main ROI_V
#define         ROI_START_END_H                 0x2082		// Main ROI H
#define         ROI_START_END_H                 0x2082		// Main ROI H
#define         FRAME_OUTPUT_EN_A               0x3510		// Context A Frame output enable
#define         FRAME_OUTPUT_EN_B               0x356A		// Context B Frame output enable

#define         ROI_START_END_V_A               0x354D		// Context A ROI_V
#define         ROI_START_END_H_A               0x354E		// Context A ROI_H
#define         ROI_START_END_V_B               0x35A7		// Context B ROI_V
#define         ROI_START_END_H_B               0x35A8		// Context B ROI_H
#define         MD_TH_MIN                       0x2083		// Threshold minimum
#define         MD_TH_STR_L                     0x2084		// Main threshold strength LSB
#define         MD_TH_STR_H                     0x2085		// Main threshold strength MSB
#define         MD_TH_STR_L_A                   0x3550		// Context A threshold strength LSB
#define         MD_TH_STR_H_A                   0x354F		// Context A threshold strength MSB
#define         MD_TH_STR_L_B                   0x35AA		// Context B threshold strength LSB
#define         MD_TH_STR_H_B                   0x35A9		// Context B threshold strength MSB
#define         MD_LIGHT_COEF                   0x2099		// MD Light coefficient
#define         MD_IIR_PARAM                    0x209A		// MD IIR parameter
#define         MD_BLOCK_NUM_TH                 0x209B		// Main MD Block Num Threshold
#define         MD_BLOCK_NUM_TH_A               0x354C		// Context A MD Block Num Threshold
#define         MD_BLOCK_NUM_TH_B               0x35A6		// Context B MD Block Num Threshold
#define         MD_LATENCY                      0x209C		// MD latency frame
#define         MD_LATENCY_TH                   0x209D		// MD latency threshold
#define         MD_CTRL1                        0x209E		// MD interrupt control

// (there are 32 ROI_OUT registers 20A1-20C0)
#define 		ROIOUTENTRIES 					32
#define         MD_ROI_OUT_0                    0x20A1		// MD ROI out (there are 32 registers 20A1-20C0)

// Context switch control registers
#define         PMU_CFG_3                       0x3024
#define         PMU_CFG_4                       0x3025
#define         PMU_CFG_5                       0x3026
#define         PMU_CFG_6                       0x3027
#define         PMU_CFG_7                       0x3028		// Output frame count
#define         PMU_CFG_8                       0x3029		// Sleep Count High
#define         PMU_CFG_9                       0x302a		// Sleep Count Low

// Operation mode control
#define         WIN_MODE                        0x3030
// IO and clock control
#define         PAD_REGISTER_07                 0x3112

// Register bits/values
#define         HIMAX_RESET                     0x01
#define         HIMAX_MODE_STANDBY              0x00
#define         HIMAX_MODE_STREAMING            0x01     // I2C triggered streaming enable
#define         HIMAX_MODE_STREAMING_NFRAMES    0x03     // Output N frames
#define         HIMAX_MODE_STREAMING_TRIG       0x05     // Hardware Trigger
#define         HIMAX_SET_HMIRROR(r, x)         ((r & 0xFE) | ((x & 1) << 0))
#define         HIMAX_SET_VMIRROR(r, x)         ((r & 0xFD) | ((x & 1) << 1))

#define         PCLK_RISING_EDGE                0x00
#define         PCLK_FALLING_EDGE               0x01
#define         AE_CTRL_ENABLE                  0x00
#define         AE_CTRL_DISABLE                 0x01

#endif //__REG_REGS_H__
