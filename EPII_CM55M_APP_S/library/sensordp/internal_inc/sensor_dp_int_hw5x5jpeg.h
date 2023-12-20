/*
 * sensor_dp_int_hw5x5jpeg.h
 *
 *  Created on: 2019¦~2¤ë24¤é
 *      Author: 902447
 */

#ifndef LIBRARY_SENSORDP_SENSOR_DP_INT_HW5X5JPEG_H_
#define LIBRARY_SENSORDP_SENSOR_DP_INT_HW5X5JPEG_H_

/**
 * \brief	datapath INT3_Txx HW5x5(YUV) --> WDMA3, HW5x5(YUV) --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]	hw5x5_cfg	HW5x5 configuration
 * \param[in]	jpeg_cfg	JPEG configuration
 * \param[in]	cyclic_buffer_cnt	cyclic buffer count configuration
 * \param[in]	wdma2_cb	 WDMA2 callback function
 * \param[in]	wdma3_cb	 WDMA3 callback function
 * \retval	void
 */
void setup_dp_int_hw5x5jpeg(
		HW5x5_CFG_T hw5x5_cfg,
		JPEG_CFG_T jpeg_cfg,
		uint8_t cyclic_buffer_cnt,
		XDMAWDMA2_ISREvent_t wdma2_cb,
	    XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief   datapath INT3_Txx HW5x5(RGB) --> WDMA3, HW5x5(RGB) --> RGB2YUV --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]   hw5x5_cfg   HW5x5 configuration
 * \param[in]   jpeg_cfg    JPEG configuration
 * \param[in]   cyclic_buffer_cnt   cyclic buffer count configuration
 * \param[in]   wdma2_cb     WDMA2 callback function
 * \param[in]   wdma3_cb     WDMA3 callback function
 * \retval  void
 */
void setup_dp_int_hw5x5rgbjpeg(
        HW5x5_CFG_T hw5x5_cfg,
        JPEG_CFG_T jpeg_cfg,
        uint8_t cyclic_buffer_cnt,
        XDMAWDMA2_ISREvent_t wdma2_cb,
        XDMAWDMA3_ISREvent_t wdma3_cb);

/**
 * \brief   datapath INT3_Txx HXCSC --> WDMA3, HXCSC --> (HXRGB2YUV) --> JPEG --> WDMA2 setup
 *
 *
 * \param[in]   hxcsc_cfg   HXCSC configuration
 * \param[in]   jpeg_cfg    JPEG configuration
 * \param[in]   cyclic_buffer_cnt   cyclic buffer count configuration
 * \param[in]   wdma2_cb     WDMA2 callback function
 * \param[in]   wdma3_cb     WDMA3 callback function
 * \retval  void
 */
void setup_dp_int_hxcscjpeg(
        HXCSC_CFG_T hxcsc_cfg,
        JPEG_CFG_T jpeg_cfg,
        uint8_t cyclic_buffer_cnt,
        XDMAWDMA2_ISREvent_t wdma2_cb,
        XDMAWDMA3_ISREvent_t wdma3_cb);


#endif /* LIBRARY_SENSORDP_SENSOR_DP_INT_HW5X5JPEG_H_ */
