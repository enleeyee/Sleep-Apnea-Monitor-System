// main.c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "hc05.h"
#include "GPIO_PortB_Init.h"
#include "MAX30102.h"
#include "ssd1306.h"
#include "../inc/I2C0.h"
#include "../inc/PLL.h"
#include "../inc/SysTick.h"
#include "../inc/UART.h"

// ======= Hardware Connections =======
// SSD1306 SCL (Yellow) to PA6
// SSD1306 SDA (Blue) to PA7

// HC05 TXD connected to PB0
// HC05 RXD connected to PB1
// MAX30102 SCL connected to PB2
// MAX30102 SDA connected to PB3

// USB TXD connected to PE4
// USB RXD connected to PE5

// ======= Function Prototypes =======
void System_Init(void);
void Peripheral_Scan(void);
void Display_InitScreen(void);
void Update_Display_Bluetooth(uint8_t bpm, uint8_t spo2);
void Check_SleepApnea(float spo2, uint32_t now, uint32_t *apneaStartTime, bool *apneaDetected);
void Check_HeartbeatLoss(uint32_t now, uint32_t *lastBeatTime);

// ======= Main Function =======
int main(void) {
  System_Init();
  Peripheral_Scan();
  Display_InitScreen();

  static uint32_t apneaStartTime = 0, lastBeatTime = 0;
  static bool apneaDetected = false;

  static uint16_t prev_ir = 0, prev_red = 0;
  static uint8_t prev_bpm = 0, prev_spo2 = 0;

  while (1) {
    uint16_t ir = MAX30102_ReadIR();
    uint16_t red = MAX30102_ReadRed();
    bool beat = MAX30102_CheckForBeat(ir);
    float bpm = MAX30102_GetBPM();
    float spo2 = MAX30102_CalculateSpO2(red, ir);
    uint32_t now = SysTick_Millis();

    if (beat) lastBeatTime = now;

    // Debug UART
    UART_OutString("IR = "); UART_OutUDec(ir);
    if (beat) {
      UART_OutString(", BPM = "); UART_OutUDec((uint16_t)bpm);
      UART_OutString(", AVG BPM = "); UART_OutUDec((uint16_t)MAX30102_GetAvgBPM());
      UART_OutString(", O2 = "); UART_OutUDec((uint16_t)spo2);
      UART_OutString("  <-- Beat detected!");
    }
    OutCRLF();

    // OLED + Bluetooth Update
    if ((ir != prev_ir) || (red != prev_red) || ((uint8_t)bpm != prev_bpm) || ((uint8_t)spo2 != prev_spo2)) {
      Update_Display_Bluetooth((uint8_t)bpm, (uint8_t)spo2);
      prev_ir = ir; prev_red = red; prev_bpm = (uint8_t)bpm; prev_spo2 = (uint8_t)spo2;
    }

    Check_SleepApnea(spo2, now, &apneaStartTime, &apneaDetected);
    Check_HeartbeatLoss(now, &lastBeatTime);
  }
}

// ======= Initialization =======
void System_Init(void) {
  PLL_Init(Bus80MHz);
  SysTick_Init();
  GPIO_PortB_Init();
  I2C_Init();
  UART_Init();
  MAX30102_Init();
  HC05_Init();
  HC05_WriteString("Bluetooth ready...\r\n");
  SysTick80_Wait10ms(10);
  I2C1_Init();
  OLED_Init();
  OLED_TurnOn();
}

void Peripheral_Scan(void) {
  for (uint8_t addr = 0; addr < 128; addr++) {
    I2C_Send1(addr, 0xFF);
    uint8_t part = I2C_Recv(addr);
    if (part != 0xFF) {
      UART_OutString("Found I2C device at 0x");
      UART_OutUHex2(addr);
      UART_OutString(" (Part ID: 0x");
      UART_OutUHex2(part);
      UART_OutString(")\r\n");
    }
  }
}

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

// ======= Display & Bluetooth Update =======
void Update_Display_Bluetooth(uint8_t bpm, uint8_t spo2) {
  char line1[16], line2[16];
  sprintf(line1, "BPM: %3u", bpm);
  sprintf(line2, "O2 : %3u%%", spo2);

  OLED_setXY(1, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
  OLED_sendStrXY(line1, 1, 0);

  OLED_setXY(2, 0); for (int i = 0; i < 16; i++) OLED_data(' ');
  OLED_sendStrXY(line2, 2, 0);

  char btjson[64];
  sprintf(btjson, "{\"bpm\":%u,\"spo2\":%u}\r\n", bpm, spo2);
  HC05_WriteString(btjson);
}

// ======= Sleep Apnea Detection =======
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

void Check_HeartbeatLoss(uint32_t now, uint32_t *lastBeatTime) {
  if ((now - *lastBeatTime) > 10000) {
    HC05_WriteString("{\"event\":\"Apnea Detected - No Heartbeat\"}\r\n");
    OLED_setXY(5, 0);
    OLED_sendStrXY("No Beat Event!", 5, 0);
    *lastBeatTime = now;
  }
}
