/*
 * hx_drv_mpc.h
 *
 *  Created on: 2021�~5��30��
 *      Author: 902447
 */

#ifndef DRIVERS_INC_HX_DRV_MPC_H_
#define DRIVERS_INC_HX_DRV_MPC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "WE2_device.h"
#include "hx_drv_scu.h"
/**
 * \defgroup	MPC_DRV	MPC  Driver
 * \ingroup		MPC_DRV
 * \brief	MPC  Driver Declaration
 */
/**
 * \defgroup	MPC_DRV_ENUM	MPC  Driver Enumeration
 * \ingroup	MPC_DRV
 * \brief	Defines some enumeration of MPC  Driver need.
 * @{
 */

#define FLASH1_BLK_INDEX	16 //16MB, 32KB
#define FLASH2_BLK_INDEX	4 //16MB, 128KB
#define MPC_FLASH1_SIZE     (0x01000000)
#define MPC_FLASH2_SIZE     (0x01000000)
/**
 * \enum MPC_ERROR_E
 * \brief MPC Errors Type
 */
typedef enum {
	MPC_NO_ERROR = 0, /**< No ERROR */
	MPC_ERROR_INVALID_PARAMETERS, /**< ERROR MSG: Invalid parameters */
	MPC_UNKNOWN_ERROR, /**< ERROR MSG: UNKNOWN ERROR*/
} MPC_ERROR_E;

/**
 * \enum MPC_BLKSIZE_E
 * \brief MPC Block Size
 */
typedef enum {
	MPC_BLKSIZE_RES0 = 0, /**< Reserved */
	MPC_BLKSIZE_RES1, /**< Reserved */
	MPC_BLKSIZE_RES2, /**< Reserved */
	MPC_BLKSIZE_256B, /**< 3: 256B */
	MPC_BLKSIZE_512B, /**< 4: 512B */
	MPC_BLKSIZE_1KB, /**< 5: 1KB */
	MPC_BLKSIZE_2KB, /**< 6: 2KB */
	MPC_BLKSIZE_4KB, /**< 7: 4KB */
	MPC_BLKSIZE_8KB, /**< 8: 8KB */
	MPC_BLKSIZE_16KB, /**< 9: 16KB */
	MPC_BLKSIZE_32KB, /**< 10: 32KB */
	MPC_BLKSIZE_64KB, /**< 11: 64KB */
	MPC_BLKSIZE_128KB, /**< 12: 128KB */
	MPC_BLKSIZE_256KB, /**< 13: 256KB */
	MPC_BLKSIZE_512KB, /**< 14: 512KB */
	MPC_BLKSIZE_1MB, /**< 15: 1MB */
} MPC_BLKSIZE_E;

/**
 * \enum MPC_LUT_STATUS_E
 * \brief MPC LUT Status
 */
typedef enum {
	MPC_LUT_STATUS_SECURE = 0, /**< Secure */
	MPC_LUT_STATUS_NONSECURE, /**< NS */
} MPC_LUT_STATUS_E;
/** @} */

/**
 * \brief	MPC interrupt status callback function
 *
 * \param[in]	event	 interrupt event
 * \param[in]	info1	 information1
 * \param[in]	info2	 information2
 * \return	void.
 */
typedef void (*MPC_ISREvent_t) (uint32_t event, uint32_t info1, uint32_t info2);

/**
 * \defgroup	MPC_DRV_REGSTRUCT	MPC  Driver Structure
 * \ingroup	MPC_DRV
 * \brief	contains definitions of MPC  Driver structure.
 * @{
 */
/**
 * \struct MPC_CFG_T
 * \brief MPC configuration by MPC Driver
 */
/**
 * \struct MPC_SRAM0_CFG_T
 * \brief MPC SRAM0 Configuration
 */
typedef struct {
	uint8_t irq_enable;
	uint32_t lut_table;
	MPC_ISREvent_t callback;
	SCU_AXI_MPC_CFG_T sram0_mpc_cfg;
} MPC_SRAM0_CFG_T;

/**
 * \struct MPC_SRAM1_CFG_T
 * \brief MPC SRAM1 Configuration
 */
typedef struct {
	uint8_t irq_enable;
	uint32_t lut_table;
	MPC_ISREvent_t callback;
	SCU_AXI_MPC_CFG_T sram1_mpc_cfg;
} MPC_SRAM1_CFG_T;

/**
 * \struct MPC_SRAM2_CFG_T
 * \brief MPC SRAM2 Configuration
 */
typedef struct {
	uint8_t irq_enable;
	uint8_t resp_err;
	uint32_t lut_table;
	MPC_ISREvent_t callback;
	SCU_AHB_MPC_CFG_T sram2_mpc_cfg;
} MPC_SRAM2_CFG_T;

/**
 * \struct MPC_FLASH1_CFG_T
 * \brief MPC FLASH1 Configuration
 */
typedef struct {
	uint8_t irq_enable;
	uint8_t resp_err;
	uint32_t lut_table[FLASH1_BLK_INDEX];
	MPC_ISREvent_t callback;
	SCU_AHB_MPC_CFG_T flash_mpc_cfg;
} MPC_FLASH1_CFG_T;

/**
 * \struct MPC_FLASH1_CFG_T
 * \brief MPC FLASH1 Configuration
 */
typedef struct {
	uint8_t irq_enable;
	uint8_t resp_err;
	uint32_t lut_table[FLASH2_BLK_INDEX];
	MPC_ISREvent_t callback;
	SCU_AHB_MPC_CFG_T flash_mpc_cfg;
} MPC_FLASH2_CFG_T;
/** @} */

/**
 * \defgroup	MPC_DRV_MACRO	MPC  Driver MACRO
 * \ingroup	MPC_DRV
 * \brief	contains definitions of MPC  Driver MACRO.
 * @{
 */

/** @} */

/**
 * \defgroup	MPC_DRV_FUNCDLR	 MPC Driver Function Declaration
 * \ingroup	MPC_DRV
 * \brief	Contains declarations of  MPC Driver functions.
 * @{
 */
#ifndef BOOT_USED
/**
 * \brief	Get SRAM0 Block Size
 *
 * \param[out]	blk_size	block size
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_blksize(MPC_BLKSIZE_E *blk_size);
/**
 * \brief	Get SRAM1 Block Size
 *
 * \param[out]	blk_size	block size
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_blksize(MPC_BLKSIZE_E *blk_size);
/**
 * \brief	Get SRAM2 Block Size
 *
 * \param[out]	blk_size	block size
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_blksize(MPC_BLKSIZE_E *blk_size);
/**
 * \brief	Get flash1 Block Size
 *
 * \param[out]	blk_size	block size
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_blksize(MPC_BLKSIZE_E *blk_size);
/**
 * \brief	Get flash2 Block Size
 *
 * \param[out]	blk_size	block size
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_blksize(MPC_BLKSIZE_E *blk_size);

/**
 * \brief	Get SRAM0 Block Max
 *
 * \param[out]	blkmax	block Max
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_blkmax(uint32_t *blkmax);
/**
 * \brief	Get SRAM1 Block Max
 *
 * \param[out]	blkmax	block Max
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_blkmax(uint32_t *blkmax);
/**
 * \brief	Get SRAM2 Block Max
 *
 * \param[out]	blkmax	block Max
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_blkmax(uint32_t *blkmax);
/**
 * \brief	Get flash1 Block Max
 *
 * \param[out]	blkmax	block max
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_blkmax(uint32_t *blkmax);

/**
 * \brief	Get flash2 Block Max
 *
 * \param[out]	blkmax	block max
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_blkmax(uint32_t *blkmax);

/**
 * \brief	Get SRAM0 SEC Lock
 *
 * \param[out]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_seclock(uint32_t *seclock);
/**
 * \brief	Get SRAM1 SEC Lock
 *
 * \param[out]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_seclock(uint32_t *seclock);
/**
 * \brief	Get SRAM2 SEC Lock
 *
 * \param[out]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_seclock(uint32_t *seclock);
/**
 * \brief	Get flash1 SEC Lock
 *
 * \param[out]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_seclock(uint32_t *seclock);
/**
 * \brief	Get flash2 SEC Lock
 *
 * \param[out]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_seclock(uint32_t *seclock);

/**
 * \brief	Set SRAM0 SEC Lock
 *
 * \param[in]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_seclock(uint32_t seclock);
/**
 * \brief	Set SRAM1 SEC Lock
 *
 * \param[in]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_seclock(uint32_t seclock);
/**
 * \brief	Set SRAM2 SEC Lock
 *
 * \param[in]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_seclock(uint32_t seclock);
/**
 * \brief	Set flash1 SEC Lock
 *
 * \param[in]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_seclock(uint32_t seclock);
/**
 * \brief	Set flash2 SEC Lock
 *
 * \param[in]	seclock	sec lock
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_seclock(uint32_t seclock);
#endif//#ifndef BOOT_USED
//////////////////////
/**
 * \brief	Set SRAM2 Resp
 *
 * \param[in]	SRAM2 Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_resp(uint8_t resp);
/**
 * \brief	Set flash1 Resp
 *
 * \param[in]	Flash Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_resp(uint8_t resp);
/**
 * \brief	Set flash2 Resp
 *
 * \param[in]	Flash Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_resp(uint8_t resp);
/**
 * \brief	Get SRAM2 Resp
 *
 * \param[out]	SRAM2 Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_resp(uint8_t *resp);
/**
 * \brief	Get flash1 Resp
 *
 * \param[out]	Flash Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_resp(uint8_t *resp);
/**
 * \brief	Get flash2 Resp
 *
 * \param[out]	Flash Resp
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_resp(uint8_t *resp);
/////////////////
/////////////////
#ifndef BOOT_USED
/**
 * \brief	Get SRAM0 Auto Inc BLK Idx enable
 *
 * \param[out]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_autoincblkidx(uint32_t *autoincblkidx);
/**
 * \brief	Get SRAM1 Auto Inc BLK Idx enable
 *
 * \param[out]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_autoincblkidx(uint32_t *autoincblkidx);
/**
 * \brief	Get SRAM2 Auto Inc BLK Idx enable
 *
 * \param[out]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_autoincblkidx(uint32_t *autoincblkidx);
/**
 * \brief	Get flash1 Auto Inc BLK Idx enable
 *
 * \param[out]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_autoincblkidx(uint32_t *autoincblkidx);
/**
 * \brief	Get flash2 Auto Inc BLK Idx enable
 *
 * \param[out]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_autoincblkidx(uint32_t *autoincblkidx);
#endif
/**
 * \brief	Set SRAM0 Auto Inc BLK Idx enable
 *
 * \param[in]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_autoincblkidx(uint32_t autoincblkidx);
/**
 * \brief	Set SRAM1 Auto Inc BLK Idx enable
 *
 * \param[in]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_autoincblkidx(uint32_t autoincblkidx);
/**
 * \brief	Set SRAM2 Auto Inc BLK Idx enable
 *
 * \param[in]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_autoincblkidx(uint32_t autoincblkidx);
/**
 * \brief	Set flash1 Auto Inc BLK Idx enable
 *
 * \param[in]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_autoincblkidx(uint32_t autoincblkidx);
/**
 * \brief	Set flash2 Auto Inc BLK Idx enable
 *
 * \param[in]	autoincblkidx	 Auto Inc BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_autoincblkidx(uint32_t autoincblkidx);
/////////////////
#ifndef BOOT_USED
/**
 * \brief	Get SRAM0 BLK Idx
 *
 * \param[out]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_blkidx(uint32_t *blkidx);
/**
 * \brief	Get SRAM1 BLK Idx
 *
 * \param[out]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_blkidx(uint32_t *blkidx);
/**
 * \brief	Get SRAM2 BLK Idx
 *
 * \param[out]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_blkidx(uint32_t *blkidx);
/**
 * \brief	Get flash1 BLK Idx enable
 *
 * \param[out]	blkidx	 BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_blkidx(uint32_t *blkidx);
/**
 * \brief	Get flash2 BLK Idx enable
 *
 * \param[out]	blkidx	 BLK Idx enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_blkidx(uint32_t *blkidx);
#endif
/**
 * \brief	Set SRAM0 BLK Idx
 *
 * \param[in]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_blkidx(uint32_t blkidx);
/**
 * \brief	Set SRAM1 BLK Idx
 *
 * \param[in]	autoincblkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_blkidx(uint32_t blkidx);
/**
 * \brief	Set SRAM2 BLK Idx
 *
 * \param[in]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_blkidx(uint32_t blkidx);
/**
 * \brief	Set flash BLK Idx
 *
 * \param[in]	blkidx	 BLK Idx
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_blkidx(uint32_t blkidx);

//////////
/**
 * \brief	Get SRAM0 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E *lut);
/**
 * \brief	Get SRAM1 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E *lut);
/**
 * \brief	Get SRAM2 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E *lut);
/**
 * \brief	Get flash1 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E *lut);
/**
 * \brief	Get flash2 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E *lut);

/**
 * \brief	Set SRAM0 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E lut);
/**
 * \brief	Set SRAM1 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E lut);
/**
 * \brief	Set SRAM2 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E lut);
/**
 * \brief	Set flash1 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E lut);
/**
 * \brief	Set flash2 LUT
 *
 * \param[in]	blkidx	 block index
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_lut_byblkidx(uint32_t blkidx, MPC_LUT_STATUS_E lut);

///////
//////////
/**
 * \brief	Get SRAM0 LUT
 *
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_lut(uint32_t *lut);
/**
 * \brief	Get SRAM1 LUT
 *
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_lut(uint32_t *lut);
/**
 * \brief	Get SRAM2 LUT
 *
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_lut(uint32_t *lut);
/**
 * \brief	Get flash1 LUT
 *
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_lut(uint32_t *lut);
/**
 * \brief	Get flash2 LUT
 *
 * \param[out]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_lut(uint32_t *lut);
/**
 * \brief	Set SRAM0 LUT
 *
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_lut(uint32_t lut);
/**
 * \brief	Set SRAM1 LUT
 *
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_lut(uint32_t lut);
/**
 * \brief	Set SRAM2 LUT
 *
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_lut(uint32_t lut);
/**
 * \brief	Set flash1 LUT
 *
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_lut(uint32_t lut);
/**
 * \brief	Set flash2 LUT
 *
 * \param[in]	LUT	 LUT
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_lut(uint32_t lut);
//////////
/**
 * \brief	Get SRAM0 IRQ Enable
 *
 * \param[out]	enable	 IRQ Enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_irq_enable(uint8_t *enable);
/**
 * \brief	Get SRAM1 IRQ Enable
 *
 * \param[out]	enable	 IRQ Enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_irq_enable(uint8_t *enable);
/**
 * \brief	Get SRAM2 IRQ Enable
 *
 * \param[out]	enable	 IRQ Enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_irq_enable(uint8_t *enable);
/**
 * \brief	Get Flash1 IRQ Enable
 *
 * \param[out]	enable	 IRQ Enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_irq_enable(uint8_t *enable);
/**
 * \brief	Get Flash2 IRQ Enable
 *
 * \param[out]	enable	 IRQ Enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_irq_enable(uint8_t *enable);
/**
 * \brief	Set SRAM0 IRQ Enable
 *
 * \param[in]	enable	 enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_irq_enable(uint8_t enable);
/**
 * \brief	Set SRAM1 IRQ Enable
 *
 * \param[in]	enable	 enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_irq_enable(uint8_t enable);
/**
 * \brief	Set SRAM2 IRQ Enable
 *
 * \param[in]	enable	 enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_irq_enable(uint8_t enable);
/**
 * \brief	Set flash1 IRQ Enable
 *
 * \param[in]	enable	 enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_irq_enable(uint8_t enable);
/**
 * \brief	Set flash2 IRQ Enable
 *
 * \param[in]	enable	 enable
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_irq_enable(uint8_t enable);
/**
 * \brief	Set SRAM0 IRQ Clear
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_irq_clear();
/**
 * \brief	Set SRAM1 IRQ Clear
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_irq_clear();
/**
 * \brief	Set SRAM2 IRQ Clear
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_irq_clear();
/**
 * \brief	Set flash1 IRQ Clear
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_irq_clear();
/**
 * \brief	Set flash2 IRQ Clear
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_irq_clear();

/**
 * \brief	Get SRAM0 IRQ Status
 *
 * \param[out]	irq_status	 irq Status
 * \param[out]	irq_info1	 irq information 1
 * \param[out]	irq_info2	 irq information 2
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_irq_status(uint32_t *irq_status, uint32_t *irq_info1, uint32_t *irq_info2);
/**
 * \brief	Get SRAM1 IRQ Status
 *
 * \param[out]	irq_status	 irq Status
 * \param[out]	irq_info1	 irq information 1
 * \param[out]	irq_info2	 irq information 2
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_irq_status(uint32_t *irq_status, uint32_t *irq_info1, uint32_t *irq_info2);
/**
 * \brief	Get SRAM2 IRQ Status
 *
 * \param[out]	irq_status	 irq Status
 * \param[out]	irq_info1	 irq information 1
 * \param[out]	irq_info2	 irq information 2
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_irq_status(uint32_t *irq_status, uint32_t *irq_info1, uint32_t *irq_info2);
/**
 * \brief	Get flash1 IRQ Status
 *
 * \param[out]	irq_status	 irq Status
 * \param[out]	irq_info1	 irq information 1
 * \param[out]	irq_info2	 irq information 2
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_irq_status(uint32_t *irq_status, uint32_t *irq_info1, uint32_t *irq_info2);

/**
 * \brief	Get flash2 IRQ Status
 *
 * \param[out]	irq_status	 irq Status
 * \param[out]	irq_info1	 irq information 1
 * \param[out]	irq_info2	 irq information 2
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_irq_status(uint32_t *irq_status, uint32_t *irq_info1, uint32_t *irq_info2);

/**
 * \brief	Set SRAM0 IRQ
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_irq();
/**
 * \brief	Set SRAM1 IRQ
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_irq();
/**
 * \brief	Set SRAM2 IRQ
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_irq();
/**
 * \brief	Set flash1 IRQ
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_irq();
/**
 * \brief	Set flash2 IRQ
 *
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_irq();
/**
 * \brief	register MPC SRAM0 Interrupt Status callback
 *
 * \param[in]	cb_event	MPC SRAM0 Interrupt callback function
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_register_sram0_cb(MPC_ISREvent_t  cb_event);

/**
 * \brief	register MPC SRAM1 Interrupt Status callback
 *
 * \param[in]	cb_event	MPC SRAM1 Interrupt callback function
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_register_sram1_cb(MPC_ISREvent_t  cb_event);

/**
 * \brief	register MPC SRAM2 Interrupt Status callback
 *
 * \param[in]	cb_event	MPC SRAM2 Interrupt callback function
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_register_sram2_cb(MPC_ISREvent_t  cb_event);

/**
 * \brief	register MPC flash1 Interrupt Status callback
 *
 * \param[in]	cb_event	MPC flash1 Interrupt callback function
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_register_flash1_cb(MPC_ISREvent_t  cb_event);

/**
 * \brief	register MPC flash2 Interrupt Status callback
 *
 * \param[in]	cb_event	MPC flash2 Interrupt callback function
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_register_flash2_cb(MPC_ISREvent_t  cb_event);

/**
 * \brief	Set SRAM0 MPC Configuration
 *
 * \param[in]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram0_cfg(MPC_SRAM0_CFG_T cfg);

/**
 * \brief	Set SRAM1 MPC Configuration
 *
 * \param[in]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram1_cfg(MPC_SRAM1_CFG_T cfg);

/**
 * \brief	Set SRAM2 MPC Configuration
 *
 * \param[in]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_sram2_cfg(MPC_SRAM2_CFG_T cfg);

/**
 * \brief	Set flash1 MPC Configuration
 *
 * \param[in]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash1_cfg(MPC_FLASH1_CFG_T cfg);

/**
 * \brief	Set flash2 MPC Configuration
 *
 * \param[in]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_set_flash2_cfg(MPC_FLASH2_CFG_T cfg);

#ifndef BOOT_USED
/**
 * \brief	Get SRAM0 MPC Configuration
 *
 * \param[out]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram0_cfg(MPC_SRAM0_CFG_T *cfg);

/**
 * \brief	Get SRAM1 MPC Configuration
 *
 * \param[out]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram1_cfg(MPC_SRAM1_CFG_T *cfg);

/**
 * \brief	Get SRAM2 MPC Configuration
 *
 * \param[out]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_sram2_cfg(MPC_SRAM2_CFG_T *cfg);

/**
 * \brief	Get flash1 MPC Configuration
 *
 * \param[out]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash1_cfg(MPC_FLASH1_CFG_T *cfg);

/**
 * \brief	Get flash2 MPC Configuration
 *
 * \param[out]	cfg	 configuration
 * \return	MPC_ERROR_E.
 */
MPC_ERROR_E hx_drv_mpc_get_flash2_cfg(MPC_FLASH2_CFG_T *cfg);
#endif//#ifndef BOOT_USED

#ifndef BOOT_USED
/**
 * \brief	MPC SRAM0 IRQ handler
 *
 * \return	none.
 */
void hx_drv_mpc_sram0_irq_handler(void);

/**
 * \brief	MPC SRAM1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_mpc_sram1_irq_handler(void);

/**
 * \brief	MPC SRAM2 IRQ handler
 *
 * \return	none.
 */
void hx_drv_mpc_sram2_irq_handler(void);

/**
 * \brief	MPC FLASH1 IRQ handler
 *
 * \return	none.
 */
void hx_drv_mpc_flash1_irq_handler(void);

/**
 * \brief	MPC FLASH2 IRQ handler
 *
 * \return	none.
 */
void hx_drv_mpc_flash2_irq_handler(void);
#endif
/** @} */
#endif /* DRIVERS_INC_HX_DRV_MPC_H_ */
