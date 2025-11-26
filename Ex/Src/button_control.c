#include "button_control.h"
#include "audio.h"
#include "gpio.h"

uint8_t read_PTT_state(void){
    return HAL_GPIO_ReadPin(PTT_GPIO_Port,  PTT_Pin);
}

void tx(void){
    LINE_IO_SW_R(1);

    LED_R_GPIO(1);
    TR_SW_GPIO(0);
    TX_EN_GPIO(1);
    RX_EN_GPIO(0);

    i2s_dma_stop();
    tx_i2s_dma_start();

    PA_BIAS_EN_GPIO(0);

    tx_en = true;
}

void rx(void){
    LINE_IO_SW_R(0);

    LED_R_GPIO(0);
    PA_BIAS_EN_GPIO(1);
    TX_EN_GPIO(0);
    RX_EN_GPIO(1);

    i2s_dma_stop();
    rx_i2s_dma_start();

    TR_SW_GPIO(1);

    tx_en = false;
}
