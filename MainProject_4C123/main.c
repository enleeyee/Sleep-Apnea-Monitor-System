// main.c
// Entry point of TM4C123-based health monitor project
// Reads pulse and SpO2 data, displays results on OLED,
// transmits values via HC-05 Bluetooth, and detects sleep apnea
// Input: none
// Output: Continuous display and Bluetooth transmission
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "hc05.h"
#include "GPIO_PortB_Init.h"
#include "ssd1306.h"
#include "SSI2.h"
#include "../inc/I2C0.h"
#include "../inc/PLL.h"
#include "../inc/SysTick.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/UART.h"


#define SAMPLE_RATE_HZ   100  // Must match Arduino's sampling rate
#define BUFFER_SIZE      100  // Circular buffer for smoothing
#define THRESHOLD        5   // Adjust based on signal noise
#define MIN_BPM 40
#define MAX_BPM 180
#define PEAK_TIMEOUT_MS 1500 // Max time between beats

uint8_t irBuffer[BUFFER_SIZE];  // Circular buffer for IR values
uint16_t bufferIndex = 0;       // Current position in buffer
uint32_t lastBeatTime = 0;      // Timestamp of last detected beat
uint8_t bpm = 0;                // Calculated BPM

uint8_t constrain(uint16_t value, uint8_t min, uint8_t max) {
    if(value < min) return min;
    if(value > max) return max;
    return (uint8_t)value;
}

// Replace your detectBeat and updateBPM functions with:
typedef struct {
    uint32_t lastBeatTime;
    uint32_t lastPeakTime;
    uint8_t lastPeakValue;
    uint16_t beatIntervals[4];
    uint8_t beatIndex;
} BeatDetector;

BeatDetector detector = {0};

// ======= Hardware Connections =======
// PA6 = I2C0 SCL (OLED SCL Yellow)
// PA7 = I2C0 SDA (OLED SDA Blue)

// PB0 = UART1 RX (HC-05 RX)
// PB1 = UART1 TX (HC-05 TX)
// PB4 = SSI2CLK (Arduino SCK D13)
// PB5 = SSI2Fss (Arduino SS D10)
// PB6 = SSI2Rx (Arduino MOSI D11)
// PB7 = SSI2Tx (Arduino MISO D12)

// USB TXD connected to PE4
// USB RXD connected to PE5

// ======= Function Prototypes =======
void System_Init(void);
void Peripheral_Scan(void);
void Display_InitScreen(void);
void Update_Display_Bluetooth(uint8_t ir, uint8_t bpm, uint8_t spo2);
void Check_SleepApnea(float spo2, uint32_t now, uint32_t *apneaStartTime, bool *apneaDetected);
void Check_HeartbeatLoss(uint32_t now, uint32_t *lastBeatTime);
void processSPI(void);
void GPIOPortB_Handler(void);

// Simple moving average to smooth the signal
uint8_t getSmoothedIR() {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < BUFFER_SIZE; i++) {
        sum += irBuffer[i];
    }
    return sum / BUFFER_SIZE;
}

bool detectBeat(uint8_t currentIR, uint32_t currentTime) {
    static uint8_t lastValue = 128;
    static bool wasRising = false;
    
    // Detect slope direction
    bool isRising = (currentIR > lastValue);
    bool beatDetected = false;
    
    // Peak detection (transition from rising to falling)
    if(wasRising && !isRising) {
        // Validate peak height and timing
        if((currentTime - detector.lastPeakTime) > (60000/MAX_BPM) &&
           (currentIR > detector.lastPeakValue + 2)) {
            beatDetected = true;
            
            // Store beat interval
            if(detector.lastBeatTime != 0) {
                detector.beatIntervals[detector.beatIndex] = 
                    currentTime - detector.lastBeatTime;
                detector.beatIndex = (detector.beatIndex + 1) % 4;
            }
            detector.lastBeatTime = currentTime;
            detector.lastPeakTime = currentTime;
            detector.lastPeakValue = currentIR;
        }
    }
    
    wasRising = isRising;
    lastValue = currentIR;
    return beatDetected;
}

// Calculate BPM from time between beats
uint8_t calculateBPM() {
    if(detector.lastBeatTime == 0) return 0;
    
    uint32_t avgInterval = 0;
    uint8_t validCount = 0;
    
    // Average last 4 beat intervals
    for(uint8_t i = 0; i < 4; i++) {
        if(detector.beatIntervals[i] > 0) {
            avgInterval += detector.beatIntervals[i];
            validCount++;
        }
    }
    
    if(validCount == 0) return 0;
    avgInterval /= validCount;
    
    // Calculate BPM (ms to minutes conversion)
    uint16_t bpm = 60000 / avgInterval;
    return constrain(bpm, MIN_BPM, MAX_BPM);
}

// ======= Main Function =======
int main(void) {
  System_Init();
  Peripheral_Scan();
  Display_InitScreen();
	
		uint8_t lastIR;

		while (1) {
				if (SSI2_SR_R & SSI_SR_RNE) {
						uint8_t irValue = SSI2_DR_R & 0xFF;
						uint32_t currentTime = SysTick_Millis();
						
						// Detect beats
						if(detectBeat(irValue, currentTime)) {
								bpm = calculateBPM();
								UART_OutString("BEAT! BPM:");
								UART_OutUDec(bpm);
								OutCRLF();
						}
						
						// Visual feedback
						UART_OutString("IR:");
						UART_OutUDec(irValue);
						UART_OutString(irValue > lastIR ? " R" : " F");
						OutCRLF();
						
						Update_Display_Bluetooth(irValue, bpm, 98);
						
						lastIR = irValue;
				}
		}
}

/*
int main(void) {
  System_Init();
  Peripheral_Scan();
  Display_InitScreen();

  uint32_t apneaStartTime = 0, lastBeatTime = 0;
  bool apneaDetected = false;

  uint8_t bpm = 0, ir = 0;

  while (1) {
    if (SSI2_SR_R & 0x04) { // Data is available from Arduino
      ir = SSI2_Recv();
      while (!(SSI2_SR_R & 0x04));
      bpm = SSI2_Recv();

      UART_OutString("IR: ");
      UART_OutUDec(ir);
      UART_OutString(", BPM: ");
      UART_OutUDec(bpm);
      OutCRLF();

      Update_Display_Bluetooth(bpm, 98); // Dummy SpO2 for now
      Check_SleepApnea(98, SysTick_Millis(), &apneaStartTime, &apneaDetected);
      Check_HeartbeatLoss(SysTick_Millis(), &lastBeatTime);
    }

    SysTick80_Wait10ms(10);
  }
}
*/

//------------System_Init------------
// Initializes system clock, GPIOs, I2C, UART, MAX30102 sensor, OLED, and HC-05 Bluetooth
// Input: none
// Output: none
void System_Init(void) {
  PLL_Init(Bus80MHz);
	SysTick_Init();
	UART_Init();
	
  GPIO_PortB_Init();
  I2C_Init();
	SSI2_Init_Slave();
	UART_OutString("SSI2 init complete.\r\n");
	/*
  HC05_Init();
  HC05_WriteString("Bluetooth ready...\r\n");
	*/
  SysTick80_Wait10ms(10);
	
	I2C1_Init();
  OLED_Init();
  OLED_TurnOn();
	UART_OutString("OLED init complete.\r\n");
}

//------------Peripheral_Scan------------
// Scans all 7-bit I2C addresses to detect connected devices
// Input: none
// Output: Detected addresses printed to UART
void Peripheral_Scan(void) {
  UART_OutString("Scanning I2C devices...\r\n");

  for (uint8_t addr = 8; addr < 120; addr++) {
    uint32_t error = I2C_Send1(addr, 0x00);  // dummy command
    if (error == 0) {
      UART_OutString("Found device at 0x");
      UART_OutUHex2(addr);
      UART_OutString("\r\n");
    }
  }

  UART_OutString("Scan complete.\r\n");
}


//------------Display_InitScreen------------
// Fills and clears OLED to visually confirm OLED initialization
// Input: none
// Output: OLED is cleared and ready for display
void Display_InitScreen(void) {
  for (uint8_t i = 0; i < 8; i++) {
    OLED_command(0xB0 + i);
    OLED_command(0x00);
    OLED_command(0x10);
    for (uint8_t j = 0; j < 128; j++) {
      OLED_data(0xFF);
    }
  }
  OLED_clearDisplay();
}

//------------Update_Display_Bluetooth------------
// Updates OLED display with BPM and SpO2 values and sends JSON to Bluetooth
// Input: bpm (beats per minute), spo2 (oxygen level percentage)
// Output: OLED updated, data sent over HC-05
void Update_Display_Bluetooth(uint8_t ir, uint8_t bpm, uint8_t spo2) {
  char line1[16], line2[16], line3[16];
	sprintf(line1, "IR: %3u", ir);
  sprintf(line2, "BPM: %3u", bpm);
  sprintf(line3, "O2 : %3u%%", spo2);

  OLED_setXY(1, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
  OLED_sendStrXY(line1, 1, 0);

  OLED_setXY(2, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
  OLED_sendStrXY(line2, 2, 0);
	
	OLED_setXY(3, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
  OLED_sendStrXY(line3, 3, 0);
	
	/*
  char btjson[64];
  sprintf(btjson, "{\"bpm\":%u,\"spo2\":%u}\r\n", bpm, spo2);
  HC05_WriteString(btjson);
	*/
}

//------------Check_SleepApnea------------
// Detects sleep apnea if SpO2 is below 90% for more than 10 seconds
// Input: spo2 - current oxygen level, now - current time in ms,
//        apneaStartTime - pointer to timer of low spo2 duration,
//        apneaDetected - pointer to flag tracking detection state
// Output: Sends apnea alert via Bluetooth and updates OLED if condition met
void Check_SleepApnea(float spo2, uint32_t now, uint32_t *apneaStartTime, bool *apneaDetected) {
  if (spo2 < 90) {
    if (!(*apneaDetected)) {
      *apneaStartTime = now;
      *apneaDetected = true;
    } else if ((now - *apneaStartTime) > 10000) {
      HC05_WriteString("{\"event\":\"Apnea Detected - Low SpO2\"}\r\n");
      OLED_setXY(4, 0);
      OLED_sendStrXY("Apnea Event!", 4, 0);
    }
  } else {
    *apneaDetected = false;
  }
}

//------------Check_HeartbeatLoss------------
// Detects heartbeat loss if no beat is detected for more than 10 seconds
// Input: now - current time in ms, lastBeatTime - pointer to time of last beat
// Output: Sends no-beat event via Bluetooth and updates OLED
void Check_HeartbeatLoss(uint32_t now, uint32_t *lastBeatTime) {
  if ((now - *lastBeatTime) > 10000) {
    HC05_WriteString("{\"event\":\"Apnea Detected - No Heartbeat\"}\r\n");
    OLED_setXY(5, 0);
    OLED_sendStrXY("No Beat Event!", 5, 0);
    *lastBeatTime = now;
  }
}

void GPIOPortB_Handler(void) {
  if (GPIO_PORTB_RIS_R & 0x20) { // PB5 (SS) interrupt
    GPIO_PORTB_ICR_R = 0x20;     // Clear interrupt
    
    if(GPIO_PORTB_DATA_R & 0x20) {
      UART_OutString("SS went HIGH - frame complete\r\n");
      
      // Read all available data (should be 4 bytes)
      uint8_t count = 0;
      while(SSI2_SR_R & SSI_SR_RNE) {
        uint8_t data = SSI2_DR_R & 0xFF;
        UART_OutString("Byte ");
        UART_OutUDec(count++);
        UART_OutString(": 0x");
        UART_OutUHex2(data);
        OutCRLF();
      }
    }
  }
}
