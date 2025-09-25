#include "ui.h"
#include "stm32h7xx_hal.h"
#include "xpt2046.h"
#include "math.h"
#include <ctype.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_palette.h>
#include <src/widgets/chart/lv_chart_private.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "w25qxx.h"
#include "adf4351.h"
#include "audio.h"

static lv_obj_t * calib_scr;
static lv_obj_t * info_label;
int pos_temp[4][2];//坐标缓存值
uint16_t d1,d2;
uint32_t tem1,tem2;
double fac; 
extern TouchScreen_Calibration ts_Calibration;
static lv_obj_t * calib_scr;
static lv_obj_t * info_label;
static lv_obj_t * dot_obj;
static int point_index = 0;
bool calibration_running;

// target positions (pixels)
static const lv_point_t target_points[4] = {
    {20, 20},  
    {300, 20},       
    {300, 220},       
    {20, 220}        
};

// ---- draw current calibration point ----
static void draw_point(void)
{
    lv_obj_clean(calib_scr);

    info_label = lv_label_create(calib_scr);
    lv_label_set_text(info_label, "Touch the red dot");
    lv_obj_align(info_label, LV_ALIGN_TOP_MID, 0, 10);

    dot_obj = lv_obj_create(calib_scr);
    lv_obj_set_size(dot_obj, 12, 12);
    lv_obj_set_style_bg_color(dot_obj, lv_color_hex(0xFF0000), 0);
    lv_obj_set_style_radius(dot_obj, LV_RADIUS_CIRCLE, 0);
    // lv_obj_align(dot_obj, LV_ALIGN_TOP_LEFT, target_points[point_index].x, target_points[point_index].y);
    lv_obj_set_pos(dot_obj, target_points[point_index].x-6, target_points[point_index].y-6);
}

// ---- called by your GPIO pen IRQ handler or polling loop ----
void calibration_touch_detected(void)
{
    // int raw_x, raw_y;

    if(!calibration_running) return;

    // read raw touch from hardware ADC
    xpt2046_update();
    pos_temp[point_index][0] = (int)ts_CoordinatesRaw.x;
    pos_temp[point_index][1] = (int)ts_CoordinatesRaw.y;
    point_index++;

    if(point_index < 4) {
            draw_point();
        } else {
            // compute calibration matrix from raw_points + target_points
            tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
			tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
			tem1*=tem1;
			tem2*=tem2;
			d1=sqrt(tem1+tem2);//得到1,2的距离
					
			tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
			tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
			tem1*=tem1;
			tem2*=tem2;
			d2=sqrt(tem1+tem2);//得到3,4的距离
			fac=(float)d1/d2;
			if(fac<0.95||fac>1.05||d1==0||d2==0)//不合格
			{
				// cnt=0;
 				// TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 			// TP_Drow_Touch_Point(20,20,RED);								//画点1
 				// TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
 				// continue;
                return;
			}
            tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
			tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
			tem1*=tem1;
			tem2*=tem2;
			d1=sqrt(tem1+tem2);//得到1,3的距离
			
			tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
			tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
			tem1*=tem1;
			tem2*=tem2;
			d2=sqrt(tem1+tem2);//得到2,4的距离
			fac=(float)d1/d2;
			if(fac<0.95||fac>1.05)//不合格
			{
				// cnt=0;
 				// TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 			// TP_Drow_Touch_Point(20,20,RED);								//画点1
 				// TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
				// continue;
                return;
			}//正确了
						   
			//对角线相等
			tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
			tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
			tem1*=tem1;
			tem2*=tem2;
			d1=sqrt(tem1+tem2);//得到1,4的距离
			tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
			tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
			tem1*=tem1;
			tem2*=tem2;
			d2=sqrt(tem1+tem2);//得到2,3的距离
			fac=(float)d1/d2;
			if(fac<0.95||fac>1.05)//不合格
			{
				// cnt=0;
 				// TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 			// TP_Drow_Touch_Point(20,20,RED);								//画点1
 				// TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//显示数据   
				// continue;
                return;
			}//正确了
			//计算结果
			ts_Calibration.xfactor=(float)(XPT2046_HEIGHT-40)/(pos_temp[0][0]-pos_temp[3][0]);//得到xfac		 
			ts_Calibration.xoffset=(XPT2046_HEIGHT-ts_Calibration.xfactor*(pos_temp[0][0]+pos_temp[3][0]))/2;//得到xoff
				  
			ts_Calibration.yfactor=(float)(XPT2046_WIDTH-40)/(pos_temp[2][1]-pos_temp[3][1]);//得到yfac
			ts_Calibration.yoffset=(XPT2046_WIDTH-ts_Calibration.yfactor*(pos_temp[2][1]+pos_temp[3][1]))/2;//得到yoff  
			if(abs(ts_Calibration.xfactor)>2||abs(ts_Calibration.yfactor)>2)//触屏和预设的相反了.
			{
				// cnt=0;
 				// TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//清除点4
   	 			// TP_Drow_Touch_Point(20,20,RED);								//画点1
				// LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
				// tp_dev.touchtype=!tp_dev.touchtype;//修改触屏类型.
				// if(tp_dev.touchtype)//X,Y方向与屏幕相反
				// {
				// 	CMD_RDX=0X90;
				// 	CMD_RDY=0XD0;	 
				// }else				   //X,Y方向与屏幕相同
				// {
				// 	CMD_RDX=0XD0;
				// 	CMD_RDY=0X90;	 
				// }			    
				// continue;
                return;
			}		
            uint8_t ID[2];
            W25Q_ReadFullID(ID);
	        if((ID[0]<<8|ID[1])==W25Q256){
                TP_Save_Adjdata();
            }
            calibration_running = false;

            lv_scr_load(lv_screen_active());  // go back to main GUI
        }
    
}

// ---- start calibration ----
void start_touch_calibration(void)
{
    calibration_running = true;
    point_index = 0;

    calib_scr = lv_obj_create(NULL);
    lv_scr_load(calib_scr);

    draw_point();
}

/*-------------------------------------------------------------------------------------------------*/

typedef struct {
    lv_obj_t *spectrum_chart;
    lv_chart_series_t *spectrum_series;
    lv_chart_series_t *peak_series;

    lv_obj_t *waterfall_canvas;
    lv_color_t *waterfall_buffer;

    lv_obj_t *status_label;
    lv_obj_t *freq_label;

    int current_row;
    bool running;
} sdr_ui_t;

Frequency_disp freq_disp = {50, 0, 0};

static sdr_ui_t sdr_ui;
static float peak_hold[SPECTRUM_POINTS] = {0};
static uint16_t peak_decay[SPECTRUM_POINTS] = {0};

static lv_obj_t *digit_kb;
static lv_obj_t *kb_container;
static lv_obj_t *input_display = NULL;

lv_obj_t *cont_main;

__attribute__((section(".ram_d2_section"))) float usb_db[FFT_LEN];
__attribute__((section(".ram_d2_section"))) float lsb_db[FFT_LEN];

static void create_spectrum(lv_obj_t *parent) {
    sdr_ui.spectrum_chart = lv_chart_create(parent);
    lv_obj_set_size(sdr_ui.spectrum_chart, SPECTRUM_WIDTH, SPECTRUM_HEIGHT);
    lv_obj_set_style_bg_color(sdr_ui.spectrum_chart, lv_color_black(), 0);

    //位置
    lv_obj_align(sdr_ui.spectrum_chart, LV_ALIGN_CENTER, 0, 5);
    lv_obj_set_style_pad_all(sdr_ui.spectrum_chart, 0, 0);
    lv_obj_set_style_radius(sdr_ui.spectrum_chart, 0, 0);

    lv_chart_set_update_mode(sdr_ui.spectrum_chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_obj_set_style_size(sdr_ui.spectrum_chart, 0, 0, LV_PART_INDICATOR);

    // 图表配置
    lv_chart_set_type(sdr_ui.spectrum_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(sdr_ui.spectrum_chart, SPECTRUM_POINTS);
    lv_chart_set_range(sdr_ui.spectrum_chart, LV_CHART_AXIS_PRIMARY_Y, MIN_DB, MAX_DB);

    // 样式设置
    // lv_obj_set_style_pad_all(sdr_ui.spectrum_chart, 5, LV_PART_MAIN);
    lv_obj_set_style_border_width(sdr_ui.spectrum_chart, 0, LV_PART_MAIN);
    lv_obj_set_style_border_color(sdr_ui.spectrum_chart, lv_color_hex(0x0000FF), LV_PART_MAIN);

    // 主数据系列
    sdr_ui.spectrum_series = lv_chart_add_series(sdr_ui.spectrum_chart,
                                               lv_palette_main(LV_PALETTE_YELLOW),
                                               LV_CHART_AXIS_PRIMARY_Y);

    // 峰值保持系列（初始隐藏）
    sdr_ui.peak_series = lv_chart_add_series(sdr_ui.spectrum_chart,
                                           lv_palette_main(LV_PALETTE_RED),
                                           LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_hide_series(sdr_ui.spectrum_chart, sdr_ui.peak_series, true);

    // 网格线
    // lv_obj_set_style_line_width(sdr_ui.spectrum_chart, 1, LV_PART_TICKS);
    // lv_obj_set_style_line_color(sdr_ui.spectrum_chart, lv_color_hex(0x303030), LV_PART_TICKS);
    lv_chart_set_div_line_count(sdr_ui.spectrum_chart, 5, 5);

    // 轴标签
    lv_obj_t * scale_dbm = lv_scale_create(sdr_ui.spectrum_chart);
    lv_scale_set_mode(scale_dbm, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_size(scale_dbm, 25, lv_pct(100));
    lv_scale_set_total_tick_count(scale_dbm, 2);
    lv_scale_set_major_tick_every(scale_dbm, 1);
    lv_obj_set_style_pad_hor(scale_dbm, lv_chart_get_first_point_center_offset(sdr_ui.spectrum_chart), 0);
    // lv_chart_set_axis_tick(sdr_ui.spectrum_chart, LV_CHART_AXIS_PRIMARY_Y,
    //                       10, 5, 6, 3, true, 60);

    // 频率标记（示例）
    // lv_coord_t mark_pos = SPECTRUM_POINTS / 2;
    // lv_point_t mark_points[] = {{mark_pos, MIN_DB}, {mark_pos, MAX_DB}};
    // lv_obj_t *mark_line = lv_line_create(sdr_ui.spectrum_chart);
    // lv_line_set_points(mark_line, mark_points, 2);
    // lv_obj_set_style_line_color(mark_line, lv_color_hex(0x808080), 0);
    // lv_obj_set_style_line_width(mark_line, 1, 0);
    //
    // lv_obj_t *mark_label = lv_label_create(sdr_ui.spectrum_chart);
    // lv_label_set_text(mark_label, "100MHz");
    // lv_obj_set_pos(mark_label, mark_pos - 30, MAX_DB - 10);
    // lv_obj_set_style_text_color(mark_label, lv_color_hex(0x808080), 0);
}

// 初始化瀑布图
static void create_waterfall(lv_obj_t *parent) {

    size_t buf_size = WATERFALL_WIDTH * MAX_WATERFALL_ROWS * sizeof(lv_color_t);
    // 分配内存缓冲区
    sdr_ui.waterfall_buffer = lv_malloc(buf_size);

    if(sdr_ui.waterfall_buffer == NULL) {
        LV_LOG_ERROR("Waterfall buffer allocation failed!");
        return;
    }

    // 创建Canvas
    sdr_ui.waterfall_canvas = lv_canvas_create(parent);
    lv_obj_set_size(sdr_ui.waterfall_canvas, WATERFALL_WIDTH, WATERFALL_HEIGHT);
    lv_canvas_set_buffer(sdr_ui.waterfall_canvas, sdr_ui.waterfall_buffer,
                        WATERFALL_WIDTH, MAX_WATERFALL_ROWS, LV_COLOR_FORMAT_RGB565);
    // lv_canvas_set_draw_buf(sdr_ui.waterfall_canvas,

    // 初始化为黑色
    lv_canvas_fill_bg(sdr_ui.waterfall_canvas, lv_color_make(0, 0, 0), LV_OPA_COVER);

    //位置
    lv_obj_align(sdr_ui.waterfall_canvas, LV_ALIGN_CENTER, 0, 80);

    // 启用垂直滚动
    // lv_obj_set_scroll_dir(sdr_ui.waterfall_canvas, LV_DIR_VER);
    // lv_obj_set_scroll_snap_y(sdr_ui.waterfall_canvas, LV_SCROLL_SNAP_START);
    // lv_obj_scroll_to_y(sdr_ui.waterfall_canvas, MAX_WATERFALL_ROWS - WATERFALL_HEIGHT, LV_ANIM_OFF);
}

// Update frequency display
static void update_freq_display(void) {
    lv_label_set_text_fmt(sdr_ui.freq_label, "%02d.%03d.%03d", freq_disp.M, freq_disp.k, freq_disp.n);
}

void set_frequency(Frequency_disp freq) {
    adf4351_update_frequency(freq);
    update_freq_display();
}

static char buffer[10] = {0};
static int idx = 0;
// Handle digit button press
static void digit_btn_event_cb(lv_event_t *e) {
    lv_obj_t * obj = lv_event_get_target_obj(e);
    const char *txt = lv_buttonmatrix_get_button_text(obj, lv_buttonmatrix_get_selected_button(obj));
    if (!txt || strlen(txt) == 0) return;

    if (strcmp(txt, "OK") == 0) {
        if (idx == 0 || buffer[0] == '\0') {
            // No input entered → exit keypad without changing frequency
            lv_obj_add_flag(kb_container, LV_OBJ_FLAG_HIDDEN);
            return;
        }

        int mhz = 0, khz = 0, hz = 0;
        sscanf(buffer, "%2d%3d%3d", &mhz, &khz, &hz);
        freq_disp.M = mhz;
        freq_disp.k = khz;
        freq_disp.n = hz;

        // Frequency bounds check
        if (freq_disp.M >= 54) {
            freq_disp.M = 54;
            freq_disp.k = 0;
            freq_disp.n = 0;
        }
        if (freq_disp.M < 50) {
            freq_disp.M = 50;
            freq_disp.k = 0;
            freq_disp.n = 0;
        }

        set_frequency(freq_disp);
        lv_obj_add_flag(kb_container, LV_OBJ_FLAG_HIDDEN);
        idx = 0;
        buffer[0] = '\0';
        return;
    }

    if (strcmp(txt, "DEL") == 0) {
        if (idx == 0) {
            // Buffer already empty → exit keypad
            lv_obj_add_flag(kb_container, LV_OBJ_FLAG_HIDDEN);
            return;
        }

        // Otherwise, delete the last digit
        buffer[--idx] = '\0';
        lv_textarea_set_text(input_display, buffer);
        return;
    }

    if (idx < 9 && isdigit(txt[0])) {
        buffer[idx++] = txt[0];
        buffer[idx] = '\0';
        lv_textarea_set_text(input_display, buffer);  // update display
    }
}

// Show keypad
static void show_digit_keypad(lv_event_t *e) {
    idx = 0;
    buffer[0] = '\0';
    lv_textarea_set_text(input_display, "");  // Clear display
    lv_obj_move_foreground(kb_container);
    lv_obj_clear_flag(kb_container, LV_OBJ_FLAG_HIDDEN);
}

#define PIXEL_TO_HZ_RATIO 300  // 1 pixel swipe = 100 Hz change

void adjust_frequency_by(int delta_hz) {
    int total_hz = freq_disp.M * 1000000 + freq_disp.k * 1000 + freq_disp.n + delta_hz;

    // Clamp to allowed range
    if (total_hz > 54000000) total_hz = 54000000;
    if (total_hz < 50000000) total_hz = 50000000;

    freq_disp.M = total_hz / 1000000;
    freq_disp.k = (total_hz / 1000) % 1000;
    freq_disp.n = total_hz % 1000;

    set_frequency(freq_disp);
}



static void spectrum_touch_event_cb(lv_event_t *e) {
    static lv_point_t last_point = {0, 0};
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        lv_indev_get_point(lv_indev_get_act(), &last_point);
    } else if (code == LV_EVENT_PRESSING) {
        lv_point_t now;
        lv_indev_get_point(lv_indev_get_act(), &now);
        int dx = last_point.x - now.x;

        if (abs(dx) > 2) { // basic threshold to avoid noise
            int hz_change = dx * PIXEL_TO_HZ_RATIO;
            adjust_frequency_by(hz_change);  // your own function
            last_point = now;
        }
    }
}

// Create the frequency UI
void create_frequency_ui(lv_obj_t *parent) {
    // Label
    lv_obj_t *freq_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(freq_cont);
    lv_obj_set_size(freq_cont, 200, LV_SIZE_CONTENT);
    // lv_obj_set_style_bg_color(freq_cont, lv_color_make(255, 255, 0), 0);

    sdr_ui.freq_label = lv_label_create(freq_cont);
    lv_label_set_text_fmt(sdr_ui.freq_label, "%02d.%03d.%03d", freq_disp.M, freq_disp.k, freq_disp.n);
    lv_obj_align(sdr_ui.freq_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_font(sdr_ui.freq_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(sdr_ui.freq_label, lv_color_make(255, 255, 0), 0);
    update_freq_display();

    // Button to open keypad
    lv_obj_t *edit_btn = lv_btn_create(parent);
    lv_obj_align(edit_btn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t *btn_label = lv_label_create(edit_btn);
    lv_label_set_text(btn_label, "Edit Freq");
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(edit_btn, show_digit_keypad, LV_EVENT_CLICKED, NULL);

    // Keypad container
    kb_container = lv_obj_create(parent);
    lv_obj_set_size(kb_container, 300, 200);
    lv_obj_align(kb_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(kb_container, LV_OBJ_FLAG_HIDDEN);

    static const char *btn_map[] = {
        "1", "2", "3", "\n",
        "4", "5", "6", "\n",
        "7", "8", "9", "\n",
        "DEL", "0", "OK", ""
    };

    digit_kb = lv_btnmatrix_create(kb_container);
    lv_btnmatrix_set_map(digit_kb, btn_map);
    lv_obj_set_size(digit_kb, 270, 150);
    lv_obj_align(digit_kb, LV_ALIGN_CENTER, 0, 20);
    lv_btnmatrix_set_btn_ctrl_all(digit_kb, LV_BTNMATRIX_CTRL_CLICK_TRIG);

    lv_obj_add_event_cb(digit_kb, digit_btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Input display text area
    input_display = lv_textarea_create(kb_container);
    lv_obj_set_width(input_display, 270);
    lv_obj_align(input_display, LV_ALIGN_TOP_MID, 0, -10);
    lv_textarea_set_text(input_display, "");
    lv_textarea_set_max_length(input_display, 9);
    lv_textarea_set_one_line(input_display, true);
    lv_obj_set_style_text_font(input_display, &lv_font_montserrat_20, 0);
    lv_obj_set_style_bg_color(input_display, lv_color_black(), 0);
    lv_obj_set_style_text_color(input_display, lv_color_white(), 0);
    // lv_textarea_set_cursor_hidden(input_display, true);  // hide blinking cursor

    //touch interation on the spectrum
    lv_obj_t *spectrum_area = lv_obj_create(parent);
    lv_obj_set_size(spectrum_area, SPECTRUM_WIDTH, SPECTRUM_HEIGHT);  // Adjust size as needed
    lv_obj_set_style_bg_opa(spectrum_area, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(spectrum_area, 0, 0);
    lv_obj_set_style_pad_all(spectrum_area, 0, 0);
    lv_obj_set_style_radius(spectrum_area, 0, 0);
    lv_obj_align(spectrum_area, LV_ALIGN_CENTER, 0, 5);
    lv_obj_add_event_cb(spectrum_area, spectrum_touch_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_clear_flag(spectrum_area, LV_OBJ_FLAG_SCROLLABLE); // disable scrolling
}

static void dfu_event_handler(lv_event_t * e)
{
    lv_obj_t *dfu_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dfu_overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(dfu_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(dfu_overlay, LV_OPA_COVER, 0);

    lv_obj_t *dfu_label = lv_label_create(dfu_overlay);
    lv_label_set_text(dfu_label, "DFU in 3");
    lv_obj_set_style_text_font(dfu_label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(dfu_label, lv_color_white(), 0);
    lv_obj_center(dfu_label);

    lv_refr_now(NULL); 
    HAL_Delay(1000);

    lv_label_set_text(dfu_label, "DFU in 2");
    lv_refr_now(NULL); 
    HAL_Delay(1000);

    lv_label_set_text(dfu_label, "DFU in 1");
    lv_refr_now(NULL); 
    HAL_Delay(1000);

    JumpToBootloader();
}
void dfu_button_temp(lv_obj_t *parent) {
    lv_obj_t * btn1 = lv_button_create(parent);
    lv_obj_add_event_cb(btn1, dfu_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn1, LV_ALIGN_TOP_RIGHT, 0, 0);
    // lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "DFU");
    lv_obj_center(label);
}

void ui_create_main(void){
    // 创建主容器
    cont_main = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont_main, LV_HOR_RES, LV_VER_RES);
    // lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_style_pad_all(cont_main, 10, 0);
    lv_obj_set_style_bg_color(cont_main, lv_color_make(122, 122, 122), 0);
    lv_obj_center(cont_main);
    lv_obj_set_style_border_width(cont_main, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(cont_main, 0, LV_PART_MAIN);
    lv_obj_clear_flag(cont_main, LV_OBJ_FLAG_SCROLLABLE);

    // 创建频谱图
    create_spectrum(cont_main);
    // 创建瀑布图
    create_waterfall(cont_main);
    create_frequency_ui(cont_main);
    dfu_button_temp(cont_main);
}

// 生成模拟频谱数据
static void generate_spectrum_data(lv_coord_t *data, uint32_t counter) {
    // 1. Irregular background noise
    for (int i = 0; i < SPECTRUM_POINTS; i++) {
        data[i] = MIN_DB + lv_rand(0, 6);  // Base noise ripple
    }

    // 2. AM Signal (symmetric)
    int am_center = 55;
    int am_width = 20;
    int am_carrier_mag = MIN_DB + 55 + lv_rand(-4, 4);
    int am_base_mag = MIN_DB + 15 + lv_rand(-2, 2); // Lower general sideband magnitude

    for (int i = am_center - am_width / 2; i <= am_center + am_width / 2; i++) {
        if (i < 0 || i >= SPECTRUM_POINTS || abs(i - am_center) <= 1) continue; // skip carrier

        int offset = abs(i - am_center);
        if (offset <= 1) continue;  // Create spectral gap

        int fading = (am_width / 2 - offset) * 2;
        int modulation = lv_rand(-15, 5);  // Some deep dips in sideband
        int mag = am_base_mag + fading + modulation;

        if (mag > data[i]) data[i] = mag;
    }
    if (am_center >= 0 && am_center < SPECTRUM_POINTS)
        data[am_center] = am_carrier_mag;

    // 3. FM Signal (symmetric)
    int fm_center = 95;
    int fm_width = 14;
    int fm_carrier_mag = MIN_DB + 60 + lv_rand(-4, 4);
    int fm_base_mag = MIN_DB + 20 + lv_rand(-2, 2); // Lower general sideband magnitude

    for (int i = fm_center - fm_width / 2; i <= fm_center + fm_width / 2; i++) {
        if (i < 0 || i >= SPECTRUM_POINTS || abs(i - fm_center) <= 1) continue;

        int offset = abs(i - fm_center);
        if (offset <= 1) continue;  // Create spectral gap

        int fading = (fm_width / 2 - offset) * 2;
        int modulation = lv_rand(-20, 8);  // Realistic dropouts
        int mag = fm_base_mag + fading + modulation;

        if (mag > data[i]) data[i] = mag;
    }
    if (fm_center >= 0 && fm_center < SPECTRUM_POINTS)
        data[fm_center] = fm_carrier_mag;

    // 4. SSB Signal (one-sided, intermittent)
    int ssb_center = 125;
    int ssb_width = 12;
    bool ssb_active = (counter / 60) % 8 != 0;  // Occasional dropout

    if (ssb_active) {
        for (int i = ssb_center; i <= ssb_center + ssb_width; i++) {
            if (i < 0 || i >= SPECTRUM_POINTS) continue;

            int offset = i - ssb_center;
            int fading = (ssb_width - offset); // Sloped voice pattern
            int modulation = lv_rand(-10, 4);   // Voice-like variations

            int mag = MIN_DB + 8 + fading + modulation;
            if (mag > data[i]) data[i] = mag;
        }
    }

    // 5. Moving ping signal (active indicator)
    int moving = (counter / 2) % SPECTRUM_POINTS;
    data[moving] = MIN_DB + 60 + lv_rand(0, 3);
}


static uint16_t magnitude_to_rgb565(int magnitude_db) {
    // Normalize dB to 0–255
    int norm = (magnitude_db - MIN_DB) * 255 / (MAX_DB - MIN_DB);
    if (norm < 0) norm = 0;
    if (norm > 255) norm = 255;

    uint8_t r = 0, g = 0, b = 0;

    if (norm < 42) {
        // 0–41: Black → Dark Blue
        b = norm * 6; // 0 → 252
    }
    else if (norm < 84) {
        // 42–83: Dark Blue → Blue
        b = 255;
        g = (norm - 42) * 6;
    }
    else if (norm < 126) {
        // 84–125: Blue → Cyan
        b = 255;
        g = 255;
        r = (norm - 84) * 6;
    }
    else if (norm < 168) {
        // 126–167: Cyan → White
        r = 255;
        g = 255;
        b = 255 - (norm - 126) * 6;
    }
    else if (norm < 210) {
        // 168–209: White → Yellow/Orange
        r = 255;
        g = 255 - (norm - 168) * 4;
        b = 0;
    }
    else {
        // 210–255: Orange → Red
        r = 255;
        g = 87 - ((norm - 210) * 2);  // From ~87 to 0
        if (g > 255) g = 0;
        b = 0;
    }

    // Convert to RGB565
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

lv_coord_t db_to_y(float db) {
    if (db < MIN_DB) db = MIN_DB;
    if (db > MAX_DB) db = MAX_DB;

    float range = MAX_DB - MIN_DB;
    float norm = (db - MIN_DB) / range;

    // Flip vertically if higher dB = higher pixel row (as LVGL y=0 is top)
    // return (lv_coord_t)((1.0f - norm) * SPECTRUM_HEIGHT);
    return (lv_coord_t)norm;
}

// Convert complex array to dB magnitude
static void fft_cmplx_to_db(const float32_t *cmplx, float32_t *db_out, uint32_t length, float fullscale) {
    const float epsilon = 1e-12f;
    for (uint32_t i = 0; i < length; i++) {
        // float re = cmplx[2 * i];
        // float im = cmplx[2 * i + 1];
        // float mag = sqrtf(re * re + im * im);
        float mag = cmplx[i];
        float norm_mag = mag / fullscale;
        if (norm_mag < epsilon) norm_mag = epsilon;
        float db = 10.0f * log10f(norm_mag);
        if (db < MIN_DB) db = MIN_DB;
        if (db > MAX_DB) db = MAX_DB;
        db_out[i] = db;
    }
}

// Compute value over a bin range based on strategy
static float bin_reduce(const float *data, int bin_start, int bin_end, const char *method) {
    float result = 0.0f;
    if (strcmp(method, "max") == 0) {
        result = MIN_DB;
        for (int i = bin_start; i < bin_end; i++) {
            if (data[i] > result) result = data[i];
        }
    } else if (strcmp(method, "avg") == 0) {
        for (int i = bin_start; i < bin_end; i++) {
            result += data[i];
        }
        result /= (bin_end - bin_start);
    } else if (strcmp(method, "rms") == 0) {
        for (int i = bin_start; i < bin_end; i++) {
            result += data[i] * data[i];
        }
        result = sqrtf(result / (bin_end - bin_start));
    } else {
        result = MIN_DB;
    }
    return result;
}

void process_spectrum_display(
    const float32_t *usb_fft,
    const float32_t *lsb_fft,
    float *usb_db_out,
    float *lsb_db_out,
    int32_t *display_buf,
    const char *method,
    float fullscale)
{
    fft_cmplx_to_db(usb_fft, usb_db_out, FFT_LEN/2, fullscale);
    fft_cmplx_to_db(lsb_fft, lsb_db_out, FFT_LEN/2, fullscale);

    int bins_per_pixel = FFT_LEN / 2 / (SPECTRUM_POINTS / 2);
    if (bins_per_pixel < 1) bins_per_pixel = 1;

    for (int x = 0; x < SPECTRUM_POINTS; x++) {
        int bin_start = (x % (SPECTRUM_POINTS / 2)) * bins_per_pixel;
        int bin_end = bin_start + bins_per_pixel;
        if (bin_end > FFT_LEN / 2) bin_end = FFT_LEN / 2;

        float db_val;
        if (x < SPECTRUM_POINTS / 2) {
            int reversed_bin = (SPECTRUM_POINTS / 2 - 1 - x);
            int bin_start = reversed_bin * bins_per_pixel;
            int bin_end = bin_start + bins_per_pixel;
            if (bin_end > FFT_LEN / 2) bin_end = FFT_LEN / 2;

            db_val = bin_reduce(lsb_db_out, bin_start, bin_end, method);
        } else {
            db_val = bin_reduce(usb_db_out, bin_start, bin_end, method);
        }

        display_buf[x] = (int32_t)db_val;
    }
}


void update_waterfall(const float *usb_db, const float *lsb_db)
{
    uint16_t *buf16 = (uint16_t *)sdr_ui.waterfall_buffer;

    // Shift down
    memmove(&buf16[WATERFALL_WIDTH],
            &buf16[0],
            2 * WATERFALL_WIDTH * (WATERFALL_HEIGHT - 1));

    int bins_per_pixel = FFT_LEN / 2 / (WATERFALL_WIDTH / 2);
    if (bins_per_pixel < 1) bins_per_pixel = 1;

    for (int x = 0; x < WATERFALL_WIDTH; x++) {
        int bin_start, bin_end;
        float db_val;

        if (x < WATERFALL_WIDTH / 2) {
            bin_start = (WATERFALL_WIDTH / 2 - 1 - x) * bins_per_pixel;
            bin_end = bin_start + bins_per_pixel;
            if (bin_end > FFT_LEN / 2) bin_end = FFT_LEN / 2;
            db_val = bin_reduce(lsb_db, bin_start, bin_end, "max");
        } else {
            bin_start = (x - WATERFALL_WIDTH / 2) * bins_per_pixel;
            bin_end = bin_start + bins_per_pixel;
            if (bin_end > FFT_LEN / 2) bin_end = FFT_LEN / 2;
            db_val = bin_reduce(usb_db, bin_start, bin_end, "max");
        }

        uint8_t yval = db_to_y(db_val);
        buf16[x] = magnitude_to_rgb565(db_val);
    }

    lv_obj_invalidate(sdr_ui.waterfall_canvas);
}

void ui_update(void){
    process_spectrum_display(fft_output_usb, fft_output_lsb, usb_db,
        lsb_db, sdr_ui.spectrum_series->y_points, "max", 50);

    lv_chart_refresh(sdr_ui.spectrum_chart);
    update_waterfall(usb_db, lsb_db);
}

static void timer_cb(lv_timer_t *timer) {
        ui_update();
}

void ui_init(void) {
    ui_create_main();
    lv_timer_create(timer_cb, UPDATE_INTERVAL_MS, NULL);
}
