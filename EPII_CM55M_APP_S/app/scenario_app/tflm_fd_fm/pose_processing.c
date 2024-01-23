
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "spi_protocol.h"

#include "pose_processing.h"

#define HEATMAP_HEIGHT     HRNET_HEATMAPS_HEIGHT
#define HEATMAP_WIDTH      HRNET_HEATMAPS_WIDTH
#define NUM_POSE_KEYPOINTS HRENT_HEATMAPS_NUM_KEYPOINTS

#define TEST_PREDS_FLOAT 1

typedef struct {
	float x;
	float y;
}struct_position_f;

static struct_position_f loc_f[17];
#define DE_QUNT(v, z, c) ((float)(v) - (z) ) * (c)

inline float dequantify_val(int8_t val, int32_t zero_point, float scale)
{
    float v = (float)val;
    float z = (float)zero_point;
    float s = (float)scale;

    return (v - z) * s;
}

// void get_max_preds(int8_t* heatmaps, int32_t zero_point, float scale, struct_position_f* preds_f, float* maxvals_f)
void get_max_preds(int8_t* heatmaps, int32_t zero_point, float scale, struct_position_f* preds_f, struct_hp_algoResult *alg)
{
    // float scale = 0.004358685;
    // int32_t zero_point = -116;
    // float maxval_f, val_f;
    int8_t maxvals_s8[17];
    
    //get idx & value which is max. 
    for(int i=0; i<NUM_POSE_KEYPOINTS; i++)
    {    

        preds_f[i].x = 0;
        preds_f[i].y = 0;
        maxvals_s8[i] = heatmaps[i];
        // maxval_f = (((float)heatmaps[i]) - zero_point)*scale;
        // maxvals[i] = heatmaps[i*HEATMAP_WIDTH*HEATMAP_HEIGHT];

        for(int y=0; y<HEATMAP_HEIGHT; y++)
        {
            
            for(int x=0; x<HEATMAP_WIDTH; x++)
            {
                int8_t val_8 = heatmaps[y*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + x*NUM_POSE_KEYPOINTS + i];
                // val_f = (((float)heatmaps[y*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + x*NUM_POSE_KEYPOINTS + i]) - zero_point)*scale;

                // if(maxval_f < val_f)
                if(maxvals_s8[i] < val_8)
                {
                        maxvals_s8[i] = val_8;
                        preds_f[i].x = x;
                        preds_f[i].y = y;
                }
            }
        }   
    }

    for(int k=0; k<NUM_POSE_KEYPOINTS; k++)
    {
        uint32_t px = (uint32_t)preds_f[k].x;
        uint32_t py = (uint32_t)preds_f[k].y;
        if( 1<px && px<(HEATMAP_WIDTH -1) &&
            1<py && py<(HEATMAP_HEIGHT -1))
        {
            float diff_x, diff_y;
            float xl, xh, yl, yh;
            float v2, v1;
            xh = heatmaps[py*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + (px+1)*NUM_POSE_KEYPOINTS + k];
            xl = heatmaps[py*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + (px-1)*NUM_POSE_KEYPOINTS + k];
            diff_x = (DE_QUNT(xh, zero_point, scale)) - (DE_QUNT(xl, zero_point, scale));
            // v2 = dequantify_val(xh, zero_point, scale);
            // v1 = dequantify_val(xl, zero_point, scale);
            // diff_x = v2 - v1;
            preds_f[k].x = (diff_x>0)? (preds_f[k].x+0.25) : (preds_f[k].x-0.25);

            yh = heatmaps[(py+1)*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + px*NUM_POSE_KEYPOINTS + k];
            yl = heatmaps[(py-1)*HEATMAP_WIDTH*NUM_POSE_KEYPOINTS + px*NUM_POSE_KEYPOINTS + k];
            diff_y = (DE_QUNT(yh, zero_point, scale)) - (DE_QUNT(yl, zero_point, scale));
            // v2 = dequantify_val(yh, zero_point, scale);
            // v1 = dequantify_val(yl, zero_point, scale);
            // diff_y = v2 - v1;            
            preds_f[k].y = (diff_y>0)? (preds_f[k].y+0.25) : (preds_f[k].y-0.25);
            // maxvals_f[k] = DE_QUNT(maxvals_s8[k], zero_point, scale);
            alg->hpr[k].score = (float)DE_QUNT(maxvals_s8[k], zero_point, scale);

        }
    }
}
// void transform_preds(struct_position* preds, struct_position_f* preds_f, struct__box* bbox, uint32_t out_w, uint32_t out_h)
void transform_preds(struct_hp_algoResult* alg, struct_position_f* preds_f, uint32_t out_w, uint32_t out_h)
{

    float scale_x = (float)alg->bbox.width / out_w;
    float scale_y = (float)alg->bbox.height / out_h;

    for(int i=0; i<NUM_POSE_KEYPOINTS; i++)
    {
        alg->hpr[i].x = (uint32_t)(preds_f[i].x * scale_x + alg->bbox.x);
        alg->hpr[i].y = (uint32_t)(preds_f[i].y * scale_y + alg->bbox.y);
    }
}

// void keypoints_from_heatmaps(int8_t* heatmaps, int32_t zero_point, float scale, struct__box* bbox, struct_position* preds, float* maxvals)
void keypoints_from_heatmaps(int8_t* heatmaps, int32_t zero_point, float scale, struct_hp_algoResult *alg)
{
    // get_max_preds(heatmaps, zero_point, scale, &(loc_f[0]), maxvals);
    // // transform_preds(preds, bbox, HEATMAP_WIDTH, HEATMAP_HEIGHT, img_width, img_height);
    // transform_preds(preds, &(loc_f[0]), bbox, HEATMAP_WIDTH, HEATMAP_HEIGHT);
    get_max_preds(heatmaps, zero_point, scale, &(loc_f[0]), alg);
    transform_preds(alg, &(loc_f[0]), HEATMAP_WIDTH, HEATMAP_HEIGHT);
}
