#include "uart.h"
#include <stdint.h>

uint8_t UART_Init(void){
    GPIOA_CRH &= ~(3<<MODE9);
    GPIOA_CRH |= (2<<MODE9);  // max out speed 2MHz

    GPIOA_CRH &= ~(3<<CNF9);
    GPIOA_CRH |= (2<<CNF9);  //  Alternate function output Push-pull

    return 0;
}


uint8_t UART_TX_Init(void){
    USART_CR1 |= (1<<UE); // enable uart
    USART_CR1 &= ~(1<<M);

    USART_CR2 &= ~(3<<USART_CR2_STOP);  // 1 - stop bit

    USART_BRR = (uint32_t)0x0D05;   // set baud rate to 2400bps

    USART_CR1 |= (1<<TE);
    // USART_CR1 |= (1<<TXEIE); // interrupt generate

    return 0;
}


void UART_Send(uint8_t character){
    USART_DR = character;
}