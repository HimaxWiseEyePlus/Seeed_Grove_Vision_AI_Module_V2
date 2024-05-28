/*!
 * @file icm42688.c
 * @brief 6DOF IMU 14 Click Driver.
 */
#include "icm42688.h"
#include "hx_drv_iic.h"
#include "timer_interface.h"
#include "xprintf.h"

/**
 * @brief Timeout value.
 * @details Definition of timeout value for jumping out of infinite loops.
 */
#define DATA_READY_TIMEOUT     1000


IIC_ERR_CODE_E icm42688_i2c_write(uint8_t reg, uint8_t *data_in, uint8_t len)
{
    uint8_t addr[1];

    addr[0] = reg;
    return hx_drv_i2cm_write_data(USE_DW_IIC_0, ICM42688_I2C_ADDR, addr, 1, data_in, len);
}


IIC_ERR_CODE_E icm42688_i2c_read(uint8_t reg, uint8_t *data_out, uint8_t len)
{
    uint8_t addr[1];

    addr[0] = reg;
    return hx_drv_i2cm_write_restart_read(USE_DW_IIC_0, ICM42688_I2C_ADDR, addr, 1, data_out, len);
}


IIC_ERR_CODE_E icm42688_reset()
{
    uint8_t tmp_data = ICM42688_DEVCONFIG_SOFT_RESET_ENABLE;

    return icm42688_i2c_write(ICM42688_REG0_DEVICE_CONFIG, &tmp_data, 1);
}


int icm42688_check_data_ready()
{
    uint8_t tmp_data;
    uint16_t timeout_cnt = 0;
    for ( ; ; )
    {
        icm42688_i2c_read(ICM42688_REG0_INT_STATUS_1, &tmp_data, 1);
	    hx_drv_timer_cm55x_delay_ms(1, TIMER_STATE_DC);
        if ( tmp_data & 0x08 )
        {
            return 0;
        }
        if ( timeout_cnt++ > DATA_READY_TIMEOUT )
        {
            return -1;
        }
    }
}


IIC_ERR_CODE_E icm42688_init()
{
    uint8_t tmp_data;
    IIC_ERR_CODE_E error_check = IIC_ERR_OK;

    xprintf("icm42688_init()\n");

    error_check = icm42688_reset();
	hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
    
    tmp_data = ICM42688_DRIVECONFIG_MIN_SLEW_RATE;
    error_check |= icm42688_i2c_write(ICM42688_REG0_DRIVE_CONFIG, &tmp_data, 1);
    
    error_check |= icm42688_i2c_read(ICM42688_REG0_WHO_AM_I, &tmp_data, 1);
    if ( tmp_data != ICM42688_DEF_WHO_AM_I )
    {
        xprintf("ICM42688 WHO_AM_I ERROR\n");
        return IIC_ERR_SYS;
    }

    tmp_data = ICM42688_PWR_TEMP_ENABLE | 
               ICM42688_PWR_IDLE_0 |
               ICM42688_PWR_GYRO_MODE_LOW_NOISE |
               ICM42688_PWR_ACCEL_MODE_LOW_NOISE;
    error_check |= icm42688_i2c_write(ICM42688_REG0_PWR_MGMT_0, &tmp_data, 1);
    
    tmp_data = ICM42688_ACFG0_FS_SEL_16G |
               ICM42688_ACFG0_ODR_1KHZ;
    error_check |= icm42688_i2c_write(ICM42688_REG0_ACCEL_CONFIG_0, &tmp_data, 1);
    
    tmp_data = ICM42688_GCFG0_FS_SEL_2000DPS |
               ICM42688_GCFG0_ODR_1KHZ;
    error_check |= icm42688_i2c_write(ICM42688_REG0_GYRO_CONFIG_0, &tmp_data, 1);
    
    tmp_data = ICM42688_FIFOCONFIG_STREAM_TO_FIFO_MODE;
    error_check |= icm42688_i2c_write(ICM42688_REG0_FIFO_CONFIG, &tmp_data, 1);
    
    tmp_data = ICM42688_FIFOCONFIG1_TEMP_EN |
               ICM42688_FIFOCONFIG1_GYRO_EN |
               ICM42688_FIFOCONFIG1_ACCEL_EN;
    error_check |= icm42688_i2c_write(ICM42688_REG0_FIFO_CONFIG_1, &tmp_data, 1);

	hx_drv_timer_cm55x_delay_ms(100, TIMER_STATE_DC);
    return error_check;
}


IIC_ERR_CODE_E icm42688_get_accel_axis(icm42688_axis_t *axis)
{
    uint8_t tmp_data;
    IIC_ERR_CODE_E error_check = IIC_ERR_OK;

    if ( icm42688_check_data_ready() != 0 )
        return IIC_ERR_SYS;

    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_X_MSB, &tmp_data, 1);
    axis->x = tmp_data;
    axis->x <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_X_LSB, &tmp_data, 1);
    axis->x |= tmp_data;

    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_Y_MSB, &tmp_data, 1);
    axis->y = tmp_data;
    axis->y <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_Y_LSB, &tmp_data, 1);
    axis->y |= tmp_data;
    
    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_Z_MSB, &tmp_data, 1);
    axis->z = tmp_data;
    axis->z <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_ACCEL_Z_LSB, &tmp_data, 1);
    axis->z |= tmp_data;
    
    return error_check;
}

IIC_ERR_CODE_E icm42688_get_gyro_axis(icm42688_axis_t *axis)
{
    uint8_t tmp_data;
    IIC_ERR_CODE_E error_check = IIC_ERR_OK;

    if ( icm42688_check_data_ready() != 0 )
        return IIC_ERR_SYS;

    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_X_MSB, &tmp_data, 1);
    axis->x = tmp_data;
    axis->x <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_X_LSB, &tmp_data, 1);
    axis->x |= tmp_data;

    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_Y_MSB, &tmp_data, 1);
    axis->y = tmp_data;
    axis->y <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_Y_LSB, &tmp_data, 1);
    axis->y |= tmp_data;
    
    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_Z_MSB, &tmp_data, 1);
    axis->z = tmp_data;
    axis->z <<= 8;
    error_check |= icm42688_i2c_read(ICM42688_REG0_GYRO_Z_LSB, &tmp_data, 1);
    axis->z |= tmp_data;
    
    return error_check;
}


IIC_ERR_CODE_E icm42688_get_data(icm42688_axis_t *acc_axis, icm42688_axis_t *gyro_axis)
{
    uint8_t tmp_data[ 16 ];
    IIC_ERR_CODE_E error_check = IIC_ERR_OK;

    if ( icm42688_check_data_ready() != 0 )
        return IIC_ERR_SYS;

    error_check |= icm42688_i2c_read(ICM42688_REG0_FIFO_DATA, tmp_data, 16 );
    
    if ( ( tmp_data[ 0 ] & 0xFC ) == ( ICM42688_FIFOHEADER_ACCEL | 
                                       ICM42688_FIFOHEADER_GYRO | 
                                       ICM42688_FIFOHEADER_TIMESTAMP_ODR ) )
    {
        acc_axis->x = tmp_data[ 1 ];
        acc_axis->x <<= 8;
        acc_axis->x |= tmp_data[ 2 ];

        acc_axis->y = tmp_data[ 3 ];
        acc_axis->y <<= 8;
        acc_axis->y |= tmp_data[ 4 ];
        
        acc_axis->z = tmp_data[ 5 ];
        acc_axis->z <<= 8;
        acc_axis->z |= tmp_data[ 6 ];
        
        gyro_axis->x = tmp_data[ 7 ];
        gyro_axis->x <<= 8;
        gyro_axis->x |= tmp_data[ 8 ];
        
        gyro_axis->y = tmp_data[ 9 ];
        gyro_axis->y <<= 8;
        gyro_axis->y |= tmp_data[ 10 ];
        
        gyro_axis->z = tmp_data[ 11 ];
        gyro_axis->z <<= 8;
        gyro_axis->z |= tmp_data[ 12 ];
    }
    
    return error_check;
}
