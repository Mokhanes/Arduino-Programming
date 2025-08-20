#include <SPI.h>
#include <MFRC522.h>

// Use default SPI pins for ESP8266
#define RST_PIN D3  // GPIO0
#define SS_PIN  D4  // GPIO2 (SDA)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for Serial Monitor

  SPI.begin();  // Use default SPI pins (SCK: D5, MISO: D6, MOSI: D7)
  mfrc522.PCD_Init();  // Init MFRC522
  delay(100);

  mfrc522.PCD_DumpVersionToSerial();  // Show MFRC522 version info
  Serial.println(F("Scan a card to see UID, SAK, type, and data blocks..."));
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Read card serial number
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID
  Serial.print("UID tag: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Dump card data
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  // Halt communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
