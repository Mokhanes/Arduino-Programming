#define BLYNK_TEMPLATE_ID "TMPL3U3dq8izS"
#define BLYNK_TEMPLATE_NAME "Fire Alert System"
#define BLYNK_AUTH_TOKEN "tPXSEM20V9OTKSEXkUx9Ak_v_vJHUScr"

#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

/* ------------ PIN CONFIG ------------ */
#define DHTPIN        4
#define DHTTYPE       DHT11
#define MQ2_PIN       34
#define MQ6_PIN       35
#define FLAME1_PIN    27
#define FLAME2_PIN    32
#define FLAME3_PIN    33

/* ------------ WiFi Credentials ------------ */
const char* ssid = "Admin";
const char* password = "12344321";

/* ------------ Blynk Setup ------------ */
char auth[] = BLYNK_AUTH_TOKEN;

/* ------------ THRESHOLDS ------------ */
const int MQ2_THRESHOLD = 350;
const int MQ6_THRESHOLD = 400;
const float TEMP_THRESHOLD = 50.0;

/* ------------ LCD & DHT ------------ */
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

/* ------------ Variables ------------ */
unsigned long lastScreenChange = 0;
const unsigned long SCREEN_INTERVAL = 2000;
int currentScreen = 0;

float fireProbability = 0;
String fireState = "Normal";

void setup() {
  Serial.begin(115200);
  delay(500);

  // ------- User's WiFi Connectivity Code (UNCHANGED) -------
  WiFi.disconnect(true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // ---------------------------------------------------------

  // Blynk Manual Connection (after WiFi is ready)
  Blynk.config(auth);
  Blynk.connect();

  // LCD Init
  Wire.begin();
  lcd.init();
  lcd.backlight();
  dht.begin();

  // Flame sensors
  pinMode(FLAME1_PIN, INPUT);
  pinMode(FLAME2_PIN, INPUT);
  pinMode(FLAME3_PIN, INPUT);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" SMART FIRE SYS ");
  lcd.setCursor(0, 1);
  lcd.print("Init...");
  delay(1500);
}

void loop() {
  Blynk.run();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  int mq2Value = analogRead(MQ2_PIN);
  int mq6Value = analogRead(MQ6_PIN);

  int flame1 = digitalRead(FLAME1_PIN);
  int flame2 = digitalRead(FLAME2_PIN);
  int flame3 = digitalRead(FLAME3_PIN);

  bool flameDetected = (flame1 || flame2 || flame3);

  // --- Fire Probability Calculation ---
  fireProbability = 0;
  if (temperature > TEMP_THRESHOLD) fireProbability += 30;
  if (mq2Value > MQ2_THRESHOLD) fireProbability += 25;
  if (mq6Value > MQ6_THRESHOLD) fireProbability += 25;
  if (flameDetected) fireProbability = 100;

  if (flameDetected) fireState = "Danger";
  else if (fireProbability >= 80) fireState = "Warning";
  else fireState = "Normal";

  if (fireProbability > 100) fireProbability = 100;

  // --- Blynk Update ---
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, map(mq6Value, 0, 4095, 0, 100));
  Blynk.virtualWrite(V3, map(mq2Value, 0, 4095, 0, 100));
  Blynk.virtualWrite(V4, fireState + " (" + String((int)fireProbability) + "%)");
  Blynk.virtualWrite(V5, (int)fireProbability);

  // --- LCD Screen Toggle ---
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
      lcd.print((char)223); lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("H:");
      lcd.print(isnan(humidity) ? 0 : humidity);
      lcd.print("%");

      lcd.setCursor(10, 0);
      lcd.print(fireState.substring(0, 3));
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("MQ2:");
      lcd.print(mq2Value);

      lcd.setCursor(0, 1);
      lcd.print("MQ6:");
      lcd.print(mq6Value);
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Flame:");
      lcd.print(flameDetected ? "YES" : "NO");

      lcd.setCursor(0, 1);
      lcd.print(fireState);
      lcd.setCursor(10, 1);
      lcd.print((int)fireProbability);
      lcd.print("%");
      break;
  }
}
