#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D8
#define FP_RX    D2
#define FP_TX    D1

#define SDA_PIN D9
#define SCL_PIN D10

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial fingerSerial(FP_RX, FP_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

const String sheet_url = "https://script.google.com/macros/s/AKfycbyeS6WqJ7ujMnRQyCXLeGkivuZRYAgB35kA2n6SldO-0WxMMdtyisytrGxfdYnWsJcOYQ/exec?name=";
#define WIFI_SSID "OPPO A7"
#define WIFI_PASSWORD "12344321"

boolean waitingForCard = true;
void setup() {

  finger.begin(57600);


  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER,HIGH);
  
  Wire.begin(SDA_PIN, SCL_PIN);
  
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

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
   
    delay(200);
  }


  
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (waitingForCard) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Scan your Card ");
    waitingForCard = false;
  }

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
 
  waitingForCard = true;  // Reset for next cycle

//  if (!mfrc522.PICC_IsNewCardPresent()) return;
//  if (!mfrc522.PICC_ReadCardSerial()) return;

  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();


  String name = "";
  int fingerID = -1;
  if (uidStr == "32C41D55") { name = "Yogesh GM"; fingerID = 1; }
  else if (uidStr == "62242D55") { name = "Sanjay S"; fingerID = 3; }
  else if (uidStr == "73ED391C") { name = "Mokhanes A"; fingerID = 2; }
  else if (uidStr == "32600A55") { name = "Gokul D"; fingerID = 4; }
  else if (uidStr == "60679714") { name = "Yuvan C"; fingerID = 5; }
  else if (uidStr == "F9EEFA03") { name = "Prithika KS"; fingerID = 6; }
  else name = "Unknown";

  if (name == "Unknown") {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Not Found!");
    digitalWrite(BUZZER,LOW);
    delay(1500);
    digitalWrite(BUZZER,HIGH);
    return;
  }

    
  // Show Welcome and name
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome..!");
  lcd.setCursor(0, 1);
  lcd.print(name.substring(0, 16));
  delay(1500);

  digitalWrite(BUZZER,LOW);
  delay(500);
  digitalWrite(BUZZER,HIGH);

  // Prompt for fingerprint
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place Finger...");

  int result = getFingerprintID();

  if (result == fingerID) {
    // Valid fingerprint
    digitalWrite(BUZZER,LOW); 
    delay(500); 
    digitalWrite(BUZZER,HIGH);
    name.replace(" ", "%20");
    sendToGoogleSheet(name);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("Mismatch!");
    digitalWrite(BUZZER,LOW); 
    delay(2000); 
    digitalWrite(BUZZER,HIGH);
    
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

int getFingerprintID() {
  for (int attempts = 0; attempts < 10; attempts++) {
    uint8_t p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      delay(500);
      continue;
    } else if (p != FINGERPRINT_OK) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FP Image Error");
      return -1;
    }

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FP Convert Err");
      return -1;
    }

    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      return finger.fingerID;
    } else {
      return -1;
    }
  }
  return -1; // Timed out
}

void sendToGoogleSheet(String name) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sending to GSheet");

  if (WiFi.status() == WL_CONNECTED) {
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    String url = sheet_url + name;

    HTTPClient https;
    if (https.begin(*client, url)) {
      int httpCode = https.GET();

      if (httpCode > 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Attendance Done");
        lcd.setCursor(0, 1);
        lcd.print("Recorded Online!");
        delay(2000);
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("HTTP Error");
      }

      https.end();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GSheet Conn Err");
    }
  }
}
