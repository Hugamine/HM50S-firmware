#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "main.h"

#define SPECTRUM_WIDTH       320
#define SPECTRUM_HEIGHT      100
#define SPECTRUM_POINTS      320
#define WATERFALL_WIDTH      SPECTRUM_WIDTH
#define WATERFALL_HEIGHT     50
#define MAX_WATERFALL_ROWS   WATERFALL_HEIGHT   // 历史深度
#define UPDATE_INTERVAL_MS   1    // 刷新周期
#define MIN_DB               -40
#define MAX_DB               0
#define FULL_SCALE           150 //refer to arm_cmplx_mag_f32

typedef struct {
    uint16_t M;
    uint16_t k;
    uint16_t n;
} Frequency_disp;

void start_touch_calibration(void);
void calibration_touch_detected(void);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif //UI_H
