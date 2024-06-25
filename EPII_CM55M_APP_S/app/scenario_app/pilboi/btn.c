
#include "btn.h"
#include "hx_drv_gpio.h"
#include "hx_drv_timer.h"
#include "WE2_device_addr.h"
#include "hx_drv_scu_export.h"

#include <stdio.h>

int init_btn()
{
    int result;
    //hx_drv_gpio_init(GPIO_GROUP_0, HX_GPIO_GROUP_0_BASE);

    // Configure PB3(D10) as GPIO14
    result = hx_drv_gpio_set_input(GPIO14);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_GPIO14, 1);

    return result;
}

uint8_t get_value(){
    uint8_t val = 0;
    int result = hx_drv_gpio_get_in_value(GPIO14,val);
    return val;
}