#define BLYNK_TEMPLATE_ID "TMPL3LmIb9nuD"
#define BLYNK_TEMPLATE_NAME "GAS Monitoring System"
#define BLYNK_AUTH_TOKEN "ucrgRR_AlZJZaAhONBemtEjZW0G-eAXk"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <BlynkSimpleEsp8266.h>

Servo myServo;

// Pin Definitions
#define MQ6_PIN A0
#define BUZZER D5
#define LED D6
#define RELAY_PIN D8
#define RESET D7
#define SERVO_PIN D3
#define LED_BUILTIN_PIN LED_BUILTIN

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Flags and Variables
bool gasPreviouslyDetected = false;
int detectionCount = 0;
bool stayOn = false;
bool smsSent = false;

unsigned long lastDetectionTime = 0;
const unsigned long debounceDelay = 5000; // 5 seconds debounce

// Wi-Fi
const char* ssid = "OPPO A7";
const char* password = "12344321";

// SMS API (CircuitDigest)
const char* apiKey = "l77fyFQ9B9dK";
const char* templateID = "101";
const char* mobileNumber = "918838417892";

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  digitalWrite(LED_BUILTIN_PIN, HIGH); // OFF

  // LCD
  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // Wi-Fi Connect with LED blink
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN_PIN, LOW); delay(300);
    digitalWrite(LED_BUILTIN_PIN, HIGH); delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN_PIN, LOW); // ON

  // Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  while (!Blynk.connected()) {
    Blynk.run();
    delay(100);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected...!");

  // Setup pins
  myServo.attach(SERVO_PIN);
  pinMode(MQ6_PIN, INPUT);
  pinMode(RESET, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED, LOW);

  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  int gasLevel = getSmoothedGasLevel();
  Serial.print("Gas Level: ");
  Serial.print(gasLevel);
  Serial.println("%");
  Blynk.virtualWrite(V0, gasLevel);

  if (digitalRead(RESET) == LOW) {
    stayOn = false;
    detectionCount = 0;
    gasPreviouslyDetected = false;
    smsSent = false;
    deactivateSystem();
    Serial.println("System Reset!");
    delay(1000);
  }

  if (stayOn) {
    showHighGas(gasLevel);
    activateSystem();

    if (!smsSent) {
      sendSMS(gasLevel);
      smsSent = true;
    }
    return;
  }

  if (gasLevel > 30) {
    if (millis() - lastDetectionTime > debounceDelay) {
      showHighGas(gasLevel);
      activateSystem();

      if (!gasPreviouslyDetected) {
        detectionCount++;
        gasPreviouslyDetected = true;
        lastDetectionTime = millis();
        Serial.print("Detection Count: ");
        Serial.println(detectionCount);
      }

      if (detectionCount >= 2) {
        stayOn = true;
        Serial.println("Permanent Mode ON");
      }
    }
  } else {
    gasPreviouslyDetected = false;

    if (detectionCount < 2) {
      showNormalGas(gasLevel);
      deactivateSystem();
    }
  }

  delay(300);
}

// -------------- Helpers --------------

int getSmoothedGasLevel() {
  int total = 0;
  const int samples = 10;
  for (int i = 0; i < samples; i++) {
    total += analogRead(MQ6_PIN);
    delay(5);
  }
  return map(total / samples, 0, 1023, 0, 100);
}

void showHighGas(int level) {
  lcd.setCursor(0, 0);
  lcd.print("Gas Level: HIGH ");
  lcd.setCursor(0, 1);
  lcd.print("Gas Conc: ");
  lcd.print(level);
  lcd.print("%   ");
}

void showNormalGas(int level) {
  lcd.setCursor(0, 0);
  lcd.print("Gas Level:Normal");
  lcd.setCursor(0, 1);
  lcd.print("Gas Conc: ");
  lcd.print(level);
  lcd.print("%   ");
}

void activateSystem() {
  myServo.write(180);
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(BUZZER, HIGH);
  digitalWrite(LED, HIGH);
}

void deactivateSystem() {
  myServo.write(10);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED, LOW);
}

void sendSMS(int gasLevel) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    String var1 = "Gas Concentration Level";
    String var2 = "HIGH (" + String(gasLevel) + "%)";
    String payload = "{\"mobiles\":\"" + String(mobileNumber) +
                     "\",\"var1\":\"" + var1 +
                     "\",\"var2\":\"" + var2 + "\"}";

    Serial.println("Sending payload: " + payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
      Serial.println("SMS Sent Successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("SMS Failed. Error: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + http.getString());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. SMS not sent.");
  }
}
