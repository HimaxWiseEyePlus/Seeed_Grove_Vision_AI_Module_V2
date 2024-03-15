#pragma once

#include <atomic>

#include <edgelab.h>

using namespace edgelab;

std::atomic<uint8_t> count;

static void p1(void *arg) {
    while (1) {
        count++;
        printf("thread 1 -> %d\n", count.load());
        el_sleep(70);
    }
}

static void p2(void *arg) {
    while (1) {
        count++;
        printf("thread 2 -> %d\n", count.load());
        el_sleep(50);
    }
}

static void p3(void *arg) {
    while (1) {
        count++;
        printf("thread 3 -> %d\n", count.load());
        el_sleep(30);
    }
}

static void app(void *arg)
{
    xTaskCreate(p1, "p1", 4096, NULL, 2, NULL);
    xTaskCreate(p2, "p2", 4096, NULL, 2, NULL);
    xTaskCreate(p3, "p3", 4096, NULL, 2, NULL);

    for (;;) {
        printf("------\n");
        el_sleep(100);
    }
}
