/**
 * @file hx_drv_adcc.h
 * @author himax/902453
 * @brief 
 * @version V1.0.0
 * @date 2018-11-28
 * 
 * @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 * 
 */

#ifndef INC_HX_DRV_ADCC_COMMON_H_
#define INC_HX_DRV_ADCC_COMMON_H_

#include "dev_common.h"

/**
 * \enum    ADCC_ERROR_E
 * \brief   ADCC Errors Type 
 * 
 */
typedef enum ADCC_ERROR_S
{
    ADCC_NO_ERROR                           = 0,    /**< No ERROR */
    ADCC_ERROR_NOT_INIT                     = 1,    /**< ERROR MSG: not init */
    ADCC_ERROR_ALREADY_INIT                 = 2,    /**< ERROR MSG: already init */
    ADCC_ERROR_INVALID_PARAMETERS           = 3,    /**< ERROR MSG: Invalid parameters */
    ADCC_UNKNOWN_ERROR                      = 4,    /**< ERROR MSG: UNKNOWN ERROR*/
} ADCC_ERROR_E;


/**
 * \enum    ADCC_CHANNEL
 * \brief   ADCC Channel
 * 
 */
typedef enum adcc_channel {
    ADCC_CHANNEL0 = 0,      /*!< ADCC channel 0  */
    ADCC_CHANNEL1 = 1,      /*!< ADCC channel 1 */
    ADCC_CHANNEL2 = 2,      /*!< ADCC channel 2 */
    ADCC_CHANNEL3 = 3       /*!< ADCC channel 3 */
} ADCC_CHANNEL;


/**
 * \enum    ADCC_CALLBACK_E
 * \brief   ADCC Interrupt Callback
 * 
 */
typedef enum adcc_callback {
    ADCC_CH0_CB = 0,        /*!< ADCC channel 0 callback */
    ADCC_CH1_CB = 1,        /*!< ADCC channel 1 callback */
    ADCC_CH2_CB = 2,        /*!< ADCC channel 2 callback */
    ADCC_CH3_CB = 3,        /*!< ADCC channel 3 callback */
    ADCC_CONSTRAIN_CB = 4,  /*!< ADCC constrain callback */
    ADCC_SG_CB = 5          /*!< ADCC single shot callback */
} ADCC_CALLBACK_E;


/**
 * \enum    ADCC_DATA_TYPE
 * \brief   ADCC data type
 * 
 */
typedef enum adcc_data_type {
    ADCC_DATA_BYPASS = 0,       /*!< ADCC data type bypass  */
    ADCC_DATA_AVERAGE = 1,      /*!< ADCC data type average */
} ADCC_DATA_TYPE;


/**
 * \enum    ADCC_DATA_SEL
 * \brief   ADCC data select
 * 
 */
typedef enum adcc_data_select {
    ADCC_DATA_MUX = 0,          /*!< ADCC data select Mux  */
    ADCC_DATA_OFFSET = 1,       /*!< ADCC data select Offset */
} ADCC_DATA_SEL;

/**
 * \enum    ADCC_VOL_SEL
 * \brief   ADCC data select
 * 
 */
typedef enum adcc_vol_select {
    ADCC_VOL_1P8V = 0,          /*!< ADCC input voltage full scale is 1.8v */
    ADCC_VOL_3P3V = 1,          /*!< ADCC input voltage full scale is 3.3v */
} ADCC_VOL_SEL;

/**
 * \struct  ADCC_CFG_T
 * \brief   ADCC configuration
 */
typedef struct ADCC_CFG_S
{
    ADCC_VOL_SEL analog_vol;
    uint16_t enable;
    uint16_t single_shot_enable;
    uint16_t channel_mask;
    uint16_t sample_rate[4];
    ADCC_DATA_TYPE data_type[4];
    uint16_t offset[4];
    uint16_t high_threshold[4];
    uint16_t low_threshold[4];
    uint16_t int_delay;
    uint32_t int_mask;
    ADCC_DATA_SEL data_select[4];
} ADCC_CFG_T;


/**
 * \brief   ADCC Interrupt callback function
 *
 * \return  void
 */
typedef void (*ADCC_ISR_CB) (uint32_t int_status);


/**
 * \brief   Contains definitions of ADDC MACRO.
 * 
 */
#define ADCC_CH0                    (1)
#define ADCC_CH1                    (2)
#define ADCC_CH2                    (4)
#define ADCC_CH3                    (8)
#define ADCC_DATA_BYPASS            (0)
#define ADCC_DATA_AVERAGE           (1)
#define ADCC_DATA_SEL_MUX           (0)
#define ADCC_DATA_SEL_OFFSET        (1)

#define ADCC_CH0_SR_MASK            (0x0000001F)    /*!< [4:0]   = channel 0 sample rate */
#define ADCC_CH1_SR_MASK            (0x00001F00)    /*!< [12:8]  = channel 1 sample rate */
#define ADCC_CH2_SR_MASK            (0x001F0000)    /*!< [20:16] = channel 2 sample rate */
#define ADCC_CH3_SR_MASK            (0x1F000000)    /*!< [28:24] = channel 3 sample rate */

#define ADCC_CH0_INT_MASK           (0x0000003F)    /*!< [5:0]   = channel 0 interrupt status: ADCC_CH0_LOW_INT_BIT | ADCC_CH0_HIGH_INT_BIT | ADCC_CH0_LOW_IN_INT_BIT | ADCC_CH0_LOW_OUT_INT_BIT | ADCC_CH0_HIGH_IN_INT_BIT | #define ADCC_CH0_HIGH_OUT_INT_BIT */
#define ADCC_CH1_INT_MASK           (0x00000FC0)    /*!< [11:6]  = channel 1 interrupt status: ADCC_CH1_LOW_INT_BIT | ADCC_CH1_HIGH_INT_BIT | ADCC_CH1_LOW_IN_INT_BIT | ADCC_CH1_LOW_OUT_INT_BIT | ADCC_CH1_HIGH_IN_INT_BIT | #define ADCC_CH1_HIGH_OUT_INT_BIT */
#define ADCC_CH2_INT_MASK           (0x0003F000)    /*!< [17:12] = channel 2 interrupt status: ADCC_CH2_LOW_INT_BIT | ADCC_CH2_HIGH_INT_BIT | ADCC_CH2_LOW_IN_INT_BIT | ADCC_CH2_LOW_OUT_INT_BIT | ADCC_CH2_HIGH_IN_INT_BIT | #define ADCC_CH2_HIGH_OUT_INT_BIT */
#define ADCC_CH3_INT_MASK           (0x00FC0000)    /*!< [23:18] = channel 3 interrupt status: ADCC_CH3_LOW_INT_BIT | ADCC_CH3_HIGH_INT_BIT | ADCC_CH3_LOW_IN_INT_BIT | ADCC_CH3_LOW_OUT_INT_BIT | ADCC_CH3_HIGH_IN_INT_BIT | #define ADCC_CH3_HIGH_OUT_INT_BIT */
#define ADCC_CONSTRAIN_INT_MASK     (0x01000000)    /*!< [24]    = constrain interrupt status */
#define ADCC_SG_INT_MASK            (0x02000000)    /*!< [25]    = Single Shot interrupt status */
#define ADCC_ALL_INT_MASK           (0x03FFFFFF)

#define ADCC_CH0_SR_BIT_SFT         (0)             /*!< [4:0]   = channel 0 sample rate */
#define ADCC_CH1_SR_BIT_SFT         (8)             /*!< [12:8]  = channel 1 sample rate */
#define ADCC_CH2_SR_BIT_SFT         (16)            /*!< [20:16] = channel 2 sample rate */
#define ADCC_CH3_SR_BIT_SFT         (24)            /*!< [28:24] = channel 3 sample rate */

#define ADCC_CH0_DATA_TYPE_BIT_SFT  (0)             /*!< [0] = channel 0 data type */
#define ADCC_CH1_DATA_TYPE_BIT_SFT  (1)             /*!< [1] = channel 1 data type */
#define ADCC_CH2_DATA_TYPE_BIT_SFT  (2)             /*!< [2] = channel 2 data type */
#define ADCC_CH3_DATA_TYPE_BIT_SFT  (3)             /*!< [3] = channel 3 data type */

#define ADCC_CH0_DATA_SEL_BIT_SFT   (0)             /*!< [0] = channel 0 data select */
#define ADCC_CH1_DATA_SEL_BIT_SFT   (1)             /*!< [1] = channel 1 data select */
#define ADCC_CH2_DATA_SEL_BIT_SFT   (2)             /*!< [2] = channel 2 data select */
#define ADCC_CH3_DATA_SEL_BIT_SFT   (3)             /*!< [3] = channel 3 data select */

#define ADCC_CH0_HIGH_OUT_INT_BIT   (1<<0)
#define ADCC_CH0_HIGH_IN_INT_BIT    (1<<1)
#define ADCC_CH0_LOW_OUT_INT_BIT    (1<<2)
#define ADCC_CH0_LOW_IN_INT_BIT     (1<<3)
#define ADCC_CH0_HIGH_INT_BIT       (1<<4)
#define ADCC_CH0_LOW_INT_BIT        (1<<5)
#define ADCC_CH1_HIGH_OUT_INT_BIT   (1<<6)
#define ADCC_CH1_HIGH_IN_INT_BIT    (1<<7)
#define ADCC_CH1_LOW_OUT_INT_BIT    (1<<8)
#define ADCC_CH1_LOW_IN_INT_BIT     (1<<9)
#define ADCC_CH1_HIGH_INT_BIT       (1<<10)
#define ADCC_CH1_LOW_INT_BIT        (1<<11)
#define ADCC_CH2_HIGH_OUT_INT_BIT   (1<<12)
#define ADCC_CH2_HIGH_IN_INT_BIT    (1<<13)
#define ADCC_CH2_LOW_OUT_INT_BIT    (1<<14)
#define ADCC_CH2_LOW_IN_INT_BIT     (1<<15)
#define ADCC_CH2_HIGH_INT_BIT       (1<<16)
#define ADCC_CH2_LOW_INT_BIT        (1<<17)
#define ADCC_CH3_HIGH_OUT_INT_BIT   (1<<18)
#define ADCC_CH3_HIGH_IN_INT_BIT    (1<<19)
#define ADCC_CH3_LOW_OUT_INT_BIT    (1<<20)
#define ADCC_CH3_LOW_IN_INT_BIT     (1<<21)
#define ADCC_CH3_HIGH_INT_BIT       (1<<22)
#define ADCC_CH3_LOW_INT_BIT        (1<<23)
#define ADCC_CONSTRAIN_INT_BIT      (1<<24)
#define ADCC_SG_INT_BIT             (1<<25)


/**
 * \brief  Register default value
 * 
 */
#define DEFAULT_ADCC_LV_CLK         (6000000)           /*!< ADC_CKIN = 6MHz = 24MHz / 4,   */ 
#define DEFAULT_ADCC_HV_CLK           (32000)           /*!< ADC_LP_CK_IN_HV = SB_32K_CLK = 32KHz (RX32K1K or XTAL_32K)*/ 
#define DEFAULT_ADCC_SR             3                   /*!< Sample rate = ADC_CLK / (2^div) */
#define DEFAULT_ADCC_VOL            ADCC_VOL_1P8V       /*!< ADCC_VOL_1P8V or ADCC_VOL_3P3V */ 
#define DEFAULT_ADCC_ENABLE         0
#define DEFAULT_ADCC_SINGLE_SHOT    0
#define DEFAULT_ADCC_CH_MASK        (ADCC_CH0 | ADCC_CH1 | ADCC_CH2 | ADCC_CH3)
#define DEFAULT_ADCC_DATA_TYPE      ADCC_DATA_AVERAGE   /*!< ADCC_DATA_BYPASS or ADCC_DATA_AVERAGE */    
#define DEFAULT_ADCC_OFFSET         0                   /*!< 13bit */
#define DEFAULT_ADCC_HV_OFFSET      0                   /*!< 9bit */
#define DEFAULT_ADCC_HIGH_TH        (0x900)             /*!< 12bit */
#define DEFAULT_ADCC_LOW_TH         (0x300)             /*!< 12bit */
#define DEFAULT_ADCC_HV_HIGH_TH      (0xB0)             /*!< 8bit */
#define DEFAULT_ADCC_HV_LOW_TH       (0x20)             /*!< 8bit */
#define DEFAULT_ADCC_INT_DELAY      0                   /*!< 0:There will be no delay 1,2,1023 : 5ms, 10ms, 5.115s (max) */
#define DEFAULT_ADCC_INT_MASK       0                   /*!< (ADCC_CH0_INT_MASK | ADCC_CH1_INT_MASK | ADCC_CH2_INT_MASK | ADCC_CH3_INT_MASK | ADCC_CONSTRAIN_INT_MASK | ADCC_SG_INT_MASK) */
#define DEFAULT_ADCC_DATA_SELECT    ADCC_DATA_SEL_MUX   /*!< ADCC_DATA_SEL_MUX or ADCC_DATA_SEL_OFFSET */

#endif /* INC_HX_DRV_ADCC_COMMON_H_ */