#include <stdint.h>
#include <stdbool.h>
#include "../inc/I2C0.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/UART.h"
#include "MAX30102.h"

// SysTick millis implementation
volatile uint32_t millis_count = 0;

void SysTick_Handler(void) {
    millis_count++;
}

void SysTick_Init(void) {
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 80000 - 1; // 1ms tick at 80MHz
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = 0x07; // Enable SysTick, interrupt, and system clock
}

uint32_t millis(void) {
    return millis_count;
}


int main(void) {
    PLL_Init(Bus80MHz);
    SysTick_Init();
    I2C0_Init();
    UART_Init();

    // Debug: Init PF2 (Blue LED)
    SYSCTL_RCGCGPIO_R |= 0x20;
    while ((SYSCTL_PRGPIO_R & 0x20) == 0);
    GPIO_PORTF_DIR_R |= 0x04;
    GPIO_PORTF_DEN_R |= 0x04;
    GPIO_PORTF_DATA_R &= ~0x04;

    // Show that init succeeded
    GPIO_PORTF_DATA_R |= 0x04;  // Turn on blue
    UART_OutString("Init done.\n");
    for (volatile int d = 0; d < 1000000; d++); // delay

    // Blink forever
    while (1) {
        GPIO_PORTF_DATA_R ^= 0x04;
        for (volatile int i = 0; i < 800000; i++);
    }
}
