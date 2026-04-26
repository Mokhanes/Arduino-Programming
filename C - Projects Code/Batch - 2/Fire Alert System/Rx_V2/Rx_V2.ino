#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// OLED Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// RF24 Configuration
RF24 radio(9, 10);  
const uint64_t address = 0xE8E8F0F0E1LL;

// Buzzer & Relay pins
#define BUZZER_PIN 3
#define RELAY_PIN  4

uint8_t rxData[5]; // fireP, smokeP, lpgP, tempI, humI
char fireState[12];

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  u8g2.begin();

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&rxData, sizeof(rxData));

    uint8_t fireP = rxData[0];
    uint8_t tempI = rxData[3];

    // Set fire state
    if (fireP >= 95) strcpy(fireState, "Fired..!");
    else if (fireP >= 80) strcpy(fireState, "Danger..!");
    else if (fireP >= 60) strcpy(fireState, "Warning..!");
    else strcpy(fireState, "Normal");

    // Control buzzer & relay
    digitalWrite(BUZZER_PIN, fireP >= 100);
    digitalWrite(RELAY_PIN, !(fireP >= 100));

    // OLED Display
    u8g2.clearBuffer();

    // Title (smaller now)
    u8g2.setFont(u8g2_font_6x13B_tr);
    u8g2.drawStr(15, 12, "FIRE ALERT SYSTEM");

    // Temperature (Medium size)
    u8g2.setFont(u8g2_font_fub14_tr);
    char tempStr[10];
    sprintf(tempStr, "%dC", tempI);
    u8g2.drawStr(10, 35, tempStr);

    // Fire Probability (Medium size)
    char fireStr[10];
    sprintf(fireStr, "%d%%", fireP);
    u8g2.drawStr(75, 35, fireStr);

    // Fire State (same readable size)
    u8g2.setFont(u8g2_font_6x13B_tr);
    u8g2.drawStr(10, 55, fireState);

    u8g2.sendBuffer();
    delay(200);
  }
}
