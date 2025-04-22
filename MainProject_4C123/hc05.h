// hc05.h
// Header for HC-05 Bluetooth module using UART1 on TM4C123GH6PM
#ifndef HC05_H
#define HC05_H

//------------HC05_Init------------
// Initializes UART1 for HC-05 Bluetooth communication
// PB0 = UART1 RX, PB1 = UART1 TX, baud rate = 9600
// Input: none
// Output: none
void HC05_Init(void);

//------------HC05_Read------------
// Reads a single character from UART1 (blocking)
// Input: none
// Output: character received from HC-05
char HC05_Read(void);

//------------HC05_Write------------
// Sends a single character over UART1 (blocking)
// Input: data - character to send
// Output: none
void HC05_Write(char data);

//------------HC05_WriteString------------
// Sends a null-terminated string over UART1
// Input: str - pointer to string to send
// Output: none
void HC05_WriteString(char *str);

#endif
