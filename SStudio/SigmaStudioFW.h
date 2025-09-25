
/*
 * File:			SigmaStudioFW.h
 *
 * Description:  	SigmaStudio System Framwork macro definitions. These
 *				macros should be implemented for your system's software.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright � 2020 Analog Devices, Inc. All rights reserved.
 */

#ifndef __SIGMASTUDIOFW_H__
#define __SIGMASTUDIOFW_H__

#include "main.h"
#include "i2c.h"
#include "stm32h7xx_hal.h"

typedef unsigned short ADI_DATA_U16;
typedef unsigned char ADI_REG_TYPE;

#define DSP_I2C_ADDR 0x70

/*
 * Parameter data format
 */
#define SIGMASTUDIOTYPE_FIXPOINT 0
#define SIGMASTUDIOTYPE_INTEGER  1

/*
 * Convert a floating-point value to SigmaDSP (5.23 or 8.24) fixed point format
 */
// #if DSP_TYPE == DSP_TYPE_SIGMA300_350
// int32_t SIGMASTUDIOTYPE_FIXPOINT_CONVERT(double value) { return int32_t(value * (0x01 << 24)); }
// #else
int32_t SIGMASTUDIOTYPE_FIXPOINT_CONVERT(double value) { 
    return (int32_t)(value * (1 << 23)) & 0xFFFFFFF;
}
// #endif

// For compatibility with certain export files, redirect SIGMASTUDIOTYPE_8_24_CONVERT to
// SIGMASTUDIOTYPE_FIXPOINT_CONVERT
#define SIGMASTUDIOTYPE_8_24_CONVERT(x) SIGMASTUDIOTYPE_FIXPOINT_CONVERT(x)

// Separate a 32-bit floating point value into four bytes
void SIGMASTUDIOTYPE_REGISTER_CONVERT(int32_t fixpt_val, uint8_t dest[4]) {
    dest[0] = (fixpt_val >> 24) & 0xFF;
    dest[1] = (fixpt_val >> 16) & 0xFF;
    dest[2] = (fixpt_val >> 8) & 0xFF;
    dest[3] = (fixpt_val)&0xFF;
}

// The I2C buffer declared by Arduino is 32 bytes long by default. Adjust for your processor.
// Longer buffers use more microcontroller RAM, but allow faster programming
// because I2C overhead is lower.
// The two address bytes shorten the data burst size by 2 bytes.
const int MAX_I2C_DATA_LENGTH = 30;

/** Return the depth (in bytes) of a certain DSP memory location.
 * Currently this function is only implemented for data memory and program memory.
 * Control registers are not included.
 * Function is only required for I2C; it exists because of buffer size limitations in the Teensy I2C library.
 */
#if USE_SPI == false
uint8_t getMemoryDepth(uint32_t address) {
#if DSP_TYPE == DSP_TYPE_SIGMA100
    if (address < 0x0400)
        return 4;    // Parameter RAM is 4 bytes deep
    else {
        return 5;    // Program RAM is 5 bytes deep
    }
#elif DSP_TYPE == DSP_TYPE_SIGMA200
    // Based on ADAU1761
    if (address < 0x0800) {
        return 4;    // Parameter RAM is 4 bytes deep
    } else {
        return 5;
    }
#elif (DSP_TYPE == DSP_TYPE_SIGMA300_350)
    if (address < 0xF000) {
        return 4;    // Program Memory, DM0, and DM1 all store 4 bytes (ADAU1463 datasheet
                     // page 90)
    } else {
        return 2;    // Control registers all store 2 bytes (ADAU1463 datasheet page 93)
    }
#else
    return 0;    // We should never reach this return
#endif
}
#endif

void SIGMA_WRITE_REGISTER_BLOCK(uint8_t devAddress, int address, int length, uint8_t pData[]) {
    HAL_I2C_Mem_Write(&hi2c2, devAddress, address, I2C_MEMADD_SIZE_16BIT, (uint8_t *)pData, length, HAL_MAX_DELAY);
}

// Alternative function call without the address (single dsp system)
// void SIGMA_WRITE_REGISTER_BLOCK(int address, int length, uint8_t pData[]) {
//     SIGMA_WRITE_REGISTER_BLOCK(DSP_I2C_ADDR, address, length, pData);
// }

// Write a 32-bit integer to the DSP. NOTE: 5.23 not supported quite yet.
void SIGMA_WRITE_REGISTER_INTEGER(int address, int32_t pData) {
    uint8_t byte_data[4];
    SIGMASTUDIOTYPE_REGISTER_CONVERT(pData, byte_data);
    SIGMA_WRITE_REGISTER_BLOCK(DSP_I2C_ADDR, address, 4, byte_data);
}

void SIGMA_WRITE_REGISTER_FLOAT(int address, double pData) {
    SIGMA_WRITE_REGISTER_INTEGER(address, SIGMASTUDIOTYPE_FIXPOINT_CONVERT(pData));
}

void SIGMA_WRITE_DELAY(uint8_t devAddress, int length, uint8_t pData[]) {
    int delay_length = 0;    // Initialize delay length variable
    for (uint8_t i = length; i > 0; i--) {
        // Unpack pData to calculate the delay length as an integer
        delay_length = (delay_length << 8) + pData[i];
    }
    HAL_Delay(delay_length);    // Delay this processor (not the DSP) by the appropriate time
}

// Function to read back data from the DSP, not called by SigmaStudio export files
void SIGMA_READ_REGISTER_BYTES(int address, int length, uint8_t *pData) {
}

int32_t SIGMA_READ_REGISTER_INTEGER(int address, int length) {
    int32_t result = 0;
    uint8_t register_value[length];
    SIGMA_READ_REGISTER_BYTES(address, length, register_value);
    for (int i = 0; i < length; i++) {
        result = (result << 8) + register_value[i];
    }
    return result;
}

double SIGMA_READ_REGISTER_FLOAT(int address) {
    int32_t integer_val = SIGMA_READ_REGISTER_INTEGER(address, 4);
// #if DSP_TYPE == DSP_TYPE_SIGMA300_350
//     return double(integer_val) / (1 << 24);
// #else
    return (double)(integer_val) / (1 << 23);
// #endif
}

// Function for reading and printing DSP registers to the serial port, not called by
// SigmaStudio export files
// Since memory must be allocated for register_value bytes, keep dataLength to a low value
void SIGMA_PRINT_REGISTER(int address, int dataLength) {

}

/* SIGMASTUDIOTYPE_INTEGER_CONVERT is included in export files but not generally required.
 * Here it is simply a passthrough macro.
 */
#define SIGMASTUDIOTYPE_INTEGER_CONVERT(_value) (_value)

#endif
