// GPIO_PortB_Init.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//------------GPIO_PortB_Init------------
// Configures:
// PB0 = UART1 RX (HC-05 RX)
// PB1 = UART1 TX (HC-05 TX)
// PB4 = SSI2CLK (Arduino SCK D13)
// PB5 = SSI2Fss (Arduino SS D10)
// PB6 = SSI2Rx (Arduino MOSI D11)
// PB7 = SSI2Tx (Arduino MISO D12)
void GPIO_PortB_Init(void) {
  SYSCTL_RCGCGPIO_R |= 0x02;    // Enable Port B clock
  while((SYSCTL_PRGPIO_R & 0x02) == 0);
  
  // Unlock PB7 if needed (some Tiva parts have locked JTAG pins)
  GPIO_PORTB_LOCK_R = 0x4C4F434B;
  GPIO_PORTB_CR_R |= 0xF0;
  GPIO_PORTB_LOCK_R = 0;
  
  GPIO_PORTB_AFSEL_R |= 0xF0;   // Enable alt func on PB4-7
  GPIO_PORTB_PCTL_R &= ~0xFFFF0000;
  GPIO_PORTB_PCTL_R |= 0x22220000; // Configure as SSI2
  GPIO_PORTB_DEN_R |= 0xF0;     // Digital enable
  
  // SS (PB5) configuration
  GPIO_PORTB_DIR_R &= ~0x20;    // Input
  GPIO_PORTB_PUR_R |= 0x20;     // Enable pull-up

  // Configure SS pin interrupt
  GPIO_PORTB_DIR_R &= ~0x20;            // PB5 as input
  GPIO_PORTB_IS_R &= ~0x20;             // Edge-sensitive
  GPIO_PORTB_IBE_R &= ~0x20;            // Not both edges
  GPIO_PORTB_IEV_R |= 0x20;             // Rising edge triggers (when SS goes HIGH)
  GPIO_PORTB_ICR_R = 0x20;              // Clear flag
  GPIO_PORTB_IM_R |= 0x20;              // Arm interrupt

  NVIC_PRI0_R = (NVIC_PRI0_R & 0xFFFFFF00) | 0x40; // Priority 2
  NVIC_EN0_R |= 1 << 1;                 // Enable IRQ1 (Port B)
}
