// SSI2.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/UART.h"

void SSI2_Init_Slave(void) {
  SYSCTL_RCGCSSI_R |= 0x04;      // Enable clock to SSI2
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}

	SSI2_CR1_R = 0x00000000;       // Disable SSI
	SSI2_CR0_R = 0x07;             // 8-bit data, Freescale SPI frame format
	SSI2_CR1_R = 0x04;             // Set as slave
	SSI2_CPSR_R = 0x02;            // Not used in slave but required

	// Flush FIFOs BEFORE enabling
	while (SSI2_SR_R & 0x04) {
		volatile uint8_t dummy = SSI2_DR_R;
	}
}

uint8_t SSI2_Recv(void) {
  while ((SSI2_SR_R & SSI_SR_RNE) == 0);  // Wait until RX FIFO is not empty

  uint8_t val = SSI2_DR_R & 0xFF;
  UART_OutString("Received byte: "); UART_OutUDec(val); OutCRLF();
  return val;
}
