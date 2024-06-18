
#include "uln2003.h"
#include "hx_drv_gpio.h"
#include "WE2_device_addr.h"
#include <stdio.h>

#define STEPPER_DELAY 2

GPIO_INDEX_E yz_motor_pins[NUM_MOTOR_PINS] = {GPIO0,GPIO1,GPIO2,GPIO3};
GPIO_INDEX_E xy_motor_pins[NUM_MOTOR_PINS] = {GPIO4,GPIO5,GPIO6,GPIO7};

GPIO_INDEX_E *get_pins(int motor_id)
{
    if (motor_id == XY_MOTOR_ID)
        return xy_motor_pins;
    if (motor_id == YZ_MOTOR_ID)
        return yz_motor_pins;
    return NULL;
}

uint8_t step_sequence[STEP_SEQ_LEN][NUM_MOTOR_PINS] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
};

uint8_t step_clockwise(uint8_t step_idx, int motor_id)
{
    /*
    cyhal_gpio_psoc6_02_124_bga_t *pins = get_pins(motor_id);

    uint8_t l_step_idx = (step_idx - 1) % STEP_SEQ_LEN;
    for (int i = 0; i < NUM_MOTOR_PINS; i++)
    {
        vTaskDelay(STEPPER_DELAY);
        cyhal_gpio_write(pins[i], step_sequence[l_step_idx][i]);
    }
    return l_step_idx;
    */
}

uint8_t step_anticlockwise(uint8_t step_idx, int motor_id)
{
    /*
    cyhal_gpio_psoc6_02_124_bga_t *pins = get_pins(motor_id);

    uint8_t l_step_idx = (step_idx + 1) % STEP_SEQ_LEN;
    for (int i = 0; i < NUM_MOTOR_PINS; i++)
    {
        vTaskDelay(STEPPER_DELAY);
        cyhal_gpio_write(pins[i], step_sequence[l_step_idx][i]);
    }
    return l_step_idx;
    */
}

int init_motors()
{
    int result;
    hx_drv_gpio_init(GPIO_GROUP_0, HX_GPIO_GROUP_0_BASE);
    for (int i = 0; i < NUM_MOTOR_PINS; i++)
    {
        // Configure PA0(D0) as GPIO0
        hx_drv_gpio_set_output(yz_motor_pins[i], GPIO_OUT_LOW);
        hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    }

    return result;
}

uint8_t step(uint8_t step_idx, int motor_id, uint8_t clockwise, int num)
{
    uint8_t idx=step_idx;
    for (int i = 0; i < num; i++)
    {
        if (clockwise) idx = step_clockwise(idx,motor_id);
        else idx = step_anticlockwise(idx,motor_id);
    }
    return idx;
}
