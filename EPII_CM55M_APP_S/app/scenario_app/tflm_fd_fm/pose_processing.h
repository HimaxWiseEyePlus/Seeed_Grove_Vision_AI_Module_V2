#ifndef POSE_PROCESSING_H
#define POSE_PROCESSING_H

#include <stdint.h>
#include "spi_protocol.h"

#define HRNET_HEATMAPS_WIDTH         48
#define HRNET_HEATMAPS_HEIGHT        64
#define HRENT_HEATMAPS_NUM_KEYPOINTS 17


#ifdef __cplusplus
extern "C"
{
#endif

// void keypoints_from_heatmaps(float* heatmaps, struct__box* bbox, uint32_t img_width, uint32_t img_height, 
//                 struct_position* preds, float* maxvals);
// void keypoints_from_heatmaps(int8_t* heatmaps, struct__box* bbox, struct_position* preds, int8_t* maxvals);
// void keypoints_from_heatmaps(int8_t* heatmaps, struct__box* bbox, struct_position* preds, float* maxvals);
// void keypoints_from_heatmaps(int8_t* heatmaps, int32_t zero_point, float scale, struct__box* bbox, struct_position* preds, float* maxvals);
void keypoints_from_heatmaps(int8_t* heatmaps, int32_t zero_point, float scale, struct_hp_algoResult *alg);
#ifdef __cplusplus
}
#endif


#endif
