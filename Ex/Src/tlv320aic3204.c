//
// Created by x on 5/31/2025.
//

#include "tlv320aic3204.h"
#include "i2c.h"

void codec_reg_w(uint8_t addr, uint8_t data)
{
    uint8_t tx_buf[2] = {addr, data};
    HAL_I2C_Master_Transmit(&hi2c2, CODEC_I2C_ADDR_W, tx_buf, 2, HAL_MAX_DELAY);
}

uint8_t codec_reg_r(uint8_t addr)
{
    uint8_t rx_buf;
    HAL_I2C_Master_Transmit(&hi2c2, CODEC_I2C_ADDR_W, &addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c2, CODEC_I2C_ADDR_R, &rx_buf, 1, HAL_MAX_DELAY);
    return rx_buf;
}

void codec_reg_page(uint8_t page)
{

    codec_reg_w(0x00, page);
}

void codec_soft_reset()
{
    codec_reg_w(0x01, 0x01);
}

void codec_init()
{
    HAL_Delay(10);

    codec_reg_page(0);
    codec_soft_reset();
    codec_reg_w(0x0B, 0x81);
    codec_reg_w(0x0C, 0x82);
    codec_reg_w(0x0D, 0x00);
    codec_reg_w(0x0E, 0x80);
    codec_reg_w(0x1B, 0x20); //AUDIO INTERFACE SETTING
    codec_reg_w(0x3C, 0x01); //DAC DSP
    codec_reg_w(0x3D, 0x07); //ADC DSP
    codec_reg_page(1);
    codec_reg_w(0x01, 0x08);
    codec_reg_w(0x02, 0x01);
    codec_reg_w(0x7B, 0x01); //REF POWER UP CONFIG
    //	codec_reg_w(0x14, 0x25);
    //	codec_reg_w(0x0A, 0x00); //CM CTRL
    codec_reg_w(0x0F, 0x08); //DAC TO LOR
    codec_reg_w(0x09, 0x05); //LOR AND MAR POWERED UP
    codec_reg_w(0x13, 0x00); //LOR UNMUTE, +3DB LOR DRIVER GAIN
    codec_reg_page(0);
    codec_reg_w(0x3F, 0xD6); //R DAC POWERED UP
    codec_reg_w(0x40, 0x00); //R DAC UNMUTE
    codec_reg_w(0x42, 0x30); //R DAC GAIN
    codec_reg_w(0x51, 0x40); //R ADC POWERED UP
    codec_reg_w(0x52, 0x80); //R ADC UNMUTE
    codec_reg_w(0x54, 0x00); //R ADC GAIN

    codec_reg_page(1);
    codec_reg_w(0x33, 0x60); //MICBIAS POWERED UP, MICBIAS = 2.5V
    codec_reg_w(0x37, 0x10); //R MICPGA POS IN2R WITH 10K
    codec_reg_w(0x39, 0x01); //R MICPGA NEG
    	// codec_reg_w(0x0F, 0x02); //MAR TO LOR
    codec_reg_w(0x3C, 60); //R MICPGA GAIN ENABLED, +XDB GAIN
}

