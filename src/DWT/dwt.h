#ifndef DWT_H
#define DWT_H

#include <stdint.h>

void DWT_Init(void);

#define DEMCR        (*(volatile uint32_t *)0xE000EDFC)
#define DWT_CTRL     (*(volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT   (*(volatile uint32_t *)0xE0001004)

#define DEMCR_TRCENA     (1 << 24)
#define DWT_CYCCNTENA    (1 << 0)

#endif