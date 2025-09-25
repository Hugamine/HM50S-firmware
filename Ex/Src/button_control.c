#include "button_control.h"
#include "audio.h"
#include "gpio.h"

uint8_t read_PTT_state(void){
    return HAL_GPIO_ReadPin(PTT_GPIO_Port,  PTT_Pin);
}

// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
//     if(GPIO_Pin == PTT_Pin){
//         if(read_PTT_state() == GPIO_PIN_RESET){
//             // TR_SW_GPIO(0);
//             TX_EN_GPIO(1);
//             // RX_EN_GPIO(0);

//             // i2s_dma_stop();
//             // tx_i2s_dma_start();

//             // PA_BIAS_EN_GPIO(0);
//         }

//         else if(read_PTT_state() == GPIO_PIN_SET){
//             // PA_BIAS_EN_GPIO(1);
//             TX_EN_GPIO(0);
//             // RX_EN_GPIO(1);

//             // i2s_dma_stop();
//             // rx_i2s_dma_start();

//             // TR_SW_GPIO(1);
//         }
//     }
//     // __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
// }
