/*
 * mb_cust_task.c
 *
 *  Created on: Nov 3, 2022
 *      Author: bigcat-himax
 */
#include "hx_drv_mb.h"
#include "mb_cust_task.h"

#define MAX_MUTEX_REQ_CNT	1000
#define INIT_CASE			4

int mb_cust_data_init() {
	uint16_t count = 0;
	int res = 0;
	uint32_t ret = 0;

	MB_MUTEX_STATUS_E mutex_status= MB_MUTEX_STATUS_NOT_GET;
	MB_CUST_MUTEX_SHAREDATA_IDX_E init_case[INIT_CASE] = {MB_CUST_MUTEX_SHAREDATA_IDX_IMG_QUALITY,
														MB_CUST_MUTEX_SHAREDATA_IDX_IDLE_TIME,
														MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS,
														MB_CUST_MUTEX_SHAREDATA_IDX_CURRENT_TIMESTAMP};
	uint32_t set_data[INIT_CASE] = {4, 1000 /*1000ms*/, MB_CUST_READ_IMAGE_END, 0};

	for(int i = 0;i<INIT_CASE;i++) {
		count = 0;
		mutex_status= MB_MUTEX_STATUS_NOT_GET;

		while(mutex_status != MB_MUTEX_STATUS_GET && count < MAX_MUTEX_REQ_CNT)
			hx_drv_mb_require_mutex(init_case[i], &mutex_status), count++;

		if(mutex_status == MB_MUTEX_STATUS_GET) {

			hx_drv_mb_set_sharedata(init_case[i], set_data[i]);

			hx_drv_mb_release_mutex(init_case[i]);
		}
		else
			res = -1;
	}

	return res;

}


int mb_cust_set_data(MB_CUST_MUTEX_SHAREDATA_IDX_E idx, uint32_t data) {
	MB_MUTEX_STATUS_E mutex_status= MB_MUTEX_STATUS_NOT_GET;
	uint16_t count = 0;
	int res = 0;
	MB_CUST_READ_IMAGE_STATUS_E read_image_status = MB_CUST_READ_IMAGE_END;

	switch(idx) {
	case MB_CUST_MUTEX_SHAREDATA_IDX_IMG_QUALITY:
		if(data == 4 || data == 10) {
			mutex_status= MB_MUTEX_STATUS_NOT_GET;
			//request mutex and update share data
			while(mutex_status != MB_MUTEX_STATUS_GET && count < MAX_MUTEX_REQ_CNT)
				hx_drv_mb_require_mutex(idx, &mutex_status), count++;

			if(mutex_status == MB_MUTEX_STATUS_GET) {
				hx_drv_mb_set_sharedata(idx, data);

				hx_drv_mb_release_mutex(idx);
			}
			else
				res = -1;

		}
		break;
	case MB_CUST_MUTEX_SHAREDATA_IDX_IDLE_TIME:
		if(data <= 1000000) {
			mutex_status= MB_MUTEX_STATUS_NOT_GET;
			//request mutex and update share data
			while(mutex_status != MB_MUTEX_STATUS_GET && count < MAX_MUTEX_REQ_CNT)
				hx_drv_mb_require_mutex(idx, &mutex_status), count++;

			if(mutex_status == MB_MUTEX_STATUS_GET) {
				hx_drv_mb_set_sharedata(idx, data);

				hx_drv_mb_release_mutex(idx);
			}
			else
				res = -1;
		}
		break;
	case MB_CUST_MUTEX_SHAREDATA_IDX_READ_IAMGE_STATUS:
	case MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_IMAGE_POS:
	case MB_CUST_MUTEX_SHAREDATA_IDX_FLASH_LENGTH_POS:
	case MB_CUST_MUTEX_SHAREDATA_IDX_CURRENT_TIMESTAMP:
	default:
		//check current share data status
		mutex_status= MB_MUTEX_STATUS_NOT_GET;
		while(mutex_status != MB_MUTEX_STATUS_GET && count < MAX_MUTEX_REQ_CNT)
			hx_drv_mb_require_mutex(idx, &mutex_status), count++;

		if(mutex_status == MB_MUTEX_STATUS_GET) {
			hx_drv_mb_set_sharedata(idx, data);

			hx_drv_mb_release_mutex(idx);
		}
		else
			res = -1;
		break;
	}

	return res;
}

int mb_cust_get_data(MB_CUST_MUTEX_SHAREDATA_IDX_E idx, uint32_t *data) {
	int res = 0;
	uint16_t count = 0;
	MB_MUTEX_STATUS_E mutex_status= MB_MUTEX_STATUS_NOT_GET;

	//request mutex and read share data
	while(mutex_status != MB_MUTEX_STATUS_GET && count < MAX_MUTEX_REQ_CNT)
		hx_drv_mb_require_mutex(idx, &mutex_status), count++;

	if(mutex_status == MB_MUTEX_STATUS_GET) {
		if(hx_drv_mb_get_sharedata(idx, data)== MB_ERROR_INVALID_PARAMETERS)
			res = -1;

		hx_drv_mb_release_mutex(idx);
	}
	else
		res = -1;

	return res;

}
