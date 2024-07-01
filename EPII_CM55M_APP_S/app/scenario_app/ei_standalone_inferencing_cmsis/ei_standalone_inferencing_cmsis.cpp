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

static const float features[] = {
	// sample raw data of the 'snake' movement
    -0.1900, -2.7600, 10.0200, 0.0700, -3.7800, 10.1300, 0.0100, -4.0500, 10.0200, 0.3100, -3.6800, 9.6300, 0.9200, -2.2800, 9.1200, 1.2500, -1.2500, 9.3700, 1.2500, -1.2500, 9.3700, 1.6000, -1.8100, 9.0700, 1.9000, -3.2300, 9.5000, 1.5000, -3.0700, 9.4400, 2.2500, -2.4900, 9.8200, 1.6600, -2.5400, 9.9300, 1.6600, -2.5400, 9.9300, 1.0100, -1.4600, 8.8700, 1.4700, -0.1500, 9.7500, 0.5200, 0.3300, 9.1500, 1.3200, 1.1700, 8.4400, 1.0000, 2.5200, 9.9100, 0.2000, 3.7100, 9.6200, 0.2000, 3.7100, 9.6200, 1.1400, 4.9800, 9.5500, 0.9900, 5.1400, 9.8500, 0.3300, 5.0300, 10.2000, -0.8000, 3.9500, 9.9400, -0.9800, 3.2700, 9.8700, -2.0100, 3.3700, 10.0400, -2.0100, 3.3700, 10.0400, -2.4400, 4.2300, 10.1300, -0.2000, 2.7600, 9.7200, -0.3700, 0.6600, 10.1500, -2.3700, -1.1500, 9.9500, -3.3500, -2.0600, 10.1600, -2.6900, -0.9900, 10.0200, -2.6900, -0.9900, 10.0200, -0.7700, 0.5000, 10.0100, 0.7200, 0.6800, 10.0400, 1.3400, -0.1900, 9.9500, 1.1200, -2.0700, 10.2000, 0.7200, -4.8000, 10.1600, -0.4600, -5.0800, 10.0600, -0.4600, -5.0800, 10.0600, 0.3900, -5.5700, 10.2300, 0.2300, -4.4400, 10.0600, 1.6900, -0.9400, 9.1800, 1.4600, 0.0400, 9.8800, 1.2400, -1.3500, 10.0200, 0.8800, -2.2700, 8.7000, 0.8800, -2.2700, 8.7000, 0.7900, -1.1800, 9.9800, 0.6800, 0.4300, 9.9000, 1.6100, 1.9400, 9.7500, 1.4200, 2.6000, 9.7400, 1.1200, 2.7700, 9.7200, 0.9400, 3.4100, 10.2600, 0.9400, 3.4100, 10.2600, 1.0600, 4.7600, 10.0500, 1.0400, 5.6500, 10.1900, 1.0500, 4.7100, 9.9200, -1.6600, 2.8100, 10.1700, -1.0800, 2.7300, 9.9500, -1.0800, 2.7300, 9.9500, -0.9100, 2.6100, 9.9900, -0.4500, 2.4700, 9.9100, 0.2700, 1.7900, 9.8000, -1.0500, 0.0100, 9.4400, -1.8500, -0.7200, 9.9800, -1.7800, -0.6700, 10.1500, -1.7800, -0.6700, 10.1500, -0.9800, -0.2000, 9.7900, -1.1100, 0.2000, 10.3600, -0.3200, -2.0000, 10.0400, -0.3000, -4.2500, 10.1300, -0.9900, -6.1800, 10.2300, -1.8400, -5.8500, 9.9300, -1.8400, -5.8500, 9.9300, -2.2500, -4.0200, 9.4700, -0.6900, -1.1800, 10.1700, -0.0100, 0.1000, 10.3000, 0.8500, -0.5600, 9.4700, 0.5000, -1.8400, 10.1300, 0.1400, -2.3700, 9.8400, 0.1400, -2.3700, 9.8400, -1.1800, -1.3800, 9.7100, -1.5200, -0.0300, 10.2000, -2.0700, 0.1200, 9.8900, -2.1200, -0.0600, 10.2000, -1.6500, -1.0000, 9.9400, -2.0900, -0.0100, 10.2000, -2.0900, -0.0100, 10.2000, -2.4900, 0.6700, 9.9300, -2.9800, 2.2900, 9.8600, -3.5300, 3.5200, 10.1800, -3.2400, 3.1200, 9.4200, -2.0800, 2.8700, 9.9400, -1.5000, 1.3300, 9.3800, -1.5000, 1.3300, 9.3800, -0.1600, 0.2500, 8.4100, 0.5600, 0.4400, 7.2500, -1.0300, 0.2600, 9.6400, -1.4600, 1.0300, 10.0900, -0.0300, 0.2300, 9.5000, 0.5000, -0.1000, 10.2900, 0.5000, -0.1000, 10.2900, 0.0400, -1.7200, 10.0500, 1.5700, -2.1500, 10.1700, 1.0100, -3.3600, 9.9600, 0.4300, -4.0200, 10.3100, 0.2200, -4.0500, 10.1100, 0.2200, -4.0500, 10.1100, 1.4900, -3.3400, 9.9400, 0.6700, -2.1900, 10.0900, 1.7300, -2.5400, 9.6600, 2.3600, -1.9300, 9.7500, 2.5600, -0.9000, 10.0500, 3.7000, -0.3300, 10.1100, 3.7000, -0.3300, 10.1100, 2.2100, -0.5100, 10.0200, 3.2100, 0.4500, 10.2000, 2.4100, 1.1400, 10.0600, 2.5800, 1.5700, 9.7100, 1.0500, 2.0100, 10.4300, -0.1400, 1.9300, 9.6500, -0.1400, 1.9300, 9.6500, -1.3500, 1.8300, 10.0300, -1.8500, 1.7600, 10.0700
};

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
		res = run_classifier(&signal, &result, false);

		// Print return code and how long it took to perform inference
		ei_printf("run_classifier returned: %d\r\n", res);
		ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
				result.timing.dsp,
				result.timing.classification,
				result.timing.anomaly);
		ei_printf("Predictions:\r\n");
		for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
			ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
			ei_printf_float(result.classification[i].value);
			ei_printf("\r\n");
		}
		hx_drv_timer_cm55x_delay_ms(500, TIMER_STATE_DC);
	}

    return 0;
}

// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features + offset)[i];
    }

    return EIDSP_OK;
}
