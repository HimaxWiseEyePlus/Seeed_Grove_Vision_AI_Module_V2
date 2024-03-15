#pragma once

#include <edgelab.h>

using namespace edgelab;

edgelab::Mutex mtx; // Define a mutex

unsigned long count = 0;

static void p1(void *arg)
{
    while (1)
    {
        mtx.lock(); // Acquire the lock
        printf("thread 1 -> %ld\n", count++);
        mtx.unlock(); // Release the lock
        el_sleep(30);
    }
}

static void p2(void *arg)
{
    while (1)
    {
        mtx.lock(); // Acquire the lock
        printf("thread 2 -> %ld\n", count++);
        mtx.unlock(); // Release the lock
        el_sleep(20);
    }
}

static void p3(void *arg)
{
    while (1)
    {
        mtx.lock(); // Acquire the lock
        printf("thread 3 -> %ld\n", count++);
        mtx.unlock(); // Release the lock
        el_sleep(10);
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
