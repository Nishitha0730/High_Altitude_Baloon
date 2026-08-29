#ifndef UART_H
#define UART_H
#include <stdint.h>

uint8_t UART_Init(void);
uint8_t UART_TX_Init(void);
void UART_Send(uint8_t character);

#define USART1_BASE (uint32_t)0x40013800

#define USART_SR (*(volatile uint32_t *)(USART1_BASE+0x00))
#define TXE 7
#define TC 6

#define USART_DR (*(volatile uint32_t *)(USART1_BASE+0x04))
#define DR 0


#define USART_BRR (*(volatile uint32_t *)(USART1_BASE+0x08))
#define DIV_Mantissa 4
#define DIV_Fraction 0

#define USART_CR1 (*(volatile uint32_t *)(USART1_BASE+0x0C))
#define UE 13
#define M 12
#define TE 3
#define TXEIE 7

#define USART_CR2 (*(volatile uint32_t *)(USART1_BASE+0x10))
#define USART_CR2_STOP 12


#define GPIOA_BASE (uint32_t)0x40010800

#define GPIOA_CRH (*(volatile uint32_t *)(GPIOA_BASE+0x04))
#define CNF9 6
#define MODE9 4

#endif