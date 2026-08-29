#include <stdint.h>
#include "global/global.h"
#include "clk/clk.h"
#include "uart/uart.h"
#include "nvic.h"
#include "imu/mpu6050.h"


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
    NVIC_ISER1 |= (1 << 5);


    uint8_t status = 0;
    status = UART_Init();
    if(status){return 1;}


    I2C_Init();

    for (volatile uint32_t i = 0; i < 500000; i++);
    MPU6050_Init();
    for (volatile uint32_t i = 0; i < 500000; i++);

    uint8_t chip_id = MPU6050_TestConnection();
    if (chip_id != 0x68) 
    {
        while(1); 
    }


    status = UART_TX_Init();
    if(status){
        return 1;
    }

    while(1){

        MPU6050_ReadAccel(imu_buf);
        memcpy(&full_data[3],&imu_buf[0],sizeof(imu_buf));

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