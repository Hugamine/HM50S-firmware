/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void JumpToBootloader(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PWR_SRC_Pin GPIO_PIN_1
#define PWR_SRC_GPIO_Port GPIOA
#define TX_EN_Pin GPIO_PIN_3
#define TX_EN_GPIO_Port GPIOA
#define SPK_EN_Pin GPIO_PIN_5
#define SPK_EN_GPIO_Port GPIOC
#define BTN_U_Pin GPIO_PIN_0
#define BTN_U_GPIO_Port GPIOB
#define BTN_L_Pin GPIO_PIN_1
#define BTN_L_GPIO_Port GPIOB
#define LO_CS_Pin GPIO_PIN_11
#define LO_CS_GPIO_Port GPIOE
#define LO_MUXOUT_Pin GPIO_PIN_15
#define LO_MUXOUT_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_14
#define LCD_RST_GPIO_Port GPIOB
#define PTT_Pin GPIO_PIN_10
#define PTT_GPIO_Port GPIOD
#define PTT_EXTI_IRQn EXTI15_10_IRQn
#define BTN_R_Pin GPIO_PIN_11
#define BTN_R_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_13
#define LED_R_GPIO_Port GPIOD
#define LCD_DC_Pin GPIO_PIN_14
#define LCD_DC_GPIO_Port GPIOD
#define LED_G_Pin GPIO_PIN_15
#define LED_G_GPIO_Port GPIOD
#define BTN_D_Pin GPIO_PIN_9
#define BTN_D_GPIO_Port GPIOC
#define LCD_BL_Pin GPIO_PIN_8
#define LCD_BL_GPIO_Port GPIOA
#define CHARGE_EN_Pin GPIO_PIN_15
#define CHARGE_EN_GPIO_Port GPIOA
#define LINE_IO_SW_R_Pin GPIO_PIN_0
#define LINE_IO_SW_R_GPIO_Port GPIOD
#define LINE_IO_SW_L_Pin GPIO_PIN_1
#define LINE_IO_SW_L_GPIO_Port GPIOD
#define LINE_AD_SW_L_Pin GPIO_PIN_2
#define LINE_AD_SW_L_GPIO_Port GPIOD
#define LINE_AD_SW_R_Pin GPIO_PIN_3
#define LINE_AD_SW_R_GPIO_Port GPIOD
#define PA_BIAS_EN_Pin GPIO_PIN_4
#define PA_BIAS_EN_GPIO_Port GPIOD
#define TR_SW_Pin GPIO_PIN_5
#define TR_SW_GPIO_Port GPIOD
#define RX_EN_Pin GPIO_PIN_6
#define RX_EN_GPIO_Port GPIOD
#define TP_CS_Pin GPIO_PIN_7
#define TP_CS_GPIO_Port GPIOD
#define TP_IRQ_Pin GPIO_PIN_3
#define TP_IRQ_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
