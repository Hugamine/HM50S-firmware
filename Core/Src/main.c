/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_core.h"
#include "usb_device.h"
#include "adau1761.h"
#include "adf4351.h"
#include "tlv320aic3204.h"
#include "hilbert.h"
#include "lvgl.h"
#include "button_control.h"
#include "./src/drivers/display/st7789/lv_st7789.h"
#include "./demos/widgets/lv_demo_widgets.h"
#include "./demos/benchmark/lv_demo_benchmark.h"
#include "lv_port_indev.h"
#include "ui.h"
#include "audio.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_H_RES       240
#define LCD_V_RES       320
#define BUS_SPI1_POLL_TIMEOUT 0x1000U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
lv_display_t *lcd_disp;
volatile int lcd_bus_busy = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void LVGL_Task();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
// JumpToBootloader();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_I2S1_Init();
  MX_I2S2_Init();
  MX_QUADSPI_Init();
  MX_SPI3_Init();
  MX_SPI4_Init();
  MX_TIM4_Init();
  MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  MX_USB_DEVICE_Init();
  audio_init();
  adau1761_init();
  adf4351_init();
  codec_init();
  rfft_init();
  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
  SCB->CCR &= ~(1 << 3);//solves a big problem for lvgl

  RX_EN_GPIO(1);
  rx_i2s_dma_start();

  LVGL_Task();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
    // HAL_Delay(1010);
    lv_timer_handler();

    // Check touch detect pin
    // if (HAL_GPIO_ReadPin(TP_IRQ_GPIO_Port, TP_IRQ_Pin) == GPIO_PIN_RESET) {
    //     calibration_touch_detected();   // process one calibration touch
    //     while (HAL_GPIO_ReadPin(TP_IRQ_GPIO_Port, TP_IRQ_Pin) == GPIO_PIN_RESET);
    //     HAL_Delay(20);
    //  } // debounce
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi)
{
        /* CS high */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        lcd_bus_busy = 0;
        lv_display_flush_ready(lcd_disp);
}

/* Initialize LCD I/O bus, reset LCD */
static int32_t lcd_io_init(void)
{
        /* Register SPI Tx Complete Callback */
        HAL_SPI_RegisterCallback(&hspi3, HAL_SPI_TX_COMPLETE_CB_ID, lcd_color_transfer_ready_cb);

        /* reset LCD */
        HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
        HAL_Delay(100);

        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);

        return HAL_OK;
}

/* Platform-specific implementation of the LCD send command function. In general this should use polling transfer. */
static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
        LV_UNUSED(disp);
        while (lcd_bus_busy);   /* wait until previous transfer is finished */
        /* Set the SPI in 8-bit mode */
        hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
        HAL_SPI_Init(&hspi3);
        /* DCX low (command) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
        /* CS low */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
        /* send command */
        if (HAL_SPI_Transmit(&hspi3, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
                /* DCX high (data) */
                HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
                /* for short data blocks we use polling transfer */
                HAL_SPI_Transmit(&hspi3, (uint8_t *)param, (uint16_t)param_size, BUS_SPI1_POLL_TIMEOUT);
                /* CS high */
                HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        }
}

/* Platform-specific implementation of the LCD send color function. For better performance this should use DMA transfer.
 * In case of a DMA transfer a callback must be installed to notify LVGL about the end of the transfer.
 */
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
        LV_UNUSED(disp);
        while (lcd_bus_busy);   /* wait until previous transfer is finished */
        /* Set the SPI in 8-bit mode */
        hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
        HAL_SPI_Init(&hspi3);
        /* DCX low (command) */
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
        /* CS low */
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
        /* send command */
        if (HAL_SPI_Transmit(&hspi3, cmd, cmd_size, BUS_SPI1_POLL_TIMEOUT) == HAL_OK) {
                /* DCX high (data) */
                HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
                /* for color data use DMA transfer */
                /* Set the SPI in 16-bit mode to match endianness */
                hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
                HAL_SPI_Init(&hspi3);
                lcd_bus_busy = 1;
          // SCB_CleanInvalidateDCache();
                HAL_SPI_Transmit_DMA(&hspi3, param, (uint16_t)param_size / 2);
                /* NOTE: CS will be reset in the transfer ready callback */
        }
}

void LVGL_Task()
{
  /* Initialize LVGL */
  lv_init();

  /* Initialize LCD I/O */
  if (lcd_io_init() != 0)
    return;

  /* Create the LVGL display object and the LCD display driver */
  lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);
  lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_270);

  /* Allocate draw buffers on the heap. In this example we use two partial buffers of 1/10th size of the screen */
  __attribute__((aligned(4)))
  lv_color_t * buf1 = NULL;
  __attribute__((aligned(4)))
  lv_color_t * buf2 = NULL;

  uint32_t buf_size = LCD_H_RES * LCD_V_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

  buf1 = lv_malloc(buf_size);
  if(buf1 == NULL) {
    LV_LOG_ERROR("display draw buffer malloc failed");
    return;
  }

  buf2 = lv_malloc(buf_size);
  if(buf2 == NULL) {
    LV_LOG_ERROR("display buffer malloc failed");
    lv_free(buf1);
    return;
  }
  lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_port_indev_init();

  // ui_init(lcd_disp);
  // lv_demo_widgets();
  // start_touch_calibration();
  ui_init();

  // for(;;) {
  //   /* The task running lv_timer_handler should have lower priority than that running `lv_tick_inc` */
  //   lv_timer_handler();
  //   /* raise the task priority of LVGL and/or reduce the handler period can improve the performance */
  //   osDelay(10);
  // }
}

/*----------------------------DFU---------------------------*/
#ifndef BOOTLOADER_ADDR
 #define BOOTLOADER_ADDR  ((uint32_t)0x1FF09800U)
#endif

typedef void (*pFunction_t)(void);
pFunction_t BootLoaderResetHandler;

void JumpToBootloader(void)
{
    uint32_t bootAddr;
    uint32_t bootStack;

    /* Read bootloader stack pointer and reset vector from ROM table */
    bootStack = *(__IO uint32_t*) (BOOTLOADER_ADDR);         /* initial MSP from ROM */
    bootAddr  = *(__IO uint32_t*) (BOOTLOADER_ADDR + 4U);    /* reset handler address */

    /* Basic sanity check: ensure bootAddr not 0xFFFFFFFF or zero */
    // if ((bootAddr == 0xFFFFFFFFU) || (bootAddr == 0U)) {
    //     /* Boot address invalid — abort jump */
    //     return;
    // }


    HAL_MPU_Disable();

    __disable_irq();

    // Enable SYSCFG and remap system memory to 0x00000000
    // __HAL_RCC_SYSCFG_CLK_ENABLE();
    // SYSCFG->MEMRMP = 0x01;

    HAL_RCC_DeInit();

    /* Disable SysTick */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    SCB_CleanDCache();
    SCB_DisableICache();
    SCB_DisableDCache();

    /* Disable all interrupts in NVIC */
    for (uint32_t i = 0; i < 8; ++i) {
        NVIC->ICER[i] = 0xFFFFFFFFU; /* disable IRQs */
        NVIC->ICPR[i] = 0xFFFFFFFFU; /* clear pending */
    }

    HAL_DeInit();

    __enable_irq();
    
    /* Set main stack pointer to bootloader stack */

    BootLoaderResetHandler = (pFunction_t) bootAddr;

    __set_MSP(bootStack);

    __set_CONTROL(0);

    /* Jump to bootloader reset handler */
    BootLoaderResetHandler();

    /* Should never return here */
    while (1) { }
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.SubRegionDisable = 0x0;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
