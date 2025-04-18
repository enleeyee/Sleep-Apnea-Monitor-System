#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "i2c.h"
#include "ssd1306.h"
#include "font.h"
#include <stdbool.h>

#define SSD1306_I2C_ADDRESS 0x3D

void OLED_command(uint8_t cmd);

void OLED_displayOn(void) {
    OLED_command(0xAF); // SSD1306 Display ON command
}


void I2C0_Write(uint8_t slaveAddr, uint8_t reg, uint8_t data) {
    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, false);
    I2CMasterDataPut(I2C0_BASE, reg);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(I2C0_BASE));

    I2CMasterDataPut(I2C0_BASE, data);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while (I2CMasterBusy(I2C0_BASE));
}

void OLED_command(uint8_t cmd) {
    I2C0_Write(SSD1306_I2C_ADDRESS, 0x00, cmd);
}

void OLED_data(uint8_t data) {
    I2C0_Write(SSD1306_I2C_ADDRESS, 0x40, data);
}

void OLED_Init(void) {
  OLED_command(0xAE); // Display off
  OLED_command(0x20); // Set Memory Addressing Mode
  OLED_command(0x00); // Horizontal Addressing Mode
  OLED_command(0xB0); // Page Start Address for Page Addressing Mode
  OLED_command(0xC8); // COM Output Scan Direction
  OLED_command(0x00); // Low column address
  OLED_command(0x10); // High column address
  OLED_command(0x40); // Start line address
  OLED_command(0x81); // Contrast control
  OLED_command(0x7F);
  OLED_command(0xA1); // Segment remap
  OLED_command(0xA6); // Normal display
  OLED_command(0xA8); // Multiplex ratio
  OLED_command(0x3F);
  OLED_command(0xA4); // Display follows RAM
  OLED_command(0xD3); // Display offset
  OLED_command(0x00);
  OLED_command(0xD5); // Display clock divide
  OLED_command(0x80);
  OLED_command(0xD9); // Pre-charge period
  OLED_command(0xF1);
  OLED_command(0xDA); // COM pins
  OLED_command(0x12);
  OLED_command(0xDB); // VCOMH deselect level
  OLED_command(0x40);
  OLED_command(0x8D); // Enable charge pump
  OLED_command(0x14);
  OLED_command(0xAF); // Turn on OLED
}



void OLED_clearDisplay(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_command(0xB0 + page); // set page
        OLED_command(0x00); // low column
        OLED_command(0x10); // high column
        for (uint8_t col = 0; col < 128; col++) {
            OLED_data(0x00);
        }
    }
}

void OLED_setXY(uint8_t row, uint8_t col) {
    OLED_command(0xB0 + row); // page
    OLED_command(0x00 + (8 * col & 0x0F)); // lower col
    OLED_command(0x10 + ((8 * col >> 4) & 0x0F)); // upper col
}

void OLED_sendCharXY(unsigned char c, int x, int y) {
    OLED_setXY(x, y);
    OLED_data(0x00);  // leading space

    if (c < 0x20 || c > 0x7F) {
        c = '?';  // fallback to a safe printable character
    }

    for (int i = 0; i < 5; i++) {
        OLED_data(ASCII_font5x8[c - 0x20][i]);
    }

    OLED_data(0x00);  // trailing space
}


void OLED_sendStrXY(char *str, int x, int y) {
    while (*str) {
        OLED_sendCharXY(*str++, x, y++);
    }
}
