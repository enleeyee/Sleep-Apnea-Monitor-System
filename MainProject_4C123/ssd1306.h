// ssd1306.h
// Header file for OLED SSD1306 driver using I2C1
#ifndef SSD1306_H_
#define SSD1306_H_

#include <stdint.h>
#include <stdbool.h>
#define SSD1306_LCDWIDTH      128
#define SSD1306_LCDHEIGHT      64
#define SSD1306_SETCONTRAST   0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2


typedef uint8_t bitmap_t[8][128];

//------------I2C1_Init------------
// Initializes I2C1 (PA6 = SCL, PA7 = SDA) for communication with OLED
// Input: none
// Output: none
void I2C1_Init(void);

//------------I2C1_WriteByte------------
// Sends a control byte and data byte to a specified I2C address
// Input: addr - 7-bit I2C slave address
//        control - 0x00 for command, 0x40 for data
//        data - byte to send
// Output: none
void I2C1_WriteByte(uint8_t addr, uint8_t control, uint8_t data);

//------------OLED_command------------
// Sends a command byte to the OLED display
// Input: cmd - command byte
// Output: none
void OLED_command(uint8_t cmd);

//------------OLED_data------------
// Sends a data byte to the OLED display
// Input: data - data byte
// Output: none
void OLED_data(uint8_t data);

//------------OLED_TurnOn------------
// Turns the OLED display on
// Input: none
// Output: none
void OLED_TurnOn(void);

//------------OLED_displayOn------------
// Alias to OLED_TurnOn; turns display on
// Input: none
// Output: none
void OLED_displayOn(void);

//------------OLED_Init------------
// Initializes the OLED display with SSD1306 settings
// Input: none
// Output: none
void OLED_Init(void);

//------------OLED_clearDisplay------------
// Clears the OLED screen (fills with 0s)
// Input: none
// Output: none
void OLED_clearDisplay(void);

//------------OLED_setXY------------
// Sets the display cursor to a specific row and column
// Input: row - page number (0 to 7)
//        col - character column (0 to 15)
// Output: none
void OLED_setXY(uint8_t row, uint8_t col);

//------------OLED_sendCharXY------------
// Displays a single character at specified coordinates
// Input: c - ASCII character
//        x - page row (0–7)
//        y - column index (0–15)
// Output: none
void OLED_sendCharXY(unsigned char c, int x, int y);

//------------OLED_sendStrXY------------
// Displays a string starting at specified coordinates
// Input: str - pointer to null-terminated string
//        x - page row (0–7)
//        y - starting column (0–15)
// Output: none
void OLED_sendStrXY(char *str, int x, int y);

#endif // __SSD1306_H__
