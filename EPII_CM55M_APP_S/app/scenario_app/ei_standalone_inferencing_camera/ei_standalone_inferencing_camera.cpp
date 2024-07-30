#include <cstdio>
#include <cstdint>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "xprintf.h"
extern "C" {
	#include "hx_drv_pmu.h"
	#include "timer_interface.h"
    #include "event_handler.h"
};
#include "ethosu_driver.h"
#include "img_proc_helium.h"
#include "WE2_core.h"
#include "WE2_device.h"
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#include "cisdp_sensor.h"

#ifdef TRUSTZONE_SEC
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#ifndef TRUSTZONE
#define U55_BASE	BASE_ADDR_APB_U55_CTRL_ALIAS
#else
#define U55_BASE	BASE_ADDR_APB_U55_CTRL
#endif
#endif


/*volatile*/ uint32_t g_img_data = 0;

static uint8_t 	g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout, g_wdt3_timeout;
static uint8_t 	g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t 	g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t 	g_time;
static uint8_t g_spi_master_initial_status;
/*volatile*/ uint32_t jpeg_addr, jpeg_sz;

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */

// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);

static uint8_t raw_image[EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * 3] __ALIGNED(32);

static void dp_var_int()
{
	g_xdma_abnormal = 0;
	g_md_detect = 0;
	g_cdm_fifoerror = 0;
	g_wdt1_timeout = 0;
	g_wdt2_timeout = 0;
	g_wdt3_timeout = 0;
	g_inp1bitparer_abnormal = 0;
	g_dp_event = 0;
	g_frame_ready = 0;
	g_time = 0;
	g_cur_jpegenc_frame = 0;
	g_hxautoi2c_error = 0;
	g_spi_master_initial_status = 0;
}

static void event_handler_cb(EVT_INDEX_E event)
{
	uint16_t err;
	int32_t read_status;
	uint32_t chipid, version;

	dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
	g_dp_event = event;

	switch(event)
	{
	case EVT_INDEX_1BITPARSER_ERR:  /*reg_inpparser_fs_cycle_error*/
		hx_drv_inp1bitparser_get_errstatus(&err);
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n",err);
		hx_drv_inp1bitparser_clear_int();
		hx_drv_inp1bitparser_set_enable(0);
		g_inp1bitparer_abnormal = 1;
		break;
	case EVT_INDEX_EDM_WDT1_TIMEOUT:
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
		g_wdt1_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT2_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
		g_wdt2_timeout = 1;
		break;
	case EVT_INDEX_EDM_WDT3_TIMEOUT:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
		g_wdt3_timeout = 1;
		break;

	case EVT_INDEX_CDM_FIFO_ERR:
		/*
		 * error happen need CDM timing & TPG setting
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_CDM_FIFO_ERR\r\n");
		g_cdm_fifoerror = 1;

		break;

	case EVT_INDEX_XDMA_WDMA1_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA2_ABNORMAL:
	case EVT_INDEX_XDMA_WDMA3_ABNORMAL:
	case EVT_INDEX_XDMA_RDMA_ABNORMAL:
		/*
		 * error happen need
		 * 1. SWRESET Datapath
		 * 2. restart streaming flow
		 */
		dbg_printf(DBG_LESS_INFO, "EVT_INDEX_XDMA_WDMA123_ABNORMAL or EVT_INDEX_XDMA_RDMA_ABNORMAL\r\n");
		g_xdma_abnormal = 1;
		break;

	case EVT_INDEX_CDM_MOTION:
		/*
		 * app anything want to do
		 * */
		dbg_printf(DBG_LESS_INFO, "Motion Detect\n");
		g_md_detect = 1;
		break;
	case EVT_INDEX_XDMA_FRAME_READY:
		g_cur_jpegenc_frame++;
    	g_frame_ready = 1;
		dbg_printf(DBG_LESS_INFO, "SENSORDPLIB_STATUS_XDMA_FRAME_READY %d \n", g_cur_jpegenc_frame);
		break;

	case EVT_INDEX_SENSOR_RTC_FIRE:
		g_time++;
		break;
	case EVT_INDEX_HXAUTOI2C_ERR:
		dbg_printf(DBG_LESS_INFO,"EVT_INDEX_HXAUTOI2C_ERR\r\n");
		g_hxautoi2c_error = 1;
		break;
	default:
		dbg_printf(DBG_LESS_INFO,"Other Event %d\n", event);
		break;
	}

	if ( g_frame_ready == 1 )
	{
		g_frame_ready = 0;

		cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

#ifdef CIS_IMX
		hx_drv_scu_get_version(&chipid, &version);
		if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
		{
			cisdp_stream_off();
			set_mipi_csirx_disable();
		}
#endif

#if FRAME_CHECK_DEBUG
		if(g_spi_master_initial_status == 0) {
			hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
			hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
			hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
			hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_SPI_M_CS, 1);
			if(hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
			{
				dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
				sensordplib_retrigger_capture();
				return ;
			}
			g_spi_master_initial_status = 1;
		}

		#if 1	// send JPG image
		read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
		dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n", read_status, jpeg_sz, jpeg_addr);
		#else	// send YUV420 image
		SPI_CMD_DATA_TYPE image_type;
		uint32_t wdam3_addr = app_get_raw_addr();
		uint32_t data_size = app_get_raw_sz();
		uint8_t imagesize_header[4];
		image_type = DATA_TYPE_RAW_HEADER_IMG_YUV420_8U3C;
		imagesize_header[0] = app_get_raw_width() & 0xFF;
		imagesize_header[1] = (app_get_raw_width()>>8) & 0xFF;
		imagesize_header[2] = app_get_raw_height() & 0xFF;
		imagesize_header[3] = (app_get_raw_height()>>8) & 0xFF;
		read_status =  hx_drv_spi_mst_protocol_write_ex(wdam3_addr, data_size, image_type, imagesize_header, 4);
		dbg_printf(DBG_LESS_INFO, "addr=0x%x, YUV write frame result %d, data size %d\n", wdam3_addr, read_status, data_size);
		#endif

#endif
		uint32_t img_w = app_get_raw_width();
		uint32_t img_h = app_get_raw_height();
		uint32_t ch = app_get_raw_channels();
		uint32_t raw_addr = app_get_raw_addr();
		float w_scale = (float)(img_w - 1) / (EI_CLASSIFIER_INPUT_WIDTH - 1);
		float h_scale = (float)(img_h - 1) / (EI_CLASSIFIER_INPUT_HEIGHT - 1);

		hx_lib_image_resize_BGR8U3C_to_RGB24_helium((uint8_t*)raw_addr, raw_image,
		                    img_w, img_h, ch, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, w_scale, h_scale);

        signal_t signal;            // Wrapper for raw input buffer
        ei_impulse_result_t result; // Used to store inference output

        // Assign callback function to fill buffer used for preprocessing/inference
        signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
        signal.get_data = &get_signal_data;

		EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

		// Print return code and how long it took to perform inference
		ei_printf("run_classifier returned: %d\r\n", res);
        display_results(&result);
#ifdef CIS_IMX
		hx_drv_scu_get_version(&chipid, &version);
		if (chipid == WE2_CHIP_VERSION_C)   // mipi workaround for WE2 chip version C
		{
    		set_mipi_csirx_enable();
    		cisdp_stream_on();
		}
#endif

		//recapture image
		sensordplib_retrigger_capture();
	}

	if(g_md_detect == 1)
	{
		g_md_detect = 0;
	}

	if(g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1
			|| g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
	{
		cisdp_sensor_stop();
	}

}

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

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);

	if(_arm_npu_init(true, true) !=0 ) {
		ei_printf("Faield to init NPU\n");
	}

    if(cisdp_sensor_init() < 0)
    {
        ei_printf("CIS Init fail\r\n");
    }

    dp_var_int();

    if(cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, event_handler_cb, g_img_data, APP_DP_RES_RGB640x480_INP_SUBSAMPLE_1X) < 0)
    {
        ei_printf("\r\nDATAPATH Init fail\r\n");
    }

    event_handler_init();

    cisdp_sensor_start();

	// this is blocking call, inference is processed in event_handler_cb
    event_handler_start();

    return 0;
}

static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    // we already have a RGB888 buffer, so recalculate offset into pixel index
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (raw_image[pixel_ix] << 16) + (raw_image[pixel_ix + 1] << 8) + raw_image[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix+=3;
        pixels_left--;
    }

    return EIDSP_OK;
}
