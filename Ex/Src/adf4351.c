//
// Created by x on 5/30/2025.
//

#include "adf4351.h"
#include "spi.h"
#include <stdint.h>
#include "tim.h"

    ADF4351_Config lo = {
        /* R0 */
        .int_val  = 16000,
        .frac_val = 0,

        /* R1 */
        .phase_adjust = PHASE_ADJ_OFF,
        .prescaler = PRESCALER_8_9,
        .phase_val = 1,
        .mod_val  = 3125,

        /* R2 */
        .lnsp_mode = LOW_SPUR,
        .muxout = DVdd,
        .ref_doubler = REF_DBL_OFF,
        .ref_div2 = REF_DIV1,
        .r_counter = 50,
        .double_buf = false,
        .chargepumpcrt = ICP_2_50_MA,
        .ldf = FRAC_N,
        .ldp = LDP_10_NS,
        .pd_polarity = POS,
        .power_down = POWER_UP,
        .cp_tristate = DIS,
        .counter_reset = false,

        /* R3 */
        .bsclk_mode = LOW,
        .abp = ABP_6NS,
        .charge_cancel = CHARGE_CANCEL_OFF,
        .csr = CSR_OFF,
        .clkdivmode = CLK_DIV_OFF,
        .clk_divider = 150,

        /* R4 */
        .feedback = FEEDBACK_FUNDAMENTAL,
        .rf_div_sel = DIV_32,
        .band_select_clk_div = 2,
        .vco_pwdn = VCO_PWUP,
        .mtld = MTLD_OFF,
        .aux_sel = DIV_OUT,
        .aux_en = AUXOUT_ON,
        .auxout_power = AUXOUT_PWR_P5dBm,
        .rf_en = RFOUT_ON,
        .out_power = OUT_PWR_P5dBm,

        /* R5 */
        .ld_mode = LD_DIGITAL
    };

void adf4351_writeReg32(uint32_t data)
{
    HAL_GPIO_WritePin(LO_CS_GPIO_Port, LO_CS_Pin, GPIO_PIN_RESET);
    Delay_us(1);
    uint8_t tx_buf[4];
    tx_buf[3] = data & 0xFF;
    tx_buf[2] = (data >> 8) & 0xFF;
    tx_buf[1] = (data >> 16) & 0xFF;
    tx_buf[0] = (data >> 24) & 0xFF;
    HAL_SPI_Transmit(&hspi4, tx_buf, 4, HAL_MAX_DELAY);
    Delay_us(1);
    HAL_GPIO_WritePin(LO_CS_GPIO_Port, LO_CS_Pin, GPIO_PIN_SET);
    // Delay_us(10);
    // HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
}

/* ---------- R‑builders (compile‑time constants improve speed) ---------- */
static uint32_t build_r0(const ADF4351_Config *c)
{
    return  ((uint32_t)c->int_val  << 15) |
            ((uint32_t)c->frac_val <<  3) |
            0U; /* C[2:0]=000 */
}

static uint32_t build_r1(const ADF4351_Config *c)
{
    return  ((uint32_t)c->phase_adjust << 28) |
            ((uint32_t)c->prescaler    << 27) |
            ((uint32_t)c->phase_val    << 15) |
            ((uint32_t)c->mod_val      <<  3) |
            1U; /* C[2:0]=001 */
}

static uint32_t build_r2(const ADF4351_Config *c)
{
    return  ((uint32_t)c->lnsp_mode    << 29) |
            ((uint32_t)c->muxout       << 26) |
            ((uint32_t)c->ref_doubler  << 25) |
            ((uint32_t)c->ref_div2     << 24) |
            ((uint32_t)c->r_counter    << 14) |
            ((uint32_t)c->double_buf   << 13) |
            ((uint32_t)c->chargepumpcrt<<  9) |
            ((uint32_t)c->ldf          <<  8) |
            ((uint32_t)c->ldp          <<  7) |
            ((uint32_t)c->pd_polarity  <<  6) |
            ((uint32_t)c->power_down   <<  5) |
            ((uint32_t)c->cp_tristate  <<  4) |
            ((uint32_t)c->counter_reset<<  3) |
            2U; /* C[2:0]=010 */
}

static uint32_t build_r3(const ADF4351_Config *c)
{
    return  ((uint32_t)c->bsclk_mode   << 23) |
            ((uint32_t)c->abp          << 22) |
            ((uint32_t)c->charge_cancel << 21) |
            ((uint32_t)c->csr          << 18) |
            ((uint32_t)c->clkdivmode   << 15) |
            ((uint32_t)c->clk_divider  <<  3) |
            3U; /* 011 */
}

static uint32_t build_r4(const ADF4351_Config *c)
{
    return  ((uint32_t)c->feedback     << 23) |
            ((uint32_t)c->rf_div_sel   << 20) |
            ((uint32_t)c->band_select_clk_div << 12) |
            ((uint32_t)c->vco_pwdn     << 11) |
            ((uint32_t)c->mtld         << 10) |
            ((uint32_t)c->aux_sel       << 9) |
            ((uint32_t)c->aux_en        << 8) |
            ((uint32_t)c->auxout_power  << 6) |
            ((uint32_t)c->rf_en         << 5) |
            ((uint32_t)c->out_power     << 3) |
            4U; /* 100 */
}

static uint32_t build_r5(const ADF4351_Config *c)
{
    return  ((uint32_t)c->ld_mode << 22) |
            ((uint32_t)0x03       << 19) |
            5U; /* 101 – remaining bits always per data‑sheet */
}

/* ---------- public helpers ---------- */
void adf4351_update_allRegs(ADF4351_Config *c)
{
    c->r[0] = build_r0(c);
    c->r[1] = build_r1(c);
    c->r[2] = build_r2(c);
    c->r[3] = build_r3(c);
    c->r[4] = build_r4(c);
    c->r[5] = build_r5(c);
}

void adf4351_update_Reg0(ADF4351_Config *c) {
    c->r[0] = build_r0(c);
}
void adf4351_update_Reg1(ADF4351_Config *c) {
    c->r[1] = build_r1(c);
}
void adf4351_update_Reg2(ADF4351_Config *c) {
    c->r[2] = build_r2(c);
}
void adf4351_update_Reg3(ADF4351_Config *c) {
    c->r[3] = build_r3(c);
}
void adf4351_update_Reg4(ADF4351_Config *c) {
    c->r[4] = build_r4(c);
}
void adf4351_update_Reg5(ADF4351_Config *c) {
    c->r[5] = build_r5(c);
}

#define FREQ_OFFSET 0

void adf4351_update_frequency(Frequency_disp freq) {
    uint32_t Frequency, remainder;
    uint16_t quotient;

    Frequency = freq.M * 1000000 + freq.k * 1000 + freq.n + FREQ_OFFSET;
    quotient = Frequency * 32 * 2 / 200000;
    remainder = Frequency * 32 * 2 % 200000;
    lo.int_val = quotient;
    lo.frac_val = remainder / 32 / 2;

    adf4351_update_Reg0(&lo);
    adf4351_writeReg32(lo.r[0]);
}

void adf4351_write_allRegs(const ADF4351_Config *c)
{
    /*  datasheet sequence: R5..R0  */
    for (int8_t i = 5; i >= 0; --i)
        adf4351_writeReg32(c->r[i]);
}

void adf4351_apply_allConfig(ADF4351_Config *cfg) {
    adf4351_update_allRegs(cfg);  // Update register values based on cfg
    adf4351_write_allRegs(cfg);    // Write all registers to the device
}


void adf4351_init(void)
{
    adf4351_apply_allConfig(&lo);
}
