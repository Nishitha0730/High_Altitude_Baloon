#include "lora.h"
#include "../clk/clk.h"
#include "../uart/uart.h"
#include <stdint.h>
#include "lora_registers.h"
#include "lora_modes.h"

void SPI_init(void){
    RCC_APB2ENR |= (1<<SPI1EN);    // SPI 1 clock enable

    SPI_CR1 &= ~(7<<BR);
    SPI_CR1 |= (4<<BR);    // fPCLK/32


    SPI_CR1 &= ~(1<<DFF);   // 8-bit frame


    SPI_CR1 |= (1<<MSTR); // master

    SPI_CR1 |= (1<<SSM);
    SPI_CR1 |= (1<<SSI);

    // GPIOA_CRL &= ~(0xFF<<8); // clear upper two bytes
    GPIOA_CRL &= ~(0xFFFF << 16);
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


uint8_t SPI_Transfer(uint8_t data)
{
    while (!(SPI_SR & (1 << LORA_TXE)));

    SPI_DR = data;

    while (!(SPI_SR & (1 << LORA_RXNE)));

    return (uint8_t)SPI_DR;
}


// uint8_t SPI_Transfer(uint8_t write, uint8_t data)
// {
//     //READ - 0, WRITE -1

//     while (!(SPI_SR & (1 << LORA_TXE)));

//     // SPI_DR = (data|(write<<7));
//     if (write) {
//         SPI_DR = data | (1 << 7);  // Force MSB to 1 for write
//     } else {
//         SPI_DR = data & ~(1 << 7); // Force MSB to 0 for read
//     }

//     while (!(SPI_SR & (1 << LORA_RXNE)));

//     return SPI_DR;
// }

void LoRa_Select(void)
{
    GPIOA_ODR &= ~(1 << ODR4);
}

void LoRa_Unselect(void)
{
    while (SPI_SR & (1 << BSY));
    GPIOA_ODR |= (1 << ODR4);
}


uint8_t LoRa_ReadRegister(uint8_t address)
{
    uint8_t value=0xEE;

    LoRa_Select();

    /* Address: MSB = 0 for read */
    SPI_Transfer(address & 0x7F);

    /* Dummy byte generates clock */
    value = SPI_Transfer(0x00);

    LoRa_Unselect();

    return value;
}



void LoRa_WriteRegister(uint8_t address, uint8_t value)
{
    LoRa_Select();

    /* Address: MSB = 1 for write */
    SPI_Transfer(address | 0x80);

    SPI_Transfer(value);

    LoRa_Unselect();
}


void Lora_Reset(void){
    GPIOA_CRL &= ~(0xF0<<8);
    GPIOA_CRL |= (2 << MODE3) | (0 << CNF3);  //RST
    GPIOA_ODR &= ~(1<<ODR3);  // low
    for(volatile uint32_t i=0;i<10000;i++){
        //delay
    }
    GPIOA_ODR |= (1<<ODR3); //high
}

void Lora_Init(uint32_t freq){
    Lora_Reset();
    uint8_t version=0;
    version = LoRa_ReadRegister(REG_VERSION);
    if(version != 0x12){while(1){}}

    LoRa_WriteRegister(REG_OP_MODE,(1<<MODE_LONG_RANGE_MODE)|(0x00)); // sleep mode+lora enable
    LoRa_WriteRegister(REG_OP_MODE,(1<<MODE_LONG_RANGE_MODE)|(0x01)); //standby mode

    // Set Frequency (e.g., 433 MHz -> Frf = (433000000 << 19) / 32000000 = 7094272 -> 0x6C4000)
    uint64_t frf = ((uint64_t)freq << 19) / 32000000;
    LoRa_WriteRegister(REG_FRF_MSB,(uint8_t)(frf >> 16));
    LoRa_WriteRegister(REG_FRF_MID,(uint8_t)(frf >> 8));
    LoRa_WriteRegister(REG_FRF_LSB,(uint8_t)(frf));

    //Set FIFO TX base address to 0x00
    LoRa_WriteRegister(REG_FIFO_TX_BASE_ADDR, 0x00);
    LoRa_WriteRegister(REG_FIFO_ADDR_PTR, 0x00);

    //Basic Modem Setup: BW=125kHz, CR=4/5, SF=7
    LoRa_WriteRegister(REG_MODEM_CONFIG_1, 0x72); 
    LoRa_WriteRegister(REG_MODEM_CONFIG_2, 0x70); 

    //Set Power Level (Output power ~ 14 dBm via PA_BOOST)
    LoRa_WriteRegister(REG_PA_CONFIG, 0xCC);
}


void LoRa_SendPacket(uint8_t *buffer, uint8_t length) {
    // 1. Go to Standby
    LoRa_WriteRegister(REG_OP_MODE,  (1<<MODE_LONG_RANGE_MODE)| 0x01);

    // 2. Reset FIFO pointer to TX Base Address - for full 256 tx
    LoRa_WriteRegister(REG_FIFO_ADDR_PTR, 0x00);

    // 3. Write data to internal FIFO
    for (uint8_t i = 0; i < length; i++) {
        LoRa_WriteRegister(REG_FIFO, buffer[i]);
    }

    // 4. Specify the total length of transmission
    LoRa_WriteRegister(REG_PAYLOAD_LENGTH, length);

    // 5. Clear TX Done Flag by writing a 1 to bit 3 of IRQ flags
    LoRa_WriteRegister(REG_IRQ_FLAGS, 0x08);

    // 6. Set to Transmit Mode (MODE_TX)
    LoRa_WriteRegister(REG_OP_MODE, (1<<MODE_LONG_RANGE_MODE)| 0x03);

    // 7. Wait until TxDone bit (0x08) is set in IRQ Flags
    while ((LoRa_ReadRegister(REG_IRQ_FLAGS) & 0x08) == 0) {
        // Wait for hardware transmission to complete...
    }

    // 8. Clear the interrupt flag again
    LoRa_WriteRegister(REG_IRQ_FLAGS, 0x08);
}