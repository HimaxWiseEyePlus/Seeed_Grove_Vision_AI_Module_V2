
#include "uln2003.h"
#include "hx_drv_gpio.h"
#include "hx_drv_timer.h"
#include "WE2_device_addr.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hx_drv_scu_export.h"

#include <stdio.h>

#define STEPPER_DELAY 2

GPIO_INDEX_E yz_motor_pins[NUM_MOTOR_PINS] = {GPIO13, GPIO14, GPIO15, GPIO0};
GPIO_INDEX_E xy_motor_pins[NUM_MOTOR_PINS] = {GPIO1, GPIO2, SB_GPIO0, SB_GPIO1};

GPIO_INDEX_E *get_pins(int motor_id)
{
    if (motor_id == XY_MOTOR_ID)
        return xy_motor_pins;
    if (motor_id == YZ_MOTOR_ID)
        return yz_motor_pins;
    return NULL;
}

uint8_t step_sequence[STEP_SEQ_LEN][NUM_MOTOR_PINS] = {
    {GPIO_OUT_HIGH, GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_LOW},
    {GPIO_OUT_HIGH, GPIO_OUT_HIGH, GPIO_OUT_LOW, GPIO_OUT_LOW},
    {GPIO_OUT_LOW, GPIO_OUT_HIGH, GPIO_OUT_LOW, GPIO_OUT_LOW},
    {GPIO_OUT_LOW, GPIO_OUT_HIGH, GPIO_OUT_HIGH, GPIO_OUT_LOW},
    {GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_HIGH, GPIO_OUT_LOW},
    {GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_HIGH, GPIO_OUT_HIGH},
    {GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_HIGH},
    {GPIO_OUT_HIGH, GPIO_OUT_LOW, GPIO_OUT_LOW, GPIO_OUT_HIGH},
};

uint8_t step_clockwise(uint8_t step_idx, int motor_id)
{
    GPIO_INDEX_E *pins = get_pins(motor_id);

    uint8_t l_step_idx = (step_idx - 1) % STEP_SEQ_LEN;
    for (int i = 0; i < NUM_MOTOR_PINS; i++)
    {
        hx_drv_gpio_set_out_value(pins[i], step_sequence[l_step_idx][i]);
    }
    vTaskDelay(2); //TICKS
    return l_step_idx;
}

uint8_t step_anticlockwise(uint8_t step_idx, int motor_id)
{
    GPIO_INDEX_E *pins = get_pins(motor_id);

    uint8_t l_step_idx = (step_idx + 1) % STEP_SEQ_LEN;
    for (int i = 0; i < NUM_MOTOR_PINS; i++)
    {
        hx_drv_gpio_set_out_value(pins[i], step_sequence[l_step_idx][i]);
    }
    vTaskDelay(2); //TICKS
    return l_step_idx;
}

int init_motors()
{
    int result;
    //hx_drv_gpio_init(GPIO_GROUP_0, HX_GPIO_GROUP_0_BASE);

    // Configure PB2(D9) as GPIO13
    hx_drv_gpio_set_output(GPIO13, GPIO_OUT_LOW);
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_GPIO13, 1);

    // Configure PB3(D10) as GPIO14
    hx_drv_gpio_set_output(GPIO14, GPIO_OUT_LOW);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_GPIO14, 1);

    // Configure PB4(D8) as GPIO15
    hx_drv_gpio_set_output(GPIO15, GPIO_OUT_LOW);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_GPIO15, 1);

    // Configure PB6(D6) as GPIO0
    hx_drv_gpio_set_output(GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_GPIO0_1, 1);

    // Configure PB7(D7) as GPIO1
    hx_drv_gpio_set_output(GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_GPIO1_1, 1);

    // Configure PB8(D2) as GPIO2
    hx_drv_gpio_set_output(GPIO2, GPIO_OUT_LOW);
    hx_drv_scu_set_PB8_pinmux(SCU_PB8_PINMUX_GPIO2_1, 1);

    // Configure PA2(D5) as SB_GPIO0
    hx_drv_gpio_set_output(SB_GPIO0, GPIO_OUT_LOW);
    hx_drv_scu_set_PA2_pinmux(SCU_PA2_PINMUX_SB_GPIO0, 1);

    // Configure PA3(D4) as SB_GPIO1
    hx_drv_gpio_set_output(SB_GPIO1, GPIO_OUT_LOW);
    hx_drv_scu_set_PA3_pinmux(SCU_PA3_PINMUX_SB_GPIO1, 1);

    return result;
}

uint8_t step_some(uint8_t step_idx, int motor_id, uint8_t clockwise, int num)
{
    uint8_t idx = step_idx;
    for (int i = 0; i < num; i++)
    {
        if (clockwise)
            idx = step_clockwise(idx, motor_id);
        else
            idx = step_anticlockwise(idx, motor_id);
    }
    return idx;
}
