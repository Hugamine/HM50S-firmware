/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define TX_EN_GPIO(state) HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, state)
#define RX_EN_GPIO(state) HAL_GPIO_WritePin(RX_EN_GPIO_Port, RX_EN_Pin, state)
#define TR_SW_GPIO(state) HAL_GPIO_WritePin(TR_SW_GPIO_Port, TR_SW_Pin, state)
#define PA_BIAS_EN_GPIO(state) HAL_GPIO_WritePin(PA_BIAS_EN_GPIO_Port, PA_BIAS_EN_Pin, state)
#define SPK_EN_GPIO(state) HAL_GPIO_WritePin(SPK_EN_GPIO_Port, SPK_EN_Pin, state)
#define CHARGE_EN_GPIO(state) HAL_GPIO_WritePin(CHARGE_EN_GPIO_Port, CHARGE_EN_Pin, state)
#define LED_R_GPIO(state) HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, state)
#define LED_G_GPIO(state) HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, state)

#define LINE_IO_SW_R(state) HAL_GPIO_WritePin(LINE_IO_SW_R_GPIO_Port, LINE_IO_SW_R_Pin, state)
#define LINE_IO_SW_L(state) HAL_GPIO_WritePin(LINE_IO_SW_L_GPIO_Port, LINE_IO_SW_L_Pin, state)
#define LINE_AD_SW_R(state) HAL_GPIO_WritePin(LINE_AD_SW_R_GPIO_Port, LINE_AD_SW_R_Pin, state)
#define LINE_AD_SW_L(state) HAL_GPIO_WritePin(LINE_AD_SW_LGPIO_Port, LINE_AD_SW_L_Pin, state)
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

