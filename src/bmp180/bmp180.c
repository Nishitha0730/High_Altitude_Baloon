#include <stdint.h>
#include "bmp180.h"
#include "../imu/mpu6050.h"
#include "../global/global.h"
#include "../clk/clk.h"


void bmp180_get_cal_param(uint8_t *buf){
    I2C2_Start();
    I2C2_Send_Address(0x77,0);
    I2C2_Send_Data(0xAA);

    I2C2_Start();
    I2C2_Send_Address(0x77,1);

    I2C2_CR1 |= (1 << ACK);

    for (int i = 0; i < 22; i++)
    {
        if (i == 21) // Last byte setup
        {
            I2C2_CR1 &= ~(1 << ACK); // NACK next byte
            I2C2_CR1 |= (1 << STOP);  // Send STOP condition after last byte
        }

        while (!(I2C2_SR1 & (1 << RxNE))); // Wait until RX buffer is not empty
        buf[i] = I2C2_DR;
    }

    // I2C_Stop();

}


void bmp180_get_ut(uint8_t *buf)
{
    I2C2_Start();
    I2C2_Send_Address(0x77,0);
    I2C2_Send_Data(0xF4);
    I2C2_Send_Data(0x2E);
    I2C2_Stop();

    for (volatile uint32_t i = 0; i < 40000; i++); // 5ms delay

    I2C2_Start();
    I2C2_Send_Address(0x77,0);
    I2C2_Send_Data(0xF6);

    I2C2_Start();
    I2C2_Send_Address(0x77,1);

    I2C2_CR1 |= (1 << ACK);

    while (!(I2C2_SR1 & (1 << RxNE)));
    buf[0] = I2C2_DR;

    I2C2_CR1 &= ~(1 << ACK);
    I2C2_CR1 |= (1 << STOP);
    while (!(I2C2_SR1 & (1 << RxNE)));
    buf[1] = I2C2_DR;


}


void bmp180_get_temperature(uint8_t *buf,int32_t *ptr)
{
    int32_t UT =
        ((uint16_t)ut_data[0] << 8) |
        ut_data[1];

    int32_t AC5 =
        ((uint16_t)bmp180_cal_data_buf[8] << 8) |
        bmp180_cal_data_buf[9];

    int32_t AC6 =
        ((uint16_t)bmp180_cal_data_buf[10] << 8) |
        bmp180_cal_data_buf[11];

    int32_t MC = (int16_t)(
        ((uint16_t)bmp180_cal_data_buf[18] << 8) |
        bmp180_cal_data_buf[19]
    );

    int32_t MD = (int16_t)(
        ((uint16_t)bmp180_cal_data_buf[20] << 8) |
        bmp180_cal_data_buf[21]
    );

    int32_t X1 = ((UT - AC6) * AC5) >> 15;
    int32_t X2 = (MC * 2048) / (X1 + MD);
    // int16_t X1 = (uint16_t((ut_data[0]<<8)|ut_data[1]) - uint16_t((bmp180_cal_data_buf[10]<<8)|bmp180_cal_data_buf[11]))*(uint16_t((bmp180_cal_data_buf[8]<<8)|bmp180_cal_data_buf[9]))>>15;
    // int16_t X2 = ((int16_t(bmp180_cal_data_buf[18]<<8|bmp180_cal_data_buf[19]))<<11)/(X1+int16_t(bmp180_cal_data_buf[20]<<8|bmp180_cal_data_buf[21]));
    
    int32_t B5 = X1+X2;
    *ptr = B5; 
    int32_t T = (B5+8)>>4;
    buf[0] = (uint8_t)(T>>24);
    buf[1] = (uint8_t)(T>>16);
    buf[2] = (uint8_t)(T>>8);
    buf[3] =(uint8_t)T;
}



void bmp180_get_up(uint8_t *buf){
    I2C2_Start();
    I2C2_Send_Address(0x77,0);
    I2C2_Send_Data(0xF4);
    I2C2_Send_Data(0x34);
    I2C2_Stop();

    for (volatile uint32_t i = 0; i < 40000; i++); // 5ms delay

    I2C2_Start();
    I2C2_Send_Address(0x77,0);
    I2C2_Send_Data(0xF6);

    I2C2_Start();
    I2C2_Send_Address(0x77,1);

    I2C2_CR1 |= (1 << ACK);

    // Byte 1 (MSB)
    while (!(I2C2_SR1 & (1 << RxNE)));
    buf[0] = I2C2_DR;

    // Prepare NACK & STOP before reading the last byte
    while (!(I2C2_SR1 & (1 << RxNE))); // Wait for Byte 2 (LSB)
    I2C2_CR1 &= ~(1 << ACK);           // NACK the upcoming Byte 3
    buf[1] = I2C2_DR;
    I2C2_CR1 |= (1 << STOP);           // Schedule STOP condition

    // Byte 3 (XLSB)
    while (!(I2C2_SR1 & (1 << RxNE)));
    buf[2] = I2C2_DR;

}


void bmp180_cal_pressure(uint8_t *cal, uint8_t *buf, int32_t UP, int32_t B5)
{
    // 1. Correct variable sizing based on official Bosch datasheet
    int16_t AC1, AC2, AC3;
    uint16_t AC4;
    int16_t B1, B2;

    int32_t B6, X1, X2, X3, B3;
    uint32_t B4, B7;
    int32_t p;
    
    // Assuming OSS (Oversampling Setting) = 0 for your code structure
    uint8_t oss = 0; 

    // 2. Parse calibration coefficients properly using explicitly matching types
    AC1 = (int16_t)(((uint16_t)cal[0] << 8) | cal[1]);
    AC2 = (int16_t)(((uint16_t)cal[2] << 8) | cal[3]);
    AC3 = (int16_t)(((uint16_t)cal[4] << 8) | cal[5]);
    AC4 = (uint16_t)(((uint16_t)cal[6] << 8) | cal[7]);
    
    // Note: cal[8] through cal[11] are AC5 and AC6 (used for temperature/B5 calculation)
    
    B1  = (int16_t)(((uint16_t)cal[12] << 8) | cal[13]);
    B2  = (int16_t)(((uint16_t)cal[14] << 8) | cal[15]);

    // 3. Official pressure compensation algorithm math
    B6 = B5 - 4000;
    X1 = (B2 * ((B6 * B6) >> 12)) >> 11;
    X2 = (AC2 * B6) >> 11;
    X3 = X1 + X2;
    
    // B3 depends heavily on your Oversampling Setting (OSS)
    B3 = ((((int32_t)AC1 * 4 + X3) << oss) + 2) >> 2;

    X1 = (AC3 * B6) >> 13;
    X2 = (B1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = (AC4 * (uint32_t)(X3 + 32768)) >> 15;
    
    // Corrected shift implementation for B7 depending on OSS
    B7 = ((uint32_t)UP - B3) * (50000 >> oss);

    if (B7 < 0x80000000)
    {
        // Safe casting to 64-bit prevents register overflow during calculation
        p = (int32_t)(((uint64_t)B7 * 2) / B4);
    }
    else
    {
        p = (int32_t)((B7 / B4) * 2);
    }

    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    
    // Final pressure value in Pascals (Pa)
    p = p + ((X1 + X2 + 3791) >> 4);

    // 4. Pack the 32-bit Pascal value back into your byte buffer
    buf[0] = (uint8_t)(p >> 24);
    buf[1] = (uint8_t)(p >> 16);
    buf[2] = (uint8_t)(p >> 8);
    buf[3] = (uint8_t)(p);
}



uint8_t bmp180_read_id(void)
{
    uint8_t id;

    I2C2_Start();

    I2C2_Send_Address(0x77, 0);

    I2C2_Send_Data(0xD0);

    I2C2_Start();

    I2C2_Send_Address(0x77, 1);

    I2C2_CR1 &= ~(1 << ACK);
    I2C2_CR1 |= (1 << STOP);

    while (!(I2C2_SR1 & (1 << RxNE)));

    id = I2C2_DR;

    return id;
}


void I2C2_Init(void)
{
    // 1. Enable Clocks for GPIOB and I2C1
    // RCC_APB2ENR |= (1 << 3);  // IOPBEN: Enable GPIOB clock
    RCC_APB1ENR |= (1 << I2C2EN); // I2C1EN: Enable I2C2 clock

    // Reset I2C2 peripheral
    RCC_APB1RSTR |=  (1 << I2C2RST);
    RCC_APB1RSTR &= ~(1 << I2C2RST);    

    // 2. Clear 4 bits for Pin 10 and Pin 11 completely (0xF = 4 bits)
    GPIOB_CRH &= ~((0xF << MODE10) | (0xF << MODE11));

    // Configure PB10 (SCL) and PB11 (SDA) as Alt Func Open-Drain, 10MHz (0b1110 -> CNF=11, MODE=10)
    GPIOB_CRH |= ((3 << CNF10) | (2 << MODE10));
    GPIOB_CRH |= ((3 << CNF11) | (2 << MODE11));

    // 3. Setup I2C Clock Configuration
    I2C2_CR2 &= ~(0x3F << FREQ); 
    I2C2_CR2 |= (8 << FREQ);      // 8 MHz APB1 clock

    I2C2_CCR &= ~0xFFF;
    I2C2_CCR |= (40 << CCR);     // Standard mode 100kHz

    I2C2_TRISE &= ~0x3F;
    I2C2_TRISE |= (9 << TRISE);

    // 4. Enable I2C
    I2C2_CR1 |= (1 << PE);
}

void I2C2_Start(void)
{
    I2C2_CR1 |= (1 << START);
    while (!(I2C2_SR1 & (1 << SB)));
}

void I2C2_Send_Address(uint8_t address, uint8_t read)
{
    I2C2_DR = (address << 1) | (read & 0x01);

    // Wait until address matched flag is set
    while (!(I2C2_SR1 & (1 << ADDR)));

    // Clear ADDR flag by reading SR1 followed by SR2
    volatile uint32_t temp = I2C2_SR1;
    temp = I2C2_SR2;
    (void)temp;
}

void I2C2_Send_Data(uint8_t data)
{
    while (!(I2C2_SR1 & (1 << TxE)));
    I2C2_DR = data;
    while (!(I2C2_SR1 & (1 << BTF))); // Wait for byte transfer complete
}

void I2C2_Stop(void)
{
    I2C2_CR1 |= (1 << STOP);
}