#include "MAX30102.h"
#include "../inc/I2C0.h"
#include <stdint.h>

void MAX30102_WriteReg(uint8_t reg, uint8_t value) {
    I2C0_Send2(MAX30102_ADDRESS, reg, value);
}

uint8_t MAX30102_ReadReg(uint8_t reg) {
    return I2C0_Recv1(MAX30102_ADDRESS, reg);
}

void MAX30102_Init(void) {
    MAX30102_Reset();

    MAX30102_WriteReg(REG_INTR_ENABLE_1, 0xC0); // A_FULL & PPG_RDY enable
    MAX30102_WriteReg(REG_INTR_ENABLE_2, 0x00);

    MAX30102_WriteReg(REG_FIFO_WR_PTR, 0x00);
    MAX30102_WriteReg(REG_OVF_COUNTER, 0x00);
    MAX30102_WriteReg(REG_FIFO_RD_PTR, 0x00);

    MAX30102_WriteReg(REG_FIFO_CONFIG, 0x0F); // Sample avg = 4, FIFO rollover disable, almost full = 17

    MAX30102_WriteReg(REG_MODE_CONFIG, 0x03); // SpO2 mode = Red + IR

    MAX30102_WriteReg(REG_SPO2_CONFIG, 0x27); // 100 Hz sample rate, 411us pulse width

    MAX30102_WriteReg(REG_LED1_PA, 0x24); // Red LED pulse amplitude
    MAX30102_WriteReg(REG_LED2_PA, 0x24); // IR LED pulse amplitude

    MAX30102_WriteReg(REG_MULTI_LED_CTRL1, 0x21); // Slot 1: RED, Slot 2: IR
    MAX30102_WriteReg(REG_MULTI_LED_CTRL2, 0x00);
}

void MAX30102_Reset(void) {
    MAX30102_WriteReg(REG_MODE_CONFIG, 0x40); // Reset bit
    while ((MAX30102_ReadReg(REG_MODE_CONFIG) & 0x40) != 0); // Wait for reset to clear
}

void MAX30102_ClearFIFO(void) {
    MAX30102_WriteReg(REG_FIFO_WR_PTR, 0x00);
    MAX30102_WriteReg(REG_OVF_COUNTER, 0x00);
    MAX30102_WriteReg(REG_FIFO_RD_PTR, 0x00);
}

uint32_t MAX30102_ReadIR(void) {
    uint8_t data[6];
    for (int i = 0; i < 6; i++) {
        data[i] = I2C0_Recv1(MAX30102_ADDRESS, REG_FIFO_DATA);
    }

    uint32_t red = ((uint32_t)(data[0] & 0x03) << 16) | ((uint32_t)data[1] << 8) | data[2];
    uint32_t ir  = ((uint32_t)(data[3] & 0x03) << 16) | ((uint32_t)data[4] << 8) | data[5];

    return ir;
}
