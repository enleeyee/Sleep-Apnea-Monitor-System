#include <stdint.h>
#include "../inc/PLL.h"
#include "../inc/SysTick.h"
#include "../inc/UART.h"

// Hardware CP2102 to TM4C123: 
// TXD connected to PB0
// RXD connected to PB1

//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int main(void){
  PLL_Init(Bus80MHz);       // 80  MHz
	SysTick_Init();
  UART_Init();

  while(1){
    UART_OutString("Main project is running.");
		OutCRLF();
		SysTick_Wait10ms(200);
  }
}
