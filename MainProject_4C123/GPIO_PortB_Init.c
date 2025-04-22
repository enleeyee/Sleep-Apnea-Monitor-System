// GPIO_PortB_Init.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//------------GPIO_PortB_Init------------
// Initializes Port B pins PB0–PB3:
// PB0, PB1 for UART1 (RX/TX)
// PB2, PB3 for I2C0 (SCL/SDA)
// Input: none
// Output: none
void GPIO_PortB_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x02;        // Enable clock for Port B
  while((SYSCTL_PRGPIO_R & 0x02) == 0); // Wait until ready

  // Enable alternate functions for PB0-PB3
  GPIO_PORTB_AFSEL_R |= 0x0F;       // PB0-3 alt function

  // Configure pin muxing:
  // PB1 = UART1 TX = 0x1, PB0 = UART1 RX = 0x1
  // PB3 = I2C0 SDA = 0x3, PB2 = I2C0 SCL = 0x3
  GPIO_PORTB_PCTL_R &= ~0x0000FFFF;
  GPIO_PORTB_PCTL_R |=  0x00003311;

  // Enable digital on PB0-3
  GPIO_PORTB_DEN_R |= 0x0F;

  // PB3 (SDA) open-drain
  GPIO_PORTB_ODR_R |= 0x08;

  // Disable analog function
  GPIO_PORTB_AMSEL_R &= ~0x0F;
}
