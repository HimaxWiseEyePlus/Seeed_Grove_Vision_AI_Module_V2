#include <cstdio>
#include <cstdint>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "xprintf.h"
extern "C" {
	#include "hx_drv_pmu.h"
	#include "timer_interface.h"
};

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

#if defined(EI_CLASSIFIER_SENSOR) && (EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE)
// audio raw samples are ususally collected in int16_t format
static const int16_t features[] = {};
#else
static const float features[] = {};
#endif

extern "C" int ei_standalone_inferencing_app(void)
{
    uint32_t wakeup_event;
	uint32_t wakeup_event1;
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

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
