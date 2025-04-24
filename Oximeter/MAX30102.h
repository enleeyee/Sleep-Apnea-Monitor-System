#pragma once

#include <stdint.h>
#include <stdbool.h>

// I2C Address
#define MAX30102_ADDRESS 0x57

// Registers
#define REG_INTR_STATUS_1    0x00
#define REG_INTR_STATUS_2    0x01
#define REG_INTR_ENABLE_1    0x02
#define REG_INTR_ENABLE_2    0x03
#define REG_FIFO_WR_PTR      0x04
#define REG_OVF_COUNTER      0x05
#define REG_FIFO_RD_PTR      0x06
#define REG_FIFO_DATA        0x07
#define REG_FIFO_CONFIG      0x08
#define REG_MODE_CONFIG      0x09
#define REG_SPO2_CONFIG      0x0A
#define REG_LED1_PA          0x0C // Red LED
#define REG_LED2_PA          0x0D // IR LED
#define REG_PILOT_PA         0x10
#define REG_MULTI_LED_CTRL1  0x11
#define REG_MULTI_LED_CTRL2  0x12
#define REG_TEMP_INT         0x1F
#define REG_TEMP_FRAC        0x20
#define REG_PART_ID          0xFF

// Function Prototypes
void MAX30102_WriteReg(uint8_t reg, uint8_t value);
uint8_t MAX30102_ReadReg(uint8_t reg);
void MAX30102_Init(void);
uint32_t MAX30102_ReadIR(void);
void MAX30102_Reset(void);
void MAX30102_ClearFIFO(void);
