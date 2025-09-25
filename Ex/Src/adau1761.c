//
// Created by x on 5/29/2025.
//

#include "adau1761.h"
#include "i2c.h"
#include <stdint.h>
#include <string.h>
#include "HM50S_ADAU1761.h"

void adau1761_writeReg(ADAU1761_Register addr, uint8_t data)
{
    uint8_t tx_buf[3] = {(addr >> 8) & 0xFF, addr & 0xFF, data};
	HAL_I2C_Master_Transmit(&hi2c2, ADAU1761_I2C_ADDR_W, tx_buf, 3, HAL_MAX_DELAY);
}

uint8_t adau1761_readReg(uint16_t addr)
{
    uint8_t rx_buf;
    uint8_t tx_buf[2] = {(addr >> 8) & 0xFF, addr & 0xFF};
	HAL_I2C_Master_Transmit(&hi2c2, ADAU1761_I2C_ADDR_W, tx_buf, 2, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, ADAU1761_I2C_ADDR_R, &rx_buf, 1, HAL_MAX_DELAY);
	return rx_buf;
}

void adau1761_writeReg_burst(uint16_t baseaddr, uint8_t *data, uint16_t size)
{
    uint8_t tx_buf[size + 2];
	tx_buf[0] = baseaddr >> 8 & 0xFF;
	tx_buf[1] = baseaddr & 0xFF;
	memcpy(&tx_buf[2], data, size);
	HAL_I2C_Master_Transmit(&hi2c2, ADAU1761_I2C_ADDR_W, tx_buf, size + 2, HAL_MAX_DELAY);
}

uint8_t* adau1761_readReg_burst(uint16_t baseaddr, uint16_t size)
{
    uint8_t *rx_buf = {0};
    uint8_t tx_buf[2] = {baseaddr >> 8 & 0xFF, baseaddr & 0xFF};
	HAL_I2C_Master_Transmit(&hi2c2, ADAU1761_I2C_ADDR_W, tx_buf, 2, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, ADAU1761_I2C_ADDR_R, rx_buf, size, HAL_MAX_DELAY);
	return rx_buf;
}

// void adau1761_writeDSP_param(void)
// {
//     adau1761_writeReg_burst(0x0000, Param_Data_IC, PARAM_SIZE_IC_1);
// }

// void adau1761_readDSP_param(uint16_t baseaddr, uint8_t *param) {
// 	HAL_I2C_Mem_Read(&hi2c2, ADAU1761_I2C_ADDR_R, baseaddr, I2C_MEMADD_SIZE_16BIT, param, 4, HAL_MAX_DELAY);
// }

// void adau1761_writeDSP_program(void)
// {
//     adau1761_writeReg_burst(0x0800, Program_Data_IC, PROGRAM_SIZE_IC_1);
// }

// void adau1761_readDSP_program(uint16_t baseaddr, uint8_t *program) {
// 	HAL_I2C_Mem_Read(&hi2c2, ADAU1761_I2C_ADDR_R, baseaddr, I2C_MEMADD_SIZE_16BIT, program, 5, HAL_MAX_DELAY);
// }

void adau1761_init(void){
	default_download_ADAU1761();
}

// int32_t SIGMA_WRITE_REGISTER_BLOCK(uint8_t devAddress, uint16_t address, uint16_t length, ADI_REG_TYPE *pData) {
// 	HAL_I2C_Mem_Write(&hi2c2, devAddress, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)pData, length, HAL_MAX_DELAY);
// 	return 0;
// }
