/* Edge Impulse ingestion SDK
 * Copyright (c) 2024 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cstring>
#include "ei_microphone.h"
#include "ei_device_we2.h"
#include "firmware-sdk/sensor-aq/sensor_aq.h"
#include "firmware-sdk/at_base64_lib.h"
#include "firmware-sdk/sensor-aq/sensor_aq_none.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#ifdef __cplusplus
extern "C" {
    #include "hx_drv_pdm_rx.h"
};
#endif
#include "hx_drv_scu.h"


// ingestion mic buffer can be quite big as we may collect a lot of samples
#define INGESTION_SAMPLE_BLOCKS_NUM         (8)     // nyumber of DMA blocks
#define INGESTION_SAMPLE_BLOCK_SIZE_BYTES   4096    // max block size is 8kB (due to uDMA limitations), so, let's use 4kB
#define INGESTION_SAMPLE_BLOCK_SIZE_SAMPLES (INGESTION_SAMPLE_BLOCK_SIZE_BYTES / sizeof(microphone_sample_t))
#define INGESTION_BUFFER_SIZE_BYTES         (INGESTION_SAMPLE_BLOCKS_NUM * INGESTION_SAMPLE_BLOCK_SIZE_BYTES)
#define INGESTION_BUFFER_SIZE_SAMPLES       (INGESTION_SAMPLE_BLOCKS_NUM * INGESTION_SAMPLE_BLOCK_SIZE_SAMPLES)
// inference mic buffer has to be quite small, so we can collect small chunks and fill the inference buffer/window one by one
// rather than collect a lot of samples at once and then fill two inference windows at once
// 4kB is 2000 samples, which is 125ms at 16kHz. There are 2 inference buffers, so any window >62.5 ms is fine
#define INFERENCE_SAMPLE_BLOCKS_NUM         (4)     // number of DMA blocks
#define INFERENCE_SAMPLE_BLOCK_SIZE_BYTES   1024    // max block size is 8kB (due to uDMA limitations), so, let's use 1kB
#define INFERENCE_SAMPLE_BLOCK_SIZE_SAMPLES (INFERENCE_SAMPLE_BLOCK_SIZE_BYTES / sizeof(microphone_sample_t))
#define INFERENCE_BUFFER_SIZE_BYTES         (INFERENCE_SAMPLE_BLOCKS_NUM * INFERENCE_SAMPLE_BLOCK_SIZE_BYTES)
#define INFERENCE_BUFFER_SIZE_SAMPLES       (INFERENCE_SAMPLE_BLOCKS_NUM * INFERENCE_SAMPLE_BLOCK_SIZE_SAMPLES)

/** Status and control struct for inferencing struct */
typedef struct {
    microphone_sample_t *buffers[2];
    uint8_t buf_select;
} mic_driver_buffer_t;

typedef struct {
    microphone_sample_t *buffers[2];
    uint8_t buf_select;
    uint8_t buf_ready;
    uint32_t buf_count; /** number of samples already in selected buffer */
    uint32_t n_samples; /** required number of samples, constant defined by model */
} inference_buffer_t;

static size_t ei_write(const void *buffer, size_t size, size_t count, EI_SENSOR_AQ_STREAM *);
static int ei_seek(EI_SENSOR_AQ_STREAM *, long int offset, int origin);

static mic_driver_buffer_t mic_buffer;
static inference_buffer_t inference_buffer;
// number of bytes required to be collected for ingestion
static uint32_t required_bytes;
// number of bytes collected so far for ingestion
static uint32_t collected_bytes;
static uint32_t headerOffset = 0;
volatile bool dma_active = false;

PDM_DEV_INFO pdm_dev_info;

static unsigned char ei_mic_ctx_buffer[1024];
static sensor_aq_signing_ctx_t ei_mic_signing_ctx;
static sensor_aq_ctx ei_mic_ctx = {
    { ei_mic_ctx_buffer, 1024 },
    &ei_mic_signing_ctx,
    &ei_write,
    &ei_seek,
    NULL,
};

/* Dummy functions for sensor_aq_ctx type */
static size_t ei_write(const void*, size_t size, size_t count, EI_SENSOR_AQ_STREAM*)
{
    return count;
}

static int ei_seek(EI_SENSOR_AQ_STREAM*, long int offset, int origin)
{
    return 0;
}

static int insert_ref(char *buffer, int hdrLength)
{
    #define EXTRA_BYTES(a)  ((a & 0x3) ? 4 - (a & 0x3) : (a & 0x03))
    const char *ref = "Ref-BINARY-i16";
    int addLength = 0;
    int padding = EXTRA_BYTES(hdrLength);

    buffer[addLength++] = 0x60 + 14 + padding;
    for(unsigned int i = 0; i < strlen(ref); i++) {
        buffer[addLength++] = *(ref + i);
    }
    for(int i = 0; i < padding; i++) {
        buffer[addLength++] = ' ';
    }

    buffer[addLength++] = 0xFF;

    return addLength;
}

static bool create_header(sensor_aq_payload_info *payload)
{
    int ret;
    EiDeviceWE2* dev = static_cast<EiDeviceWE2*>(EiDeviceWE2::get_device());
    EiDeviceMemory* mem = dev->get_memory();
    sensor_aq_init_none_context(&ei_mic_signing_ctx);

    ret = sensor_aq_init(&ei_mic_ctx, payload, NULL, true);

    if (ret != AQ_OK) {
        ei_printf("sensor_aq_init failed (%d)\n", ret);
        return false;
    }

    // then we're gonna find the last byte that is not 0x00 in the CBOR buffer.
    // That should give us the whole header
    size_t end_of_header_ix = 0;
    for (size_t ix = ei_mic_ctx.cbor_buffer.len - 1; ix != 0; ix--) {
        if (((uint8_t *)ei_mic_ctx.cbor_buffer.ptr)[ix] != 0x0) {
            end_of_header_ix = ix;
            break;
        }
    }

    if (end_of_header_ix == 0) {
        ei_printf("Failed to find end of header\n");
        return false;
    }

    int ref_size = insert_ref(((char*)ei_mic_ctx.cbor_buffer.ptr + end_of_header_ix), end_of_header_ix);
    // and update the signature
    ret = ei_mic_ctx.signature_ctx->update(ei_mic_ctx.signature_ctx, (uint8_t*)(ei_mic_ctx.cbor_buffer.ptr + end_of_header_ix), ref_size);
    if (ret != 0) {
        ei_printf("Failed to update signature from header (%d)\n", ret);
        return false;
    }
    end_of_header_ix += ref_size;

    // Write to blockdevice
    ret = mem->write_sample_data((uint8_t*)ei_mic_ctx.cbor_buffer.ptr, 0, end_of_header_ix);
    if ((size_t)ret != end_of_header_ix) {
        ei_printf("Failed to write to header blockdevice (%d)\n", ret);
        return false;
    }

    headerOffset = end_of_header_ix;

    return true;
}

bool ei_microphone_init(void)
{
    int ret;

    // select pin mux for PDM, enable PDM CLK(PB9) and PDM DATA0 pin(PB10)
    hx_drv_scu_set_PB9_pinmux(SCU_PB9_PINMUX_PDM_CLK_13, 1);
    hx_drv_scu_set_PB10_pinmux(SCU_PB10_PINMUX_PDM_DATA0_13, 1);

    // setup PDM configuration
    pdm_dev_info.pdm_cfg.reg_addr = HIMAX_PDM_BASE_ADDR;
    pdm_dev_info.pdm_cfg.rx_fifo_threshold = 5;
    pdm_dev_info.pdm_cfg.oversampling_ratio = 8; // 6: 32KHz, 8: 16KHz
    pdm_dev_info.pdm_cfg.cic_stages = 0;
    pdm_dev_info.pdm_cfg.cic_delay = 0;
    pdm_dev_info.pdm_cfg.dc_removal = 6;
    pdm_dev_info.pdm_cfg.bit_range_shift = 5;

    // use DATA0 with left channnel selected only(mono)
    pdm_dev_info.pdm_cfg.data_in_0_en = PDM_DATA_IN_ENABLE;
    pdm_dev_info.pdm_cfg.data_in_1_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.data_in_2_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.data_in_3_en = PDM_DATA_IN_DISABLE;
    pdm_dev_info.pdm_cfg.capture_channel = PDM_CAPTURE_CHANNEL_LEFT_ONLY;

    // sampling rate = 16K, use DMA2 channel 0
    pdm_dev_info.pdm_cfg.dma_ch = PDM_USE_DMA2_CH_0;
    pdm_dev_info.pdm_cfg.pdm_clk_src = PDM_CLKSRC_LSCREF;
    pdm_dev_info.pdm_cfg.sample_rate = PDM_PCM_FREQ_16K;
    pdm_dev_info.pdm_err_cb = NULL;

    return true;
}

static void ingestion_samples_callback(uint32_t status)
{
    EiDeviceWE2* dev = static_cast<EiDeviceWE2*>(EiDeviceInfo::get_device());
    EiDeviceMemory* mem = dev->get_memory();
    uint32_t chunk_size;

    if (!dma_active) {
        hx_drv_pdm_deinit();
        free(mic_buffer.buffers[0]);
        free(mic_buffer.buffers[1]);
        return;
    }

    // switch buffer and continue sampling
    mic_buffer.buf_select ^= 1;
    hx_drv_pdm_dma_lli_transfer((void *)mic_buffer.buffers[mic_buffer.buf_select], INGESTION_SAMPLE_BLOCKS_NUM, INGESTION_SAMPLE_BLOCK_SIZE_BYTES, 0);

    // calculate how many bytes we can write to memory
    chunk_size = (collected_bytes + INGESTION_SAMPLE_BLOCK_SIZE_BYTES > required_bytes) ?
        (required_bytes - collected_bytes) :
        INGESTION_BUFFER_SIZE_BYTES;

    // write already collected samples to memory
    mem->write_sample_data((const uint8_t*)mic_buffer.buffers[mic_buffer.buf_select ^ 1], headerOffset + collected_bytes, chunk_size);

    // update data hash
    ei_mic_ctx.signature_ctx->update(ei_mic_ctx.signature_ctx, (uint8_t*)mic_buffer.buffers[mic_buffer.buf_select ^ 1], chunk_size);

    // update number of collected bytes
    collected_bytes += chunk_size;
}

bool ei_microphone_sample_start(void)
{
    EiDeviceWE2* dev = static_cast<EiDeviceWE2*>(EiDeviceInfo::get_device());
    EiDeviceMemory* mem = dev->get_memory();

    uint8_t *page_buffer;
    int ret;
    uint32_t required_samples;

    sensor_aq_payload_info payload = {
        dev->get_device_id().c_str(),
        dev->get_device_type().c_str(),
        dev->get_sample_interval_ms(),
        { { "audio", "wav" } }
    };

    ei_printf("Sampling settings:\n");
    ei_printf("\tInterval: ");
    ei_printf_float(dev->get_sample_interval_ms());
    ei_printf(" ms.\n");
    ei_printf("\tLength: %lu ms.\n", dev->get_sample_length_ms());
    ei_printf("\tName: %s\n", dev->get_sample_label().c_str());
    ei_printf("\tHMAC Key: %s\n", dev->get_sample_hmac_key().c_str());
    ei_printf("\tFile name: %s\n", dev->get_sample_label().c_str());

    required_samples = (uint32_t)((dev->get_sample_length_ms()) / dev->get_sample_interval_ms());

    /* Round to even number of samples for word align flash write */
    if(required_samples & 1) {
        required_samples++;
    }

    required_bytes = required_samples * sizeof(microphone_sample_t);
    collected_bytes = 0;

    if(required_bytes > mem->get_available_sample_bytes()) {
        ei_printf("ERR: Sample length is too long. Maximum allowed is %lu ms at 16000 Hz.\r\n",
            ((mem->get_available_sample_bytes() / (16000 * sizeof(microphone_sample_t))) * 1000));
        return false;
    }

    dev->set_state(eiStateErasingFlash);

    // Minimum delay of 2000 ms for daemon
    uint32_t delay_time_ms = ((required_bytes / mem->block_size) + 1) * mem->block_erase_time;
    ei_printf("Starting in %lu ms... (or until all flash was erased)\n", delay_time_ms < 2000 ? 2000 : delay_time_ms);

    if(mem->erase_sample_data(0, required_bytes) != (required_bytes)) {
        return false;
    }

    // if erasing took less than 2 seconds, wait additional time
    if(delay_time_ms < 2000) {
        ei_sleep(2000 - delay_time_ms);
    }

    if (create_header(&payload) == false) {
        return false;
    }

    while(dma_active) {
        ei_printf("Waiting for previous DMA transfer to finish...\n");
        ei_sleep(250);
    }

    pdm_dev_info.pdm_rx_cb = ingestion_samples_callback;
    hx_drv_pdm_init(&pdm_dev_info);
    if ( ret != PDM_NO_ERROR ) {
        ei_printf("ERR: Failed to initialize PDM\n");
        return false;
    }

    mic_buffer.buffers[0] = (microphone_sample_t*)aligned_alloc(32, INGESTION_BUFFER_SIZE_BYTES);
    if(mic_buffer.buffers[0] == NULL) {
        ei_printf("ERR: Failed to allocate audio buffer 0\n");
        return false;
    }

    mic_buffer.buffers[1] = (microphone_sample_t*)aligned_alloc(32, INGESTION_BUFFER_SIZE_BYTES);
    if(mic_buffer.buffers[1] == NULL) {
        ei_printf("ERR: Failed to allocate audio buffer 1\n");
        ei_free(mic_buffer.buffers[0]);
        return false;
    }

    mic_buffer.buf_select = 0;

    ei_printf("Sampling...\n");

    dev->set_state(eiStateSampling);

    dma_active = true;

    hx_drv_pdm_dma_lli_transfer((void *)mic_buffer.buffers[mic_buffer.buf_select], INGESTION_SAMPLE_BLOCKS_NUM, INGESTION_SAMPLE_BLOCK_SIZE_BYTES, 0);

    while (collected_bytes < required_bytes) {
        ei_sleep(10);
    }

    dma_active = false;

    ret = ei_mic_ctx.signature_ctx->finish(ei_mic_ctx.signature_ctx, ei_mic_ctx.hash_buffer.buffer);
    if (ret != 0) {
        ei_printf("Failed to finish signature (%d)\n", ret);
        return false;
    }

    // load the first page in flash...
    page_buffer = (uint8_t*)ei_malloc(mem->block_size);
    if (!page_buffer) {
        ei_printf("Failed to allocate a page buffer to write the hash\n");
        return false;
    }

    ret = mem->read_sample_data(page_buffer, 0, mem->block_size);
    if ((uint32_t)ret != mem->block_size) {
        ei_printf("Failed to read first page (read %d, requersted %lu)\n", ret, mem->block_size);
        ei_free(page_buffer);
        return false;
    }

    // update the hash
    uint8_t *hash = ei_mic_ctx.hash_buffer.buffer;
    // we have allocated twice as much for this data (because we also want to be able to represent in hex)
    // thus only loop over the first half of the bytes as the signature_ctx has written to those
    for (size_t hash_ix = 0; hash_ix < ei_mic_ctx.hash_buffer.size / 2; hash_ix++) {
        // this might seem convoluted, but snprintf() with %02x is not always supported e.g. by newlib-nano
        // we encode as hex... first ASCII char encodes top 4 bytes
        uint8_t first = (hash[hash_ix] >> 4) & 0xf;
        // second encodes lower 4 bytes
        uint8_t second = hash[hash_ix] & 0xf;

        // if 0..9 -> '0' (48) + value, if >10, then use 'a' (97) - 10 + value
        char first_c = first >= 10 ? 87 + first : 48 + first;
        char second_c = second >= 10 ? 87 + second : 48 + second;

        page_buffer[ei_mic_ctx.signature_index + (hash_ix * 2) + 0] = first_c;
        page_buffer[ei_mic_ctx.signature_index + (hash_ix * 2) + 1] = second_c;
    }

    ret = mem->erase_sample_data(0, mem->block_size);
    if ((uint32_t)ret != mem->block_size) {
        ei_printf("Failed to erase first page (read %d, requested %lu)\n", ret, mem->block_size);
        ei_free(page_buffer);
        return false;
    }

    ret = mem->write_sample_data(page_buffer, 0, mem->block_size);
    ei_free(page_buffer);

    if ((uint32_t)ret != mem->block_size) {
        ei_printf("Failed to write first page with updated hash (read %d, requested %lu)\n", ret, mem->block_size);
        return false;
    }

    ei_printf("Done sampling, total bytes collected: %lu\n", required_bytes);
    ei_printf("[1/1] Uploading file to Edge Impulse...\n");
    ei_printf("Not uploading file, not connected to WiFi. Used buffer, from=0, to=%lu.\n", required_bytes + headerOffset);
    ei_printf("OK\n");

    return true;
}

static void inference_samples_callback(uint32_t status)
{
    if(dma_active == false) {
        // this will happen after the last DMA transfer
        hx_drv_pdm_deinit();
        ei_free(mic_buffer.buffers[0]);
        ei_free(mic_buffer.buffers[1]);
        return;
    }

    // switch buffer and continue sampling
    mic_buffer.buf_select ^= 1;
    hx_drv_pdm_dma_lli_transfer((void *)mic_buffer.buffers[mic_buffer.buf_select], INFERENCE_SAMPLE_BLOCKS_NUM, INFERENCE_SAMPLE_BLOCK_SIZE_BYTES, 0);

    if (inference_buffer.n_samples - inference_buffer.buf_count <= INFERENCE_BUFFER_SIZE_SAMPLES) {
        const uint32_t remaining_samples = INFERENCE_BUFFER_SIZE_SAMPLES - (inference_buffer.n_samples - inference_buffer.buf_count);
        ei_printf("Inference buffer is full (%d)\n", remaining_samples);
        // we can't fit all samples from mic buffer into inference buffer, so:
        // 1. copy as much as we can to the current inference buffer
        std::memcpy(inference_buffer.buffers[inference_buffer.buf_select],
                    mic_buffer.buffers[mic_buffer.buf_select ^ 1],
                    (inference_buffer.n_samples - inference_buffer.buf_count) * sizeof(microphone_sample_t));
        // 2. switch inference buffer and mark it as ready
        inference_buffer.buf_select ^= 1;
        inference_buffer.buf_ready = 1;
        // 3. copy the remaining samples to the new inference buffer
        // if inference_buffer.n_samples - inference_buffer.buf_count == INFERENCE_BUFFER_SIZE_SAMPLES then
        // we won't copy anything here, but the previous inference buffer is already marked as ready
        std::memcpy(inference_buffer.buffers[inference_buffer.buf_select],
                    mic_buffer.buffers[mic_buffer.buf_select ^ 1],
                    remaining_samples * sizeof(microphone_sample_t));
        inference_buffer.buf_count = remaining_samples;

    } else {
        // we have more space in the inference buffer than samples in the mic buffer
        // so copy all of them
        std::memcpy(inference_buffer.buffers[inference_buffer.buf_select] + inference_buffer.buf_count,
                    mic_buffer.buffers[mic_buffer.buf_select ^ 1],
                    INFERENCE_SAMPLE_BLOCK_SIZE_SAMPLES * sizeof(microphone_sample_t));
        inference_buffer.buf_count += INFERENCE_SAMPLE_BLOCK_SIZE_SAMPLES;
    }
}

int ei_microphone_inference_get_data(size_t offset, size_t length, float *out_ptr)
{
    return ei::numpy::int16_to_float(&inference_buffer.buffers[inference_buffer.buf_select ^ 1][offset], out_ptr, length);
}

bool ei_microphone_inference_start(uint32_t n_samples, float interval_ms)
{
    int ret;

    while(dma_active) {
        ei_printf("Waiting for previous DMA transfer to finish...\n");
        ei_sleep(250);
    }

    // init PDM driver
    pdm_dev_info.pdm_rx_cb = inference_samples_callback;
    ret = hx_drv_pdm_init(&pdm_dev_info);
    if ( ret != PDM_NO_ERROR ) {
        ei_printf("ERR: Failed to initialize PDM (%d)\n", ret);
        return false;
    }

    ei_printf("Allocating buffers: %d bytes\n", 2 * n_samples * sizeof(microphone_sample_t));
    // allocate inference buffers
    inference_buffer.buffers[0] = (microphone_sample_t*)ei_malloc(n_samples * sizeof(microphone_sample_t));
    if(inference_buffer.buffers[0] == NULL) {
        ei_printf("ERR: Failed to allocate inference buffer 0\n");
        return false;
    }

    inference_buffer.buffers[1] = (microphone_sample_t*)ei_malloc(n_samples * sizeof(microphone_sample_t));
    if(inference_buffer.buffers[1] == NULL) {
        ei_printf("ERR: Failed to allocate inference buffer 1\n");
        ei_free(inference_buffer.buffers[0]);
        return false;
    }

    inference_buffer.buf_select = 0;
    inference_buffer.buf_count  = 0;
    inference_buffer.n_samples  = n_samples;
    inference_buffer.buf_ready  = 0;

    // allocate mic buffers but for inference purposes
    mic_buffer.buffers[0] = (microphone_sample_t*)aligned_alloc(32, INFERENCE_BUFFER_SIZE_BYTES);
    if(mic_buffer.buffers[0] == NULL) {
        ei_printf("ERR: Failed to allocate audio buffer 0\n");
        ei_free(inference_buffer.buffers[0]);
        ei_free(inference_buffer.buffers[1]);
        return false;
    }

    mic_buffer.buffers[1] = (microphone_sample_t*)aligned_alloc(32, INFERENCE_BUFFER_SIZE_BYTES);
    if(mic_buffer.buffers[1] == NULL) {
        ei_printf("ERR: Failed to allocate audio buffer 1\n");
        ei_free(inference_buffer.buffers[0]);
        ei_free(inference_buffer.buffers[1]);
        ei_free(mic_buffer.buffers[0]);
        return false;
    }

    mic_buffer.buf_select = 0;

    dma_active = true;

    // start sampling
    hx_drv_pdm_dma_lli_transfer((void *)mic_buffer.buffers[mic_buffer.buf_select], INFERENCE_SAMPLE_BLOCKS_NUM, INFERENCE_SAMPLE_BLOCK_SIZE_BYTES, 0);

    return true;
}

bool ei_microphone_inference_is_recording(void)
{
    return inference_buffer.buf_ready == 0;
}

void ei_microphone_inference_reset_buffers(void)
{
    inference_buffer.buf_ready = 0;
    inference_buffer.buf_count = 0;
}

bool ei_microphone_inference_end(void)
{
    int ret;

    dma_active = false;

    // mic buffer is being freed in the inference callback function (after last DMA transfer)
    ei_free(inference_buffer.buffers[0]);
    ei_free(inference_buffer.buffers[1]);

    return true;
}
