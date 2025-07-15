#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#ifdef TRUSTZONE_SEC
#ifdef FREERTOS
/* Trustzone config. */
//
/* FreeRTOS includes. */
//#include "secure_port_macros.h"
#else
#if (__ARM_FEATURE_CMSE & 1) == 0
#error "Need ARMv8-M security extensions"
#elif (__ARM_FEATURE_CMSE & 2) == 0
#error "Compile with --cmse"
#endif
#include "arm_cmse.h"
//#include "veneer_table.h"
//
#endif
#endif

#include "xprintf.h"
#include "WE2_device.h"
#include "WE2_device_addr.h"
#include "board.h"
#include "hx_drv_scu.h"
#include "hx_drv_sdio.h"

#include "hx_drv_gpio.h"
#include "hx_drv_timer.h"
#include "hx_drv_pmu_export.h"
#include "powermode.h"
#include "hx_drv_swreg_aon.h"

#include "hx_hal_sec.h"
#include "customer_seeed.h"
#include "hx_drv_spi.h"
#include "hx_drv_uart.h"

// CGP - added these to avoid errors
#include "timer_interface.h"
#include "console_io.h"

int sdio_test(void);
void dump_memory(uint32_t addr, uint32_t len);
int uart_test(void);
void pmu_test(void);
void setCM55MTimerAlarmPMU(uint32_t timer_ms);
static void print_wakeup_event(uint32_t event, uint32_t event1);
void security_test(void);
void spi_m_test(void);

/**
 * This function sets TIMER ID 2 or AON_GPIO0(PA0)/AON_GPIO1(PA1) as Power Down mode wake up sources.
 * This function supports memory retention or memory no retention.
 *
 * @param timer_ms The wake up timer(millisecond).
 * @param aon_gpio The index of the AON_GPIO pin to set as wake up source.
 * 				   0: AON_GPIO0(PA0), 1: AON_GPIO1(PA1), 0xFF: disable aon_gpio wake up.
 * @param retention Memory retention or not.
 *
 */
void sleep_mode_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention);


/*!
 * @brief Main function
 */
int app_main(void) {

	xprintf("Start Seeed Sample Application, Build date : %s %s\n", __DATE__, __TIME__);

	hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI2AHB_DO, 1);
	hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI2AHB_DI, 1);
	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI2AHB_SCLK, 1);
	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI2AHB_CS, 1);

    SCU_PAD_PULL_LIST_T pad_pull_cfg;

    hx_drv_scu_get_all_pull_cfg(&pad_pull_cfg);
    pad_pull_cfg.pb2.pull_en = SCU_PAD_PULL_EN;
    pad_pull_cfg.pb2.pull_sel = SCU_PAD_PULL_UP;
    pad_pull_cfg.pb3.pull_en = SCU_PAD_PULL_EN;
    pad_pull_cfg.pb3.pull_sel = SCU_PAD_PULL_UP;
    pad_pull_cfg.pb5.pull_en = SCU_PAD_PULL_EN;
    pad_pull_cfg.pb5.pull_sel = SCU_PAD_PULL_UP;
    hx_drv_scu_set_all_pull_cfg(&pad_pull_cfg);

	SCU_LSCSSPIMSRC_E sspimsrc_clk;
	uint8_t sspim_div;

	hx_drv_scu_get_pdlsc_sspimclk_cfg(&sspimsrc_clk, &sspim_div);
	xprintf("org sspimsrc_clk = %d, sspim_div = %d\n", sspimsrc_clk, sspim_div);

#if 0
	sspimsrc_clk = SCU_LSCSSPISSRC_RC24M1M;
	sspim_div = 0;

	hx_drv_scu_set_pdlsc_sspimclk_cfg(sspimsrc_clk, sspim_div);
	xprintf("new sspimsrc_clk = %d, sspim_div = %d\n", sspimsrc_clk, sspim_div);
#endif

	SCU_QSPICLKSRC_E qspi_src;
	uint8_t qspi_div;

	hx_drv_scu_get_pdhsc_qspisrc_cfg(&qspi_src, &qspi_div);
	xprintf("org qspi_src = %d, qspi_div = %d\n", qspi_src, qspi_div);

	qspi_src = SCU_QSPICLKSRC_RC24M1M;
	qspi_div = 0;

	hx_drv_scu_set_pdhsc_qspisrc_cfg(qspi_src, qspi_div);
	xprintf("new qspi_src = %d, qspi_div = %d\n", qspi_src, qspi_div);

	// CGP uncomment one of these tests, as required
	//uart_test();
	//sdio_test();
	pmu_test();
	//security_test();
	//spi_m_test();

	while(1)
	{
		xprintf(".\n");
		hx_drv_timer_cm55x_delay_ms(1000, TIMER_STATE_DC);
	}

	return 0;
}


#define SWAP_32(v)	   (((v>>24)&0x000000ff) | ((v>>8)&0x0000ff00) | ((v<<8)&0x00ff0000) | ((v<<24)&0xff000000))

void security_test(void)
{
	SecError_t status = SEC_ERROR_FAIL;
	CCHashResult_t SocID;
	OTP_USER_DATA_T user_data;

	xprintf("Start security test(%s) ...\n", __DATE__);

	status = himax_sec_init();
	xprintf("sec_status=%d\n", status);

	status= himax_sec_SOCID(CMD_TYPE_GEN_SOCID,SocID);

	if (status == SEC_ERROR_OK)
	{
		xprintf("SocID = ");
		for (int i = 0; i < 8; i++)
		{
			SocID[i] = SWAP_32(SocID[i]);
			xprintf("%08x", SocID[i]);
		}
		xprintf("\r\n");
	}
	else
	{
		xprintf("status = 0x%08x\n", status);
	}

	xprintf("OTP write/read test ...\n");

#if 0
	user_data.data[0] = 0x12345678;
	user_data.data[1] = 0x5a5a5a5a;
	user_data.data[2] = 0xa5a5a5a5;
	user_data.data[3] = 0x87654321;
	cust_seeed_set_otp_user_data(&user_data);
#endif

	cust_seeed_get_otp_user_data(&user_data);
	xprintf("OTP data0 = 0x%x\r\n", user_data.data[0]);
	xprintf("OTP data1 = 0x%x\r\n", user_data.data[1]);
	xprintf("OTP data2 = 0x%x\r\n", user_data.data[2]);
	xprintf("OTP data3 = 0x%x\r\n", user_data.data[3]);

}


void sspi_callback_fun_tx(void *status)
{
	xprintf("SPI Master0 TX CB_FUN\n");
	return; 
}


void spi_m_test(void)
{
	xprintf("Start SPI Master test ...\r\n");

	//Set SPI_M pinmux(0)
	hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SPI_M_DO_1, 0);
    hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SPI_M_DI_1, 0);
    hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SPI_M_SCLK_1, 0);
    hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SPI_M_CS_1, 0);

	//Set PB2/PB3/PB5 PULL_UP
    SCU_PAD_PULL_LIST_T pad_pull_cfg;
    hx_drv_scu_get_all_pull_cfg(&pad_pull_cfg);
	pad_pull_cfg.pb2.pull_en = SCU_PAD_PULL_EN;
	pad_pull_cfg.pb2.pull_sel = SCU_PAD_PULL_UP;
	pad_pull_cfg.pb3.pull_en = SCU_PAD_PULL_EN;
	pad_pull_cfg.pb3.pull_sel = SCU_PAD_PULL_UP;
	pad_pull_cfg.pb5.pull_en = SCU_PAD_PULL_EN;
	pad_pull_cfg.pb5.pull_sel = SCU_PAD_PULL_UP;
    hx_drv_scu_set_all_pull_cfg(&pad_pull_cfg);

	///initialize SSPIM
	uint32_t max_spi_freq, val32;
	DEV_SPI_PTR dev_spi_m;
	hx_drv_spi_mst_init(USE_DW_SPI_MST_S, DW_SPI_S_RELBASE);
	dev_spi_m = hx_drv_spi_mst_get_dev(USE_DW_SPI_MST_S);
	//get max spi output freq.
	dev_spi_m->spi_control(SPI_CMD_MST_UPDATE_SYSCLK, (SPI_CTRL_PARAM)&max_spi_freq);

	dbg_printf(DBG_LESS_INFO, "max_spi_freq = %d\r\n", max_spi_freq);

	//max_spi_freq = 12000000;	//12M
	max_spi_freq = 40000000;	//40M
	dev_spi_m->spi_open(DEV_MASTER_MODE, max_spi_freq);
	///get SPI output clock at current
	dev_spi_m->spi_control(SPI_CMD_MST_GET_CURRENT_FREQ, (SPI_CTRL_PARAM)&val32);
		dbg_printf(DBG_LESS_INFO, "set spi_freq = %d\r\n", val32);
 
	dbg_printf(DBG_LESS_INFO, "SPI master init done\r\n");

	int data_size = 256;
	unsigned char m_tx_buf[256] __ALIGNED(__SCB_DCACHE_LINE_SIZE);

	dev_spi_m->spi_control(SPI_CMD_SET_TXCB, (SPI_CTRL_PARAM)sspi_callback_fun_tx);

	for (int data_count = 0; data_count < data_size; data_count++)
		m_tx_buf[data_count] = data_count;

	while( 1 )
	{
		dbg_printf(DBG_LESS_INFO, "Press any key to start SPI master TX\r\n");
		console_getchar();
		dev_spi_m->spi_write(m_tx_buf, data_size);
	}

}


#define I2C_SLAVE_IF_BUFF_SIZE	1024

int sdio_test()
{
	uint32_t sdmem_addr = 0;
	uint32_t size = I2C_SLAVE_IF_BUFF_SIZE;
	uint32_t mem_rbuffer[I2C_SLAVE_IF_BUFF_SIZE];
	uint32_t mem_wbuffer[I2C_SLAVE_IF_BUFF_SIZE];

	for ( int i = 0; i < size/4; i++ )
	{
		mem_wbuffer[i] = i;
	}

	xprintf("dump mem_rbuffer[] before read SD card :\n");
	dump_memory((uint32_t)mem_rbuffer, size/4);

	// select SDIO pin mux
	hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_SDIO_CMD, 1);
	hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_SDIO_SDCLK, 1);
	hx_drv_scu_set_PB2_pinmux(SCU_PB2_PINMUX_SDIO_DAT1, 1);
	hx_drv_scu_set_PB3_pinmux(SCU_PB3_PINMUX_SDIO_DAT0, 1);
	hx_drv_scu_set_PB4_pinmux(SCU_PB4_PINMUX_SDIO_DAT2, 1);
	hx_drv_scu_set_PB5_pinmux(SCU_PB5_PINMUX_SDIO_DAT3, 1);

	if ( hx_drv_sdio_init(25000, 4, SD_REG_BASE)!=SDIO_PASS ) {
		xprintf("sdio init fail\n");
		return -1;
	}

	//sdmem_addr and size should align to 512
	if ( hx_drv_sdio_mem_write(sdmem_addr, size, (unsigned char *)(mem_wbuffer))!=SDIO_PASS ) {
		xprintf("write fail\n");
		return -1;
	}

	//sdmem_addr and size should align to 512
	if ( hx_drv_sdio_mem_read(sdmem_addr, size, (unsigned char *)(mem_rbuffer))!=SDIO_PASS ) {
		xprintf("read fail\n");
		return -1;
	}

	xprintf("dump mem_rbuffer[] after write SD card :\n");
	dump_memory((uint32_t)mem_rbuffer, size/4);

	return 0;
}


void dump_memory(uint32_t addr, uint32_t len)
{
	int i;

	addr &= (~0x3);

	for (i = 0; i < len; i++) {
		if (i % 4 == 0)
			xprintf("\n%08x:", addr);
		xprintf(" %08x", *((volatile unsigned int*) addr));
		addr += 4;
	}
	xprintf("\n");
}


static void uart1_dma_tx_cb(void)
{
    /** Disable DMA request **/
    xprintf("\nUART1 DMA Transfer Done\n");
}

#define UART_BUFF_SIZE	1024*256
static volatile uint8_t *uart_wbuffer = (uint8_t *)SRAM2_BASE;

int uart_test()
{
	DEV_UART_PTR dev_uart_ptr;
	// CGP - unused variable uint32_t sdmem_addr = 0;
	uint32_t size = UART_BUFF_SIZE;
	//uint8_t mem_wbuffer[UART_BUFF_SIZE];
	uint32_t ascii = 0x21;

	//Set UART1 pinmux
	hx_drv_scu_set_PB6_pinmux(SCU_PB6_PINMUX_UART1_RX, 0);
    hx_drv_scu_set_PB7_pinmux(SCU_PB7_PINMUX_UART1_TX, 0);

	for ( int i = 0; i < size; i++ )
	{
		if ( ascii > 0x7E )
			ascii = 0x21;
		*(uart_wbuffer+i) = ascii++;
	}

	while ( 1 )
	{
		dbg_printf(DBG_LESS_INFO, "Press any key to start UART DMA Transfer\r\n");
		console_getchar();

		//Transmit data using DMA mode with UART1
		hx_CleanDCache_by_Addr((volatile void *)uart_wbuffer, (sizeof(char) *size));
		dev_uart_ptr = hx_drv_uart_get_dev(USE_DW_UART_1);
		dev_uart_ptr->uart_open(UART_BAUDRATE_921600);
		// CGP - changed to avoid warning
		//dev_uart_ptr->uart_write_udma(uart_wbuffer, size, uart1_dma_tx_cb);
		dev_uart_ptr->uart_write_udma((const void *) uart_wbuffer, size, uart1_dma_tx_cb);

#if 0	// test DMA transfer not completed and issue another transfer
		dbg_printf(DBG_LESS_INFO, "Delay(200)ms\r\n");
		hx_drv_timer_cm55x_delay_ms(200, TIMER_STATE_DC);
		dbg_printf(DBG_LESS_INFO, "Tx NOT completed, start another UART DMA Transfer\r\n");
		dev_uart_ptr->uart_write_udma(uart_wbuffer, size, uart1_dma_tx_cb);
#endif
	}

	return 0;
}


static volatile uint32_t *g_mem_pattern = (uint32_t *)0x340a0000;

void pmu_test()
{
	// CGP changed to avoid error
	PMU_WAKEUPEVENT_E wakeup_event;
	PMU_WAKEUPEVENT1_E wakeup_event1;
	//uint32_t wakeup_event;
	//uint32_t wakeup_event1;
	uint32_t boot_cnt;

	// CGP change the message
	//xprintf("PMU PD Sample Code\r\n");
	xprintf("\r\nPower Management Test. Waking from sleep. Details following...\r\n");

	/*Get PMU Wakeup Event*/
	hx_lib_pm_get_event(&wakeup_event, &wakeup_event1);
    xprintf("WakeupEvt=0x%x, WakeupEvt1=0x%x\n", wakeup_event, wakeup_event1);
    print_wakeup_event(wakeup_event, wakeup_event1);

	if ((wakeup_event == PMU_WAKEUP_NONE) && (wakeup_event1 == PMU_WAKEUPEVENT1_NONE))
	{
		/*Cold Boot*/
		xprintf("### Cold Boot ###\n");

		/*Setup PMU PD without Video Pre-rolling*/
	    sleep_mode_enter_sleep(5000, 0xFF, 0);	// 5 second or AON_GPIO0 wake up, memory no retention
	}
	else
	{
		/*Warm Boot*/
		xprintf("### Warm Boot ###\n");

		boot_cnt = hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C);
		if ( boot_cnt > 5 )
		{
			xprintf("memory retention, g_mem_pattern = 0x%x\n", *g_mem_pattern);
			*g_mem_pattern = 0x5a5a5a5a;
			sleep_mode_enter_sleep(5000, 0xFF, 1);	// 5 second or AON_GPIO0 wake up, memory retention
		}
		else
		{
			xprintf("memory no retention, g_mem_pattern = 0x%x\n", *g_mem_pattern);
			*g_mem_pattern = 0x5a5a5a5a;
			sleep_mode_enter_sleep(5000, 0xFF, 0);	// 5 second or AON_GPIO0 wake up, memory no retention
		}
	}
}


#define MAX_STRING  100

static char wakeup_event[][MAX_STRING]={
	{"[0]: PS_DPD : wakeup by (ext_force_pmu_dc_sync)"},
	{"[1]: PS_DPD wakeup by (RTC_timer_int)"},
	{"[2]: PS_DPD wakeup by (anti_tamp_int)"},
	{"[3]: PS_PD wakeup by ((REG_FORCE_EVENT_NSTATE==5'b10000) && REG_FORCE_EVENT_TRIG)"},
	{"[4]: PS_PD wakeup by (ext_int_nxt_dc)"},
	{"[5]: PS_PD wakeup by (RTC_timer_int)"},
	{"[6]: PS_PD wakeup by (SB_timer_int)"},
	{"[7]: PS_PD wakeup by (CMP_int)"},
	{"[8]: PS_PD wakeup by (TS_int)"},
	{"[9]: PS_PD wakeup by (I2C_W_int)"},
	{"[10]: PS_PD wakeup by (REG_CPU_SCENARIO == 4'b0010 && SB_timer_int[0])"},
};

static char wakeup_event1[][MAX_STRING]={
	{"[0]: PS_DPD wakeup by (~pmu_sc_active &&PAD_AON_GPIO_0)"},
	{"[1]: PS_DPD wakeup by (~pmu_sc_active &&PAD_VMUTE)"},
	{"[2]: PS_DPD wakeup by (~pmu_sc_active && ext_int_nxt_dc)"},
	{"[8]: PS_PD wakeup by (anti_tamp_int)"},
};

static void print_wakeup_event(uint32_t event, uint32_t event1)
{

	if((event & 0x1)  != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[0]);
	}
	if((event & 0x2) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[1]);
	}
	if((event & 0x4) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[2]);
	}
	if((event & 0x8) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[3]);
	}
	if((event & 0x10) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[4]);
	}
	if((event & 0x20) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[5]);
	}
	if((event & 0x40) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[6]);
	}
	if((event & 0x80) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[7]);
	}
	if((event & 0x100) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[8]);
	}
	if((event & 0x200) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[9]);
	}
	if((event & 0x400) != 0)
	{
		xprintf("event=%s\r\n", wakeup_event[10]);
	}

	if((event1 & 0x1)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[0]);
	}
	if((event1 & 0x2)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[1]);
	}
	if((event1 & 0x4)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[2]);
	}
	if((event1 & 0x100)  != 0)
	{
		xprintf("event1=%s\r\n", wakeup_event1[3]);
	}

}


void setCM55MTimerAlarmPMU(uint32_t timer_ms)
{
	// CGP removed to avoid 'set but not used' error
	//TIMER_ERROR_E ret;
	TIMER_CFG_T timer_cfg;

	timer_cfg.period = timer_ms;
	timer_cfg.mode = TIMER_MODE_ONESHOT;
	timer_cfg.ctrl = TIMER_CTRL_PMU;
	timer_cfg.state = TIMER_STATE_PMU;

	//ret = hx_drv_timer_cm55m_start(&timer_cfg, NULL);
	hx_drv_timer_cm55m_start(&timer_cfg, NULL);
}


void sleep_mode_enter_sleep(uint32_t timer_ms, uint32_t aon_gpio, uint32_t retention)
{
	uint8_t  gpio_value;
	uint32_t boot_cnt;
	PM_PD_NOVIDPRE_CFG_T cfg;
	uint32_t freq;
	SCU_LSC_CLK_CFG_T lsc_cfg;
	SCU_PDHSC_HSCCLK_CFG_T hsc_cfg;
	PM_CFG_PWR_MODE_E mode;
	SCU_PLL_FREQ_E pmuwakeup_pll_freq;
	SCU_HSCCLKDIV_E pmuwakeup_cm55m_div;
	SCU_LSCCLKDIV_E pmuwakeup_cm55s_div;

	boot_cnt = hx_get_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C);
	boot_cnt++;
	hx_set_memory(BASE_ADDR_APB_SWREG_AON_ALIAS+0x3C, boot_cnt);
	xprintf("boot cnt= %d\r\n", boot_cnt);

	/*Clear PMU Wakeup Event*/
	hx_lib_pm_clear_event();

	/*Clear Wakeup related IP Interrupt*/
	hx_drv_gpio_clr_int_status(AON_GPIO0);
	hx_drv_gpio_clr_int_status(AON_GPIO1);
	hx_drv_timer_ClearIRQ(TIMER_ID_2);

	if ( aon_gpio == 0 )
	{
		hx_drv_gpio_set_input(AON_GPIO0);
		hx_drv_gpio_set_int_type(AON_GPIO0, GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH);
		hx_drv_scu_set_PA0_pinmux(SCU_PA0_PINMUX_AON_GPIO0_0, 1);
		hx_drv_gpio_set_int_enable(AON_GPIO0, 1);
		hx_drv_gpio_get_in_value(AON_GPIO0, &gpio_value);
		xprintf("AON_GPIO0 = %d\n", gpio_value);
	}
	else if ( aon_gpio == 1 )
	{
		hx_drv_gpio_set_input(AON_GPIO1);
		hx_drv_gpio_set_int_type(AON_GPIO1, GPIO_IRQ_TRIG_TYPE_LEVEL_HIGH);
		hx_drv_scu_set_PA1_pinmux(SCU_PA1_PINMUX_AON_GPIO1, 1);
		hx_drv_gpio_set_int_enable(AON_GPIO1, 1);
		hx_drv_gpio_get_in_value(AON_GPIO1, &gpio_value);
		xprintf("AON_GPIO1 = %d\n", gpio_value);
	}

	/*Get System Current Clock*/
	hx_drv_swreg_aon_get_pmuwakeup_freq(&pmuwakeup_pll_freq, &pmuwakeup_cm55m_div, &pmuwakeup_cm55s_div);
	hx_drv_swreg_aon_get_pllfreq(&freq);
	xprintf("pmuwakeup_freq_type=%d, pmuwakeup_cm55m_div=%d, pmuwakeup_cm55s_div=%d\n", pmuwakeup_pll_freq, pmuwakeup_cm55m_div, pmuwakeup_cm55s_div);
	xprintf("pmuwakeup_run_freq=%d\n", freq);

	mode = PM_MODE_PS_NOVID_PREROLLING;
	hx_lib_pm_get_defcfg_bymode(&cfg, mode);

	/*Setup bootrom clock speed when PMU Warm boot wakeup*/
	cfg.bootromspeed.bootromclkfreq = pmuwakeup_pll_freq;
	cfg.bootromspeed.pll_freq = freq;
	cfg.bootromspeed.cm55m_div = pmuwakeup_cm55m_div;
	cfg.bootromspeed.cm55s_div = pmuwakeup_cm55s_div;

	/*Setup CM55 Small can be reset*/
	cfg.cm55s_reset = SWREG_AON_PMUWAKE_CM55S_RERESET_YES;
	/*Mask RTC Interrupt for PMU*/
	cfg.pmu_rtc_mask = PM_RTC_INT_MASK_ALLMASK;
	/*Mask PA23 Interrupt for PMU*/
	cfg.pmu_pad_pa23_mask = PM_IP_INT_MASK;
	/*Mask I2CWakeup Interrupt for PMU*/
	cfg.pmu_i2cw_mask = PM_IP_INT_MASK;
	/*Mask CMP Interrupt for PMU*/
	cfg.pmu_cmp_mask = PM_IP_INT_MASK;
	/*Mask TS Interrupt for PMU*/
	cfg.pmu_ts_mask = PM_IP_INT_MASK;
	/*Mask ANTI TAMPER Interrupt for PMU*/
	cfg.pmu_anti_mask = PM_IP_INT_MASK;
	/*No Debug Dump message*/
	cfg.support_debugdump = 0;

	/*UnMask PA01 Interrupt for PMU*/
	cfg.pmu_pad_pa01_mask = PM_IP_INT_MASK_ALL_UNMASK;

	/*UnMask Timer2 Interrupt others timer interrupt are mask for PMU*/
	cfg.pmu_timer_mask = 0x1FB;

	if ( retention == 1 )
	{
		/*Setup Memory retention*/
		xprintf("Setup Memory retention\n");
		cfg.tcm_retention = PM_MEM_RET_YES;			/**< CM55M TCM Retention**/
		cfg.hscsram_retention[0] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[1] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[2] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.hscsram_retention[3] = PM_MEM_RET_YES;	/**< HSC SRAM Retention**/
		cfg.lscsram_retention = PM_MEM_RET_NO;		/**< LSC SRAM Retention**/
		cfg.skip_bootflow.sec_mem_flag = SWREG_AON_RETENTION;			/**< Skip Boot Flow**/
		cfg.skip_bootflow.first_bl_flag = SWREG_AON_RETENTION; /*!< First BL Retention */
		cfg.skip_bootflow.cm55m_s_app_flag = SWREG_AON_RETENTION; /*!< cm55m_s_app Retention */
		cfg.skip_bootflow.cm55m_ns_app_flag = SWREG_AON_RETENTION; /*!< cm55m_ns_app Retention */
		cfg.skip_bootflow.cm55s_s_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_s_app Retention */
		cfg.skip_bootflow.cm55s_ns_app_flag = SWREG_AON_NO_RETENTION; /*!< cm55s_ns_app Retention */
		cfg.skip_bootflow.cm55m_model_flag = SWREG_AON_RETENTION; /*!< cm55m model Retention */
		cfg.skip_bootflow.cm55s_model_flag = SWREG_AON_NO_RETENTION; /*!< cm55s model Retention */
		cfg.skip_bootflow.cm55m_appcfg_flag = SWREG_AON_RETENTION; /*!< cm55m appcfg Retention */
		cfg.skip_bootflow.cm55s_appcfg_flag = SWREG_AON_NO_RETENTION; /*!< cm55s appcfg Retention */
		cfg.skip_bootflow.cm55m_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_s_app RW Data Retention */
		cfg.skip_bootflow.cm55m_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55m_ns_app RW Data Retention */
		cfg.skip_bootflow.cm55s_s_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_s_app RW Data Retention */
		cfg.skip_bootflow.cm55s_ns_app_rwdata_flag = SWREG_AON_NO_RETENTION;/*!< cm55s_ns_app RW Data Retention */
		cfg.skip_bootflow.secure_debug_flag = SWREG_AON_RETENTION;
	}
	else
	{
		/*Setup Memory no retention*/
		xprintf("Setup Memory no retention\n");
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
	}

	/**No Pre-capture when boot up**/
	cfg.support_bootwithcap = PM_BOOTWITHCAP_NO;

	/*Not DCDC pin output*/
	cfg.pmu_dcdc_outpin = PM_CFG_DCDC_MODE_OFF;
	/** No Pre-capture when boot up**/
	cfg.ioret = PM_CFG_PD_IORET_ON;

	cfg.sensor_type = PM_SENSOR_TIMING_FVLDLVLD_CON;
	/*SIMO on in PD*/
	cfg.simo_pd_onoff = PM_SIMO_PD_ONOFF_ON;

	xprintf("speed=%d,reset=%d,nframeend_ctrl=%d,retention=%d,precap=%d\n", cfg.bootromspeed.bootromclkfreq, cfg.cm55s_reset, cfg.nframeend_ctrl,cfg.tcm_retention ,cfg.support_bootwithcap);
	xprintf("pa01_mask=0x%x,rtc_mask=0x%x,pa23_mask=0x%x,i2cw_mask=0x%x,timer_mask=0x%x,cmp_mask=0x%x,ts_mask=0x%x\n", cfg.pmu_pad_pa01_mask,cfg.pmu_rtc_mask,cfg.pmu_pad_pa23_mask,cfg.pmu_i2cw_mask,cfg.pmu_timer_mask,cfg.pmu_cmp_mask,cfg.pmu_ts_mask);
	xprintf("dcdcpin=%d, pmu_anti_mask=0x%x\n", cfg.pmu_dcdc_outpin, cfg.pmu_anti_mask);
	xprintf("freq=%d, cm55mdiv=%d,cm55sdiv=%d\n", cfg.bootromspeed.pll_freq, cfg.bootromspeed.cm55m_div, cfg.bootromspeed.cm55s_div);

	hx_lib_pm_cfg_set(&cfg, NULL, mode);

	/* Setup CM55M Timer(Timer2) Wakeup */
	setCM55MTimerAlarmPMU(timer_ms);

	/* Use PMU lib to control HSC_CLK and LSC_CLK so set those parameter to 0 */
	memset(&hsc_cfg, 0, sizeof(SCU_PDHSC_HSCCLK_CFG_T));
	memset(&lsc_cfg, 0, sizeof(SCU_LSC_CLK_CFG_T));

	/* Trigger to PMU mode */
	hx_lib_pm_trigger(hsc_cfg, lsc_cfg, PM_CLK_PARA_CTRL_BYPMLIB);
}

