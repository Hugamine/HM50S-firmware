//
// Created by x on 5/30/2025.
//

#ifndef ADF4351_H
#define ADF4351_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "ui.h"

/* ─────────────── Enumerations ─────────────── */

/* R1 DB28                                             */
typedef enum { PHASE_ADJ_OFF = 0, PHASE_ADJ_ON = 1 } ADF4351_PhaseAdj;

/* R1 DB27                                             */
typedef enum { PRESCALER_4_5 = 0, PRESCALER_8_9 = 1 } ADF4351_Prescaler;

/* R2 DB29‒DB30                                        */
typedef enum { LOW_NOISE = 0, LOW_SPUR = 3 } ADF4351_LNSpurMode;

/* R2 DB28‒26                                          */
typedef enum {
    MUX_TriState = 0, DVdd, DGND, R_Counter_out,
    N_DIV_out,     Analog_Lock_Detect,      Digital_Lock_Detect
} ADF4351_Muxout;

/* R2 DB25                                             */
typedef enum { REF_DBL_OFF = 0, REF_DBL_ON  = 1 } ADF4351_RefDoubler;

/* R2 DB24                                             */
typedef enum { REF_DIV1 = 0, REF_DIV2 = 1 } ADF4351_RefDiv2;

/* R2 DB12-9                                           */
typedef enum {
    ICP_0_31_MA = 0, ICP_0_63_MA, ICP_0_94_MA, ICP_1_25_MA,
    ICP_1_56_MA, ICP_1_88_MA, ICP_2_19_MA, ICP_2_50_MA,
    ICP_2_81_MA, ICP_3_13_MA, ICP_3_44_MA, ICP_3_75_MA,
    ICP_4_06_MA, ICP_4_38_MA, ICP_4_69_MA, ICP_5_00_MA
} ADF4351_ChargePumpCrt;

/* R2 DB8                                             */
typedef enum { FRAC_N = 0, INT_N = 1 } ADF4351_LDF;

/* R2 DB7                                             */
typedef enum { LDP_10_NS = 0, LDP_6_NS = 1 } ADF4351_LDP;

/* R2 DB6                                              */
typedef enum { NEG = 0, POS = 1 } ADF4351_PDPolarity;

/* R2 DB5                                              */
typedef enum { POWER_UP = 0, POWER_DOWN  = 1 } ADF4351_PowerDown;

/* R2 DB4                                              */
typedef enum { DIS = 0, EN = 1 } ADF4351_CPTriState;

/* R3 DB23                                             */
typedef enum { LOW = 0, HIGH = 1 } ADF4351_BSClkMode;

/* R3 DB22                                             */
typedef enum { ABP_6NS = 0, ABP_3NS = 1 } ADF4351_ABP;

/* R3 DB21                                             */
typedef enum { CHARGE_CANCEL_OFF = 0, CHARGE_CANCEL_ON = 1 } ADF4351_ChargeCancel;

/* R3 DB18                                             */
typedef enum { CSR_OFF = 0, CSR_ON = 1 } ADF4351_CSR;

/* R3 DB16‒15                                          */
typedef enum { CLK_DIV_OFF = 0, FAST_LOCK_EN, RESYNC_EN } ADF4351_ClkDivMode;

/* R3 DB14‒3  (clock‑divider value)                    */

/* R4 DB23                                             */
typedef enum { FEEDBACK_DIVIDED = 0, FEEDBACK_FUNDAMENTAL = 1 } ADF4351_FeedbackSel;

/* R4 DB22‒20 (RF Divider Select)                      */
typedef enum {
    DIV_1 = 0, DIV_2, DIV_4, DIV_8, DIV_16, DIV_32, DIV_64
} ADF4351_RFDivider;

/* R4 DB19‒12  (band select clk divider)               */

/* R4 DB11                                             */
typedef enum { VCO_PWUP = 0, VCO_PWDN = 1 } ADF4351_VCOPwdn;

/* R4 DB10                                             */
typedef enum { MTLD_OFF = 0, MTLD_ON = 1 } ADF4351_MuteTillLock;

/* R4 DB9                                              */
typedef enum { DIV_OUT = 0, FUND_OUT = 1 } ADF4351_AuxSel;

/* R4 DB8                                              */
typedef enum { AUXOUT_OFF = 0, AUXOUT_ON  = 1 } ADF4351_AuxEnable;

/* R4 DB7‒6                                            */
typedef enum { AUXOUT_PWR_M4dBm = 0, AUXOUT_PWR_M1dBm, AUXOUT_PWR_P2dBm, AUXOUT_PWR_P5dBm } ADF4351_AuxOutPower;

/* R4 DB5                                              */
typedef enum { RFOUT_OFF = 0, RFOUT_ON  = 1 } ADF4351_RfEnable;

/* R4 DB4‒3                                            */
typedef enum { OUT_PWR_M4dBm = 0, OUT_PWR_M1dBm, OUT_PWR_P2dBm, OUT_PWR_P5dBm } ADF4351_OutPower;

/* R5 DB23‒22 (LD pin mode)                            */
typedef enum { LD_LOW = 0, LD_DIGITAL = 1, LD_HIGH = 3 } ADF4351_LDMode;

/* ─────────────── Main configuration container ─────────────── */

typedef struct
{
    /* ----------- R0 ----------- */
    uint16_t int_val;                    /* DB30‒15  */
    uint16_t frac_val;                   /* DB14‒3   */

    /* ----------- R1 ----------- */
    ADF4351_PhaseAdj   phase_adjust;     /* DB28     */
    ADF4351_Prescaler  prescaler;        /* DB27     */
    uint16_t phase_val;                  /* DB26‒15  */
    uint16_t mod_val;                    /* DB14‒3   */

    /* ----------- R2 ----------- */
    ADF4351_LNSpurMode lnsp_mode;        /* DB30‒29  */
    ADF4351_Muxout     muxout;           /* DB28‒26  */
    ADF4351_RefDoubler ref_doubler;      /* DB25     */
    ADF4351_RefDiv2    ref_div2;         /* DB24     */
    uint16_t r_counter;                  /* DB23‒14  */
    bool               double_buf;       /* DB13     */
    ADF4351_ChargePumpCrt chargepumpcrt; /* DB12-9   */
    ADF4351_LDF        ldf;              /* DB8      */
    ADF4351_LDP        ldp;              /* DB7      */
    ADF4351_PDPolarity    pd_polarity;   /* DB6      */
    ADF4351_PowerDown  power_down;       /* DB5      */
    ADF4351_CPTriState   cp_tristate;    /* DB4      */
    bool               counter_reset;    /* DB3      */

    /* ----------- R3 ----------- */
    ADF4351_BSClkMode    bsclk_mode;     /* DB23     */
    ADF4351_ABP          abp;            /* DB22     */
    ADF4351_ChargeCancel charge_cancel;  /* DB21     */
    ADF4351_CSR          csr;            /* DB18     */
    ADF4351_ClkDivMode clkdivmode;       /* DB16‒15  */
    uint16_t clk_divider;                /* DB14‒3   */

    /* ----------- R4 ----------- */
    ADF4351_FeedbackSel  feedback;       /* DB23     */
    ADF4351_RFDivider    rf_div_sel;     /* DB22‒20  */
    uint8_t  band_select_clk_div;        /* DB19‒12  */
    ADF4351_VCOPwdn vco_pwdn;            /* DB11     */
    ADF4351_MuteTillLock mtld;           /* DB10     */
    ADF4351_AuxSel       aux_sel;        /* DB9      */
    ADF4351_AuxEnable    aux_en;         /* DB8      */
    ADF4351_AuxOutPower  auxout_power;   /* DB7-6    */
    ADF4351_RfEnable     rf_en;          /* DB5      */
    ADF4351_OutPower     out_power;      /* DB4‒3    */

    /* ----------- R5 ----------- */
    ADF4351_LDMode       ld_mode;        /* DB23‒22  */

    /* ----------- cached words ----------- */
    uint32_t r[6];
} ADF4351_Config;


void adf4351_init(void);
void adf4351_update_frequency(Frequency_disp freq);

#endif //ADF4351_H
