#ifndef YOLO_POSTPROCESSING_H
#define YOLO_POSTPROCESSING_H

#include <stdint.h>
#include <forward_list>

typedef struct boxabs {
    float left, right, top, bot;
} boxabs;


typedef struct branch {
    int resolution;
    int num_box;
    float *anchor;
    int8_t *tf_output;
    float scale;
    int zero_point;
    size_t size;
    float scale_x_y = 1.0;
} branch;

typedef struct network {
    int input_w;
    int input_h;
    int num_classes;
    int num_branch;
    branch *branchs;
    int topN;
} network;


typedef struct box {
    float x, y, w, h;
} box;

typedef struct detection{
    box bbox;
    float *prob;
    float objectness;
} detection;

branch create_brach(int resolution, int num_box, float *anchor, int8_t *tf_output, size_t size, float scale, int zero_point);
network creat_network(int input_w, int input_h, int num_classes, int num_branch, branch* branchs, int topN);

std::forward_list<detection> get_network_boxes(network *net, int image_w, int image_h, float thresh, int *num);

void do_nms_sort(std::forward_list<detection> &dets, int classes, float thresh);
void diounms_sort(std::forward_list<detection> &dets, int classes, float thresh);

void free_dets(std::forward_list<detection> &dets);
float sigmoid(float x);
float box_iou(box a, box b);
#endif
