#include <stdint.h>
#include "global.h"


uint8_t empty_data_reg=0;

uint8_t imu_buf[14];
// uint8_t full_data[17]={[0]=0xFF,0xFF,0xFF};

uint8_t full_data[21]={[0]=0xFF,0xFE,0xFF}; // +cpu cycles


// find clk cycles for each task
uint32_t start;
uint32_t end;
uint32_t cycles;