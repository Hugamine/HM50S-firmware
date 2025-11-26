//
// Created by x on 6/19/2025.
//

#ifndef AUDIO_H
#define AUDIO_H

#include "main.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include <stdbool.h>

#define AUDIO_BUF_LEN 8192
#define FFT_LEN AUDIO_BUF_LEN/4 //32 to 4096

#define WINDOW_HAMMING  1
#define WINDOW_HANN     2
#define WINDOW_BLACKMAN 3

extern bool tx_en;

// extern float32_t fft_output[FFT_LEN];
extern float32_t fft_output_usb[FFT_LEN/2];
extern float32_t fft_output_lsb[FFT_LEN/2];

void rfft_init(void);

void tx_i2s_dma_start(void);
void rx_i2s_dma_start(void);
void i2s_dma_stop(void);

void usb_start(void);
void usb_test(uint8_t *pbuf, uint32_t size);


void audio_init(void);
void audio_start_dma(void);

#endif //AUDIO_H
