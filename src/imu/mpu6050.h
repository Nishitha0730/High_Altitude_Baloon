#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>


void I2C_Init(void);
void I2C_Start(void);
void I2C_Send_Address(uint8_t address, uint8_t read);
void I2C_Send_Data(uint8_t data);
void I2C_Stop(void);
uint8_t I2C_Read_ACK(void);
uint8_t I2C_Read_NACK(void);
void MPU6050_Init(void);
void MPU6050_ReadAccel(uint8_t *buf);
uint8_t MPU6050_TestConnection(void);

#define I2C1_BASE 0x40005400

#define I2C_CR1 (*(volatile uint32_t *)(I2C1_BASE+0x00))
#define PE 0
#define START 8
#define ACK 10
#define STOP 9

#define I2C_CR2 (*(volatile uint32_t *)(I2C1_BASE+0x04))
#define FREQ 0
#define ITBUFEN 10

#define I2C_DR (*(volatile uint32_t *)(I2C1_BASE+0x10))
#define DR 0


#define I2C_SR1 (*(volatile uint32_t *)(I2C1_BASE+0x14))
#define SB 0
#define ADDR 1
#define RxNE 6
#define TxE 7
#define BTF 2

#define BERR  8
#define ARLO  9
#define AF    10
#define OVR   11




#define I2C_SR2 (*(volatile uint32_t *)(I2C1_BASE+0x18))
#define BUSY 1
#define MSL 0

#define I2C_CCR (*(volatile uint32_t *)(I2C1_BASE+0x1C))
#define CCR 0

#define I2C_TRISE (*(volatile uint32_t *)(I2C1_BASE+0x20))
#define TRISE 0



#define PORTB_BASE 0x40010C00

#define GPIOB_CRL (*(volatile uint32_t *)(PORTB_BASE+0x00))
#define MODE6 24
#define CNF6 26
#define MODE7 28
#define CNF7 30



#define RCC_BASE        0x40021000

// #define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))
// #define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x1C))

#define RCC_APB1RSTR (*(volatile uint32_t *)(0x40021000 + 0x10))
#define I2C1RST 21

#endif