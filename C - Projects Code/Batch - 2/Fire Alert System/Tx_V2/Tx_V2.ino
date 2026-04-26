#define BLYNK_TEMPLATE_ID   "TMPL3U3dq8izS"
#define BLYNK_TEMPLATE_NAME "Fire Alert System"
#define BLYNK_AUTH_TOKEN    "tPXSEM20V9OTKSEXkUx9Ak_v_vJHUScr"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

  // ====== Fire Probability Calculation ======
  int fireProbability = 0;
  if (flameDetected) {
    fireProbability = 100;
  } else {
    fireProbability = (smokePercent * 0.35) + (lpgPercent * 0.25) + 
                      (temperature * 0.25) + ((100 - humidity) * 0.15);
    if (fireProbability > 100) fireProbability = 100;
  }

  // ====== Fire State Logic ======
  String fireState;
  if (fireProbability >= 95) fireState = "Fired..!";
  else if (fireProbability >= 80) fireState = "Danger..!";
  else if (fireProbability >= 60) fireState = "Warning";
  else fireState = "Normal";

  String stateWithPercent = fireState + " (" + String(fireProbability) + "%)";

  // ====== Send to Blynk ======
  Blynk.virtualWrite(V0, smokePercent);   // Smoke
  Blynk.virtualWrite(V1, lpgPercent);     // LPG
  Blynk.virtualWrite(V2, temperature);    // Temperature
  Blynk.virtualWrite(V3, humidity);       // Humidity
  Blynk.virtualWrite(V4, stateWithPercent); // Fire State String
  Blynk.virtualWrite(V5, fireProbability); // Fire State String


  // ====== Serial Monitor Output ======
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" | Hum: "); Serial.print(humidity);
  Serial.print(" | Smoke: "); Serial.print(smokePercent);
  Serial.print(" | LPG: "); Serial.print(lpgPercent);
  Serial.print(" | Fire%: "); Serial.print(fireProbability);
  Serial.print(" | State: "); Serial.println(stateWithPercent);

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

  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecting");
  delay(1500);
}

void loop() {
  Blynk.run();
  timer.run();
}
