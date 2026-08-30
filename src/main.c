#include <stdint.h>
#include "global/global.h"
#include "clk/clk.h"
#include "uart/uart.h"
#include "nvic.h"
#include "imu/mpu6050.h"
#include "lora/lora.h"

#include <string.h>

volatile uint8_t data_ind = 0;
volatile uint8_t tx_busy = 0;


void SystemInit(void) {
}

void USART1_IRQHandler(void){
    if((USART_SR&(1<<TXE))&&(data_ind<sizeof(full_data))){
        UART_Send(full_data[data_ind++]);
    }else{
        USART_CR1 &= ~(1<<TXEIE);  // clear interrupt
        data_ind=0;
        tx_busy=0;
    }
}


int main(void){
    Clk_Init();
    SPI_init();
    NVIC_ISER1 |= (1 << 5);

    uint8_t status = 0;
    status = UART_Init();
    if(status){return 1;}

    status = UART_TX_Init();
    if(status){
        return 1;
    }


    lora_id = LoRa_ReadRegister(0x42);
    full_data[sizeof(full_data)-1]=lora_id;
    // memcpy(&full_data[3],&lora_id,sizeof(lora_id));

    while(1){

        // memcpy(&full_data[3],&lora_id,sizeof(lora_id));

        for(uint32_t i=0;i<0xFFFF;i++){
            //delay
        }

        if(!tx_busy){
            tx_busy=1;
            data_ind=0;
            USART_CR1 |= (1<<TXEIE); // interrupt generate
        }

    }

    return 0;
}