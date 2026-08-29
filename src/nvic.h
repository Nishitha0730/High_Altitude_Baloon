#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

#define NVIC_ISER0 (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1 (*(volatile uint32_t *)0xE000E104)

#endif