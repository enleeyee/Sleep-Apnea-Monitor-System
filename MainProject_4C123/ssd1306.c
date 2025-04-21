#include <stdint.h>
#include <stdbool.h>
#include "font.h"
#include "ssd1306.h"
#include "../inc/tm4c123gh6pm.h"

#define OLED_I2C_ADDRESS 0x3D

void I2C1_Init(void) {
  SYSCTL_RCGCI2C_R |= 0x02;          // Enable I2C1 clock
  SYSCTL_RCGCGPIO_R |= 0x01;         // Enable GPIOA clock
  while ((SYSCTL_PRGPIO_R & 0x01) == 0) {}; // Wait for GPIOA

  GPIO_PORTA_AFSEL_R |= 0xC0;        // PA6,PA7 I2C
  GPIO_PORTA_ODR_R |= 0x80;          // PA7 open-drain
  GPIO_PORTA_DEN_R |= 0xC0;          // Digital enable PA6,PA7
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & ~0xFF000000) | 0x33000000; // I2C1 SCL/SDA

  I2C1_MCR_R = 0x10;                 // Master mode
  I2C1_MTPR_R = 39;                  // 100 kHz with 80 MHz clock
}

void I2C1_WriteByte(uint8_t addr, uint8_t control, uint8_t data) {
  while (I2C1_MCS_R & 0x01);                         // Wait if busy
  I2C1_MSA_R = (addr << 1) & ~0x01;                  // Address + write
  I2C1_MDR_R = control;                              // Send control byte
  I2C1_MCS_R = 0x03;                                 // START + RUN
  while (I2C1_MCS_R & 0x01);                         // Wait
  if (I2C1_MCS_R & 0x02) return;                     // Error?

  I2C1_MDR_R = data;                                 // Send data byte
  I2C1_MCS_R = 0x05;                                 // STOP + RUN
  while (I2C1_MCS_R & 0x01);                         // Wait
}

void OLED_command(uint8_t cmd) {
  I2C1_WriteByte(OLED_I2C_ADDRESS, 0x00, cmd);
}

void OLED_data(uint8_t data) {
  I2C1_WriteByte(OLED_I2C_ADDRESS, 0x40, data);
}

void OLED_TurnOn(void) {
  OLED_command(0xAF); // Display ON
}

void OLED_displayOn(void) {
  OLED_command(0xAF);
}

void OLED_Init(void) {
  OLED_command(0xAE); OLED_command(0x20); OLED_command(0x00);
  OLED_command(0xB0); OLED_command(0xC8); OLED_command(0x00);
  OLED_command(0x10); OLED_command(0x40); OLED_command(0x81);
  OLED_command(0x7F); OLED_command(0xA1); OLED_command(0xA6);
  OLED_command(0xA8); OLED_command(0x3F); OLED_command(0xA4);
  OLED_command(0xD3); OLED_command(0x00); OLED_command(0xD5);
  OLED_command(0x80); OLED_command(0xD9); OLED_command(0xF1);
  OLED_command(0xDA); OLED_command(0x12); OLED_command(0xDB);
  OLED_command(0x40); OLED_command(0x8D); OLED_command(0x14);
  OLED_command(0xAF); // Turn on OLED
}

void OLED_clearDisplay(void) {
  for (uint8_t page = 0; page < 8; page++) {
    OLED_command(0xB0 + page); OLED_command(0x00); OLED_command(0x10);
    for (uint8_t col = 0; col < 128; col++) {
      OLED_data(0x00);
    }
  }
}

void OLED_setXY(uint8_t row, uint8_t col) {
  OLED_command(0xB0 + row);
  OLED_command(0x00 + (8 * col & 0x0F));
  OLED_command(0x10 + ((8 * col >> 4) & 0x0F));
}

void OLED_sendCharXY(unsigned char c, int x, int y) {
  OLED_setXY(x, y);
  OLED_data(0x00);
  if (c < 0x20 || c > 0x7F) c = '?';
  for (int i = 0; i < 5; i++) {
    OLED_data(ASCII_font5x8[c - 0x20][i]);
  }
  OLED_data(0x00);
}

void OLED_sendStrXY(char *str, int x, int y) {
  while (*str) {
    OLED_sendCharXY(*str++, x, y++);
  }
}
