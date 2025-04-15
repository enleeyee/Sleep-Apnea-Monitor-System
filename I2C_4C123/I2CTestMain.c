// I2CTestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided in I2C0.c by periodically sampling
// the temperature, parsing the result, and sending it to the UART.
// Daniel Valvano
// May 3, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Section 8.6.4 Programs 8.5, 8.6 and 8.7

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
// I2C0SCL connected to PB2 and to pin 4 of HMC6352 compass or pin 3 of TMP102 thermometer
// I2C0SDA connected to PB3 and to pin 3 of HMC6352 compass or pin 2 of TMP102 thermometer
// SCL and SDA lines pulled to +3.3 V with 10 k resistors (part of breakout module)
// ADD0 pin of TMP102 thermometer connected to GND
#ifndef __I2C_H__
#define __I2C_H__
#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "hw_i2c.h"
#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "i2c.h"
#include "pin_map.h"
#include "ssd1306.h"
#include "font.h"



#define OLED_I2C_ADDRESS 0x3D // SSD1306 default I2C address

	void I2C0_Init(void) {
    // Enable I2C0 and GPIOB
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
		
		GPIOPinConfigure(GPIO_PB2_I2C0SCL);
		GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Initialize and configure the master module
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false); // false = 100kbps
		
}
void I2C0_WriteByte(uint8_t deviceAddr, uint8_t controlByte, uint8_t dataByte) {
    // Send device address + write flag
    I2CMasterSlaveAddrSet(I2C0_BASE, deviceAddr, false);

    // Send control byte (0x00 = command, 0x40 = data)
    I2CMasterDataPut(I2C0_BASE, controlByte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(I2C0_BASE));

    // Send data byte
    I2CMasterDataPut(I2C0_BASE, dataByte);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while (I2CMasterBusy(I2C0_BASE));
}
void OLED_TurnOn(void) {
    I2C0_WriteByte(OLED_I2C_ADDRESS, 0x00, 0xAF); // 0xAF = SSD1306 Display ON
}
int main(void) {
    // Set system clock to 50MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  
		I2C0_Init();            // Initialize I2C0
    OLED_TurnOn();          // Send Display ON command

    OLED_Init();            // Initialize OLED
    OLED_clearDisplay();    // Optional: Clear screen
	OLED_sendStrXY("HELLO carshawn! ", 0, 0);  // Send string at (row=0, col=0)
		SysCtlDelay(5000000);
  while (1) {
    // Could blink an LED here or animate the screen
  }
}
#endif  // __I2C_H__

