// MAX30102.c
#include "MAX30102.h"
#include "../inc/I2C0.h"
#include "../inc/SysTick.h"

#define RATE_SIZE 4

static uint8_t rates[RATE_SIZE];
static uint8_t rateSpot = 0;
static uint32_t lastBeat = 0;
static float beatsPerMinute = 0;
static float beatAvg = 0;

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
  SysTick_Wait10ms(10); // extra delay after reset

  // Disable interrupts
  I2C_Send2(MAX30102_ADDR, 0x02, 0x00); // INT_ENABLE1
  I2C_Send2(MAX30102_ADDR, 0x03, 0x00); // INT_ENABLE2

  // FIFO Configuration: sample avg = 4, rollover enabled, almost full = 17
  I2C_Send2(MAX30102_ADDR, 0x08, 0x4F); // FIFO_CONFIG

  // Mode: SpO2 mode (0x03)
  I2C_Send2(MAX30102_ADDR, 0x09, 0x03); // MODE_CONFIG

  // SpO2 Configuration: 100 Hz sample rate, 411us (18-bit) pulse width
  I2C_Send2(MAX30102_ADDR, 0x0A, 0x27); // SPO2_CONFIG

  // Set LED1 (IR) and LED2 (Red) pulse amplitudes
  I2C_Send2(MAX30102_ADDR, 0x0C, 0x24); // LED1 (IR)
  I2C_Send2(MAX30102_ADDR, 0x0D, 0x24); // LED2 (Red)

  // Multi-LED Mode control: slot 1 = red, slot 2 = IR
  I2C_Send2(MAX30102_ADDR, 0x11, 0x03); // LED_SLOT

  // Clear FIFO pointers
  I2C_Send2(MAX30102_ADDR, 0x04, 0x00); // FIFO_WR_PTR
  I2C_Send2(MAX30102_ADDR, 0x05, 0x00); // OVF_COUNTER
  I2C_Send2(MAX30102_ADDR, 0x06, 0x00); // FIFO_RD_PTR
}

//------------MAX30102_ReadIR------------
// Reads the 16-bit IR LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (IR pulse data)
uint16_t MAX30102_ReadIR(void){
  // Point at FIFO_DATA
  I2C_Send1(MAX30102_ADDR, 0x07);
	I2C_DebugStatus();
  // Read two bytes in one transaction (START, ACK, STOP)
	uint16_t value = I2C_Recv2(MAX30102_ADDR);
	I2C_DebugStatus();
  return value;
}

//------------MAX30102_ReadRed------------
// Reads the 16-bit Red LED sensor data from FIFO
// Input: none
// Output: 16-bit unsigned value (Red pulse data)
uint16_t MAX30102_ReadRed(void) {
  I2C_Send1(MAX30102_ADDR, 0x07);           // Set FIFO data pointer
  return I2C_Recv2(MAX30102_ADDR);          // Read 16-bit Red LED data
}

//------------MAX30102_CheckForBeat------------
// Detects heartbeat by analyzing IR signal waveform transitions
// Input: 16-bit IR LED value
// Output: true if beat detected, false otherwise
bool MAX30102_CheckForBeat(uint16_t irValue){
  static uint16_t prevIR = 0;
  static uint16_t prevDiff = 0;
  static uint8_t peakDetected = 0;
  bool beatDetected = false;

  int16_t diff = irValue - prevIR;
  int16_t slope = diff - prevDiff;

  // ?? Relaxed condition: look for rising then falling transition
  if (slope < 0 && prevDiff > 300 && peakDetected == 0) {
    uint32_t now = SysTick_Millis();
    uint32_t delta = now - lastBeat;
    lastBeat = now;

    beatsPerMinute = 60.0f / (delta / 1000.0f);
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (uint8_t)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      uint16_t sum = 0;
      for (uint8_t i = 0; i < RATE_SIZE; i++) sum += rates[i];
      beatAvg = sum / RATE_SIZE;
    }
    beatDetected = true;
    peakDetected = 1;
  }

  if (slope > 0) {
    peakDetected = 0; // allow next peak
  }

  prevDiff = diff;
  prevIR = irValue;
  return beatDetected;
}

//------------MAX30102_CalculateSpO2------------
// Calculates estimated SpO2 from IR and Red values
// Input: red and ir sensor values
// Output: estimated SpO2 percentage (0.0 - 100.0)
float MAX30102_CalculateSpO2(uint16_t red, uint16_t ir) {
  // Prevent division by zero
  if (red == 0 || ir == 0) return 0;

  float ratio = (float)red / (float)ir;
  float spo2 = 110.0f - 25.0f * ratio;

  if (spo2 > 100.0f) spo2 = 100.0f;
  if (spo2 < 0.0f) spo2 = 0.0f;

  return spo2;
}

//------------MAX30102_GetBPM------------
// Returns the most recent BPM reading
// Input: none
// Output: BPM value as float
float MAX30102_GetBPM(void){
  return beatsPerMinute;
}

//------------MAX30102_GetAvgBPM------------
// Returns the average BPM over last few beats
// Input: none
// Output: averaged BPM value
float MAX30102_GetAvgBPM(void){
  return beatAvg;
}
