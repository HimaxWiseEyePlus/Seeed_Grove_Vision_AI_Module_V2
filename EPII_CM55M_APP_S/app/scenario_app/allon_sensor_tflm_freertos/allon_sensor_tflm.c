#include "allon_sensor_tflm.h"
#include "xprintf.h"
#include "WE2_debug.h"
#include "hx_drv_scu.h"
#include "hx_drv_swreg_aon.h"
#include "driver_interface.h"
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
#ifdef IP_edm
#include "hx_drv_edm.h"
#endif
#ifdef IP_gpio
#include "hx_drv_gpio.h"
#endif
#ifdef IP_swreg_aon
#include "hx_drv_swreg_aon.h"
#endif
#include "hx_drv_pmu.h"
#include "powermode.h"
#ifdef FREERTOS
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#endif
#include "hx_drv_CIS_common.h"
#ifdef TRUSTZONE_SEC
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
#ifdef NSC
#include "veneer_table.h"
#endif
/* Trustzone config. */

#ifndef TRUSTZONE_SEC_ONLY
/* FreeRTOS includes. */
#include "secure_port_macros.h"
#endif
#endif

#include "app_msg.h"
#include "app_state.h"
#include "dp_task.h"
#include "comm_task.h"
#include "algo_task.h"
#include "cvapp.h"
#include "sleep_mode.h"
#include "pinmux_cfg.h"

#define CIS_XSHUT_SGPIO0
#ifdef CIS_XSHUT_SGPIO0
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#else
#define DEAULT_XHSUTDOWN_PIN    AON_GPIO2
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define dp_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define comm_task_PRIORITY	(configMAX_PRIORITIES - 1)
#define main_task_PRIORITY	(configMAX_PRIORITIES - 2)
#define algo_task_PRIORITY	(configMAX_PRIORITIES - 3)

#define DP_TASK_QUEUE_LEN   		10
#define COMM_TASK_QUEUE_LEN   		10
#define MAIN_TASK_QUEUE_LEN   		10
#define ALGO_TASK_QUEUE_LEN   		10
#define VAD_BUFF_SIZE  				2048
#define ENTER_SLEEP_MODE			1		// 0 : always on, 1 : enter Sleep mode
#define SENSOR_AE_STABLE_CNT		10
#define ENTER_PMU_MODE_FRAME_CNT	1

volatile APP_MAIN_TASK_STATE_E g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
volatile APP_ALGO_TASK_STATE_E g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
volatile APP_DP_TASK_STATE_E g_dptask_state = APP_DP_TASK_STATE_UNINIT;
volatile APP_COMM_TASK_STATE_E g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

QueueHandle_t     xMainTaskQueue;
QueueHandle_t     xDPTaskQueue;
QueueHandle_t     xCommTaskQueue;
QueueHandle_t     xAlgoTaskQueue;

uint32_t g_algo_done_frame = 0;
uint32_t g_enter_pmu_frame_cnt = 0;

extern void app_start_state(APP_STATE_E state);

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void main_task(void *pvParameters);
void pinmux_init();


/*******************************************************************************
 * Code
 ******************************************************************************/
void pinmux_init()
{
	SCU_PINMUX_CFG_T pinmux_cfg;

	hx_drv_scu_get_all_pinmux_cfg(&pinmux_cfg);

	/* Init UART0 pin mux to PB0 and PB1 */
	uart0_pinmux_cfg(&pinmux_cfg);

	/* Init AON_GPIO1 pin mux to PA1 for OV5647 enable pin */
	aon_gpio1_pinmux_cfg(&pinmux_cfg);

	/* Init I2C slave 0 pin mux to PA2, PA3 (SCL, SDA)*/
	i2cs0_pinmux_cfg(&pinmux_cfg);

	/* Init SPI master pin mux */
	spi_m_pinmux_cfg(&pinmux_cfg);

	/* Init Arm SWD interface pin mux to PB6, PB7, PB8 (nR, CLK, DIO)*/
	//swd_pinmux_cfg(&pinmux_cfg);

	hx_drv_scu_set_all_pinmux_cfg(&pinmux_cfg, 1);
}


/*!
 * @brief Main function
 */
int app_main(void)
{
	pinmux_init();

	dbg_printf(DBG_LESS_INFO, "freertos rtos_app\r\n");

	g_maintask_state = APP_MAIN_TASK_STATE_UNINIT;
	g_algotask_state = APP_ALGO_TASK_STATE_UNINIT;
	g_dptask_state = APP_DP_TASK_STATE_UNINIT;
	g_commtask_state = APP_COMM_TASK_STATE_UNINIT;

	xDPTaskQueue  = xQueueCreate( DP_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xDPTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xDPTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}
	xCommTaskQueue  = xQueueCreate( COMM_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xCommTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xCommTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

	xMainTaskQueue  = xQueueCreate( MAIN_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xMainTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xMainTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

	xAlgoTaskQueue  = xQueueCreate( ALGO_TASK_QUEUE_LEN  , sizeof(APP_MSG_T) );
	if(xAlgoTaskQueue == 0)
	{
        dbg_printf(DBG_LESS_INFO, "xAlgoTaskQueue creation failed!.\r\n");
        while (1)
            ;
	}

    if (xTaskCreate(dp_task, "DP_task", 512, NULL, dp_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "dp_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(comm_task, "Comm_task", 512, NULL, comm_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "comm_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(main_task, "Main_task", 512, NULL, main_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "main_task creation failed!.\r\n");
        while (1)
            ;
    }

    if (xTaskCreate(algo_task, "Algo_task", 512, NULL, algo_task_PRIORITY, NULL) !=
        pdPASS)
    {
        dbg_printf(DBG_LESS_INFO, "algo_task creation failed!.\r\n");
        while (1)
            ;
    }

	dbg_printf(DBG_LESS_INFO, "start scheduler\r\n");

    vTaskStartScheduler();

    for (;;)
        ;

    return 0;
}


void main_task(void *pvParameters)
{
	APP_MSG_T main_recv_msg;
	APP_MSG_T algo_send_msg;
	APP_MSG_T dp_send_msg;
    uint8_t main_motion_detect = 0;
    uint8_t main_waitstart_cap = 0;
    uint8_t gpioValue;
	uint32_t wakeup_event;
	uint32_t wakeup_event1;

	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT, &wakeup_event);
	hx_drv_pmu_get_ctrl(PMU_pmu_wakeup_EVT1, &wakeup_event1);
    xprintf("wakeup_event=0x%x,WakeupEvt1=0x%x\n", wakeup_event, wakeup_event1);

#if (FLASH_XIP_MODEL == 1)
    hx_lib_spi_eeprom_open(USE_DW_SPI_MST_Q);
    hx_lib_spi_eeprom_enable_XIP(USE_DW_SPI_MST_Q, true, FLASH_QUAD, true);
#endif

    if(cv_init(true, true)<0) {
    	xprintf("cv init fail\n");
        while (1)
            ;
    }

	g_maintask_state = APP_MAIN_TASK_STATE_INIT;

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
	{
		/*Cold Boot*/
		xprintf("### Cold Boot ###\n");
		g_enter_pmu_frame_cnt = SENSOR_AE_STABLE_CNT;
    	app_start_state(APP_STATE_ALLON);
	}
	else
	{
		/*Warm Boot*/
		xprintf("### Warm Boot ###\n");
		g_enter_pmu_frame_cnt = ENTER_PMU_MODE_FRAME_CNT;
		xprintf("drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_CPU)\n");
		drv_interface_set_mipi_ctrl(SCU_MIPI_CTRL_CPU);
        sensordplib_csirx_disable();
    	app_start_state(APP_STATE_RESTART);
	}

#ifdef SUPPORT_DUAL_CORE
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_RESET);
	hx_drv_scu_set_cm55s_state(SCU_CM55S_STATE_NORMAL);
	hx_drv_scu_set_CM55S_CPUWAIT(SCU_CM55_CPU_RUN);
#endif
    for (;;)
    {
    	if (xQueueReceive ( xMainTaskQueue , &(main_recv_msg) , __QueueRecvTicksToWait ) == pdTRUE )
    	{
    	   	dbg_printf(DBG_LESS_INFO, "main_recv_msg=0x%x\r\n", main_recv_msg.msg_event);
    	   	switch(main_recv_msg.msg_event)
    	   	{
    	   	case APP_MSG_MAINEVENT_CAP_FRAME_DONE:
    	   		algo_send_msg.msg_data = 0;
    	   		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_START_ALGO;
    	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_SENSOR_TIMER:
    	   		break;
    	   	case APP_MSG_MAINEVENT_DP_ERROR:
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;
    	   	case APP_MSG_MAINEVENT_SENSOR_DP_ERROR:
    	   		g_dptask_state = APP_DP_TASK_STATE_ERROR;
    	   	    dbg_printf(DBG_LESS_INFO, "main_recv_msg.msg_data=0x%x fail\r\n", main_recv_msg.msg_data);
#ifdef TODO
    	   	    app_dump_dplib_edminfo();
    	   		main_motion_detect = 0;
    	   	    app_setup_dplib();
#endif
    	   	    break;
    	   	case APP_MSG_MAINEVENT_START_CAPTURE:
	   			dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_START_CAPTURE\n");
	   			main_waitstart_cap = 0;
	   			g_dptask_state = APP_DP_TASK_STATE_SETUP_CAP_END;
    	   		break;
    	   	case APP_MSG_MAINEVENT_STOP_CAPTURE:
	   			dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_STOP_CAPTURE main_waitstart_cap=%d\n", main_waitstart_cap);
    	   		xQueueReset(xDPTaskQueue);
    	   		g_dptask_state = APP_DP_TASK_STATE_STOP_CAP_END;
    	   		if(main_waitstart_cap == 1)
    	   		{
        	   		if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
        	   		{
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
            	   		{
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}else{
        	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_visionalgotask_state=0x%x\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_MOTION_DETECT:
    	   		main_motion_detect = 1;
    	   		break;
    	   	case APP_MSG_MAINEVENT_AON_GPIO0_INT:
    	   	    hx_drv_gpio_get_in_value(AON_GPIO0, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_AON_GPIO0_INT %d AON_GPIO0=%d\n", main_recv_msg.msg_data, gpioValue);
#if 0
    	   	    if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END))
    	   	    {
    	   	    	dbg_printf(DBG_LESS_INFO, "g_dptask_state=0x%x no send STOPCAPTURE\r\n", g_dptask_state);
    	   	    }else{
        	   		dp_send_msg.msg_data = 0;
        	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STOPCAPTURE;
        	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
        	   		}
    	   	    }
    	   	    if((g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO) || (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
    	   	    {
    	   	    	dbg_printf(DBG_LESS_INFO, "g_algotask_state=0x%x no send STOP_ALGO\r\n", g_algotask_state);
    	   	    }else{
        	   		algo_send_msg.msg_data = 0;
        	   		algo_send_msg.msg_event = APP_MSG_VISIONALGOEVENT_STOP_ALGO;
        	   		if(xQueueSend( xAlgoTaskQueue , (void *) &algo_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send algo_send_msg=0x%x fail\r\n", algo_send_msg.msg_event);
        	   		}
    	   	    }
#endif
    	   		break;
    	   	case APP_MSG_MAINEVENT_AON_GPIO1_INT:
    	   	    hx_drv_gpio_get_in_value(AON_GPIO1, &gpioValue);
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_AON_GPIO1_INT %d gpioValue=%d\n", main_recv_msg.msg_data, gpioValue);
#if 0
    	   		if(((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)) || \
    	   				((g_dptask_state == APP_DP_TASK_STATE_ERROR) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE)))
    	   		{
    	   			main_waitstart_cap = 0;
        	   		dp_send_msg.msg_data = 0;
        	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
        	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
        	   		{
        	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
        	   		}
    	   		}else{
    	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_algotask_state=0x%x\n", g_dptask_state, g_algotask_state);
    	   			if((g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_START) || (g_dptask_state == APP_DP_TASK_STATE_SETUP_CAP_END))
    	   			{
    	   				dbg_printf(DBG_LESS_INFO, "already in start cap g_dptask_state=%d\r\n", g_dptask_state);
    	   			}else{
        	   			main_waitstart_cap = 1;
    	   			}
    	   		}
#endif
    	   		break;
    	   	case APP_MSG_MAINEVENT_SB_GPIO0_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO0_INT\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_SB_GPIO1_INT:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_EVENT_MAIN_SB_GPIO1_INT\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_VISIONALGO_STARTDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VISIONALGO_STARTDONE\n");
    	   		g_algotask_state = APP_ALGO_TASK_STATE_DOALGO_DONE;
				g_algo_done_frame++;
				dbg_printf(DBG_LESS_INFO, "g_algo_done_frame = %d\n", g_algo_done_frame);
				#if ( ENTER_SLEEP_MODE == 1 )
				if ( g_algo_done_frame == g_enter_pmu_frame_cnt )
				{
					app_start_state(APP_STATE_STOP);
					dbg_printf(DBG_LESS_INFO, "\nEnter Sleep 1000ms\n");
					app_pmu_enter_sleep(1000, 0xFF, 0);	// 1 second or AON_GPIO0 wake up, memory no retention
				}
				#endif	// ENTER_SLEEP_MODE
    	   		dp_send_msg.msg_data = 0;
    	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_RECAPTURE;
    	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
    	   		{
    	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_VISIONALGO_STOPDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VISIONALGO_STOPDONE\n");
    	   		g_algotask_state = APP_ALGO_TASK_STATE_STOPALGO_DONE;

    	   		if(main_waitstart_cap == 1)
    	   		{
        	   		if((g_dptask_state == APP_DP_TASK_STATE_STOP_CAP_END) && (g_algotask_state == APP_ALGO_TASK_STATE_STOPALGO_DONE))
        	   		{
        	   			main_waitstart_cap = 0;
            	   		dp_send_msg.msg_data = 0;
            	   		dp_send_msg.msg_event = APP_MSG_DPEVENT_STARTCAPTURE;
            	   		if(xQueueSend( xDPTaskQueue , (void *) &dp_send_msg , __QueueSendTicksToWait) != pdTRUE)
            	   		{
            	    	   	dbg_printf(DBG_LESS_INFO, "send dp_send_msg=0x%x fail\r\n", dp_send_msg.msg_event);
            	   		}
        	   		}else{
        	   			dbg_printf(DBG_LESS_INFO, "wait start g_dptask_state=x%x, g_algotask_state=0x%x\n", g_dptask_state, g_algotask_state);
        	   		}
    	   		}
    	   		break;
    	   	case APP_MSG_MAINEVENT_I2CCOMM:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_I2CCOMM\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STARTDONE\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_AUDIOALGO_STOPDONE\n");
    	   		break;
    	   	case APP_MSG_MAINEVENT_VADBUF1_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF1_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;
    	   	case APP_MSG_MAINEVENT_VADBUF2_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_VADBUF2_NOTIFY size=0x%x\n", main_recv_msg.msg_data);
    	   		break;
    	   	case APP_MSG_MAINEVENT_CM55SRDY_NOTIFY:
    	   		dbg_printf(DBG_LESS_INFO, "APP_MSG_MAINEVENT_CM55SRDY_NOTIFY\n");
    	   		break;
    	   	default:
    	   		break;
    	   	}

    	}
    }
}
