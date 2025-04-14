#include <sys/time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "time_handling.h"

int _gettimeofday(struct timeval *tv, void *tzvp)
{
    if (tv)
    {
        TickType_t ticks = xTaskGetTickCount();
        tv->tv_sec = EPOCH_TIME_BASE + (ticks / configTICK_RATE_HZ);
        tv->tv_usec = (ticks % configTICK_RATE_HZ) * (1000000 / configTICK_RATE_HZ);
    }
    return 0; // Success
}
