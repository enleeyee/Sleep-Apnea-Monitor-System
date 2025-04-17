// MAX30102.h
// Driver for MAX30102 Pulse Oximeter & Heart Rate Sensor over I2C
// Works with TM4C123GXL using I2C0 (PB2 = SCL, PB3 = SDA)
// Created in the style of UART.h driver module
// Compatible with Valvano’s I2C0.c structure

#ifndef __MAX30102_H__
#define __MAX30102_H__

#include <stdint.h>

// I2C address for MAX30102 sensor
#define MAX30102_ADDR 0x57

//------------MAX30102_Reset------------
// Sends software reset to MAX30102 via MODE_CONFIG register
// Input: none
// Output: none
void MAX30102_Reset(void);

//------------MAX30102_Init------------
// Initialize MAX30102 for SpO2 mode (IR/Red LEDs), 100Hz sampling
// Input: none
// Output: none
void MAX30102_Init(void);

//------------MAX30102_ReadIR------------
// Reads the 16-bit IR LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (IR pulse data)
uint16_t MAX30102_ReadIR(void);

#endif
