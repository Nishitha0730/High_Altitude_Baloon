#include "lora.h"
#include "../clk/clk.h"
#include "../uart/uart.h"
#include <stdint.h>

void SPI_init(void){
    RCC_APB2ENR |= (1<<SPI1EN);    // SPI 1 clock enable

    SPI_CR1 &= ~(7<<BR);
    SPI_CR1 |= (4<<BR);    // fPCLK/32


    SPI_CR1 &= ~(1<<DFF);   // 8-bit frame


    SPI_CR1 |= (1<<MSTR); // master

    SPI_CR1 |= (1<<SSM);
    SPI_CR1 |= (1<<SSI);

    GPIOA_CRL &= ~(0xFF<<8); // clear upper two bytes
    GPIOA_CRL |= (2 << MODE4) | (0 << CNF4);
    GPIOA_CRL |= (2 << MODE5) | (2 << CNF5);
    GPIOA_CRL |= (0 << MODE6) | (1 << CNF6);
    GPIOA_CRL |= (2 << MODE7) | (2 << CNF7);
    // GPIOA_CRL |= (2<<MODE4);GPIOA_CRL &= ~(3<<CNF4);  // NSS
    // GPIOA_CRL |= (2<<MODE5);GPIOA_CRL |= (2<<CNF5);   // SCK
    // GPIOA_CRL |= (0<<MODE6);GPIOA_CRL |= (2<<CNF6);   // MISO
    // GPIOA_CRL |= (2<<MODE7);GPIOA_CRL |= (2<<CNF7);   // MOSI

    GPIOA_ODR |= (1 << ODR4);  // cs pin high (not selected)

    SPI_CR1 |= (1<<SPE);  // spi enable
}


uint8_t SPI_Transfer(uint8_t write, uint8_t data)
{
    //READ - 0, WRITE -1

    while (!(SPI_SR & (1 << LORA_TXE)));

    // SPI_DR = (data|(write<<7));
    if (write) {
        SPI_DR = data | (1 << 7);  // Force MSB to 1 for write
    } else {
        SPI_DR = data & ~(1 << 7); // Force MSB to 0 for read
    }

    while (!(SPI_SR & (1 << LORA_RXNE)));

    return SPI_DR;
}

void LoRa_Select(void)
{
    GPIOA_ODR &= ~(1 << ODR4);
}

void LoRa_Unselect(void)
{
    while (SPI_SR & (1 << BSY));
    GPIOA_ODR |= (1 << ODR4);
}