// MAX30102.c
#include "MAX30102.h"
#include "../inc/I2C0.h"
#include "../inc/SysTick.h"

//------------MAX30102_Reset------------
// Sends software reset to MAX30102 via MODE_CONFIG register
// Input: none
// Output: none
void MAX30102_Reset(void){
  // Reset via MODE_CONFIG register
  I2C_Send2(MAX30102_ADDR, 0x09, 0x40); // Reset command
  SysTick_Wait10ms(10); // Wait for reset
}

//------------MAX30102_Init------------
// Initialize MAX30102 for SpO2 mode (IR/Red LEDs), 100Hz sampling
// Input: none
// Output: none
void MAX30102_Init(void){
  MAX30102_Reset();

  // Set SpO2 mode: SpO2 enabled (0x03)
  I2C_Send2(MAX30102_ADDR, 0x09, 0x03);

  // LED Pulse Amplitude
  I2C_Send2(MAX30102_ADDR, 0x0C, 0x24); // IR = 6.4mA, Red = 6.4mA

  // SpO2 Configuration: 100 Hz sample rate, 1600us pulse width
  I2C_Send2(MAX30102_ADDR, 0x0A, 0x27);

  // Enable interrupts (optional)
  // I2C_Send2(MAX30102_ADDR, 0x02, 0xC0); // FIFO almost full & new data ready
}

//------------MAX30102_ReadIR------------
// Reads the 16-bit IR LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (IR pulse data)
uint16_t MAX30102_ReadIR(void){
  uint8_t irHigh, irLow;

  // Set pointer to FIFO_DATA register
  I2C_Send1(MAX30102_ADDR, 0x07);

  // Read two bytes of IR data
  irHigh = I2C_Recv(MAX30102_ADDR);
  irLow = I2C_Recv(MAX30102_ADDR);

  return (irHigh << 8) | irLow;
}
