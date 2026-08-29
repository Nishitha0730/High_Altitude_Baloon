#include <stdint.h>
#include "mpu6050.h"
#include "../clk/clk.h"


void I2C_Init(void)
{
    // 1. Enable Clocks for GPIOB and I2C1
    RCC_APB2ENR |= (1 << 3);  // IOPBEN: Enable GPIOB clock
    RCC_APB1ENR |= (1 << 21); // I2C1EN: Enable I2C1 clock

    // Reset I2C1 peripheral
    RCC_APB1RSTR |=  (1 << I2C1RST);
    RCC_APB1RSTR &= ~(1 << I2C1RST);    

    // 2. Clear 4 bits for Pin 6 and Pin 7 completely (0xF = 4 bits)
    GPIOB_CRL &= ~((0xF << MODE6) | (0xF << MODE7));

    // Configure PB6 (SCL) and PB7 (SDA) as Alt Func Open-Drain, 10MHz (0b1110 -> CNF=11, MODE=10)
    GPIOB_CRL |= ((3 << CNF6) | (2 << MODE6));
    GPIOB_CRL |= ((3 << CNF7) | (2 << MODE7));

    // 3. Setup I2C Clock Configuration
    I2C_CR2 &= ~(0x3F << FREQ); 
    I2C_CR2 |= (8 << FREQ);      // 8 MHz APB1 clock

    I2C_CCR &= ~0xFFF;
    I2C_CCR |= (40 << CCR);     // Standard mode 100kHz

    I2C_TRISE &= ~0x3F;
    I2C_TRISE |= (9 << TRISE);

    // 4. Enable I2C
    I2C_CR1 |= (1 << PE);
}

void I2C_Start(void)
{
    I2C_CR1 |= (1 << START);
    while (!(I2C_SR1 & (1 << SB)));
}

void I2C_Send_Address(uint8_t address, uint8_t read)
{
    I2C_DR = (address << 1) | (read & 0x01);

    // Wait until address matched flag is set
    while (!(I2C_SR1 & (1 << ADDR)));

    // Clear ADDR flag by reading SR1 followed by SR2
    volatile uint32_t temp = I2C_SR1;
    temp = I2C_SR2;
    (void)temp;
}

void I2C_Send_Data(uint8_t data)
{
    while (!(I2C_SR1 & (1 << TxE)));
    I2C_DR = data;
    while (!(I2C_SR1 & (1 << BTF))); // Wait for byte transfer complete
}

void I2C_Stop(void)
{
    I2C_CR1 |= (1 << STOP);
}

// Correct Multi-Byte Acceleration Reading for STM32 I2C
void MPU6050_ReadAccel(uint8_t *buf)
{
    // Write register address to read from
    I2C_Start();
    I2C_Send_Address(0x68, 0);
    I2C_Send_Data(0x3B); // ACCEL_XOUT_H

    // Repeated Start for Reading
    I2C_Start();
    I2C_Send_Address(0x68, 1);

    // Enable ACK for multi-byte reception
    I2C_CR1 |= (1 << ACK);

    for (int i = 0; i < 14; i++)
    {
        if (i == 13) // Last byte setup
        {
            I2C_CR1 &= ~(1 << ACK); // NACK next byte
            I2C_CR1 |= (1 << STOP);  // Send STOP condition after last byte
        }

        while (!(I2C_SR1 & (1 << RxNE))); // Wait until RX buffer is not empty
        buf[i] = I2C_DR;
    }

    I2C_Stop();
}

void MPU6050_Init(void)
{
    I2C_Start();
    I2C_Send_Address(0x68, 0);
    I2C_Send_Data(0x6B); // PWR_MGMT_1
    I2C_Send_Data(0x00); // Wake up MPU6050
    I2C_Stop();
}

uint8_t MPU6050_TestConnection(void)
{
    uint8_t id = 0;

    I2C_Start();
    I2C_Send_Address(0x68, 0); // Write mode
    I2C_Send_Data(0x75);        // Register address for WHO_AM_I

    I2C_Start();               // Repeated Start
    I2C_Send_Address(0x68, 1); // Read mode

    I2C_CR1 &= ~(1 << ACK);    // Disable ACK for single-byte read
    I2C_CR1 |= (1 << STOP);    // Generate STOP after byte

    while (!(I2C_SR1 & (1 << RxNE))); // Wait for RX
    id = I2C_DR;

    return id; // Should return 0x68
}