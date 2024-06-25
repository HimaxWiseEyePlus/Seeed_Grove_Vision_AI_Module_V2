#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "timed_interrupt_fatfs.h"
#include "powermode_export.h"

#define WE2_CHIP_VERSION_C 0x8538000c
#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif

// #ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
// #ifdef NSC
// #include "veneer_table.h"
// #endif
/* Trustzone config. */

// #ifndef TRUSTZONE_SEC_ONLY
// /* FreeRTOS includes. */
// #include "secure_port_macros.h"
#endif

#include "xprintf.h"
#include "sleep_mode.h"
#include "WE2_debug.h"
#include "spi_fatfs.h"
#include "cisdp_cfg.h"
#include "event_handler.h"
#include "cisdp_sensor.h"
#include "cvapp.h"
#include "common_config.h"
#include "person_detect_model_data_vela.h"
#include "hx_drv_pmu_export.h"
#include "hx_drv_pmu.h"

#include "WE2_device.h"
#include "spi_master_protocol.h"
#include "hx_drv_spi.h"
#include "spi_eeprom_comm.h"
#include "board.h"
#include "WE2_core.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"

#ifdef IP_sensorctrl
#include "hx_drv_sensorctrl.h"
#endif
#ifdef IP_xdma
#include "hx_drv_xdma.h"
#include "sensor_dp_lib.h"
#endif
#ifdef IP_cdm
#include "hx_drv_cdm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#include "hx_drv_pmu_export.h"
#include "hx_drv_pmu.h"
#include "powermode.h"
#include "spi_fatfs.h"
#include "BITOPS.h"

#ifdef EPII_FPGA
#define DBG_APP_LOG (1)
#else
#define DBG_APP_LOG (1)
#endif
#if DBG_APP_LOG
#define dbg_app_log(fmt, ...) xprintf(fmt, ##__VA_ARGS__)
#else
#define dbg_app_log(fmt, ...)
#endif

/* Task priorities. */
#define hello_task1_PRIORITY (configMAX_PRIORITIES - 2)
#define main_task_PRIORITY (configMAX_PRIORITIES - 1)

#define ENTER_SLEEP_MODE 1 // 0: Always on, 1: Enter sleep mode
#define SEND_IMAGE_SPI 0

#define MAX_STRING 100
#define DEBUG_SPIMST_SENDPICS (0x01) // 0x00: off/ 0x01: JPEG/0x02: YUV422/0x03: YUV420/0x04: YUV400/0x05: RGB
#define SPI_SEN_PIC_CLK (10000000)

// To prevent information losses when M55M sleep w/o retentation, we will add needed information in the MB share data
/*volatile*/ uint32_t g_flash_record_start_pos = 0;
/*volatile*/ uint32_t g_flash_image_cur_pos = 0;
/*volatile*/ uint32_t g_flash_length_cur_pos = 0;
/*volatile*/ uint32_t g_idle_time = 0;
/*volatile*/ uint32_t g_img_data = 0;

static uint8_t g_xdma_abnormal, g_md_detect, g_cdm_fifoerror, g_wdt1_timeout, g_wdt2_timeout, g_wdt3_timeout;
static uint8_t g_hxautoi2c_error, g_inp1bitparer_abnormal;
static uint32_t g_dp_event;
static uint8_t g_frame_ready;
static uint32_t g_cur_jpegenc_frame;
static uint8_t g_time;
static uint8_t g_spi_master_initial_status;
/*volatile*/ uint32_t jpeg_addr, jpeg_sz;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static void hello_task1(void *pvParameters);
static void interrupt_task(void *pvParameters);
static void main_task(void *pvParameters); // Added interrupt task prototype
void app_start_state(APP_STATE_E state);
void aon_gpio0_cb(uint8_t group, uint8_t aIndex);
void timed_reset(APP_STATE_E state);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
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

void aon_gpio0_interupt_init()
{
    hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
    hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_EDGE_RISING);
    hx_drv_gpio_cb_register(AON_GPIO0, aon_gpio0_cb);
    hx_drv_gpio_set_input(AON_GPIO0);
    hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
}

static void hello_task1(void *pvParameters)
{
    for (;;)
    {
        printf("Hello world freertos task1.\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

static void interrupt_task(void *pvParameters)
{
    for (;;)
    {
        printf("Interrupt Task.\r\n");
#if (ENTER_SLEEP_MODE == 1)
        app_start_state(APP_STATE_STOP);
        dbg_printf(DBG_LESS_INFO, "\nEnter Sleep 5000ms\n");
        app_pmu_enter_sleep(5000, 0xFF, 1); // 5 seconds or AON_GPIO0 wake up, memory no retention
#endif
    }
}

// Added interrupt task implementation
static void main_task(void *pvParameters)
{
#if SEND_IMAGE_SPI
    hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 1);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 1);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 1);
    hx_drv_scu_set_PB11_pinmux(SCU_PB11_PINMUX_SPI_M_CS, 1);
    if (hx_drv_spi_mst_open_speed(SPI_SEN_PIC_CLK) != 0)
    {
        dbg_printf(DBG_LESS_INFO, "DEBUG SPI master init fail\r\n");
        sensordplib_retrigger_capture();
        return;
    }
#else
    fatfs_init();
#endif

    if (cv_init(true, true) < 0)
    {
        xprintf("cv init fail\n");
        return -1;
    }

    for (;;)
    {
        // Perform interrupt task operations here
        printf("Main Task Running...\r\n");
        app_start_state(APP_STATE_ALLON);
        // vTaskDelay(pdMS_TO_TICKS(5000));
    }
    // Interrupt task completed
    printf("Interrupt task completed.\r\n");
    vTaskDelete(NULL);
}

static void dp_app_cv_eventhdl_cb(EVT_INDEX_E event)
{
    uint16_t err;
    int32_t read_status;
    uint32_t chipid, version;
    char filename[20];

    dbg_printf(DBG_LESS_INFO, "EVT event = %d\n", event);
    g_dp_event = event;

    switch (event)
    {
    case EVT_INDEX_1BITPARSER_ERR: /*reg_inpparser_fs_cycle_error*/
        hx_drv_inp1bitparser_get_errstatus(&err);
        dbg_printf(DBG_LESS_INFO, "EVT_INDEX_1BITPARSER_ERR err=0x%x\r\n", err);
        hx_drv_inp1bitparser_clear_int();
        hx_drv_inp1bitparser_set_enable(0);
        g_inp1bitparer_abnormal = 1;
        break;
    case EVT_INDEX_EDM_WDT1_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT1_TlenIMEOUT\r\n");
        g_wdt1_timeout = 1;
        break;
    case EVT_INDEX_EDM_WDT2_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT2_TIMEOUT\r\n");
        g_wdt2_timeout = 1;
        break;
    case EVT_INDEX_EDM_WDT3_TIMEOUT:
        dbg_printf(DBG_LESS_INFO, "EVT_INDEX_EDM_WDT3_TIMEOUT\r\n");
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
        dbg_printf(DBG_LESS_INFO, "EVT_INDEX_HXAUTOI2C_ERR\r\n");
        g_hxautoi2c_error = 1;
        break;
    default:
        dbg_printf(DBG_LESS_INFO, "Other Event %d\n", event);
        break;
    }

    if (g_frame_ready == 1)
    {
        g_frame_ready = 0;

        cisdp_get_jpginfo(&jpeg_sz, &jpeg_addr);

#ifdef CIS_IMX
        hx_drv_scu_get_version(&chipid, &version);
        if (chipid == WE2_CHIP_VERSION_C) // mipi workaround for WE2 chip version C
        {
            cisdp_stream_off();
            set_mipi_csirx_disable();
        }
#endif

#if SEND_IMAGE_SPI
#if 1 // send JPG image
        read_status = hx_drv_spi_mst_protocol_write_sp(jpeg_addr, jpeg_sz, DATA_TYPE_JPG);
        dbg_printf(DBG_LESS_INFO, "write frame result %d, data size=%d,addr=0x%x\n", read_status, jpeg_sz, jpeg_addr);
#else // send YUV420 image
        SPI_CMD_DATA_TYPE image_type;
        uint32_t wdam3_addr = app_get_raw_addr();
        uint32_t data_size = app_get_raw_sz();
        uint8_t imagesize_header[4];
        image_type = DATA_TYPE_RAW_HEADER_IMG_YUV420_8U3C;
        imagesize_header[0] = app_get_raw_width() & 0xFF;
        imagesize_header[1] = (app_get_raw_width() >> 8) & 0xFF;
        imagesize_header[2] = app_get_raw_height() & 0xFF;
        imagesize_header[3] = (app_get_raw_height() >> 8) & 0xFF;
        read_status = hx_drv_spi_mst_protocol_write_ex(wdam3_addr, data_size, image_type, imagesize_header, 4);
        dbg_printf(DBG_LESS_INFO, "addr=0x%x, YUV write frame result %d, data size %d\n", wdam3_addr, read_status, data_size);
#endif
#else
        xsprintf(filename, "image%04d.jpg", g_cur_jpegenc_frame);
        dbg_printf(DBG_LESS_INFO, "write frame to %s, data size=%d,addr=0x%x\n", filename, jpeg_sz, jpeg_addr);
        read_status = fastfs_write_image(jpeg_addr, jpeg_sz, filename);
#endif

        cv_run();

#ifdef CIS_IMX
        hx_drv_scu_get_version(&chipid, &version);
        if (chipid == WE2_CHIP_VERSION_C) // mipi workaround for WE2 chip version C
        {
            set_mipi_csirx_enable();
            cisdp_stream_on();
        }
#endif

        // recapture image
        sensordplib_retrigger_capture();
    }

    if (g_md_detect == 1)
    {
        g_md_detect = 0;
    }

    if (g_inp1bitparer_abnormal == 1 || g_wdt1_timeout == 1 || g_wdt2_timeout == 1 || g_wdt3_timeout == 1 || g_cdm_fifoerror == 1 || g_xdma_abnormal == 1 || g_hxautoi2c_error == 1)
    {
        cisdp_sensor_stop();
    }
    if (g_cur_jpegenc_frame == 10)
    {
        interrupt_task(APP_STATE_STOP);
    }
}

void app_start_state(APP_STATE_E state)
{
    printf("app_start_state\n");
    dp_var_int();
    // if wdma variable is zero when not init yet, then this step is a must be to retrieve wdma address
    if (cisdp_dp_init(true, SENSORDPLIB_PATH_INT_INP_HW5X5_JPEG, dp_app_cv_eventhdl_cb, g_img_data, APP_DP_RES_YUV640x480_INP_SUBSAMPLE_1X) < 0)
    {
        xprintf("\r\nDATAPATH Init fail\r\n");
        APP_BLOCK_FUNC();
    }
    if (state == APP_STATE_ALLON)
    {
        if (cisdp_sensor_init(true) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
        else
        {
            printf("event_handler_init\n");
            event_handler_init();
            cisdp_sensor_start();
            xprintf("APP_STATE_START\n");
            event_handler_start();
        }
    }
    else if (state == APP_STATE_RESTART)
    {
        xprintf("APP_STATE_RESTART\n");
        if (cisdp_sensor_init(false) < 0)
        {
            xprintf("\r\nCIS Init fail\r\n");
            APP_BLOCK_FUNC();
        }
        else
        {
            event_handler_init();
            cisdp_sensor_start();
            event_handler_start();
        }
    }
    else if (state == APP_STATE_STOP)
    {
        xprintf("APP_STATE_STOP\n");
        event_handler_stop();
        cisdp_sensor_stop();
    }
}

/*!
 * @brief Main function
 */
int app_main(void)
{
    printf("Task creation Hello world. No MPU\r\n");

    if (xTaskCreate(hello_task1, "Hello_task1", 512, NULL, hello_task1_PRIORITY, NULL) != pdPASS)
    {
        printf("Hello_task1 creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(main_task, "Main_task", 512, NULL, main_task_PRIORITY, NULL) != pdPASS)
    {
        printf("Main_task creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();

    for (;;)
        ;
}