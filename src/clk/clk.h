#ifndef CLK_H
#define CLK_H

#include <stdint.h>

void Clk_Init(void);

#define RCC_BASE 0x40021000

#define RCC_CR (*(volatile uint32_t *)(RCC_BASE+0x00))
#define HSERDY 17
#define HSEON 16

#define RCC_CFGR (*(volatile uint32_t *)(RCC_BASE+0x04))
#define SW 0
#define SWS 2
#define HPRE 4
#define PPRE2 11

#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE+0x18))
#define USART1EN 14
#define IOPAEN 2
#define IOPBEN 3
#define AFIOEN 0

#define RCC_APB1ENR (*(volatile uint32_t *)(RCC_BASE+0x1C))
#define I2C1EN 21

#endif