/*
 * Copyright (c) 2019-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ETHOSU_DRIVER_H
#define ETHOSU_DRIVER_H

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "ethosu_types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Defines
 ******************************************************************************/

#define ETHOSU_DRIVER_VERSION_MAJOR 0  ///< Driver major version
#define ETHOSU_DRIVER_VERSION_MINOR 16 ///< Driver minor version
#define ETHOSU_DRIVER_VERSION_PATCH 0  ///< Driver patch version

/******************************************************************************
 * Types
 ******************************************************************************/

// Forward declare
struct ethosu_device;

enum ethosu_job_state
{
    ETHOSU_JOB_IDLE = 0,
    ETHOSU_JOB_RUNNING,
    ETHOSU_JOB_DONE
};

struct ethosu_job
{
    volatile enum ethosu_job_state state;
    const void *custom_data_ptr;
    int custom_data_size;
    const uint64_t *base_addr;
    const size_t *base_addr_size;
    int num_base_addr;
    void *user_arg;
};

struct ethosu_driver
{
    struct ethosu_device *dev;
    struct ethosu_driver *next;
    struct ethosu_job job;
    void *semaphore;
    uint64_t fast_memory;
    size_t fast_memory_size;
    uint32_t power_request_counter;
    bool status_error;
    bool reserved;
};

struct ethosu_driver_version
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
};

enum ethosu_request_clients
{
    ETHOSU_PMU_REQUEST       = 0,
    ETHOSU_INFERENCE_REQUEST = 1,
};

/******************************************************************************
 * Prototypes (weak functions in driver)
 ******************************************************************************/

/**
 * Interrupt handler to be called on IRQ from Ethos-U
 */
void ethosu_irq_handler(struct ethosu_driver *drv);

/*
 * Flush/clean the data cache by address and size. Passing NULL as p argument
 * expects the whole cache to be flushed.
 */

void ethosu_flush_dcache(uint32_t *p, size_t bytes);
/*
 * Invalidate the data cache by address and size. Passing NULL as p argument
 * expects the whole cache to be invalidated.
 */
void ethosu_invalidate_dcache(uint32_t *p, size_t bytes);

/*
 * Minimal sempahore and mutex implementation for baremetal applications. See
 * ethosu_driver.c.
 */
void *ethosu_mutex_create(void);
void *ethosu_semaphore_create(void);
/*
 * Returns:
 *   -1 on error
 *    0 on success
 */
int ethosu_mutex_lock(void *mutex);
int ethosu_mutex_unlock(void *mutex);
int ethosu_semaphore_take(void *sem);
int ethosu_semaphore_give(void *sem);

/*
 * Callbacks for begin/end of inference. user_data pointer is passed to the
 * ethosu_invoke() call and forwarded to the callback functions.
 */
void ethosu_inference_begin(struct ethosu_driver *drv, void *user_arg);
void ethosu_inference_end(struct ethosu_driver *drv, void *user_arg);

/******************************************************************************
 * Prototypes
 ******************************************************************************/

/**
 * Initialize the Ethos-U driver.
 */
int ethosu_init(struct ethosu_driver *drv,
                const void *base_address,
                const void *fast_memory,
                const size_t fast_memory_size,
                uint32_t secure_enable,
                uint32_t privilege_enable);

/**
 * Deinitialize the Ethos-U driver.
 */
void ethosu_deinit(struct ethosu_driver *drv);

/**
 * Soft resets the Ethos-U device.
 */
bool ethosu_soft_reset(struct ethosu_driver *drv);

/**
 * Request to disable Q-channel power gating of the Ethos-U device.
 * Power requests are ref.counted. Increases count.
 * (Note: clock gating is made to follow power gating)
 */
bool ethosu_request_power(struct ethosu_driver *drv);

/**
 * Release disable request for Q-channel power gating of the Ethos-U device.
 * Power requests are ref.counted. Decreases count.
 */
void ethosu_release_power(struct ethosu_driver *drv);

/**
 * Get Ethos-U driver version.
 */
void ethosu_get_driver_version(struct ethosu_driver_version *ver);

/**
 * Get Ethos-U hardware information.
 */
void ethosu_get_hw_info(struct ethosu_driver *drv, struct ethosu_hw_info *hw);

/**
 * Invoke Vela command stream.
 */
int ethosu_invoke_v3(struct ethosu_driver *drv,
                     const void *custom_data_ptr,
                     const int custom_data_size,
                     const uint64_t *base_addr,
                     const size_t *base_addr_size,
                     const int num_base_addr,
                     void *user_arg);

#define ethosu_invoke(drv, custom_data_ptr, custom_data_size, base_addr, base_addr_size, num_base_addr)                \
    ethosu_invoke_v3(drv, custom_data_ptr, custom_data_size, base_addr, base_addr_size, num_base_addr, 0)

/**
 * Invoke Vela command stream using async interface.
 * Must be followed by call(s) to ethosu_wait() upon successful return.
 * Returns
 *   -1 on error
 *    0 on success
 */
int ethosu_invoke_async(struct ethosu_driver *drv,
                        const void *custom_data_ptr,
                        const int custom_data_size,
                        const uint64_t *base_addr,
                        const size_t *base_addr_size,
                        const int num_base_addr,
                        void *user_arg);

/**
 * Wait for inference to complete (block=true)
 * Poll status or finish up if inference is complete (block=false)
 * (This function is only intended to be used in conjuction with ethosu_invoke_async)
 * Returns
 *    1 on inference running (only for block=false)
 *    0 on inference success
 *   -1 on inference error
 *   -2 on inference not invoked
 */
int ethosu_wait(struct ethosu_driver *drv, bool block);

/**
 * Reserves a driver to execute inference with
 */
struct ethosu_driver *ethosu_reserve_driver(void);

/**
 * Change driver status to available
 */
void ethosu_release_driver(struct ethosu_driver *drv);

/**
 * Static inline for backwards-compatibility
 */
static inline int ethosu_invoke_v2(const void *custom_data_ptr,
                                   const int custom_data_size,
                                   const uint64_t *base_addr,
                                   const size_t *base_addr_size,
                                   const int num_base_addr)
{
    struct ethosu_driver *drv = ethosu_reserve_driver();
    int result = ethosu_invoke_v3(drv, custom_data_ptr, custom_data_size, base_addr, base_addr_size, num_base_addr, 0);
    ethosu_release_driver(drv);
    return result;
}

#ifdef __cplusplus
}
#endif

#endif // ETHOSU_DRIVER_H
