#ifndef APP_SSCMA_NETWORK_
#define APP_SSCMA_NETWORK_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "BITOPS.h"
#include "WE2_core.h"
#include "WE2_device.h"
#include "board.h"
#include "hx_drv_pmu.h"
#include "hx_drv_pmu_export.h"
#include "powermode.h"
#include "powermode_export.h"
#include "xprintf.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" int app_main(void);
#else
int app_main(void);
#endif

#endif /* APP_SSCMA_NETWORK_ */
