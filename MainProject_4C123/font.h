// font.h
// Header file for 5x8 and large font definitions for OLED display
#ifndef FONT_H
#define FONT_H

//------------ASCII_font5x8------------
// 5x8 font array for 96 printable ASCII characters (from 0x20 to 0x7F)
// Each character is represented by 5 bytes (columns) for vertical display
// Format: ASCII_font5x8[character - 0x20][5]
extern const unsigned char ASCII_font5x8[96][5];

//------------bigNumbers------------
// 3x5 font array for rendering large digits (0–9)
// Each digit is represented by a 5-row by 3-column block of bytes
// Format: bigNumbers[digit][row][column]
extern const unsigned char bigNumbers[10][5][3];

#endif
