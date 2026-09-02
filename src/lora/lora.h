#ifndef LORA_H
#define LORA_H
#include <stdint.h>

void SPI_init(void);
// uint8_t SPI_Transfer(uint8_t read, uint8_t data);
uint8_t SPI_Transfer(uint8_t data);
// void LoRa_Select(void);
// void LoRa_Unselect(void);

uint8_t LoRa_ReadRegister(uint8_t address);
void LoRa_WriteRegister(uint8_t address, uint8_t value);




void Lora_Reset(void);
void Lora_Init(uint32_t freq);
void LoRa_SendPacket(uint8_t *buffer, uint8_t length);


#define SPI1_BASE 0x40013000UL

#define SPI_CR1 *(volatile uint32_t *)(SPI1_BASE+0x00)
#define DFF 11
#define SPE 6
#define MSTR 2
#define SSM 9
#define SSI 8
#define BR 3

#define SPI_DR *(volatile uint32_t *)(SPI1_BASE+0x0C)
#define DR 0


#define SPI_SR *(volatile uint32_t *)(SPI1_BASE+0x08)
#define BSY 7
#define LORA_TXE 1
#define LORA_RXNE 0


#define SPI1EN 12


#define GPIOA_CRL *(volatile uint32_t *)(GPIOA_BASE+0x00)
#define MODE4 16
#define MODE5 20
#define MODE6 24
#define MODE7 28
#define CNF4 18
#define CNF5 22
#define CNF6 26
#define CNF7 30

#define GPIOA_ODR *(volatile uint32_t *)(GPIOA_BASE+0x0C)
#define ODR4 4


//Lora RST - PA3
#define MODE3 12
#define CNF3 14
#define ODR3 3

#endif