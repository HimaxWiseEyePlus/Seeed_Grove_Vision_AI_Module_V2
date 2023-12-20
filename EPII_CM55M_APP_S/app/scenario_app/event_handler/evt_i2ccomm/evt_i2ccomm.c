/**
 ********************************************************************************************
 *  @file      evt_i2ccomm.c
 *  @details   This file contains all i2c communication event related function
 *  @author    himax/902452
 *  @version   V1.0.0
 *  @date      14-July-2019
 *  @copyright (C) COPYRIGHT, Himax, Inc. ALL RIGHTS RESERVED
 *******************************************************************************************/

/********************************************************************************************
 *  History :
 *  2019.07.17 - Initial version
 *  2019.08.02 - issue fixed : refer to i2c_comm.c issue (1)
 *
 *******************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "event_handler_evt.h"
#include "hxevent.h"
#include "i2c_comm.h"
#include "evt_i2ccomm.h"

#include "WE2_core.h"
#include "xprintf.h"
#include "WE2_debug.h"

#include "hx_drv_swreg_aon_export.h"
#include "hx_drv_pmu_export.h"

#include "powermode.h"

#define DBG_EVT_IICS_CMD_LOG             (1)
#define DBG_EVT_IICS_CALLBACK_LOG        (0)

#if DBG_EVT_IICS_CMD_LOG
    #define dbg_evt_iics_cmd(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cmd(fmt, ...)
#endif

#if DBG_EVT_IICS_CALLBACK_LOG
    #define dbg_evt_iics_cb(fmt, ...)   xprintf(fmt, ##__VA_ARGS__)
#else
    #define dbg_evt_iics_cb(fmt, ...)
#endif

/****************************************************
 * Constant Definition                              *
 ***************************************************/
#define DATA_SFT_OFFSET_0           0
#define DATA_SFT_OFFSET_8           8
#define DATA_SFT_OFFSET_16          16
#define DATA_SFT_OFFSET_24          24

#define EVT_I2CS_0_SLV_ADDR         0x62
#define EVT_I2CS_1_SLV_ADDR         0x64

/***************************************************
 * Function Declaration
 **************************************************/
typedef void (*funcptr_void)(void);

void setTimer0AlarmPMU();
void setPS_PDNoVid();
/***************************************************
 * Static Function Declaration
 **************************************************/
static void i2cs_cb_tx(void *param);
static void i2cs_cb_rx(void *param);
static void i2cs_cb_err(void *param);

static void evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_E iic_id);
static i2ccomm_customer i2ccomm_cmd_customer_process[DW_IIC_S_NUM] = {NULL, NULL};

/****************************************************
 * Variable Declaration                             *
 ***************************************************/
extern hx_event_t g_event[];

unsigned char gWrite_buf[DW_IIC_S_NUM][I2CCOMM_MAX_WBUF_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);
unsigned char gRead_buf[DW_IIC_S_NUM][I2CCOMM_MAX_RBUF_SIZE] __ALIGNED(__SCB_DCACHE_LINE_SIZE);

I2CCOMM_CFG_T gI2CCOMM_cfg[DW_IIC_S_NUM] = {
    {   
        EVT_I2CS_0_SLV_ADDR,
        i2cs_cb_tx,
        i2cs_cb_rx,
        i2cs_cb_err},
    {   
        EVT_I2CS_1_SLV_ADDR,
        i2cs_cb_tx,
        i2cs_cb_rx,
        i2cs_cb_err},
};

/***************************************************
 * Function Implementation
 **************************************************/
static void prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_E iic_id)
{
    memset((void *)&gRead_buf[iic_id], 0xFF, 4);
    // hx_CleanDCache_by_Addr((volatile void *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
}

EVT_IICCOM_ERR_E evt_i2ccomm_init(USE_DW_IIC_SLV_E iic_id)
{
    I2CCOMM_STATUS_E status = I2CCOMM_STATUS_UNKNOWN;

    hx_lib_i2ccomm_init(iic_id, gI2CCOMM_cfg[iic_id]);
    hx_lib_i2ccomm_status(iic_id, &status);

    if (status == I2CCOMM_STATUS_OPEN){
        return EVT_IICCOMM_ERR_DRVFAIL;
    }

    prv_evt_i2ccomm_clear_read_buf_header(iic_id);
    hx_lib_i2ccomm_start(iic_id, (unsigned char *)&gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);

    return EVT_IICCOMM_NO_ERROR;
}

void i2ccomm_cmd_customer_register_cb(USE_DW_IIC_SLV_E iic_id, i2ccomm_customer cb_func)
{
    i2ccomm_cmd_customer_process[iic_id] = cb_func;
}

static void i2cs_cb_tx(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_0_TX]);
    }else if(iic_info_ptr->slv_addr == EVT_I2CS_1_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:1) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_1_TX]);
    }
}

static void i2cs_cb_rx(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_0_RX]);
    }else if(iic_info_ptr->slv_addr == EVT_I2CS_1_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:1) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_1_RX]);
    }
}

static void i2cs_cb_err(void *param)
{
    HX_DRV_DEV_IIC *iic_obj = param;
    HX_DRV_DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

    if(iic_info_ptr->slv_addr == EVT_I2CS_0_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:0) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_0_ERR]);
    }else if(iic_info_ptr->slv_addr == EVT_I2CS_1_SLV_ADDR){
        dbg_evt_iics_cb("%s(iic_id:1) \n", __FUNCTION__);
        hx_event_activate_ISR(g_event[EVT_INDEX_I2CS_1_ERR]);
    }
}

uint8_t evt_i2ccomm_0_tx_cb(void)
{
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_i2ccomm_0_err_cb(void)
{
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_i2ccomm_0_rx_cb(void)
{
    unsigned char feature = gRead_buf[USE_DW_IIC_SLV_0][I2CFMT_FEATURE_OFFSET];
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s(feature:0x%02x) \n", __FUNCTION__, feature);
    funcptr_void ResetHandler;
    
    switch (feature)
    {
        case I2CCOMM_FEATURE_SYS:
            evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_0);
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
            break;

        case I2CCOMM_FEATURE_OTA_RESERVED:
            /* jump to 2ndloader */
            dbg_evt_iics_cmd("Into 2ndloader upgrade:\n");
            hx_drv_swreg_aon_set_ota_flag(SWREG_AON_OTA_YES_FLAG);
            setPS_PDNoVid();
            break;

        case I2CCOMM_FEATURE_CUSTOMER_MIN ... I2CCOMM_FEATURE_CUSTOMER_MAX:
            i2ccomm_cmd_customer_process[USE_DW_IIC_SLV_0]();
            break;
        
        case I2CCOMM_FEATURE_MAX:
            break;

        default:
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_0);
            hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_0, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_0], I2CCOMM_MAX_RBUF_SIZE);
            break;
    }

    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_i2ccomm_1_tx_cb(void)
{
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_1, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_1], I2CCOMM_MAX_RBUF_SIZE);
    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_i2ccomm_1_err_cb(void)
{
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s \n", __FUNCTION__);
    prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
    hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_1, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_1], I2CCOMM_MAX_RBUF_SIZE);
    return HX_EVENT_RETURN_DONE;
}

uint8_t evt_i2ccomm_1_rx_cb(void)
{
    unsigned char feature = gRead_buf[USE_DW_IIC_SLV_1][I2CFMT_FEATURE_OFFSET];
    dbg_evt_iics_cmd("\n");
    dbg_evt_iics_cmd("%s(feature:0x%02x) \n", __FUNCTION__, feature);

    switch (feature)
    {
        case I2CCOMM_FEATURE_SYS:
            evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_1);
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
            break;

        case I2CCOMM_FEATURE_CUSTOMER_MIN ... I2CCOMM_FEATURE_CUSTOMER_MAX:
            i2ccomm_cmd_customer_process[USE_DW_IIC_SLV_1]();
            break;

        case I2CCOMM_FEATURE_MAX:
            break;

        default:
            /* undefine feature code */
            prv_evt_i2ccomm_clear_read_buf_header(USE_DW_IIC_SLV_1);
            hx_lib_i2ccomm_enable_read(USE_DW_IIC_SLV_1, (unsigned char *) &gRead_buf[USE_DW_IIC_SLV_1], I2CCOMM_MAX_RBUF_SIZE);
            break;
    }

    return HX_EVENT_RETURN_DONE;
}

// Command process for FEATURE:SYSTEM_INFO
static void evt_i2cs_cmd_process_sysinfo(USE_DW_IIC_SLV_E iic_id)
{
    int retval = 0;
    unsigned int data;
    unsigned char cmd;
    unsigned short checksum;

    retval = hx_lib_i2ccomm_validate_checksum((unsigned char *) &gRead_buf[iic_id]);
    if (retval != I2CCOMM_NO_ERROR)
    {
        dbg_evt_iics_cmd("%s - checksum validation : FAIL\n", __FUNCTION__);
        prv_evt_i2ccomm_clear_read_buf_header(iic_id);
        hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
        return;
    }

    cmd = gRead_buf[iic_id][I2CFMT_COMMAND_OFFSET];
    dbg_evt_iics_cmd("%s(iic_id:%d, cmd:0x%02x) \n", __FUNCTION__, iic_id, cmd);

    switch (cmd)
    {
        case I2CCOMM_CMD_SYS_GET_VER_BSP:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_BSP;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP >> 8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP & 0xFF;

            data = 0xA1B2C3D4;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;
            
            retval = hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP, &checksum);

            if (retval == I2CCOMM_NO_ERROR)
            {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else
            {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_BSP + 1] = 0xFF;
            }

            // hx_CleanDCache_by_Addr((volatile void *) &gWrite_buf[iic_id], I2CCOMM_MAX_WBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        case I2CCOMM_CMD_SYS_GET_VER_I2C:
            // prepare write buffer for write process
            gWrite_buf[iic_id][I2CFMT_FEATURE_OFFSET] = gRead_buf[iic_id][I2CFMT_FEATURE_OFFSET];
            gWrite_buf[iic_id][I2CFMT_COMMAND_OFFSET] = I2CCOMM_CMD_SYS_GET_VER_I2C;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_MSB_OFFSET] = (I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C >> 8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOADLEN_LSB_OFFSET] = I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C & 0xFF;

            data = hx_lib_i2ccomm_version();
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET] = data & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 1] = (data >> DATA_SFT_OFFSET_8) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 2] = (data >> DATA_SFT_OFFSET_16) & 0xFF;
            gWrite_buf[iic_id][I2CFMT_PAYLOAD_OFFSET + 3] = (data >> DATA_SFT_OFFSET_24) & 0xFF;
            retval = hx_lib_i2ccomm_generate_checksum((unsigned char *) &gWrite_buf[iic_id], I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C, &checksum);
            
            if (retval == I2CCOMM_NO_ERROR)
            {
                dbg_evt_iics_cmd("checksum generation : 0x%04x \n", checksum);
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C] = checksum & 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C + 1] = (checksum >> DATA_SFT_OFFSET_8) & 0xFF;
            }
            else
            {
                dbg_evt_iics_cmd("[Warning] i2c cmd - checksum generation : FAIL\n");
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C] = 0xFF;
                gWrite_buf[iic_id][I2CCOMM_HEADER_SIZE + I2CCOMM_SYS_CMD_PAYLOAD_VER_I2C + 1] = 0xFF;
            }

            // hx_CleanDCache_by_Addr((volatile void *) &gWrite_buf[iic_id], I2CCOMM_MAX_WBUF_SIZE);
            hx_lib_i2ccomm_enable_write(iic_id, (unsigned char *) &gWrite_buf[iic_id]);
            break;

        default:
            /* only get command without write data or information to external I2C master 
               or
               undefine command */
            prv_evt_i2ccomm_clear_read_buf_header(iic_id);
            hx_lib_i2ccomm_enable_read(iic_id, (unsigned char *) &gRead_buf[iic_id], I2CCOMM_MAX_RBUF_SIZE);
            break;
    }
}

void setTimer0AlarmPMU() {
//
	TIMER_ID_E id;
	TIMER_ERROR_E ret;
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = 1;

	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_PMU;
	timer_cfg.state = TIMER_STATE_PMU;
	id = TIMER_ID_0;

	ret = hx_drv_timer_hw_start(id, &timer_cfg, NULL);

}

void setPS_PDNoVid()
{
	PM_PD_NOVIDPRE_CFG_T cfg;
	uint32_t freq;
	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
	PM_CFG_PWR_MODE_E mode;
	SCU_PLL_FREQ_E pmuwakeup_pll_freq;
	SCU_HSCCLKDIV_E pmuwakeup_cm55m_div;
	SCU_LSCCLKDIV_E pmuwakeup_cm55s_div;
	SCU_PLL_CLK_CFG_T pll_cfg;

	/*Get System Current Clock*/
	hx_drv_swreg_aon_get_pmuwakeup_freq(&pmuwakeup_pll_freq, &pmuwakeup_cm55m_div, &pmuwakeup_cm55s_div);
	hx_drv_swreg_aon_get_pllfreq(&freq);
	dbg_printf(DBG_MORE_INFO,"pmuwakeup_freq_type=%d, pmuwakeup_cm55m_div=%d, pmuwakeup_cm55s_div=%d\n", 
                              pmuwakeup_pll_freq, pmuwakeup_cm55m_div, pmuwakeup_cm55s_div);
	dbg_printf(DBG_MORE_INFO,"pmuwakeup_run_freq=%d\n", freq);

#ifdef SUPPORT_PRECAP
	/*Setup Sensor and Datapath*/
	app_setup_dplib();
#endif

	mode = PM_MODE_PS_NOVID_PREROLLING;
	hx_lib_pm_get_defcfg_bymode(&cfg, mode);

	/*Setup bootrom clock speed when PMU Warm boot wakeup*/
	cfg.bootromspeed.bootromclkfreq = pmuwakeup_pll_freq;
	cfg.bootromspeed.pll_freq = freq;
	cfg.bootromspeed.cm55m_div = pmuwakeup_cm55m_div;
	cfg.bootromspeed.cm55s_div = pmuwakeup_cm55s_div;

	/*Setup CM55 Small can be reset*/
	cfg.cm55s_reset = SWREG_AON_PMUWAKE_CM55S_RERESET_YES;
	/*UnMask PA01 Interrupt for PMU*/
	cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;
	/*UnMask RTC Interrupt for PMU*/
	cfg.pmu_rtc_mask = PM_RTC_INT_MASK_ALLUNMASK;
	/*UnMask PA23 Interrupt for PMU*/
	cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK_ALL_UNMASK;
	/*Mask I2CWakeup Interrupt for PMU*/
	cfg.pmu_i2cw_mask = PM_IP_INT_MASK;
	/*UnMask Timer0 Interrupt others timer interrupt are mask for PMU*/
	cfg.pmu_timer_mask = 0x1FE;
	/*Mask CMP Interrupt for PMU*/
	cfg.pmu_cmp_mask = PM_IP_INT_MASK;
	/*Mask TS Interrupt for PMU*/
	cfg.pmu_ts_mask = PM_IP_INT_MASK;
	/*Mask ANTI TAMPER Interrupt for PMU*/
	cfg.pmu_anti_mask = PM_IP_INT_MASK;
	/*No Debug Dump message*/
	cfg.support_debugdump = 0;
#ifdef SUPPORT_PRECAP
#if defined(CIS_HM0360) || defined(CIS_HM11B1)
	/**< HW pin control to capture frame **/
	cfg.nframeend_ctrl = PMU_NFRAMEEND_CTRL_SC;
#endif
#if defined(CIS_HM2170)
	/**< Auto I2C control to capture frame **/
	cfg.nframeend_ctrl = PMU_NFRAMEEND_CTRL_I2C;
#endif
#endif
#ifdef SUPPORT_MEM_RETENTION
	/*Setup Memory retention*/
	cfg.tcm_retention = PM_MEM_RET_YES;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = PM_MEM_RET_YES;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = SWREG_AON_RETENTION;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = SWREG_AON_RETENTION; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_RETENTION; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_RETENTION; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_RETENTION; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_RETENTION; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
	cfg.skip_bootflow.secure_debug_flag = SWREG_AON_RETENTION;
#else
	/*Setup Memory no retention*/
	cfg.tcm_retention = PM_MEM_RET_NO;			/**< CM55M TCM Retention**/
	cfg.hscsram_retention[0] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[1] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[2] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.hscsram_retention[3] = PM_MEM_RET_NO;	/**< HSC SRAM Retention**/
	cfg.lscsram_retention = PM_MEM_RET_NO;		/**< LSC SRAM Retention**/
	cfg.skip_bootflow.sec_mem_flag = SWREG_AON_NO_RETENTION;			/**< Skip Boot Flow**/
	cfg.skip_bootflow.first_bl_flag = SWREG_AON_NO_RETENTION; /*!< First BL Retention */
	cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55m_s_app Retention */
	cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55m_ns_app Retention */
	cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_s_app Retention */
	cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_ns_app Retention */
	cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55m model Retention */
	cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55s model Retention */
	cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55m appcfg Retention */
	cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55s appcfg Retention */
	cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
	cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
	cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
	cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
	cfg.skip_bootflow.secure_debug_flag = SWREG_AON_NO_RETENTION;
#endif
#ifdef SUPPORT_PRECAP
	/**Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_YES;
#else
	/**No Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_NO;
#endif
	/*Not DCDC pin output*/
	cfg.pmu_dcdc_outpin = PM_CFG_DCDC_MODE_OFF;
	/** No Pre-capture when boot up**/
	cfg.ioret = PM_CFG_PD_IORET_ON;
#ifdef SUPPORT_PRECAP
#if defined(CIS_HM0360) || defined(CIS_HM11B1)
	/*No MIPI*/
	cfg.mipi_lane_en = PMU_MIPI_LANE_ALL_DISABLE;
#endif
#if defined(CIS_HM2170)
	/*MIPI 2lanes*/
	cfg.mipi_lane_en = PMU_MIPI_LANE_ALL_LANE_EN;
#endif
#endif
	cfg.sensor_type = PM_SENSOR_TIMING_FVLDLVLD_CON;
	/*SIMO on in PD*/
	cfg.simo_pd_onoff = PM_SIMO_PD_ONOFF_ON;

	dbg_printf(DBG_MORE_INFO,"speed=%d,reset=%d,nframeend_ctrl=%d,retention=%d,precap=%d\n", 
                             cfg.bootromspeed.bootromclkfreq, cfg.cm55s_reset, 
                             cfg.nframeend_ctrl,cfg.tcm_retention ,cfg.support_bootwithcap);
	dbg_printf(DBG_MORE_INFO,"pa01_mask=0x%x,rtc_mask=0x%x,pa23_mask=0x%x,i2cw_mask=0x%x,timer_mask=0x%x,cmp_mask=0x%x,ts_mask=0x%x\n",
                             cfg.pmu_pad_pa01_mask,cfg.pmu_rtc_mask,cfg.pmu_pad_pa23_mask,cfg.pmu_i2cw_mask,
                             cfg.pmu_timer_mask,cfg.pmu_cmp_mask,cfg.pmu_ts_mask);
	dbg_printf(DBG_MORE_INFO,"dcdcpin=%d, pmu_anti_mask=0x%x\n", cfg.pmu_dcdc_outpin, cfg.pmu_anti_mask);
	dbg_printf(DBG_MORE_INFO,"freq=%d, cm55mdiv=%d,cm55sdiv=%d\n", 
                             cfg.bootromspeed.pll_freq, cfg.bootromspeed.cm55m_div, cfg.bootromspeed.cm55s_div);

	hx_lib_pm_cfg_set(&cfg, NULL, mode);
	/*Setup Timer0 Wakeup*/
	setTimer0AlarmPMU();

	/*Use PMU lib to control HSC_CLK and LSC_CLK so set thoes parameter to 0*/
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));
	/*Trigger to PMU mode*/
	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}


