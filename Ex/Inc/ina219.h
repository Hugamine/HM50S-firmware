#ifndef INA219_H
#define INA219_H

#include "main.h"

typedef enum {no, reset} INA219_Reset;
typedef enum {_16V, _32V} INA219_Bus_Voltage_Range;
typedef enum {_40mV, _80mV, _160mV, _320mV} INA219_PGA;
typedef enum {b_9bit, b_10bit, b_11bit, b_12bit, b_12bit_1 = 8, b_12bit_2, b_12bit_4, b_12bit_8, b_12bit_16, b_12bit_32, b_12bit_64, b_12bit_128} INA219_BADC;
typedef enum {s_9bit, s_10bit, s_11bit, s_12bit, s_12bit_1 = 8, s_12bit_2, s_12bit_4, s_12bit_8, s_12bit_16, s_12bit_32, s_12bit_64, s_12bit_128} INA219_SADC;
typedef enum {pwdn, shunt_volt_trig, bus_volt_trig, shunt_bus_trig, adc_off, shunt_volt_cont, bus_volt_cont, shunt_bus_cont} INA219_Mode;

typedef struct {
    uint8_t i2c_addr_w, i2c_addr_r;
    /*------r5------*/
    uint8_t reset;
    INA219_Bus_Voltage_Range bus_volt_rng;
    INA219_PGA pga;
    INA219_BADC badc;
    INA219_SADC sadc;
    INA219_Mode mode;
} INA219_Config;

#define INA219_BAT_ADDR_W 0x82
#define INA219_BAT_ADDR_R 0x83
#define INA219_EXT_ADDR_W 0x80
#define INA219_EXT_ADDR_R 0x81

#define INA219_CFG_REG 0x00
#define INA219_SHUNT_VOLT_REG 0x01
#define INA219_BUS_VOLT_REG 0x02
#define INA219_POWER_REG 0x03
#define INA219_CURRENT_REG 0x04
#define INA219_CAL_REG 0x05

#define INA219_CURRENT_LSE 0.0001 //max current = 3.2768A
#define INA219_POWER_LSE INA219_CURRENT_LSE * 20
#define INA219_CAL_VALUE 0.04096 / (INA219_CURRENT_LSE * 0.01) //shunt R = 10mR

uint16_t ina219_readReg(INA219_Config *c, uint8_t addr);

void ina219_init_all(void);
float ina219_get_bus_volt(INA219_Config *c);
float ina219_get_power(INA219_Config *c);
float ina219_get_current(INA219_Config *c);


#endif
