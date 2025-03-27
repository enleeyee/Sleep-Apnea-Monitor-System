#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

int main (void) {
	SYSCTL_RCGCGPIO_R |= 0x20;
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_DEN_R = 0x0E;
	
	while(1) GPIO_PORTF_DATA_R = 0x02; // turn on red LED
}
