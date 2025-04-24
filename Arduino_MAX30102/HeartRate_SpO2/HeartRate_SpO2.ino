#include <Wire.h>
#include <SPI.h>
#include "MAX30105.h"
#include "heartRate.h"

#define START_BYTE 0xF7
#define SPI_SS 10

MAX30105 particleSensor;

// Heart rate averaging
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;

long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

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

    // Dynamic normalization around baseline
    static long baseline = 123200; // Set to your observed center value
    const long RANGE = 500;  // Expected variation around baseline
    // Calculate percentage change from baseline
    float change = (irValue - baseline) / (float)baseline;
    uint8_t toSend = constrain(128 + (change * 500), 0, 255); // Amplify changes

    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.print(beatAvg);
    Serial.print(" toSend=");
    Serial.println(toSend);

    // Send toSend IR value via SPI
    SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE1));
    digitalWrite(SPI_SS, LOW);
    SPI.transfer(toSend);
    digitalWrite(SPI_SS, HIGH);
    SPI.endTransaction();

    delay(10); // Adjust delay for desired sampling rate (~100Hz)
}
