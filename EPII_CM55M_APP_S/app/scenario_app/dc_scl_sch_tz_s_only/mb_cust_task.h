/*
 * mb_cust_task.h
 *
 *  Created on: Nov 3, 2022
 *      Author: bigcat-himax
 */

#ifndef APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_MB_CUST_TASK_H_
#define APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_MB_CUST_TASK_H_

typedef enum MB_CUST_IMG_QUALITY_E
{
	MB_CUST_IMG_QUALITY_4X      = 0,				/**< JPEG image compression ratio - 4X */
	MB_CUST_IMG_QUALITY_10X,						/**< JPEG image compression ratio - 10X */
} MB_CUST_IMG_QUALITY_E;

typedef enum MB_CUST_READ_IMAGE_STATUS_E
{
	MB_CUST_READ_IMAGE_END		= 0x1,				/**< request to read flash image */
	MB_CUST_READ_IMAGE_START,						/**< read flahs image process is done */
} MB_CUST_READ_IMAGE_STATUS_E;


typedef enum MB_CUST_MUTEX_SHAREDATA_IDX_E
{
	MB_CUST_MUTEX_SHAREDATA_IDX_IMG_QUALITY      = 0,	/**< JPEG image compression ratio, can be 4X or 10X */
	MB_CUST_MUTEX_SHAREDATA_IDX_IDLE_TIME,				/**< Idle time in ms of a loop, can be 1000 ~ 1000000 Millisecond  */
	MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS,		/**< Read image operation status,  */
	MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_INIT_POS,	/** information for g_flash_record_start_pos, this is start position of flash to store image */
	MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS,		/** information for g_flash_image_cur_pos */
	MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS,		/** information for g_flash_length_cur_pos */
	MB_CUST_MUTEX_SHAREDATA_IDX_CURRENT_TIMESTAMP,		/** information for idle time check usage */
} MB_CUST_MUTEX_SHAREDATA_IDX_E;

int mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_E idx, uint32_t *data);
int mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_E idx, uint32_t data);
int mb_cust_data_init();

#endif /* APP_SCENARIO_DC_SCL_SCH_TZ_S_ONLY_MB_CUST_TASK_H_ */
