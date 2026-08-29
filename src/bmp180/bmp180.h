#ifndef BMP180_H
#define BMP180_H

#include <stdint.h>

void bmp180_get_cal_param(uint8_t *buf);
void bmp180_get_ut(uint8_t *buf);
// void bmp180_get_temperature(uint8_t *buf);
void bmp180_get_temperature(uint8_t *buf,int32_t *ptr);
void bmp180_get_up(uint8_t *buf);
void bmp180_cal_pressure(uint8_t *cal,uint8_t *buf,int32_t UP,int32_t B5);

uint8_t bmp180_read_id(void);



void I2C2_Init(void);
void I2C2_Start(void);
void I2C2_Send_Address(uint8_t address, uint8_t read);
void I2C2_Send_Data(uint8_t data);
void I2C2_Stop(void);
uint8_t I2C2_Read_ACK(void);
uint8_t I2C2_Read_NACK(void);



#define I2C2_BASE 0x40005800

#define I2C2_CR1 (*(volatile uint32_t *)(I2C2_BASE+0x00))


#define I2C2_CR2 (*(volatile uint32_t *)(I2C2_BASE+0x04))


#define I2C2_DR (*(volatile uint32_t *)(I2C2_BASE+0x10))



#define I2C2_SR1 (*(volatile uint32_t *)(I2C2_BASE+0x14))



#define I2C2_SR2 (*(volatile uint32_t *)(I2C2_BASE+0x18))


#define I2C2_CCR (*(volatile uint32_t *)(I2C2_BASE+0x1C))


#define I2C2_TRISE (*(volatile uint32_t *)(I2C2_BASE+0x20))






#define GPIOB_CRH (*(volatile uint32_t *)(PORTB_BASE+0x04))
#define MODE10 8
#define CNF10 10
#define MODE11 12
#define CNF11 14


// #define RCC_APB1RSTR (*(volatile uint32_t *)(0x40021000 + 0x10))
#define I2C2RST 22
#define I2C2EN 22

#endif