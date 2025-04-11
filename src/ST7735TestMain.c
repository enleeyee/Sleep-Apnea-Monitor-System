#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// MAX30100 I2C Address
#define MAX30100_ADDRESS 0x57

// MAX30100 Registers (for basic init and ID check)
#define MAX30100_PART_ID_REG 0xFF

void delay(long d) {
		while(d--);
}

void I2C3_Init(void) {
    // Enable I2C3 and Port D
    SYSCTL_RCGCI2C_R |= 0x08;      // Enable I2C3
    SYSCTL_RCGCGPIO_R |= 0x08;     // Enable Port D
    while((SYSCTL_PRGPIO_R & 0x08) == 0) {}; // Wait for Port D to be ready

    // Configure PD0 and PD1 for I2C3
    GPIO_PORTD_AFSEL_R |= 0x03;    // Enable alternate function on PD0 and PD1
    GPIO_PORTD_ODR_R |= 0x02;      // Enable open-drain on PD1 (SDA)
    GPIO_PORTD_DEN_R |= 0x03;      // Enable digital I/O on PD0 and PD1
    GPIO_PORTD_PCTL_R &= ~0x000000FF;
    GPIO_PORTD_PCTL_R |= 0x00000033; // Configure PD0 and PD1 as I2C

    // Configure I2C3 Master
    I2C3_MCR_R = 0x10;             // Enable I2C Master function
    I2C3_MTPR_R = 0x07;            // Set SCL clock speed (assuming 50 MHz system clock)
}

void I2C3_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t data) {
    I2C3_MSA_R = (slave_addr << 1);       // Set slave address and write mode
    I2C3_MDR_R = reg_addr;                // Set register address
    I2C3_MCS_R = 0x03;                    // Start and run
    while(I2C3_MCS_R & 0x01);             // Wait until transmission completes
    I2C3_MDR_R = data;                    // Set data to write
    I2C3_MCS_R = 0x05;                    // Start and run
    while(I2C3_MCS_R & 0x01);             // Wait until transmission completes
}

uint8_t I2C3_Read(uint8_t slave_addr, uint8_t reg_addr) {
    uint8_t data;
    I2C3_MSA_R = (slave_addr << 1);       // Set slave address and write mode
    I2C3_MDR_R = reg_addr;                // Set register address
    I2C3_MCS_R = 0x03;                    // Start and run
    while(I2C3_MCS_R & 0x01);             // Wait until transmission completes

    I2C3_MSA_R = (slave_addr << 1) + 1;   // Set slave address and read mode
    I2C3_MCS_R = 0x07;                    // Start, run, and stop
    while(I2C3_MCS_R & 0x01);             // Wait until transmission completes
    data = I2C3_MDR_R;                    // Read data
    return data;
}

int main(void) {
    // Step 1: Initialize I2C3 on Port D
    I2C3_Init();

    // Optional: Delay after power-on for stability
    delay(1000000);

    // Step 2: Read from the PART ID register to test communication
    uint8_t part_id = I2C3_Read(MAX30100_ADDRESS, MAX30100_PART_ID_REG);

    // Step 3: Simple debug loop (e.g., toggle LED based on ID match)
    SYSCTL_RCGCGPIO_R |= 0x20; // Enable Port F for LED
    GPIO_PORTF_DIR_R |= 0x0E;  // PF1, PF2, PF3 = output
    GPIO_PORTF_DEN_R |= 0x0E;

    while (1) {
        if (part_id == 0x11) { // Expected part ID for MAX30100
            GPIO_PORTF_DATA_R = 0x02; // Red LED ON = success
        } else {
            GPIO_PORTF_DATA_R = 0x04; // Blue LED ON = error
        }
        delay(1000000);
        GPIO_PORTF_DATA_R = 0x00;
        delay(1000000);
    }
}
