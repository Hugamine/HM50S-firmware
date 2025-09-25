//
// Created by x on 6/19/2025.
//

#include "audio.h"
#include "i2s.h"
#include "adau1761.h"
#include "adf4351.h"
#include "stm32h7xx_hal.h"
#include "tlv320aic3204.h"
// #include "hilbert.h"

// float32_t audio_sine[AUDIO_BUF_LEN];
// float32_t dc_offset[AUDIO_BUF_LEN] = {0.8};

__attribute__((section(".ram_d2_section"))) int32_t i2s1_rx_buf[AUDIO_BUF_LEN];
__attribute__((section(".ram_d2_section"))) int32_t i2s1_tx_buf[AUDIO_BUF_LEN];
__attribute__((section(".ram_d2_section"))) int32_t i2s2_rx_buf[AUDIO_BUF_LEN];
__attribute__((section(".ram_d2_section"))) int32_t i2s2_tx_buf[AUDIO_BUF_LEN];

__attribute__((section(".ram_d2_section"))) int32_t i2s_dummy_buf[AUDIO_BUF_LEN] = {0};

// static volatile int32_t *inBufptr;
// static volatile int32_t *outBufptr;
// static volatile int32_t *inBufptr1;
// static volatile int32_t *outBufptr1;

static volatile int32_t *baseband_inputBufptr;

bool tx_en = false;

// static float32_t hilbert_outI[AUDIO_BUF_LEN/2];
// static float32_t hilbert_outQ[AUDIO_BUF_LEN/2];

// __attribute__((section(".ram_d2_section"))) float32_t fft_output[FFT_LEN];
__attribute__((section(".ram_d2_section"))) float32_t fft_output_usb[FFT_LEN/2];
__attribute__((section(".ram_d2_section"))) float32_t fft_output_lsb[FFT_LEN/2];

__attribute__((section(".ram_d2_section"))) int32_t baseband_usb[AUDIO_BUF_LEN/4];
__attribute__((section(".ram_d2_section"))) int32_t baseband_lsb[AUDIO_BUF_LEN/4];




void tx_i2s_dma_start(void){
    HAL_I2SEx_TransmitReceive_DMA(&hi2s1, (uint16_t *)i2s2_rx_buf, (uint16_t *)i2s1_rx_buf, AUDIO_BUF_LEN);
    HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t *)i2s_dummy_buf, (uint16_t *)i2s2_rx_buf, AUDIO_BUF_LEN);
}

void rx_i2s_dma_start(void){
    HAL_I2SEx_TransmitReceive_DMA(&hi2s1, (uint16_t *)i2s_dummy_buf, (uint16_t *)i2s1_rx_buf, AUDIO_BUF_LEN);
    // HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t *)i2s2_tx_buf, (uint16_t *)i2s2_rx_buf, AUDIO_BUF_LEN);
}

void i2s_dma_stop(void){
    HAL_I2S_DMAStop(&hi2s1);
    HAL_I2S_DMAStop(&hi2s2);
}

arm_rfft_fast_instance_f32 rfft_instance;

void rfft_init(void) {
    arm_rfft_fast_init_f32(&rfft_instance, FFT_LEN);
}

void apply_window(float *i_buf, float *q_buf, uint32_t length, uint8_t type) {
    float sum_w = 0.0f;

    for (uint32_t i = 0; i < length; i++) {
        float w = 1.0f;
        float n = (float)i / (float)(length - 1);

        switch (type) {
            case WINDOW_HAMMING:
                w = 0.54f - 0.46f * cosf(2.0f * M_PI * n);
                break;
            case WINDOW_HANN:
                w = 0.5f * (1.0f - cosf(2.0f * M_PI * n));
                break;
            case WINDOW_BLACKMAN:
                w = 0.42f - 0.5f * cosf(2.0f * M_PI * n) + 0.08f * cosf(4.0f * M_PI * n);
                break;
            default:
                w = 1.0f; // Rectangular
                break;
        }

        sum_w += w;
        i_buf[i] *= w;
        if (q_buf) q_buf[i] *= w;
    }
}

void baseband_fft(const volatile int32_t *input, float32_t *fft_output_usb, float32_t *fft_output_lsb){
    static float32_t audio_usb[AUDIO_BUF_LEN/4];
    static float32_t audio_lsb[AUDIO_BUF_LEN/4];

    for(int i=0; i<AUDIO_BUF_LEN/2; i+=2) {
        audio_usb[i/2] = (float32_t)(input[i+1] >> 8) / 8388608.0f; // 24bit转浮点
        audio_lsb[i/2] = (float32_t)(input[i] >> 8) / 8388608.0f;
    }

    apply_window(audio_usb, audio_lsb, FFT_LEN, WINDOW_BLACKMAN);

    float32_t fft_usb_buf[AUDIO_BUF_LEN/4];
    float32_t fft_lsb_buf[AUDIO_BUF_LEN/4];

    arm_rfft_fast_f32(&rfft_instance, audio_usb, fft_usb_buf, 0);
    arm_rfft_fast_f32(&rfft_instance, audio_lsb, fft_lsb_buf, 0);

    arm_cmplx_mag_f32(fft_usb_buf, fft_output_usb, FFT_LEN/2);
    arm_cmplx_mag_f32(fft_lsb_buf, fft_output_lsb, FFT_LEN/2);
}


// void processData(volatile int32_t *input, volatile int32_t *output)
// {
//     static float32_t input_right[AUDIO_BUF_LEN/4];
//     static float32_t hilbert_outI[AUDIO_BUF_LEN/4];
//     static float32_t hilbert_outQ[AUDIO_BUF_LEN/4];

//     // 提取右声道
//     for(int i=0; i<AUDIO_BUF_LEN/2; i+=2) {
//         input_right[i/2] = (float32_t)(input[i+1] >> 8) / 8388608.0f; // 24bit转浮点
//         // input_right[i/2] = audio_sine[i/2]; // 24bit转浮点
//     }

//     // 执行变换
//     Hilbert_Transform(&arm_cfft_sR_f32_len2048, input_right, hilbert_outI, hilbert_outQ, fft_output);

//     // 交叉填充声道
//     for(int i=0; i<AUDIO_BUF_LEN/4; i++) {
//         output[2*i]   = (int32_t)(hilbert_outI[i] * 8388608.0f) << 8; // 左：原信号
//         output[2*i+1] = (int32_t)(hilbert_outQ[i] * 8388608.0f) << 8; // 右：正交信号
//     }
// }

// void processData1(const volatile int32_t *input, volatile int32_t *output)
// {
//     static float32_t input_I[AUDIO_BUF_LEN/4];
//     static float32_t input_Q[AUDIO_BUF_LEN/4];
//     static float32_t usb_out[AUDIO_BUF_LEN/4];
//     static float32_t sb_out[AUDIO_BUF_LEN/4];

//     for(int i=0; i<AUDIO_BUF_LEN/2; i+=2) {
//         input_Q[i/2] = (float32_t)(input[i] >> 8) / 8388608.0f;
//         input_I[i/2] = (float32_t)(input[i+1] >> 8) / 8388608.0f;
//     }

//     // 执行变换
//     Hilbert_Transform_R(&arm_cfft_sR_f32_len2048, input_I, input_Q, usb_out, NULL,
//         fft_output_usb, fft_output_lsb);

//     // Hilbert_Transform(&arm_cfft_sR_f32_len2048, input_Q, NULL, sb_out, NULL);
//     //
//     // for (int i=0; i<FFT_LEN; i++) {
//     //     usb_out[i] = input_I[i];
//     // }
//     // 交叉填充声道
//     for(int i=0; i<AUDIO_BUF_LEN/4; i++) {
//         output[2*i+1] = (int32_t)(usb_out[i] * 8388608.0f) << 8; // 右
//         // output[2*i+1] = input[2*i+1];
//     }
// }

void i2s1_Tx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s1_Rx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {
    baseband_inputBufptr = &i2s1_rx_buf[0];
    baseband_fft(baseband_inputBufptr, fft_output_usb, fft_output_lsb);
}
void i2s1_Tx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s1_Rx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {
    baseband_inputBufptr = &i2s1_rx_buf[AUDIO_BUF_LEN/2];
    baseband_fft(baseband_inputBufptr, fft_output_usb, fft_output_lsb);
}
void i2s1_TxRx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {
    if(tx_en == false){
        baseband_inputBufptr = &i2s1_rx_buf[0];
        baseband_fft(baseband_inputBufptr, fft_output_usb, fft_output_lsb);
    } else if(tx_en == true){

    }
}
void i2s1_TxRx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {
    if(tx_en == false){
        baseband_inputBufptr = &i2s1_rx_buf[AUDIO_BUF_LEN/2];
        baseband_fft(baseband_inputBufptr, fft_output_usb, fft_output_lsb);
    } else if(tx_en == true){

    }
}
void i2s2_Tx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s2_Rx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s2_Tx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s2_Rx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {

}
void i2s2_TxRx_HalfCplt_Callback(I2S_HandleTypeDef *hi2s) {
    
}
void i2s2_TxRx_Cplt_Callback(I2S_HandleTypeDef *hi2s) {
    // inBufptr = &i2s3_rx_buf[AUDIO_BUF_LEN/2];
    // outBufptr = &i2s2_tx_buf[AUDIO_BUF_LEN/2];

    // processData(inBufptr, outBufptr);
}

void audio_init(void) {
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_HALF_COMPLETE_CB_ID, i2s1_Tx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_RX_HALF_COMPLETE_CB_ID, i2s1_Rx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_COMPLETE_CB_ID, i2s1_Tx_Cplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_RX_HALF_COMPLETE_CB_ID, i2s1_TxRx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_TX_RX_COMPLETE_CB_ID, i2s1_TxRx_Cplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s1, HAL_I2S_RX_COMPLETE_CB_ID, i2s1_Rx_Cplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_TX_HALF_COMPLETE_CB_ID, i2s2_Tx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_RX_HALF_COMPLETE_CB_ID, i2s2_Rx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_TX_COMPLETE_CB_ID, i2s2_Tx_Cplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_RX_COMPLETE_CB_ID, i2s2_Rx_Cplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_TX_RX_HALF_COMPLETE_CB_ID, i2s2_TxRx_HalfCplt_Callback);
    HAL_I2S_RegisterCallback(&hi2s2, HAL_I2S_TX_RX_COMPLETE_CB_ID, i2s2_TxRx_Cplt_Callback);

    for(int i = 0; i < AUDIO_BUF_LEN; i++) {
      i2s_dummy_buf[i] = 0;
    }
}

// void audio_start_dma(void) {
//     HAL_I2SEx_TransmitReceive_DMA(&hi2s1, (uint16_t *) i2s2_tx_buf, (uint16_t *) i2s2_rx_buf, AUDIO_BUF_LEN);
//     HAL_I2SEx_TransmitReceive_DMA(&hi2s2, (uint16_t *) i2s3_tx_buf, (uint16_t *) i2s3_rx_buf, AUDIO_BUF_LEN);
// }
