// MAX30102.h
#ifndef __MAX30102_H__
#define __MAX30102_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX30102_ADDR 0x57

//------------MAX30102_Reset------------
// Sends software reset to MAX30102 via MODE_CONFIG register
// Input: none
// Output: none
void MAX30102_Init(void);

//------------MAX30102_Init------------
// Initialize MAX30102 for SpO2 mode (IR/Red LEDs), 100Hz sampling
// Input: none
// Output: none
void MAX30102_Reset(void);

//------------MAX30102_ReadIR------------
// Reads the 16-bit IR LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (IR pulse data)
uint16_t MAX30102_ReadIR(void);

//------------MAX30102_ReadRed------------
// Reads the 16-bit Red LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (Red pulse data)
uint16_t MAX30102_ReadRed(void);

//------------MAX30102_CheckForBeat------------
// Detects heartbeat by analyzing IR signal waveform transitions
// Input: 16-bit IR LED value
// Output: true if beat detected, false otherwise
bool MAX30102_CheckForBeat(uint16_t irValue);

//------------MAX30102_CalculateSpO2------------
// Calculates estimated SpO2 from IR and Red values
// Input: red and ir sensor values
// Output: estimated SpO2 percentage (0.0 - 100.0)
float MAX30102_CalculateSpO2(uint16_t red, uint16_t ir);

//------------MAX30102_GetBPM------------
// Returns the most recent BPM reading
// Input: none
// Output: BPM value as float
float MAX30102_GetBPM(void);

//------------MAX30102_GetAvgBPM------------
// Returns the average BPM over last few beats
// Input: none
// Output: averaged BPM value
float MAX30102_GetAvgBPM(void);

#endif // __MAX30102_H__
