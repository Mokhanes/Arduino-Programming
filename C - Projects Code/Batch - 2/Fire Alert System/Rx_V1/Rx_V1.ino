#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// CE = 9, CSN = 10
RF24 radio(9, 10);
const uint64_t address = 0xE8E8F0F0E1LL;

uint8_t rxData[5];  // [fireP, smokeP, lpgP, tempI, humI]

void setup() {
  Serial.begin(9600);
  Serial.println("NRF24 Receiver Started...");

  if (!radio.begin()) {
    Serial.println("NRF24 initialization failed!");
    while (1);
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();  // Receiver mode

  Serial.println("Listening for data...");
}

void loop() {
  if (radio.available()) {
    radio.read(&rxData, sizeof(rxData));

    uint8_t fireP  = rxData[0];
    uint8_t smokeP = rxData[1];
    uint8_t lpgP   = rxData[2];
    uint8_t tempI  = rxData[3];
    uint8_t humI   = rxData[4];

    Serial.println("------ FIRE ALERT DATA RECEIVED ------");
    Serial.print("Fire Probability: ");
    Serial.print(fireP);
    Serial.println(" %");

    Serial.print("Smoke Level: ");
    Serial.print(smokeP);
    Serial.println(" %");

    Serial.print("LPG Level: ");
    Serial.print(lpgP);
    Serial.println(" %");

    Serial.print("Temperature: ");
    Serial.print(tempI);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humI);
    Serial.println(" %");

    Serial.println("--------------------------------------");
    delay(300);
  }
}
