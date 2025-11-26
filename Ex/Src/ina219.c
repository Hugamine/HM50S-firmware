#include "ina219.h"
#include "i2c.h"

INA219_Config ina219_bat = {
    .i2c_addr_w = INA219_BAT_ADDR_W,
    .i2c_addr_r = INA219_BAT_ADDR_R,
    .reset = no,
    .bus_volt_rng = _16V,
    .pga = _40mV,
    .badc = b_12bit,
    .sadc = s_12bit,
    .mode = shunt_bus_cont
};

INA219_Config ina219_ext = {
    .i2c_addr_w = INA219_EXT_ADDR_W,
    .i2c_addr_r = INA219_EXT_ADDR_R,
    .reset = no,
    .bus_volt_rng = _16V,
    .pga = _40mV,
    .badc = b_12bit,
    .sadc = s_12bit,
    .mode = shunt_bus_cont
};

void ina219_writeReg(INA219_Config *c, uint8_t addr, uint16_t data){
    uint8_t tx_buf[3] = {addr, (data >> 8) & 0xFF, data & 0xFF};
    HAL_I2C_Master_Transmit(&hi2c2, c->i2c_addr_w, tx_buf, 3, HAL_MAX_DELAY);
}

uint16_t ina219_readReg(INA219_Config *c, uint8_t addr){
    uint8_t rx_buf[2];
    uint8_t tx_buf[1] = {addr};
    HAL_I2C_Master_Transmit(&hi2c2, c->i2c_addr_w, tx_buf, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, c->i2c_addr_r, rx_buf, 2, HAL_MAX_DELAY);
    return ((uint16_t)rx_buf[0] << 8) | (uint16_t)rx_buf[1];
}

static uint16_t build_cfg_reg(const INA219_Config *c){
    return ((uint16_t)c->reset << 15) |
           ((uint16_t)c->bus_volt_rng << 13) |
           ((uint16_t)c->pga << 11) |
           ((uint16_t)c->badc << 7) |
           ((uint16_t)c->sadc << 3) |
           ((uint16_t)c->mode);
}

void ina219_init(INA219_Config *c){
    ina219_writeReg(c, INA219_CFG_REG, build_cfg_reg(c));
    ina219_writeReg(c, INA219_CAL_REG, INA219_CAL_VALUE);
}

void ina219_init_all(void){
    ina219_init(&ina219_bat);
    ina219_init(&ina219_ext);
}

uint16_t ina219_get_bus_volt_mv(INA219_Config *c){
    return (ina219_readReg(c, INA219_BUS_VOLT_REG) >> 3) * 4; //LSB = 4mV
}

float ina219_get_bus_volt(INA219_Config *c){
    return (float)ina219_get_bus_volt_mv(c) / 1000;
}

float ina219_get_power(INA219_Config *c){
    return (float)ina219_readReg(c, INA219_POWER_REG) * INA219_POWER_LSE;
}

float ina219_get_current(INA219_Config *c){
    return (float)ina219_readReg(c, INA219_CURRENT_REG) * INA219_CURRENT_LSE;
}
