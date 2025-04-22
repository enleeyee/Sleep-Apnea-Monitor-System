// hc05.c
#include <stdint.h>
#include "hc05.h"
#include "../inc/tm4c123gh6pm.h"

// Initialize UART1 for HC-05 Bluetooth (PB0 = RX, PB1 = TX)
void HC05_Init(void) {
  SYSCTL_RCGCUART_R |= 0x02;              // Enable UART1 clock
  while ((SYSCTL_PRUART_R & 0x02) == 0);  // Wait for UART1 to be ready

  UART1_CTL_R &= ~UART_CTL_UARTEN;        // Disable UART1
  UART1_IBRD_R = 520;                     // 80,000,000 / (16 * 9600) = 520.833
  UART1_FBRD_R = 53;                      // .833 * 64 ˜ 53
  UART1_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN); // 8N1 with FIFO
  UART1_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE); // Enable UART1 TX and RX
}

// Blocking read from UART1
char HC05_Read(void) {
  while (UART1_FR_R & UART_FR_RXFE);     // Wait until RXFE is 0
  return UART1_DR_R & 0xFF;
}

// Blocking write to UART1
void HC05_Write(char data) {
  while (UART1_FR_R & UART_FR_TXFF);     // Wait until TXFF is 0
  UART1_DR_R = data;
}

// Write a null-terminated string over UART1
void HC05_WriteString(char *str) {
  while (*str) {
    HC05_Write(*str++);
  }
}

