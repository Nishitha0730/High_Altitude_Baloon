#include <stdint.h>
#include "global.h"


uint8_t empty_data_reg=0;

uint8_t bmp180_cal_data_buf[22];
uint8_t ut_data[2];
uint8_t T_buf[4];
uint8_t up_data[3];
uint8_t pressure_data[4];
int32_t B5;


uint8_t full_data[19]={[0]=0xFF,0xFF,0xFF};





uint32_t start;
uint32_t end;
uint32_t cycles;
