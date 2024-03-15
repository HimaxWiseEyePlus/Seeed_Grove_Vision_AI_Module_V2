#include "sscma.h"

#include "sscma/main_task.hpp"

static void app(void* arg) { sscma::main_task::run(); }

extern "C" int app_main(void) {
    puts("Build date: " __DATE__ " " __TIME__);
    if (xTaskCreate(app, "app", 20480, NULL, 3, NULL) != pdPASS) {
        puts("APP creation failed!");
        while (1) {
        }
    }

    vTaskStartScheduler();

    return 0;
}
