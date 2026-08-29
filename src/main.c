#include <stdint.h>
#include "global/global.h"
#include "clk/clk.h"
#include "uart/uart.h"
#include "nvic.h"
#include "bmp180/bmp180.h"
#include "dwt/dwt.h"

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
    DWT_Init();
    NVIC_ISER1 |= (1 << 5);


    uint8_t status = 0;
    status = UART_Init();
    if(status){return 1;}


    I2C2_Init();

    uint8_t bmp180_id = bmp180_read_id();
    if(bmp180_id!=0x55){while(1);}

    for (volatile uint32_t i = 0; i < 500000; i++);
    bmp180_get_cal_param(bmp180_cal_data_buf);
    for (volatile uint32_t i = 0; i < 500000; i++);

    for (volatile uint32_t i = 0; i < 500000; i++);
    bmp180_get_ut(ut_data);
    for (volatile uint32_t i = 0; i < 500000; i++);


    status = UART_TX_Init();
    if(status){
        return 1;
    }

    while(1){
        start = DWT_CYCCNT;
        bmp180_get_ut(ut_data);
        bmp180_get_temperature(T_buf,&B5);
        end = DWT_CYCCNT;
        cycles = end - start;
        memcpy(&full_data[11],&cycles,sizeof(cycles));

        for (volatile uint32_t i = 0; i < 40000; i++); // 5ms
        start = DWT_CYCCNT;
        bmp180_get_up(up_data);

        uint32_t raw_up = (((uint32_t)up_data[0] << 16) | ((uint32_t)up_data[1] << 8) | (uint32_t)up_data[2]) >> (8 - 0);
        bmp180_cal_pressure(bmp180_cal_data_buf,pressure_data,raw_up,B5);
        end = DWT_CYCCNT;
        cycles = end - start;
        memcpy(&full_data[15],&cycles,sizeof(cycles));


        memcpy(&full_data[3],&T_buf[0],sizeof(T_buf));
        memcpy(&full_data[7],&pressure_data[0],sizeof(pressure_data));


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