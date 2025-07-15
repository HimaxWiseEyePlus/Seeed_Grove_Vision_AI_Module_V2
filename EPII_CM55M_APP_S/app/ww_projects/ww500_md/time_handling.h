#include <sys/time.h>
#include "FreeRTOS.h"
#include "task.h"

// this is the base timestamp for Jan 1 2025 00:00:00.
// in order to actually calculate the current_time, we'll need to retrieve the datetime from the mobile on deployment
#define EPOCH_TIME_BASE 1735689600
// example being:
// static time_t epoch_base = 0; // set at boot

// void set_epoch_base(time_t current_time) {
//     epoch_base = current_time - (xTaskGetTickCount() / configTICK_RATE_HZ);
// }

int _gettimeofday(struct timeval *tv, void *tzvp);