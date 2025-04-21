#ifndef __MAX30102_H__
#define __MAX30102_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX30102_ADDR 0x57

void MAX30102_Init(void);
void MAX30102_Reset(void);
uint16_t MAX30102_ReadIR(void);
uint16_t MAX30102_ReadRed(void);
bool MAX30102_CheckForBeat(uint16_t irValue);
float MAX30102_GetBPM(void);
float MAX30102_GetAvgBPM(void);
float MAX30102_CalculateSpO2(uint16_t red, uint16_t ir);

#endif // __MAX30102_H__
