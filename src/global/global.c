#include <stdint.h>
#include "global.h"


uint8_t empty_data_reg=0;

uint8_t imu_buf[14];
uint8_t full_data[17]={[0]=0xFF,0xFF,0xFF};