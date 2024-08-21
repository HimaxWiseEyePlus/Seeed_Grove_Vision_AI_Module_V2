#ifndef KWS_PDM_RECORD_H
#define KWS_PDM_RECORD_H

#define APP_BLOCK_FUNC() do { \
    __asm volatile("b    ."); \
} while (0)

int kws_pdm_record_app(void);

#endif /* KWS_PDM_RECORD_H_ */
