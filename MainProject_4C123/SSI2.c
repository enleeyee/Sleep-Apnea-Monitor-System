// SSI2.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/UART.h"

extern void Debug_SSI2_Status(void);

void SSI2_Init_Slave(void) {
  SYSCTL_RCGCSSI_R |= 0x04;     // Enable SSI2 clock
  while((SYSCTL_PRSSI_R & 0x04) == 0); // Wait for ready
  
  SSI2_CR1_R = 0x00000000;      // Disable SSI during config
  SSI2_CR0_R = 0x00000007;      // SPO=0, SPH=1 (Mode 1), SCR=1 (slower clock)
  SSI2_CPSR_R = 32; // 80MHz/32 = 2.5MHz clock
  
  // Clear FIFOs
  while(SSI2_SR_R & SSI_SR_RNE) {
    volatile uint8_t dummy = SSI2_DR_R;
  }
  
  SSI2_CR1_R = 0x00000006;      // Slave mode, SSE=1
	
	// Debug output
  UART_OutString("SSI2 Config:\r\n");
  UART_OutString("CR0: 0x"); UART_OutUHex(SSI2_CR0_R); OutCRLF();
  UART_OutString("CR1: 0x"); UART_OutUHex(SSI2_CR1_R); OutCRLF();
  UART_OutString("SR: 0x"); UART_OutUHex(SSI2_SR_R); OutCRLF();
}

uint8_t SSI2_Recv(void) {
    while ((SSI2_SR_R & SSI_SR_RNE) == 0);
    return SSI2_DR_R & 0xFF;
}

void Debug_SSI2_Status(void) {
	UART_OutString("SSI2 Config:\r\n");
	UART_OutString("CR0: 0x"); UART_OutUHex(SSI2_CR0_R); 
	UART_OutString(" (SPO="); UART_OutUDec((SSI2_CR0_R >> 6) & 1);
	UART_OutString(", SPH="); UART_OutUDec((SSI2_CR0_R >> 7) & 1);
  OutCRLF();
}
