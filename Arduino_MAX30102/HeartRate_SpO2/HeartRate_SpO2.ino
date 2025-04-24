#include <Wire.h>
#include <SPI.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

// Heart rate averaging
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// SPI SS pin
const int SPI_SS = 10;

void setup() {
  Serial.begin(115200);

  // Initialize SPI
  SPI.begin();
  pinMode(SPI_SS, OUTPUT);
  digitalWrite(SPI_SS, HIGH);  // Deselect Tiva initially

  // Initialize MAX30102 sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); // Default config
  particleSensor.setPulseAmplitudeRed(0x0A);    // Lower power
  particleSensor.setPulseAmplitudeGreen(0);     // Disable green
}

void loop() {
  static bool started = false;
  if (!started) {
    delay(500); // give Tiva time to initialize SSI2
    started = true;
  }

  long irValue = particleSensor.getIR();

  // BPM detection
  if (checkForBeat(irValue)) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute > 20 && beatsPerMinute < 255) {
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // Debug output to serial
  Serial.print("IR=");
  Serial.print(irValue);
  Serial.print(", BPM=");
  Serial.print(beatsPerMinute);
  Serial.print(", Avg BPM=");
  Serial.print(beatAvg);

  // Send data to Tiva over SPI (8-bit values)
  uint8_t irLSB = (uint8_t)(irValue & 0xFF);
  uint8_t bpmByte = (uint8_t)beatAvg;
  uint8_t checksum = irLSB ^ bpmByte;

  // Debug output to serial
  Serial.print(", irLSB=");
  Serial.print(irLSB);
  Serial.print(", bpmByte=");
  Serial.print(bpmByte);
  Serial.print(", checksum=");
  Serial.print(checksum);

  // Send binary frame: [START][IR][BPM][CHECKSUM]
  uint8_t frame[] = {0xF7, irLSB, bpmByte, checksum};

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SPI_SS, LOW); // Begin transmission

  for (uint8_t i = 0; i < sizeof(frame); i++) {
    SPI.transfer(frame[i]); // Send byte, discard response
  }

  digitalWrite(SPI_SS, HIGH); // End transmission
  delayMicroseconds(100);
  SPI.endTransaction();
  Serial.print(" SPI frame sent");

  if (irValue < 50000)
    Serial.print(" No finger?");
  Serial.println();
}
