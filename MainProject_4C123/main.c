#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "GPIO_PortB_Init.h"
#include "MAX30102.h"
#include "ssd1306.h"
#include "../inc/I2C0.h"
#include "../inc/PLL.h"
#include "../inc/SysTick.h"
#include "../inc/UART.h"

// Hardware CP2102 to TM4C123: 
// USB TXD connected to PB0
// USB RXD connected to PB1
// Oximeter SCL connected to PB2
// Oximeter SDA connected to PB3

// OLED SCL (Yellow) to PA6
// OLED SDA (Blue) to PA7

int main(void){
  PLL_Init(Bus80MHz);
  SysTick_Init();
	GPIO_PortB_Init();
  I2C_Init();
  UART_Init();
	
	SysTick80_Wait10ms(10); // Delay before initializing I2C1 + OLED
	
	I2C1_Init();            // Initialize I2C1
	OLED_Init();
	OLED_TurnOn();          // Send Display ON command
	
	UART_OutString("OLED init sequence done\r\n");
	for (uint8_t i = 0; i < 8; i++) {
		OLED_command(0xB0 + i);  // Set page
		OLED_command(0x00);      // Set column lower nibble
		OLED_command(0x10);      // Set column upper nibble
		for (uint8_t j = 0; j < 128; j++) {
			OLED_data(0xFF);       // Fill with white pixels
		}
	}
	OLED_clearDisplay();
	SysTick80_Wait10ms(100); // 100 x 10ms = 1 second
	
	for (uint8_t addr = 0; addr < 128; addr++) {
		I2C_Send1(addr, 0xFF);   // Try reading Part ID
		uint8_t part = I2C_Recv(addr);
		if (part != 0xFF) {
			UART_OutString("Found I2C device at 0x");
			UART_OutUHex2(addr);
			UART_OutString(" (Part ID: 0x");
			UART_OutUHex2(part);
			UART_OutString(")\r\n");
		}
	}

  UART_OutString("Initializing MAX30102...\r\n");
  MAX30102_Init();
  UART_OutString("Initialization done.\r\n");

  while(1){
		// Previous value tracking
		static uint16_t prev_ir = 0, prev_red = 0;
		static uint8_t prev_bpm = 0;
		static uint8_t prev_spo2 = 0;

		uint16_t ir = MAX30102_ReadIR();
		uint16_t red = MAX30102_ReadRed();
		bool beat = MAX30102_CheckForBeat(ir);

		float bpm = MAX30102_GetBPM();
		float spo2 = MAX30102_CalculateSpO2(red, ir);
		
		// Update UART (debug only)
		UART_OutString("IR = ");
		UART_OutUDec(ir);

		if (beat) {
				UART_OutString(", BPM = ");
				UART_OutUDec((uint16_t)bpm);
				UART_OutString(", AVG BPM = ");
				UART_OutUDec((uint16_t)MAX30102_GetAvgBPM());
				UART_OutString(", O2 = ");
				UART_OutUDec((uint16_t)spo2);
				UART_OutString("  <-- Beat detected!");
		}
		OutCRLF();

		// Check if values changed significantly
		if ((ir != prev_ir) || (red != prev_red) || ((uint8_t)bpm != prev_bpm) || ((uint8_t)spo2 != prev_spo2)) {
			char line1[16], line2[16];
			sprintf(line1, "BPM: %3u", (uint8_t)bpm);
			sprintf(line2, "O2 : %3u%%", (uint8_t)spo2);

			// Clear only the lines you're updating
			OLED_setXY(1, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
			OLED_sendStrXY(line1, 1, 0);

			OLED_setXY(2, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
			OLED_sendStrXY(line2, 2, 0);

			// Store previous values
			prev_ir = ir;
			prev_red = red;
			prev_bpm = (uint8_t)bpm;
			prev_spo2 = (uint8_t)spo2;
		}
	}
}
