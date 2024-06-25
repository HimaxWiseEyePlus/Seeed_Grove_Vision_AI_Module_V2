
#include "btn.h"
#include "hx_drv_gpio.h"
#include "hx_drv_timer.h"
#include "WE2_device_addr.h"
#include "hx_drv_scu_export.h"
#include "evt_datapath.h"

#include <stdio.h>

uint8_t g_val;

/// register callback function
void aon_gpio_0_cb(uint8_t group, uint8_t aIndex)
{
    GPIO_INDEX_E gpio_idx = (group << 4 | aIndex);

    hx_drv_gpio_get_in_value(gpio_idx, &g_val);
    xprintf("aon_gpio_0_cb(%d, %d, %d)\n", group, aIndex, g_val);

    if (g_val) evt_Pilboi_BtnDown_cb();
    else evt_Pilboi_BtnUp_cb();
}

int btn_init()
{
    /// The pin of AON_GPIO0 is defined by the user application.
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_2, 1);

    /// Initialize GPIO_GROUP_5_AON
    hx_drv_gpio_init(GPIO_GROUP_5, HX_GPIO_GROUP_5_BASE);

    /// set AON_GPIO0 as input mode
    hx_drv_gpio_set_input(AON_GPIO0);

    hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio_0_cb);

    /// Set interrupt triggering mode
    hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_BOTH);

    /// enable interrupt
    hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
}

uint8_t get_value()
{
    xprintf("gpio results - %d\n", g_val);
    return g_val;
}