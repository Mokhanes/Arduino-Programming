
/****************************************************
   ESP32 Fire Detection System + Blynk + LCD
   --------------------------------------------------
   V0 → Smoke % (MQ2)
   V1 → LPG %   (MQ6)
   V2 → Temperature (°C)
   V3 → Humidity (%)
   V4 → Fire State String (Normal / Warning / Danger / Fired)
 ****************************************************/

// ============ Blynk Credentials ============
#define BLYNK_TEMPLATE_ID   "TMPL3U3dq8izS"
#define BLYNK_TEMPLATE_NAME "Fire Alert System"
#define BLYNK_AUTH_TOKEN    "tPXSEM20V9OTKSEXkUx9Ak_v_vJHUScr"

// ============ Libraries ============
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

// ============ WiFi ============
char ssid[] = "Admin";
char password[] = "12344321";

// ============ Sensor Config ============
#define DHTPIN        4
#define DHTTYPE       DHT11

#define MQ2_PIN       34
#define MQ6_PIN       35

#define FLAME1_PIN    27
#define FLAME2_PIN    32
#define FLAME3_PIN    33

// ============ LCD ============
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// ============ Thresholds ============
const int MQ2_THRESHOLD = 350;
const int MQ6_THRESHOLD = 400;
const float TEMP_THRESHOLD = 50.0;

// ============ Screen Rotation ============
unsigned long lastScreenChange = 0;
const unsigned long SCREEN_INTERVAL = 2000;
int currentScreen = 0;

// ====================================================
// ======================= SETUP ======================
// ====================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Use this instead of WiFi.begin(...) + Blynk.begin(...)
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // LCD & Sensor Initialization
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(FLAME1_PIN, INPUT);
  pinMode(FLAME2_PIN, INPUT);
  pinMode(FLAME3_PIN, INPUT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" FIRE ALERT SYS ");
  lcd.setCursor(0, 1);
  lcd.print(" Initializing ");
  delay(1500);
}



// ====================================================
// ===================== LOOP =========================
// ====================================================
void loop() {
  Blynk.run();

  // Read Sensors
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int smokeRaw = analogRead(MQ2_PIN); 
  int lpgRaw = analogRead(MQ6_PIN);  

  // Convert raw to percentage (0–100%)
  int smokePercent = map(smokeRaw, 0, 4095, 0, 100);
  int lpgPercent   = map(lpgRaw, 0, 4095, 0, 100);

  bool flame1 = digitalRead(FLAME1_PIN);
  bool flame2 = digitalRead(FLAME2_PIN);
  bool flame3 = digitalRead(FLAME3_PIN);

  bool flameDetected = (flame1 || flame2 || flame3);

  // -------- Fire Probability Calculation ----------
  int fireProbability = 0;

  if (flameDetected) {
    fireProbability = 100;
  } else {
    fireProbability = (smokePercent * 0.4) + (lpgPercent * 0.3) + (temperature * 0.2) + (humidity < 30 ? 10 : 0);
    if (fireProbability > 100) fireProbability = 100;
  }

  // -------- Fire State Logic ----------
  String fireState = "Normal";
  if (fireProbability >= 95 || flameDetected) fireState = "🔥 FIRED";
  else if (fireProbability >= 80) fireState = "⚠️ Danger";
  else if (fireProbability >= 60) fireState = "Warning";
  else fireState = "Normal";

  // ======== Send to Blynk =========
  Blynk.virtualWrite(V0, smokePercent);
  Blynk.virtualWrite(V1, lpgPercent);
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, fireState);

  // ======== Serial Monitor Debug ========
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" | Hum: "); Serial.print(humidity);
  Serial.print(" | Smoke: "); Serial.print(smokePercent);
  Serial.print(" | LPG: "); Serial.print(lpgPercent);
  Serial.print(" | Fire%: "); Serial.print(fireProbability);
  Serial.print(" | State: "); Serial.println(fireState);

  // ======== LCD Display =========
  if (millis() - lastScreenChange > SCREEN_INTERVAL) {
    lastScreenChange = millis();
    currentScreen = (currentScreen + 1) % 3;
    lcd.clear();
  }

  switch (currentScreen) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(isnan(temperature) ? 0 : temperature);
      lcd.print((char)223);
      lcd.print("C  ");

      lcd.setCursor(0, 1);
      lcd.print("H:");
      lcd.print(isnan(humidity) ? 0 : humidity);
      lcd.print("%");
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Smoke:");
      lcd.print(smokePercent);
      lcd.print("%");

      lcd.setCursor(0, 1);
      lcd.print("LPG:");
      lcd.print(lpgPercent);
      lcd.print("%");
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Fire:");
      lcd.print(fireProbability);
      lcd.print("%");

      lcd.setCursor(0, 1);
      lcd.print(fireState);
      break;
  }

  delay(200);
}
