#ifndef LORA_REGISTERS_H
#define LORA_REGISTERS_H

/*Lora Mode*/

#define REG_FIFO 0x00
#define REG_OP_MODE 0x01

#define REG_FRF_MSB 0x06
#define REG_FRF_MID 0x07
#define REG_FRF_LSB 0x08

#define REG_PA_CONFIG 0x09

#define REG_FIFO_ADDR_PTR 0x0D
#define REG_FIFO_TX_BASE_ADDR 0x0E
#define REG_FIFO_RX_BASE_ADDR 0x0F

#define REG_FIFO_RX_CURRENT_ADDR    0x10

#define REG_IRQ_FLAGS_MASK          0x11
#define REG_IRQ_FLAGS               0x12

#define REG_RX_NB_BYTES             0x13

#define REG_MODEM_CONFIG_1          0x1D
#define REG_MODEM_CONFIG_2          0x1E

#define REG_PREAMBLE_MSB            0x20
#define REG_PREAMBLE_LSB            0x21

#define REG_PAYLOAD_LENGTH          0x22

#define REG_MODEM_CONFIG_3          0x26

#define REG_DIO_MAPPING_1           0x40

#define REG_VERSION                 0x42


#endif