#include <stdint.h>
#include "MAX30102.h"
#include "../inc/I2C0.h"
#include "../inc/PLL.h"
#include "../inc/SysTick.h"
#include "../inc/UART.h"

// Hardware CP2102 to TM4C123: 
// TXD connected to PB0
// RXD connected to PB1
// SCL connected to PB2
// SDA connected to PB3

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int main(void){
	I2C_Init();						// I2C0 on PB2/PB3
  PLL_Init(Bus80MHz);   // 80 MHz
  SysTick_Init();       // delay utility
  UART_Init();          // UART on PB0/PB1

  UART_OutString("Initializing MAX30100...\r\n");
  MAX30102_Init();
  UART_OutString("Initialization done.\r\n");

  while(1){
    // Read IR data from FIFO
    uint16_t irValue = MAX30102_ReadIR();

    UART_OutString("IR = ");
    UART_OutUDec(irValue);
    OutCRLF();

    SysTick_Wait10ms(10); // 1-second readable delay
  }
}
