#include "dwt.h"

void DWT_Init(void)
{
    /* Enable trace system */
    DEMCR |= DEMCR_TRCENA;

    /* Reset cycle counter */
    DWT_CYCCNT = 0;

    /* Enable cycle counter */
    DWT_CTRL |= DWT_CYCCNTENA;
}