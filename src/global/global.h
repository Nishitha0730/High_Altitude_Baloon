#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

extern uint8_t empty_data_reg;

extern uint8_t bmp180_cal_data_buf[22];
extern uint8_t ut_data[2];
extern uint8_t T_buf[4];
extern uint8_t up_data[3];
extern uint8_t pressure_data[4];
extern int32_t B5;


extern uint8_t full_data[19];

extern uint32_t start;
extern uint32_t end;
extern uint32_t cycles;

#endif