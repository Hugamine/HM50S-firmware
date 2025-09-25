//
// Created by x on 5/31/2025.
//

// #include "hilbert.h"
// #include "audio.h"
// #include "ui.h"
// #include <math.h>
// #include <stdint.h>

// #define HILBERT_TAPS 65  // 64阶

// const float32_t hilbert_96k_taps[HILBERT_TAPS] = {
//     // 前32个系数 (索引0~31)
//     0.0f,    -0.0011f, -0.0020f, -0.0028f, -0.0034f,
//     -0.0036f, -0.0032f, -0.0022f, -0.0006f,  0.0015f,
//     0.0041f,  0.0068f,  0.0093f,  0.0111f,  0.0118f,
//     0.0108f,  0.0078f,  0.0028f,  -0.0040f, -0.0120f,
//     -0.0205f, -0.0284f, -0.0345f, -0.0374f, -0.0360f,
//     -0.0296f, -0.0177f, -0.0008f,  0.0195f,  0.0416f,
//     0.0640f,  0.0847f,
//     // 中心抽头 (索引32 = 0)
//     0.0f,
//     // 后32个系数 (索引33~64, 奇对称)
//     -0.0847f, -0.0640f, -0.0416f, -0.0195f,  0.0008f,
//     0.0177f,  0.0296f,  0.0360f,  0.0374f,  0.0345f,
//     0.0284f,  0.0205f,  0.0120f,  0.0040f, -0.0028f,
//     -0.0078f, -0.0108f, -0.0118f, -0.0111f, -0.0093f,
//     -0.0068f, -0.0041f, -0.0015f,  0.0006f,  0.0022f,
//     0.0032f,  0.0036f,  0.0034f,  0.0028f,  0.0020f,
//     0.0011f,  0.0f  // 严格65值，满足 h[n] = -h[64-n]
// };

// arm_rfft_fast_instance_f32 rfft_instance;

// arm_fir_instance_f32 hilbert_fir;
// float32_t hilbert_state[HILBERT_TAPS + FFT_LEN - 1];

// void fir_hilbert_init(void) {
//     arm_fir_init_f32(&hilbert_fir, HILBERT_TAPS, (float32_t *)hilbert_96k_taps, hilbert_state, FFT_LEN);
// }

// void rfft_init(void) {
//     arm_rfft_fast_init_f32(&rfft_instance, FFT_LEN);
// }


// // Apply window to both I and Q buffers (in-place)
// void apply_window(float *i_buf, float *q_buf, uint32_t length, uint8_t type) {
//     float sum_w = 0.0f;

//     for (uint32_t i = 0; i < length; i++) {
//         float w = 1.0f;
//         float n = (float)i / (float)(length - 1);

//         switch (type) {
//             case WINDOW_HAMMING:
//                 w = 0.54f - 0.46f * cosf(2.0f * M_PI * n);
//                 break;
//             case WINDOW_HANN:
//                 w = 0.5f * (1.0f - cosf(2.0f * M_PI * n));
//                 break;
//             case WINDOW_BLACKMAN:
//                 w = 0.42f - 0.5f * cosf(2.0f * M_PI * n) + 0.08f * cosf(4.0f * M_PI * n);
//                 break;
//             default:
//                 w = 1.0f; // Rectangular
//                 break;
//         }

//         sum_w += w;
//         i_buf[i] *= w;
//         if (q_buf) q_buf[i] *= w;
//     }

//     // Compensation: normalize window gain to preserve average amplitude
//     // float gain = (float)length / sum_w;
//     //
//     // for (uint32_t i = 0; i < length; i++) {
//     //     i_buf[i] *= gain;
//     //     if (q_buf) q_buf[i] *= gain;
//     // }
// }

// #define I0_MAX_ITER 20  // Accuracy for I₀ Bessel approximation

// // Approximate zeroth-order modified Bessel function of the first kind (I₀)
// static float bessel_i0(float x) {
//     float sum = 1.0f;
//     float term = 1.0f;
//     float half_x = x / 2.0f;
//     for (int k = 1; k < I0_MAX_ITER; k++) {
//         term *= (half_x * half_x) / (k * k);
//         sum += term;
//     }
//     return sum;
// }

// void apply_kaiser_window(float *i_buf, float *q_buf, uint32_t length, float beta) {
//     float denom = bessel_i0(beta);  // Precompute I₀(β)

//     for (uint32_t i = 0; i < length; i++) {
//         float r = 2.0f * (float)i / (length - 1) - 1.0f;
//         float w = bessel_i0(beta * sqrtf(1.0f - r * r)) / denom;

//         i_buf[i] *= w;
//         if (q_buf) q_buf[i] *= w;  // Optional for complex
//     }
// }


// void Hilbert_Transform(const arm_cfft_instance_f32 *cfft, float32_t *input, float32_t *outputI, float32_t *outputQ, float32_t *outputFFT)
// {
//     if (cfft->fftLen != FFT_LEN) return;

//     float32_t rfft_buf[FFT_LEN] = {0};
//     float32_t cfft_buf[FFT_LEN*2] = {0};

//     // apply_window(input, NULL, FFT_LEN, WINDOW_BLACKMAN);

//     arm_rfft_fast_f32(&rfft_instance, input, rfft_buf, 0);

//     for (int i = 0; i < FFT_LEN/16; i++) {
//         outputFFT[i] = rfft_buf[i];
//         cfft_buf[i] = rfft_buf[i]*2;
//     }

//     cfft_buf[0] = 0;
//     cfft_buf[1] = 0;

//     arm_cfft_f32(cfft, cfft_buf, 1, 1);

//     for(int i=0; i<FFT_LEN*2; i+=2)
//     {
//         outputI[i/2] = cfft_buf[i];
//         outputQ[i/2] = cfft_buf[i+1];
//     }
// }

// void Hilbert_Transform_R(const arm_cfft_instance_f32 *cfft, float32_t *inputI, float32_t *inputQ, float32_t *usb_time,
//                                                          float32_t *lsb_time, float32_t *usb_spectrum, float32_t *lsb_spectrum) {
//     if (cfft->fftLen != FFT_LEN) return;

//     float32_t hilbertQ[FFT_LEN];

//     float32_t usb_buf[FFT_LEN];
//     float32_t lsb_buf[FFT_LEN];

//     float32_t iq_data[FFT_LEN*2];  // interleaved complex: I,Q
//     // Fill with your demodulated I/Q baseband signal

//     // float win[FFT_LEN];
//     // for (int i = 0; i < FFT_LEN; i++) {
//     //     win[i] = 0.42f - 0.5f * cosf(2.0f * M_PI * i / (FFT_LEN - 1)) + 0.08f * cosf(4.0f * M_PI * i / (FFT_LEN - 1));
//     // }
//     // arm_mult_f32(inputI, win, inputI, FFT_LEN);
//     // arm_mult_f32(inputQ, win, inputQ, FFT_LEN);

//     // apply_window(inputI, inputQ, FFT_LEN, WINDOW_HANN);
//     apply_kaiser_window(inputI, inputQ, FFT_LEN, 6.0f);


//     for (int i = 0; i < FFT_LEN*2; i+=2) {
//         iq_data[i] = inputI[i/2];
//         iq_data[i+1] = -inputQ[i/2];
//     }

//     arm_cfft_f32(cfft, iq_data, 0, 1);

//     /*Only store Positive frequency (LSB) Then together divided by 2 (inluding Nyquist not DC)*/
//     for (int i = 0; i < FFT_LEN/2; i++) {
//         lsb_buf[2*i] = iq_data[2*i+2]/2;
//         lsb_buf[2*i+1] = iq_data[2*i+3]/2;
//     }
//     /*Only store Positive frequency (USB) Then together divided by 2 (inluding DC not Nyquist)*/
//     usb_buf[0] = iq_data[0]/2;
//     usb_buf[1] = iq_data[1]/2;
//     for (int i = 1; i < FFT_LEN/2; i++) {
//         usb_buf[2*i] = iq_data[FFT_LEN*2-i*2]/2;
//         usb_buf[2*i+1] = iq_data[FFT_LEN*2-i*2+1]/2;
//     }

//     memcpy(usb_spectrum, usb_buf, sizeof(usb_buf));
//     memcpy(lsb_spectrum, lsb_buf, sizeof(lsb_buf));


//     // for (int i = FFT_LEN/16 + 1; i < FFT_LEN; i++) {
//     //     usb_buf[i] = 0;
//     // }

//     arm_fir_f32(&hilbert_fir, inputQ, hilbertQ, FFT_LEN);
//     for (int i = 0; i < FFT_LEN; i++) {
//         usb_time[i] = inputI[i] - hilbertQ[i]; // USB
//         // output_lsb[i] = input_I[i] + hilbertQ[i]; // LSB
//     }


//     if (usb_time != NULL) {
//         // arm_rfft_fast_f32(&rfft_instance, usb_buf, usb_time, 1);
//     }

//     if (lsb_time != NULL) {
//         // Start from the back to avoid overwriting
//         for (int i = FFT_LEN - 4; i >= 0; i -= 2) {
//             lsb_buf[i + 2] = lsb_buf[i];       // Move real part
//             lsb_buf[i + 3] = lsb_buf[i + 1];   // Move imag part
//         }
//         lsb_buf[0] = iq_data[0]/2;
//         lsb_buf[1] = iq_data[1]/2;

//         arm_rfft_fast_f32(&rfft_instance, lsb_buf, lsb_time, 1);
//     }
// }

