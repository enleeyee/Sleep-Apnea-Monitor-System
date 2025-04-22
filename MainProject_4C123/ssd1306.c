// ssd1306.c
// OLED driver using I2C1 on TM4C123GH6PM
#include <stdint.h>
#include <stdbool.h>
#include "font.h"
#include "ssd1306.h"
#include "../inc/tm4c123gh6pm.h"

#define OLED_I2C_ADDRESS 0x3D

//------------I2C1_Init------------
// Initializes I2C1 on PA6 (SCL) and PA7 (SDA) for OLED communication
// Input: none
// Output: none
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

//------------I2C1_WriteByte------------
// Sends a single control/data byte pair to the OLED over I2C1
// Input: addr - 7-bit I2C address
//        control - 0x00 (command) or 0x40 (data)
//        data - byte to send
// Output: none
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

//------------OLED_command------------
// Sends a command byte to the OLED
// Input: cmd - command byte
// Output: none
void OLED_command(uint8_t cmd) {
  I2C1_WriteByte(OLED_I2C_ADDRESS, 0x00, cmd);
}

//------------OLED_data------------
// Sends a data byte to the OLED
// Input: data - data byte
// Output: none
void OLED_data(uint8_t data) {
  I2C1_WriteByte(OLED_I2C_ADDRESS, 0x40, data);
}

//------------OLED_TurnOn------------
// Turns on the OLED display (alias of displayOn)
// Input: none
// Output: none
void OLED_TurnOn(void) {
  OLED_command(0xAF); // Display ON
}

//------------OLED_displayOn------------
// Turns on the OLED display
// Input: none
// Output: none
void OLED_displayOn(void) {
  OLED_command(0xAF);
}

//------------OLED_Init------------
// Initializes the OLED with a standard sequence of commands
// Input: none
// Output: none
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

//------------OLED_clearDisplay------------
// Clears all pixels on the OLED display
// Input: none
// Output: none
void OLED_clearDisplay(void) {
  for (uint8_t page = 0; page < 8; page++) {
    OLED_command(0xB0 + page); OLED_command(0x00); OLED_command(0x10);
    for (uint8_t col = 0; col < 128; col++) {
      OLED_data(0x00);
    }
  }
}

//------------OLED_setXY------------
// Sets the cursor to a specific row and column
// Input: row - page number (0–7)
//        col - character column (0–127 / 8)
// Output: none
void OLED_setXY(uint8_t row, uint8_t col) {
  OLED_command(0xB0 + row);
  OLED_command(0x00 + (8 * col & 0x0F));
  OLED_command(0x10 + ((8 * col >> 4) & 0x0F));
}

//------------OLED_sendCharXY------------
// Writes a single character at specified (x, y) position
// Input: c - ASCII character to print
//        x - row (page)
//        y - column (character column)
// Output: none
void OLED_sendCharXY(unsigned char c, int x, int y) {
  OLED_setXY(x, y);
  OLED_data(0x00);
  if (c < 0x20 || c > 0x7F) c = '?';
  for (int i = 0; i < 5; i++) {
    OLED_data(ASCII_font5x8[c - 0x20][i]);
  }
  OLED_data(0x00);
}

//------------OLED_sendStrXY------------
// Displays a string starting at specified (x, y) location
// Input: str - null-terminated string
//        x - row (page)
//        y - starting column
// Output: none
void OLED_sendStrXY(char *str, int x, int y) {
  while (*str) {
    OLED_sendCharXY(*str++, x, y++);
  }
}
