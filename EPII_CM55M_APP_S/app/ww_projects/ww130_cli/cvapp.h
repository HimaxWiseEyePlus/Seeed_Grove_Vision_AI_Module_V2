/*
 * cvapp.h
 *
 *  Created on: 2018�~12��4��
 *      Author: 902452
 */

#ifndef APP_WW130_CLI_CVAPP_
#define APP_WW130_CLI_CVAPP_

#include "spi_protocol.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    // TBP - created ModelResuls struct to handle more dynamic model score results
    // So the next question is, we need a dynamic method to load the number of model tracked categories.
    // Probably run a loop somewhere that we treat each model category as a model_category[i].
    //
    // Something like:
    // typedef struct {
    //     int8_t* scores; // Pointer to dynamically allocated array of scores
    //     size_t num_categories; // Number of categories
    // } ModelResults;
    //
    // void initializeModelResults(ModelResults& model, size_t num_categories) {
    //     model.num_categories = num_categories;
    //     model.scores = (int8_t*)malloc(num_categories * sizeof(int8_t));
    //     for (size_t i = 0; i < num_categories; ++i) {
    //         model.scores[i] = i * 2; // Example initialization
    //     }
    // }
    typedef struct
    {
        int8_t rat_score;
        int8_t no_rat_score;
        int error_code; // Error code for model run, 0 = no error
    } ModelResults;

int cv_init(bool security_enable, bool privilege_enable, uint32_t model_addr);

ModelResults cv_run(ModelResults model_scores);

int cv_deinit();
#ifdef __cplusplus
}
#endif

#endif /* APP_WW130_CLI_CVAPP_ */
