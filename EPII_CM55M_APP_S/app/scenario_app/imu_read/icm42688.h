/*!
 * @file ICM42688.h
 * @brief This file contains API for 6DOF IMU 14 Click Driver.
 */

#ifndef ICM42688_REGISTERS_H_
#define ICM42688_REGISTERS_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "hx_drv_iic.h"

/*!
 * @addtogroup ICM42688 6DOF IMU 14 Click Driver
 * @brief API for configuring and manipulating 6DOF IMU 14 Click driver.
 * @{
 */

/**
 * @defgroup ICM42688_reg 6DOF IMU 14 Registers List
 * @brief List of registers of 6DOF IMU 14 Click driver.
 */

/**
 * @addtogroup ICM42688_reg
 * @{
 */

#define ICM42688_I2C_ADDR                     0x68

/**
 * @brief 6DOF IMU 14 Axis structure object.
 * @details Axis structure object definition of 6DOF IMU 14 Click driver.
 */
typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
} icm42688_axis_t;

/**
 * @brief 6DOF IMU 14 I2C writing function.
 * @details This function writes a desired number of data bytes starting from
 * the selected register by using I2C serial interface.
 * @param[in] reg : Start register address.
 * @param[in] data_in : Data to be written.
 * @param[in] len : Number of bytes to be written.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #IIC_ERR_CODE_E definition for detailed explanation.
 * @note None.
 */
IIC_ERR_CODE_E icm42688_i2c_write(uint8_t reg, uint8_t *data_in, uint8_t len);

/**
 * @brief 6DOF IMU 14 I2C reading function.
 * @details This function reads a desired number of data bytes starting from
 * the selected register by using I2C serial interface.
 * @param[in] reg : Start register address.
 * @param[out] data_out : Output read data.
 * @param[in] len : Number of bytes to be read.
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #IIC_ERR_CODE_E definition for detailed explanation.
 * @note None.
 */
IIC_ERR_CODE_E icm42688_i2c_read(uint8_t reg, uint8_t *data_out, uint8_t len);

/**
 * @brief 6DOF IMU 14 init function.
 * @details This function initializes icm42688
 * @return @li @c  0 - Success,
 *         @li @c -1 - Error.
 *
 * See #IIC_ERR_CODE_E definition for detailed explanation.
 * @note None.
 */
IIC_ERR_CODE_E icm42688_init();

IIC_ERR_CODE_E icm42688_get_accel_axis(icm42688_axis_t *axis);

IIC_ERR_CODE_E icm42688_get_gyro_axis(icm42688_axis_t *axis);

IIC_ERR_CODE_E icm42688_get_data(icm42688_axis_t *acc_axis, icm42688_axis_t *gyro_axis);

/**
 * @brief 6DOF IMU 14 USER BANK 0 register map summary.
 * @details The list of USER BANK 0 registers.
 */
#define ICM42688_REG0_DEVICE_CONFIG           0x11
#define ICM42688_REG0_DRIVE_CONFIG            0x13
#define ICM42688_REG0_INT_CONFIG              0x14
#define ICM42688_REG0_FIFO_CONFIG             0x16
#define ICM42688_REG0_TEMP_DATA_MSB           0x1D
#define ICM42688_REG0_TEMP_DATA_LSB           0x1E
#define ICM42688_REG0_ACCEL_X_MSB             0x1F
#define ICM42688_REG0_ACCEL_X_LSB             0x20
#define ICM42688_REG0_ACCEL_Y_MSB             0x21
#define ICM42688_REG0_ACCEL_Y_LSB             0x22
#define ICM42688_REG0_ACCEL_Z_MSB             0x23
#define ICM42688_REG0_ACCEL_Z_LSB             0x24
#define ICM42688_REG0_GYRO_X_MSB              0x25
#define ICM42688_REG0_GYRO_X_LSB              0x26
#define ICM42688_REG0_GYRO_Y_MSB              0x27
#define ICM42688_REG0_GYRO_Y_LSB              0x28
#define ICM42688_REG0_GYRO_Z_MSB              0x29
#define ICM42688_REG0_GYRO_Z_LSB              0x2A
#define ICM42688_REG0_TMST_FSYNC_MSB          0x2B
#define ICM42688_REG0_TMST_FSYNC_LSB          0x2C
#define ICM42688_REG0_INT_STATUS_1            0x2D
#define ICM42688_REG0_FIFO_COUNT_MSB          0x2E
#define ICM42688_REG0_FIFO_COUNT_LSB          0x2F
#define ICM42688_REG0_FIFO_DATA               0x30
#define ICM42688_REG0_APEX_DATA_LSB           0x31
#define ICM42688_REG0_APEX_DATA_MSB           0x32
#define ICM42688_REG0_APEX_STEP_CADENCE       0x33
#define ICM42688_REG0_APEX_DATA_3             0x34
#define ICM42688_REG0_APEX_DATA_4             0x35
#define ICM42688_REG0_APEX_DATA_5             0x36
#define ICM42688_REG0_INT_STATUS_2            0x37
#define ICM42688_REG0_INT_STATUS_3            0x38
#define ICM42688_REG0_SIGNAL_PATH_RESET       0x4B
#define ICM42688_REG0_INTF_CONFIG_0           0x4C
#define ICM42688_REG0_INTF_CONFIG_1           0x4D
#define ICM42688_REG0_PWR_MGMT_0              0x4E
#define ICM42688_REG0_GYRO_CONFIG_0           0x4F
#define ICM42688_REG0_ACCEL_CONFIG_0          0x50
#define ICM42688_REG0_GYRO_CONFIG_1           0x51
#define ICM42688_REG0_GYRO_ACCEL_CFG_0        0x52
#define ICM42688_REG0_ACCEL_CONFIG_1          0x53
#define ICM42688_REG0_TMST_CONFIG             0x54
#define ICM42688_REG0_APRX_CONFIG_0           0x56
#define ICM42688_REG0_SMD_CONFIG              0x57
#define ICM42688_REG0_FIFO_CONFIG_1           0x5F
#define ICM42688_REG0_FIFO_CONFIG_2           0x60
#define ICM42688_REG0_FIFO_CONFIG_3           0x61
#define ICM42688_REG0_FSYNC_CONFIG            0x62
#define ICM42688_REG0_INT_CONFIG_0            0x63
#define ICM42688_REG0_INT_CONFIG_1            0x64
#define ICM42688_REG0_INT_SOURCE_0            0x65
#define ICM42688_REG0_INT_SOURCE_1            0x66
#define ICM42688_REG0_INT_SOURCE_3            0x68
#define ICM42688_REG0_INT_SOURCE_4            0x69
#define ICM42688_REG0_FIFO_LOST_PKT_0         0x6C
#define ICM42688_REG0_FIFO_LOST_PKT_1         0x6D
#define ICM42688_REG0_SELF_TEST_CONFIG        0x70
#define ICM42688_REG0_WHO_AM_I                0x75
#define ICM42688_REG0_BANK_SEL                0x76

/**
 * @brief 6DOF IMU 14 USER BANK 1 register map summary.
 * @details The list of USER BANK 1 registers.
 */
#define ICM42688_REG1_SENSOR_CONFIG_0         0x03
#define ICM42688_REG1_GYRO_CONFIG_2           0x0B
#define ICM42688_REG1_GYRO_CONFIG_3           0x0C
#define ICM42688_REG1_GYRO_CONFIG_4           0x0D
#define ICM42688_REG1_GYRO_CONFIG_5           0x0E
#define ICM42688_REG1_GYRO_CONFIG_6           0x0F
#define ICM42688_REG1_GYRO_CONFIG_7           0x10
#define ICM42688_REG1_GYRO_CONFIG_8           0x11
#define ICM42688_REG1_GYRO_CONFIG_9           0x12
#define ICM42688_REG1_GYRO_CONFIG_10          0x13
#define ICM42688_REG1_XG_ST_DATA              0x5F
#define ICM42688_REG1_YG_ST_DATA              0x60
#define ICM42688_REG1_ZG_ST_DATA              0x61
#define ICM42688_REG1_TMST_VALUE_LSB          0x62
#define ICM42688_REG1_TMST_VALUE_MID          0x63
#define ICM42688_REG1_TMST_VALUE_MSB          0x64
#define ICM42688_REG1_INTF_CONFIG_4           0x7A
#define ICM42688_REG1_INTF_CONFIG_5           0x7B
#define ICM42688_REG1_INTF_CONFIG_6           0x7C

/**
 * @brief 6DOF IMU 14 USER BANK 2 register map summary.
 * @details The list of USER BANK 2 registers.
 */
#define ICM42688_REG2_ACCEL_CONFIG_2          0x03
#define ICM42688_REG2_ACCEL_CONFIG_3          0x04
#define ICM42688_REG2_ACCEL_CONFIG_4          0x05
#define ICM42688_REG2_XA_ST_DATA              0x3B
#define ICM42688_REG2_YA_ST_DATA              0x3C
#define ICM42688_REG2_ZA_ST_DATA              0x3D

/**
 * @brief 6DOF IMU 14 USER BANK 4 register map summary.
 * @details The list of USER BANK 4 registers.
 */
#define ICM42688_REG4_APEX_CONFIG_1           0x40
#define ICM42688_REG4_APEX_CONFIG_2           0x41
#define ICM42688_REG4_APEX_CONFIG_3           0x42
#define ICM42688_REG4_APEX_CONFIG_4           0x43
#define ICM42688_REG4_APEX_CONFIG_5           0x44
#define ICM42688_REG4_APEX_CONFIG_6           0x45
#define ICM42688_REG4_APEX_CONFIG_7           0x46
#define ICM42688_REG4_APEX_CONFIG_8           0x47
#define ICM42688_REG4_APEX_CONFIG_9           0x48
#define ICM42688_REG4_ACCEL_WOM_X_THR         0x4A
#define ICM42688_REG4_ACCEL_WOM_Y_THR         0x4B
#define ICM42688_REG4_ACCEL_WOM_Z_THR         0x4C
#define ICM42688_REG4_INT_SOURCE_6            0x4D
#define ICM42688_REG4_INT_SOURCE_7            0x4E
#define ICM42688_REG4_INT_SOURCE_8            0x4F
#define ICM42688_REG4_INT_SOURCE_9            0x50
#define ICM42688_REG4_INT_SOURCE_10           0x51
#define ICM42688_REG4_OFFSET_USER_0           0x77
#define ICM42688_REG4_OFFSET_USER_1           0x78
#define ICM42688_REG4_OFFSET_USER_2           0x79
#define ICM42688_REG4_OFFSET_USER_3           0x7A
#define ICM42688_REG4_OFFSET_USER_4           0x7B
#define ICM42688_REG4_OFFSET_USER_5           0x7C
#define ICM42688_REG4_OFFSET_USER_6           0x7D
#define ICM42688_REG4_OFFSET_USER_7           0x7E
#define ICM42688_REG4_OFFSET_USER_8           0x7F


/*! @} */ // ICM42688_reg

/**
 * @defgroup ICM42688_set 6DOF IMU 14 Registers Settings
 * @brief Settings for registers of 6DOF IMU 14 Click driver.
 */

/**
 * @addtogroup ICM42688_set
 * @{
 */

/**
 * @brief 6DOF IMU 14 Signal path reset register settings.
 * @details Signal path reset register setting flags.
 */
#define ICM42688_SPR_DMP_INIT_EN                  0x40
#define ICM42688_SPR_DMP_MEM_RST_EN               0x20
#define ICM42688_SPR_ABORT_AND_RESET              0x08
#define ICM42688_SPR_TMST_STROBE                  0x04
#define ICM42688_SPR_FIFO_FLUSH                   0x02

/**
 * @brief 6DOF IMU 14 INTF config 0 register settings.
 * @details INTF config 0 register setting flags ( Default - 0x30 ).
 */
#define ICM42688_INTF0_FIFO_HOLD_LAST_DATA_EN     0x80
#define ICM42688_INTF0_FIFO_HOLD_LAST_DATA_DIS    0x00 
#define ICM42688_INTF0_FIFO_COUNT_REC_0           0x00 
#define ICM42688_INTF0_FIFO_COUNT_REC_1           0x40
#define ICM42688_INTF0_FIFO_COUNT_ENDIAN_0        0x00
#define ICM42688_INTF0_FIFO_COUNT_ENDIAN_1        0x20 
#define ICM42688_INTF0_SENSOR_DATA_ENDIAN_0       0x00
#define ICM42688_INTF0_SENSOR_DATA_ENDIAN_1       0x10 
#define ICM42688_INTF0_UI_SIFS_CFG_DISABLE_SPI    0x00 
#define ICM42688_INTF0_UI_SIFS_CFG_DISABLE_I2C    0x01

/**
 * @brief 6DOF IMU 14 INTF config 1 register settings.
 * @details INTF config 1 register setting flags ( Default - 0x91 ).
 */
#define ICM42688_INTF1_ACLP_CLK_SEL_WAKEUP_CLK    0x00
#define ICM42688_INTF1_ACLP_CLK_SEL_RC_CLK        0x08
#define ICM42688_INTF1_RTC_MODE_NO_INPUT          0x00
#define ICM42688_INTF1_RTC_MODE_INPUT             0x04
#define ICM42688_INTF1_CLKSEL_RC_OS               0x00
#define ICM42688_INTF1_CLKSEL_PLL_RC_OS           0x01
#define ICM42688_INTF1_CLKSEL_DISABLE_ALL_OS      0x11

/**
 * @brief 6DOF IMU 14 PWR-MEGMT0 register settings.
 * @details PWR-MEGMT0 register setting flags ( Default - 0x00 ).
 */
#define ICM42688_PWR_TEMP_ENABLE                  0x00
#define ICM42688_PWR_TEMP_DISABLE                 0x20
#define ICM42688_PWR_IDLE_1                       0x10
#define ICM42688_PWR_IDLE_0                       0x00
#define ICM42688_PWR_GYRO_MODE_OFF                0x00
#define ICM42688_PWR_GYRO_MODE_STAND_BY           0x04
#define ICM42688_PWR_GYRO_MODE_LOW_NOISE          0x0C
#define ICM42688_PWR_ACCEL_MODE_OFF               0x00
#define ICM42688_PWR_ACCEL_MODE_LOW_POWER         0x02
#define ICM42688_PWR_ACCEL_MODE_LOW_NOISE         0x03

/**
 * @brief 6DOF IMU 14 GYRO CONFIG 0 register settings.
 * @details GYRO CONFIG 0 register setting flags ( Default - 0x06 ).
 */
#define ICM42688_GCFG0_FS_SEL_2000DPS             0x00
#define ICM42688_GCFG0_FS_SEL_1000DPS             0x20
#define ICM42688_GCFG0_FS_SEL_500DPS              0x40
#define ICM42688_GCFG0_FS_SEL_250DPS              0x60
#define ICM42688_GCFG0_FS_SEL_125DPS              0x80
#define ICM42688_GCFG0_FS_SEL_62p5DPS             0xA0
#define ICM42688_GCFG0_FS_SEL_31p25DPS            0xC0
#define ICM42688_GCFG0_FS_SEL_15p625DPS           0xE0
#define ICM42688_GCFG0_ODR_32KHZ                  0x01
#define ICM42688_GCFG0_ODR_16KHZ                  0x02
#define ICM42688_GCFG0_ODR_8KHZ                   0x03
#define ICM42688_GCFG0_ODR_4KHZ                   0x04
#define ICM42688_GCFG0_ODR_2KHZ                   0x05
#define ICM42688_GCFG0_ODR_1KHZ                   0x06
#define ICM42688_GCFG0_ODR_200HZ                  0x07
#define ICM42688_GCFG0_ODR_100HZ                  0x08
#define ICM42688_GCFG0_ODR_50HZ                   0x09
#define ICM42688_GCFG0_ODR_25HZ                   0x0A
#define ICM42688_GCFG0_ODR_12p5HZ                 0x0B
#define ICM42688_GCFG0_ODR_500HZ                  0x0F

/**
 * @brief 6DOF IMU 14 ACCEL CONFIG 0 register settings.
 * @details ACCEL CONFIG 0 register setting flags ( Default - 0x06 ).
 */
#define ICM42688_ACFG0_FS_SEL_16G                 0x00
#define ICM42688_ACFG0_FS_SEL_8G                  0x20
#define ICM42688_ACFG0_FS_SEL_4G                  0x40
#define ICM42688_ACFG0_FS_SEL_2G                  0x60
#define ICM42688_ACFG0_ODR_32KHZ                  0x01
#define ICM42688_ACFG0_ODR_16KHZ                  0x02
#define ICM42688_ACFG0_ODR_8KHZ                   0x03
#define ICM42688_ACFG0_ODR_4KHZ                   0x04
#define ICM42688_ACFG0_ODR_2KHZ                   0x05
#define ICM42688_ACFG0_ODR_1KHZ                   0x06
#define ICM42688_ACFG0_ODR_200HZ                  0x07
#define ICM42688_ACFG0_ODR_100HZ                  0x08
#define ICM42688_ACFG0_ODR_50HZ                   0x09
#define ICM42688_ACFG0_ODR_25HZ                   0x0A
#define ICM42688_ACFG0_ODR_12p5HZ                 0x0B
#define ICM42688_ACFG0_ODR_6p25HZ                 0x0C
#define ICM42688_ACFG0_ODR_3p125HZ                0x0D
#define ICM42688_ACFG0_ODR_1p562HZ                0x0E
#define ICM42688_ACFG0_ODR_500HZ                  0x0F

/**
 * @brief 6DOF IMU 14 GYRO CONFIG 1 register settings.
 * @details GYRO CONFIG 1 register setting flags ( Default - 0x16 ).
 */
#define ICM42688_GCFG1_FILT_BW_4000HZ             0x00
#define ICM42688_GCFG1_FILT_BW_170HZ              0x20
#define ICM42688_GCFG1_FILT_BW_82HZ               0x40
#define ICM42688_GCFG1_FILT_BW_40HZ               0x60
#define ICM42688_GCFG1_FILT_BW_20HZ               0x80
#define ICM42688_GCFG1_FILT_BW_10HZ               0xA0
#define ICM42688_GCFG1_FILT_BW_5HZ                0xC0
#define ICM42688_GCFG1_UI_FILT_ORD_1ST_OTHER      0x00
#define ICM42688_GCFG1_UI_FILT_ORD_2ND_OTHER      0x04
#define ICM42688_GCFG1_UI_FILT_ORD_3RD_OTHER      0x08
#define ICM42688_GCFG1_DEC2_M2_ORD_3RD_OTHER      0x02

/**
 * @brief 6DOF IMU 14 GYRO ACCEL CONFIG 0 register settings.
 * @details GYRO ACCEL CONFIG 0 register setting flags ( Default - 0x11 ).
 */
#define ICM42688_GACFG0_AC_UI_BW_ODR_2            0x00
#define ICM42688_GACFG0_AC_UI_BW_ODR_4            0x10
#define ICM42688_GACFG0_AC_UI_BW_ODR_5            0x20
#define ICM42688_GACFG0_AC_UI_BW_ODR_8            0x30
#define ICM42688_GACFG0_AC_UI_BW_ODR_10           0x40
#define ICM42688_GACFG0_AC_UI_BW_ODR_16           0x50
#define ICM42688_GACFG0_AC_UI_BW_ODR_20           0x60
#define ICM42688_GACFG0_AC_UI_BW_ODR_40           0x70
#define ICM42688_GACFG0_GY_UI_BW_ODR_2            0x00
#define ICM42688_GACFG0_GY_UI_BW_ODR_4            0x01
#define ICM42688_GACFG0_GY_UI_BW_ODR_5            0x02
#define ICM42688_GACFG0_GY_UI_BW_ODR_8            0x03
#define ICM42688_GACFG0_GY_UI_BW_ODR_10           0x04
#define ICM42688_GACFG0_GY_UI_BW_ODR_16           0x05
#define ICM42688_GACFG0_GY_UI_BW_ODR_20           0x06
#define ICM42688_GACFG0_GY_UI_BW_ODR_40           0x07

/**
 * @brief 6DOF IMU 14 ACCEL CONFIG 1 register settings.
 * @details ACCEL CONFIG 1 register setting flags ( Default - 0x0D ).
 */
#define ICM42688_ACFG1_UI_FILT_ORD_1ST_OTHER      0x00
#define ICM42688_ACFG1_UI_FILT_ORD_2ND_OTHER      0x04
#define ICM42688_ACFG1_UI_FILT_ORD_3RD_OTHER      0x08
#define ICM42688_ACFG1_DEC2_M2_ORD_3RD_OTHER      0x02

/**
 * @brief 6DOF IMU 14 TMST CONFIG register settings.
 * @details TMST CONFIG register setting flags ( Default - 0x23 ).
 */
#define ICM42688_TMSTCFG_TO_REGS_EN               0x10  
#define ICM42688_TMSTCFG_TO_REGS_DIS              0x00  
#define ICM42688_TMSTCFG_RES_0                    0x00  
#define ICM42688_TMSTCFG_RES_1                    0x08  
#define ICM42688_TMSTCFG_DELTA_EN                 0x04  
#define ICM42688_TMSTCFG_DELTA_DIS                0x00  
#define ICM42688_TMSTCFG_FSYNC_EN                 0x02  
#define ICM42688_TMSTCFG_FSYNC_DIS                0x00  
#define ICM42688_TMSTCFG_TMST_ENABLE              0x01  
#define ICM42688_TMSTCFG_TMST_DISABLE             0x00

/**
 * @brief 6DOF IMU 14 APEX CONFIG 0 register settings.
 * @details APEX CONFIG 0 register setting flags.
 */
#define ICM42688_APEXCFG0_DMP_NOT_ACTIVE          0x00 
#define ICM42688_APEXCFG0_DMP_ACTIVE              0x80 
#define ICM42688_APEXCFG0_TAP_ENABLE              0x40 
#define ICM42688_APEXCFG0_TAP_DISABLE             0x00 
#define ICM42688_APEXCFG0_PED_ENABLE              0x20 
#define ICM42688_APEXCFG0_PED_DISABLE             0x00 
#define ICM42688_APEXCFG0_TILT_ENABLE             0x10 
#define ICM42688_APEXCFG0_TILT_DISABLE            0x00 
#define ICM42688_APEXCFG0_R2W_EMABLE              0x08 
#define ICM42688_APEXCFG0_R2W_DISABLE             0x00 
#define ICM42688_APEXCFG0_DMP_ODR_25HZ            0x00 
#define ICM42688_APEXCFG0_DMP_ODR_50HZ            0x03

/**
 * @brief 6DOF IMU 14 SMD CONFIG register settings.
 * @details SMD CONFIG register setting flags.
 */
#define ICM42688_SMDCFG_WOM_INT_MODE_0            0x00 
#define ICM42688_SMDCFG_WOM_INT_MODE_1            0x08 
#define ICM42688_SMDCFG_WOM_MODE_0                0x00 
#define ICM42688_SMDCFG_WOM_MODE_1                0x04 
#define ICM42688_SMDCFG_SMD_MODE_DISABLE          0x00 
#define ICM42688_SMDCFG_SMD_MODE_SHORT            0x02 
#define ICM42688_SMDCFG_SMD_MODE_LONG             0x03 

/**
 * @brief 6DOF IMU 14 INT SOURCE 0 register settings.
 * @details INT SOURCE 0 register setting flags.
 */
#define ICM42688_ISRC0_UI_FSYNC_INT1_EN           0x40
#define ICM42688_ISRC0_UI_FSYNC_INT1_DIS          0x00
#define ICM42688_ISRC0_PLL_RDY_INT1_EN            0x20
#define ICM42688_ISRC0_PLL_RDY_INT1_DIS           0x00
#define ICM42688_ISRC0_RESET_DONE_INT1_EN         0x10
#define ICM42688_ISRC0_RESET_DONE_INT1_DIS        0x00
#define ICM42688_ISRC0_UI_DRDY_INT1_EN            0x08
#define ICM42688_ISRC0_UI_DRDY_INT1_DIS           0x00
#define ICM42688_ISRC0_FIFO_THS_INT1_EN           0x04
#define ICM42688_ISRC0_FIFO_THS_INT1_DIS          0x00
#define ICM42688_ISRC0_FIFO_FULL_INT1_EN          0x02
#define ICM42688_ISRC0_FIFO_FULL_INT1_DIS         0x00
#define ICM42688_ISRC0_UI_AGC_RDY_INT1_EN         0x01
#define ICM42688_ISRC0_UI_AGC_RDY_INT1_DIS        0x00

/**
 * @brief 6DOF IMU 14 WHO AM I register settings.
 * @details WHO AM I register setting ( Default - 0x47 ).
 */
#define ICM42688_DEF_WHO_AM_I                     0x47

/**
 * @brief 6DOF IMU 14 REG BANK SELECT register settings.
 * @details REG BANK SELECT register setting flags ( Default - 0x00 ).
 */
#define ICM42688_BANK_0                           0x00
#define ICM42688_BANK_1                           0x01
#define ICM42688_BANK_2                           0x02
#define ICM42688_BANK_3                           0x03
#define ICM42688_BANK_4                           0x04

/**
 * @brief 6DOF IMU 14 SENSOR CONFIG 0 register settings.
 * @details SENSOR CONFIG 0 register setting flags.
 */
#define ICM42688_SENSOR_CFG_GYRO_Z_EN             0x00
#define ICM42688_SENSOR_CFG_GYRO_Z_DIS            0x20
#define ICM42688_SENSOR_CFG_GYRO_Y_EN             0x00
#define ICM42688_SENSOR_CFG_GYRO_Y_DIS            0x10
#define ICM42688_SENSOR_CFG_GYRO_X_EN             0x00
#define ICM42688_SENSOR_CFG_GYRO_X_DIS            0x08
#define ICM42688_SENSOR_CFG_ACCEL_Z_EN            0x00
#define ICM42688_SENSOR_CFG_ACCEL_Z_DIS           0x04
#define ICM42688_SENSOR_CFG_ACCEL_Y_EN            0x00
#define ICM42688_SENSOR_CFG_ACCEL_Y_DIS           0x02
#define ICM42688_SENSOR_CFG_ACCEL_X_EN            0x00
#define ICM42688_SENSOR_CFG_ACCEL_X_DIS           0x01

/**
 * @brief 6DOF IMU 14 APEX CONFIG 1 register settings.
 * @details APEX CONFIG 1 register setting flags.
 */
#define ICM42688_APEX_CFG1_LOW_ENERGY_SEL         0xA0
#define ICM42688_APEX_CFG1_TIME_SEL_0SEC          0x00
#define ICM42688_APEX_CFG1_TIME_SEL_4SEC          0x01
#define ICM42688_APEX_CFG1_TIME_SEL_8SEC          0x02
#define ICM42688_APEX_CFG1_TIME_SEL_12SEC         0x03
#define ICM42688_APEX_CFG1_TIME_SEL_16SEC         0x04
#define ICM42688_APEX_CFG1_TIME_SEL_20SEC         0x05
#define ICM42688_APEX_CFG1_TIME_SEL_24SEC         0x06
#define ICM42688_APEX_CFG1_TIME_SEL_28SEC         0x07
#define ICM42688_APEX_CFG1_TIME_SEL_32SEC         0x08
#define ICM42688_APEX_CFG1_TIME_SEL_36SEC         0x09
#define ICM42688_APEX_CFG1_TIME_SEL_40SEC         0x0A
#define ICM42688_APEX_CFG1_TIME_SEL_44SEC         0x0B
#define ICM42688_APEX_CFG1_TIME_SEL_48SEC         0x0C
#define ICM42688_APEX_CFG1_TIME_SEL_52SEC         0x0D
#define ICM42688_APEX_CFG1_TIME_SEL_56SEC         0x0E
#define ICM42688_APEX_CFG1_TIME_SEL_60SEC         0x0F

/**
 * @brief 6DOF IMU 14 APEX CONFIG 2 register settings.
 * @details APEX CONFIG 2 register setting flags.
 */
#define ICM42688_APEX_CFG2_PED_AMP_TH_SEL         0x80
#define ICM42688_APEX_CFG2_PED_CNT_SEL_0          0x00
#define ICM42688_APEX_CFG2_PED_CNT_SEL_1          0x01
#define ICM42688_APEX_CFG2_PED_CNT_SEL_2          0x02
#define ICM42688_APEX_CFG2_PED_CNT_SEL_3          0x03
#define ICM42688_APEX_CFG2_PED_CNT_SEL_4          0x04
#define ICM42688_APEX_CFG2_PED_CNT_SEL_5          0x05
#define ICM42688_APEX_CFG2_PED_CNT_SEL_6          0x06
#define ICM42688_APEX_CFG2_PED_CNT_SEL_7          0x07
#define ICM42688_APEX_CFG2_PED_CNT_SEL_8          0x08
#define ICM42688_APEX_CFG2_PED_CNT_SEL_9          0x09
#define ICM42688_APEX_CFG2_PED_CNT_SEL_10         0x0A
#define ICM42688_APEX_CFG2_PED_CNT_SEL_11         0x0B
#define ICM42688_APEX_CFG2_PED_CNT_SEL_12         0x0C
#define ICM42688_APEX_CFG2_PED_CNT_SEL_13         0x0D
#define ICM42688_APEX_CFG2_PED_CNT_SEL_14         0x0E
#define ICM42688_APEX_CFG2_PED_CNT_SEL_15         0x0F

/**
 * @brief 6DOF IMU 14 APEX CONFIG 3 register settings.
 * @details APEX CONFIG 3 register setting flags.
 */
#define ICM42688_APEX_CFG3_STEP_DET_SEL_0         0x00
#define ICM42688_APEX_CFG3_STEP_DET_SEL_1         0x20
#define ICM42688_APEX_CFG3_STEP_DET_SEL_2         0x40
#define ICM42688_APEX_CFG3_STEP_DET_SEL_3         0x60
#define ICM42688_APEX_CFG3_STEP_DET_SEL_4         0x80
#define ICM42688_APEX_CFG3_STEP_DET_SEL_5         0xA0
#define ICM42688_APEX_CFG3_STEP_DET_SEL_6         0xC0
#define ICM42688_APEX_CFG3_STEP_DET_SEL_7         0xE0
#define ICM42688_APEX_CFG3_TIMER_SEL_0            0x00
#define ICM42688_APEX_CFG3_TIMER_SEL_1            0x04
#define ICM42688_APEX_CFG3_TIMER_SEL_2            0x08
#define ICM42688_APEX_CFG3_TIMER_SEL_3            0x0C
#define ICM42688_APEX_CFG3_TIMER_SEL_4            0x10
#define ICM42688_APEX_CFG3_TIMER_SEL_5            0x14
#define ICM42688_APEX_CFG3_TIMER_SEL_6            0x18
#define ICM42688_APEX_CFG3_TIMER_SEL_7            0x1C
#define ICM42688_APEX_CFG3_HI_EN_TH_SEL           0x01

/**
 * @brief 6DOF IMU 14 APEX CONFIG 4 register settings.
 * @details APEX CONFIG 4 register setting flags.
 */
#define ICM42688_APEX_CFG4_TILT_WAIT_SEL_0S         0x00
#define ICM42688_APEX_CFG4_TILT_WAIT_SEL_2S         0x40
#define ICM42688_APEX_CFG4_TILT_WAIT_SEL_4S         0x80
#define ICM42688_APEX_CFG4_TILT_WAIT_SEL_6S         0xC0
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_1p28SEC  0x00
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_2p56SEC  0x08
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_3p84SEC  0x10
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_5p12SEC  0x18
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_6p40SEC  0x20
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_7p68SEC  0x28
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_8p96SEC  0x30
#define ICM42688_APEX_CFG4_TILT_SLEEP_TIME_10p24SEC 0x38

/**
 * @brief 6DOF IMU 14 INT SOURCE 6 register settings.
 * @details INT SOURCE 6 register setting flags.
 */
#define ICM42688_INTSRC6_STEP_DET_INT1_EN         0x20
#define ICM42688_INTSRC6_STEP_DET_INT1_DIS        0x00
#define ICM42688_INTSRC6_STEP_CNT_OFL_INT1_EN     0x10
#define ICM42688_INTSRC6_STEP_CNT_OFL_INT1_DIS    0x00
#define ICM42688_INTSRC6_TILT_DET_INT1_EN         0x08
#define ICM42688_INTSRC6_TILT_DET_INT1_DIS        0x00
#define ICM42688_INTSRC6_WAKE_DET_INT1_EN         0x04
#define ICM42688_INTSRC6_WAKE_DET_INT1_DIS        0x00
#define ICM42688_INTSRC6_SLEEP_DET_INT1_EN        0x02
#define ICM42688_INTSRC6_SLEEP_DET_INT1_DIS       0x00
#define ICM42688_INTSRC6_TAP_DET_INT1_EN          0x01
#define ICM42688_INTSRC6_TAP_DET_INT1_DIS         0x00

/**
 * @brief 6DOF IMU 14 INT SOURCE 7 register settings.
 * @details INT SOURCE 7 register setting flags.
 */
#define ICM42688_INTSRC7_STEP_DET_INT2_EN         0x20
#define ICM42688_INTSRC7_STEP_DET_INT2_DIS        0x00
#define ICM42688_INTSRC7_STEP_CNT_OFL_INT2_EN     0x10
#define ICM42688_INTSRC7_STEP_CNT_OFL_INT2_DIS    0x00
#define ICM42688_INTSRC7_TILT_DET_INT2_EN         0x08
#define ICM42688_INTSRC7_TILT_DET_INT2_DIS        0x00
#define ICM42688_INTSRC7_WAKE_DET_INT2_EN         0x04
#define ICM42688_INTSRC7_WAKE_DET_INT2_DIS        0x00
#define ICM42688_INTSRC7_SLEEP_DET_INT2_EN        0x02
#define ICM42688_INTSRC7_SLEEP_DET_INT2_DIS       0x00
#define ICM42688_INTSRC7_TAP_DET_INT2_EN          0x01
#define ICM42688_INTSRC7_TAP_DET_INT2_DIS         0x00

/**
 * @brief 6DOF IMU 14 DEVICE CONFIG register settings.
 * @details DEVICE CONFIG register setting flags.
 */
#define ICM42688_DEVCONFIG_SPI_MODE_03            0x00
#define ICM42688_DEVCONFIG_SPI_MODE_12            0x10
#define ICM42688_DEVCONFIG_SOFT_RESET_NORMAL      0x00
#define ICM42688_DEVCONFIG_SOFT_RESET_ENABLE      0x01

/**
 * @brief 6DOF IMU 14 DRIVE CONFIG register settings.
 * @details DRIVE CONFIG register setting flags.
 */
#define ICM42688_DRIVECONFIG_MAX_SLEW_RATE        0x55
#define ICM42688_DRIVECONFIG_MIN_SLEW_RATE        0x00

/**
 * @brief 6DOF IMU 14 FIFO CONFIG register settings.
 * @details FIFO CONFIG register setting flags.
 */
#define ICM42688_FIFOCONFIG_BYPASS_MODE           0x00
#define ICM42688_FIFOCONFIG_STREAM_TO_FIFO_MODE   0x40
#define ICM42688_FIFOCONFIG_STOP_ON_FULL_MODE     0x80

/**
 * @brief 6DOF IMU 14 FIFO CONFIG 1 register settings.
 * @details FIFO CONFIG 1 register setting flags.
 */
#define ICM42688_FIFOCONFIG1_RESUME_PARTIAL_RD     0x40
#define ICM42688_FIFOCONFIG1_HIRES_EN              0x10
#define ICM42688_FIFOCONFIG1_TMST_FSYNC_EN         0x08
#define ICM42688_FIFOCONFIG1_TEMP_EN               0x04
#define ICM42688_FIFOCONFIG1_GYRO_EN               0x02
#define ICM42688_FIFOCONFIG1_ACCEL_EN              0x01

/**
 * @brief 6DOF IMU 14 INT STATUS register settings.
 * @details INT STATUS register setting flags.
 */
#define ICM42688_INTSTATUS_UI_FSYNC                0x40
#define ICM42688_INTSTATUS_PLL_RDY                 0x20
#define ICM42688_INTSTATUS_RESET_DONE              0x10
#define ICM42688_INTSTATUS_DATA_RDY                0x08
#define ICM42688_INTSTATUS_FIFO_THS                0x04
#define ICM42688_INTSTATUS_FIFO_FULL               0x02
#define ICM42688_INTSTATUS_AGC_RDY                 0x01

/**
 * @brief 6DOF IMU 14 FIFO HEADER byte settings.
 * @details FIFO HEADER byte setting flags.
 */
#define ICM42688_FIFOHEADER_MSG                    0x80
#define ICM42688_FIFOHEADER_ACCEL                  0x40
#define ICM42688_FIFOHEADER_GYRO                   0x20
#define ICM42688_FIFOHEADER_20                     0x10
#define ICM42688_FIFOHEADER_TIMESTAMP_ODR          0x08
#define ICM42688_FIFOHEADER_ODR_ACCEL              0x02
#define ICM42688_FIFOHEADER_ODR_GYRO               0x01

/**
 * @brief 6DOF IMU 14 SPI read/write bit settings.
 * @details SPI read/write bit setting flags.
 */
#define ICM42688_SPI_READ_MASK                    0x80
#define ICM42688_SPI_WRITE_MASK                   0x7F

#ifdef __cplusplus
}
#endif
#endif // ICM42688_H

/*! @} */ // ICM42688

// ------------------------------------------------------------------------ END
