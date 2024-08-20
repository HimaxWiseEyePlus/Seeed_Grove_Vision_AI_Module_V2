#ifndef SCENARIO_KWS_CVAPP
#define SCENARIO_KWS_CVAPP

#include "spi_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

int cv_kws_init(bool security_enable, bool privilege_enable, uint32_t model_addr);

int cv_kws_run(struct_kws_algoResult *algoresult_kws_pdm_record, int16_t *audio_buf, int32_t audio_clip_length, void (*callback)(void));

int cv_kws_deinit();

#ifdef __cplusplus
}
#endif

#endif