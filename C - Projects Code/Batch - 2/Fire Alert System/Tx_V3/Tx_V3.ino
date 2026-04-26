#define BLYNK_TEMPLATE_ID   "TMPL3U3dq8izS"
#define BLYNK_TEMPLATE_NAME "Fire Alert System"
#define BLYNK_AUTH_TOKEN    "tPXSEM20V9OTKSEXkUx9Ak_v_vJHUScr"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ========== WiFi ==========
char ssid[] = "Admin";
char pass[] = "12344321";

// ========== Sensor Pins ==========
#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ2_PIN 35  // Smoke Sensor
#define MQ6_PIN 34  // LPG Sensor
#define FLAME1_PIN 25  
#define FLAME2_PIN 33  
#define FLAME3_PIN 32  

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;

RF24 radio(15, 5);  // CE = G15, CSN = G5
const uint64_t address = 0xE8E8F0F0E1LL;

// 🆕 Data structure to send all values

uint8_t txData[5];



// 🆕 SMS Credentials & Flag
const char* apiKey = "P9txnl1rho77";       // API Key
const char* templateID = "101";            // Template ID
const char* mobileNumber = "919384777909"; // Mobile Number
bool smsSent = false;  // To prevent multiple SMS

// 🆕 Function: Send SMS
void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    String var1 = "Storage Area";
    String var2 = "Temperature: "+ String(temperature)+" C and Fire was Deducted...!";

    String payload = "{\"mobiles\":\"" + String(mobileNumber) +
                     "\",\"var1\":\"" + var1 +
                     "\",\"var2\":\"" + var2 + "\"}";

    Serial.println("Sending SMS Payload: " + payload);

    int response = http.POST(payload);

    if (response == 200) {
      Serial.println("SMS Sent Successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("SMS Failed. Error Code: ");
      Serial.println(response);
      Serial.println("Response: " + http.getString());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. SMS not sent.");
  }
}

// ========== Function: Calculate & Send Data ==========
void sendSensorData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int smokeRaw = analogRead(MQ2_PIN);
  int lpgRaw   = analogRead(MQ6_PIN);

  int smokePercent = map(smokeRaw, 0, 4095, 0, 100);
  int lpgPercent   = map(lpgRaw, 0, 4095, 0, 100);

  bool flame1 = digitalRead(FLAME1_PIN);
  bool flame2 = digitalRead(FLAME2_PIN);
  bool flame3 = digitalRead(FLAME3_PIN);

  bool flameDetected = (flame1 || flame2 || !flame3);

  int fireProbability = 0;
  if (flameDetected) {
    fireProbability = 100;
  } else {
    fireProbability = (smokePercent * 0.35) + (lpgPercent * 0.25) + 
                      (temperature * 0.25) + ((100 - humidity) * 0.15);
    if (fireProbability > 100) fireProbability = 100;
  }

  String fireState;
  if (fireProbability >= 95) fireState = "Fired..!";
  else if (fireProbability >= 80) fireState = "Danger..!";
  else if (fireProbability >= 60) fireState = "Warning";
  else fireState = "Normal";

  String stateWithPercent = fireState + " (" + String(fireProbability) + "%)";

  // ====== Send to Blynk ======
  Blynk.virtualWrite(V0, smokePercent);
  Blynk.virtualWrite(V1, lpgPercent);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, stateWithPercent);
  Blynk.virtualWrite(V5, fireProbability);

  // ====== Serial Monitor Output ======
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" | Hum: "); Serial.print(humidity);
  Serial.print(" | Smoke: "); Serial.print(smokePercent);
  Serial.print(" | LPG: "); Serial.print(lpgPercent);
  Serial.print(" | Fire%: "); Serial.print(fireProbability);
  Serial.print(" | State: "); Serial.println(stateWithPercent);

  // 🆕 Send SMS Only When Fired (One Time)
  if (fireProbability == 100 && !smsSent) {
    sendSMS();
    smsSent = true;
    Serial.println("🔥 SMS Sent! (One-time Alert)");
  }else if (fireProbability < 100 && smsSent) {
    smsSent = false;
  }


    // ========= NRF24 TX: send all data as BYTE (0–100) =========
  // Clamp values to 0–100 range just to be safe
  uint8_t fireP = (fireProbability < 0) ? 0 : (fireProbability > 100 ? 100 : fireProbability);
  uint8_t smokeP = (smokePercent < 0) ? 0 : (smokePercent > 100 ? 100 : smokePercent);
  uint8_t lpgP   = (lpgPercent   < 0) ? 0 : (lpgPercent   > 100 ? 100 : lpgPercent);
  uint8_t tempI  = (uint8_t) (temperature < 0 ? 0 : (temperature > 100 ? 100 : (int)temperature));
  uint8_t humI   = (uint8_t) (humidity    < 0 ? 0 : (humidity    > 100 ? 100 : (int)humidity));

  txData[0] = fireP;  // Fire Probability %
  txData[1] = smokeP; // Smoke %
  txData[2] = lpgP;   // LPG %
  txData[3] = tempI;  // Temperature (°C, int)
  txData[4] = humI;   // Humidity %

  bool report = radio.write(&txData, sizeof(txData));

  Serial.print("📡 NRF24 Send: ");
  Serial.println(report ? "Success" : "Fail");



  // ====== LCD Display ======
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");

  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("F:");
  lcd.print(fireProbability);
  lcd.print("% ");

  lcd.print(fireState);
}

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  dht.begin();

  pinMode(FLAME1_PIN, INPUT);
  pinMode(FLAME2_PIN, INPUT);
  pinMode(FLAME3_PIN, INPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendSensorData);

  // 🆕 --- NRF24 Setup (Just Added, Not Changed Anything Else) ---
  SPI.begin(18, 19, 23);  // SCK = 18, MISO = 19, MOSI = 23  (ESP32 HW SPI pins)

  if (!radio.begin()) {
    Serial.println("NRF24 initialization failed!");
  } else {
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_1MBPS);
    radio.stopListening();  // Transmitter mode
    Serial.println("NRF24 Initialized OK. Ready to send...");
  }



  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecting");
  delay(1500);
}

void loop() {
  Blynk.run();
  timer.run();
}
