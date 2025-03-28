#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"


/*
OLED Display STEMMA QT Connections
PB2 - I2C0SCL
PB3 - I2C0SDA

PD1 - I2C3SDA
PD0 - I2C3SCL

PE5 - I2C2SDA
PE4 - I2C2SCL
*/

void delay(long d) {
	while(d--);
}

int main (void) {
	SYSCTL_RCGCGPIO_R |= 0x20;
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_DEN_R = 0x0E;
	
	while(1) {
		GPIO_PORTF_DATA_R = 0x02; // turn on red LED
		delay(1000);
		GPIO_PORTF_DATA_R = 0x04; // blue LED
		delay(1000);
		GPIO_PORTF_DATA_R = 0x08; // green LED
	}
}
