#include <cstdio>
#include <cstdint>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "xprintf.h"
extern "C" {
	#include "hx_drv_pmu.h"
	#include "timer_interface.h"
};
#include "ethosu_driver.h"

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE)
// audio raw samples are ususally collected in int16_t format
static const int16_t features[] = {};
#else
static const float features[] = {};
#endif

static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)U55_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    EPII_NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

}

static int _arm_npu_init(bool security_enable, bool privilege_enable)
{
    int err = 0;

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    void * const ethosu_base_address = (void *)(U55_BASE);

    if (0 != (err = ethosu_init(
                            &ethosu_drv,             /* Ethos-U driver device pointer */
                            ethosu_base_address,     /* Ethos-U NPU's base address. */
                            NULL,       /* Pointer to fast mem area - NULL for U55. */
                            0, /* Fast mem region size. */
							security_enable,                       /* Security enable. */
							privilege_enable))) {                   /* Privilege enable. */
    	ei_printf("failed to initalise Ethos-U device\n");
            return err;
        }

    ei_printf("Ethos-U55 device initialised\n");

    return 0;
}

extern "C" int ei_standalone_inferencing_app(void)
{
    uint32_t wakeup_event;
	uint32_t wakeup_event1;
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	if(_arm_npu_init(true, true) !=0 ) {
		ei_printf("Faield to init NPU\n");
	}

    // Calculate the length of the buffer
    size_t buf_len = sizeof(features) / sizeof(features[0]);

    // Assign callback function to fill buffer used for preprocessing/inference
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &get_signal_data;

	while (1)
	{
		// Perform DSP pre-processing and inference
		res = run_classifier(&signal, &result, true);
		if (res == EI_IMPULSE_OK) {
			display_results(&result);
		}
		else {
			ei_printf("ERR: Failed to run impulse (%d)\n", res);
		}
		hx_drv_timer_cm55x_delay_ms(500, TIMER_STATE_DC);
	}

    return 0;
}

#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE)
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
	// audio samples are usually int16_t, so we need to convert them to float
	return ei::numpy::int16_to_float(features + offset, out_ptr, length);
}
#else
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features + offset)[i];
    }

    return EIDSP_OK;
}
#endif
