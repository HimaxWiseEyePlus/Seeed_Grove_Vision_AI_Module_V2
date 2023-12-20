#ifndef INCLUDE_HX_DRV_OTP_H
#define INCLUDE_HX_DRV_OTP_H

#include "hx_drv_def.h"
#include "hx_cc312_drv_sec.h"


extern unsigned long gCcRegBase;
#define g_testHwRegBaseAddr	gCcRegBase
/*! Defines the CM life-cycle state value. */
#define CC_BSV_CHIP_MANUFACTURE_LCS     0x0
/*! Defines the DM life-cycle state value. */
#define CC_BSV_DEVICE_MANUFACTURE_LCS       0x1
/*! Defines the Secure life-cycle state value. */
#define CC_BSV_SECURE_LCS           0x5
/*! Defines the RMA life-cycle state value. */
#define CC_BSV_RMA_LCS              0x7


SecError_t  hx_drv_otp_wc_read(otp_wc_read *otp_wc_read_para);
SecError_t  hx_drv_otp_wc_write(otp_wc_write *otp_wc_write_para);
SecError_t  hx_drv_otp_Monotonic_Read_CM(otp_wc_read *otp_wc_read_para);
SecError_t  hx_drv_otp_Monotonic_incr_CM(otp_wc_write *otp_wc_write_para);
SecError_t  hx_drv_otp_Monotonic_Read_DM(otp_wc_read *otp_wc_read_para);
SecError_t  hx_drv_otp_Monotonic_incr_DM(otp_wc_write *otp_wc_write_para);
#endif

