// Smart Attendance System with Google Sheets and LCD Display

#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <LiquidCrystal_I2C.h>

//-----------------------------------------
#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D8

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

//-----------------------------------------
int blockNum = 2;
byte bufferLen = 18;
byte readBlockData[18];

//-----------------------------------------
String card_holder_name;
const String sheet_url = "https://script.google.com/macros/s/AKfycbyeS6WqJ7ujMnRQyCXLeGkivuZRYAgB35kA2n6SldO-0WxMMdtyisytrGxfdYnWsJcOYQ/exec?name=";

//-----------------------------------------
#define WIFI_SSID "OPPO A7"
#define WIFI_PASSWORD "12344321"

//-----------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

/****************************************************************************************************
 * setup() function
 ****************************************************************************************************/
void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }

  // WiFi Connectivity
  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(BUZZER, OUTPUT);
  SPI.begin();
}

/****************************************************************************************************
 * loop() function
 ****************************************************************************************************/
void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Scan your Card ");

  mfrc522.PCD_Init();
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("\nReading last data from RFID...");
  ReadDataFromBlock(blockNum, readBlockData);

  // Print raw byte data (for debugging)
  Serial.print("Raw Data: ");
  for (int i = 0; i < 16; i++) {
    Serial.print(readBlockData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // Convert read data to clean string
  String name = "";
  for (int j = 0; j < 16; j++) {
    char c = (char)readBlockData[j];
    if (isPrintable(c) && c != 0) {
      name += c;
    }
  }
  name.trim();  // Remove extra whitespace

  Serial.print("Your Name: ");
  Serial.println(name);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hey!");
  lcd.setCursor(0, 1);
  lcd.print(name.substring(0, 16));  // Trim to LCD width
  delay(1000);

  // Buzzer sound
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }

  // Send to Google Sheet
  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();  // Skip SSL verification

    String url = sheet_url + name;
    url.trim();
    Serial.println("Request URL: " + url);

    HTTPClient https;
    Serial.println("[HTTPS] begin...");

    if (https.begin(*client, url)) {
      Serial.println("[HTTPS] GET...");
      int httpCode = https.GET();

      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        lcd.setCursor(0, 0);
        lcd.print("Attendance Done");
        lcd.setCursor(0, 1);
        lcd.print("Recorded Online!");
        delay(2000);
      } else {
        Serial.printf("[HTTPS] GET failed: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
      delay(1000);
    } else {
      Serial.println("[HTTPS] Unable to connect");
    }
  }
}

/****************************************************************************************************
 * ReadDataFromBlock() function
 ****************************************************************************************************/
void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block read success");
  }
}
