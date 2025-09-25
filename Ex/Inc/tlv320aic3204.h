//
// Created by x on 5/31/2025.
//

#ifndef TLV320AIC3204_H
#define TLV320AIC3204_H

#include "main.h"

#define CODEC_I2C_ADDR_W 0x30
#define CODEC_I2C_ADDR_R 0x31

void codec_reg_w(uint8_t addr, uint8_t data);
uint8_t codec_reg_r(uint8_t addr);
void codec_reg_page(uint8_t page);
void codec_soft_reset();
void codec_init();

#endif //TLV320AIC3204_H
