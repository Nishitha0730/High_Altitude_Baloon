#include "clk.h"
#include <stdint.h>


void Clk_Init(void){
    RCC_CR |= (1<<HSEON);    // HSE oscillator ON
    while(!(RCC_CR&(1<<HSERDY)));   // wait HSE oscillator ready 

    //  HSE selected as system clock
    RCC_CFGR &= ~(0x3 << SW);
    RCC_CFGR |=  (0x1 << SW);

    while (((RCC_CFGR >> SWS) & 0x3) != 0x1);   // wait HSE oscillator used as system clock
    RCC_CFGR &= ~(0xF<<HPRE);  // SYSCLK not divided
    RCC_CFGR &= ~(7<<PPRE2);  // 0xx: HCLK not divided

    RCC_APB2ENR |= (1<<USART1EN);  // USART1 clock enable
    RCC_APB2ENR |= (1<<IOPAEN);   // I/O port A clock enable
    RCC_APB2ENR |= (1<<IOPBEN);   // // I/O port B clock enable
    RCC_APB2ENR |= (1<<AFIOEN); 

    RCC_APB1ENR |= (1<<I2C1EN);  // I2C 1 clock enable
}