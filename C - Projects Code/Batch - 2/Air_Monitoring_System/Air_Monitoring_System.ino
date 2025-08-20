#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* ssid = "OPPO A7";
const char* pass = "12344321";

// Blynk
#define BLYNK_TEMPLATE_NAME "Air Monitoring System"
#define BLYNK_AUTH_TOKEN "o9EfkoFg2U7pWPaEfoIO3esBhMym9caL"
char auth[] = BLYNK_AUTH_TOKEN;

#define BLYNK_TEMPLATE_ID "TMPL3fqScBIYq"


const char* var1 = "Carbon Concentration";
int svm = 0;

BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
#define SENSOR1_VCC D5
#define SENSOR2_VCC D3
#define ANALOG_PIN A0
#define RELAY_PIN D6

// SMS API
const char* apiKey = "1qULXu63RpKZ";
const char* templateID = "101";
const char* mobileNumber = "918072781993";
bool smsHighSent = false;
bool smsNormalSent = true;

void setup() {
  Serial.begin(115200);

  pinMode(SENSOR1_VCC, OUTPUT);
  pinMode(SENSOR2_VCC, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(SENSOR1_VCC, LOW);
  digitalWrite(SENSOR2_VCC, LOW);
  digitalWrite(RELAY_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Gas Monitor");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  delay(1500);

  digitalWrite(LED_BUILTIN, LOW);
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(2000L, monitorGas);
}

int readGasSensor(int vccPin) {
  digitalWrite(vccPin, HIGH);
  delay(2000);
  int value = analogRead(ANALOG_PIN);
  digitalWrite(vccPin, LOW);
  return value;
}

void monitorGas() {
  int gas1 = readGasSensor(SENSOR1_VCC);
  int gas2 = readGasSensor(SENSOR2_VCC);

  int gas1Percent = map(gas1, 0, 1023, 0, 100);
  int gas2Percent = map(gas2, 0, 1023, 0, 100);

  Serial.print("Sensor 1: ");
  Serial.print(gas1Percent);
  Serial.print("% | Sensor 2: ");
  Serial.println(gas2Percent);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1:");
  lcd.print(gas1Percent);
  lcd.print("% S2:");
  lcd.print(gas2Percent);
  lcd.print("%");

  lcd.setCursor(0, 1);
  if (gas1Percent > 60) {
    lcd.print("Con Level: High ");
    lcd.setCursor(0, 1);
    lcd.print("Fan: ON ");
    digitalWrite(RELAY_PIN, HIGH);
    svm = gas1Percent;

    if (!smsHighSent) {
      sendSMS(String(gas1Percent), "High");
      smsHighSent = true;
      smsNormalSent = false;
    }
  } else {
    lcd.print("Con Level:Normal");
    lcd.setCursor(0, 1);
    lcd.print("Fan:OFF ");
    digitalWrite(RELAY_PIN, LOW);

    if (!smsNormalSent) {
      sendSMS(String(gas1Percent), "Normal");
      smsNormalSent = true;
      smsHighSent = false;
    }
  }

  Blynk.virtualWrite(V0, gas1Percent);
  Blynk.virtualWrite(V1, gas2Percent);
}

void sendSMS(String value, String level) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    String levelText = (level == "High") ? "High" : "Normal";
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + String(var1) + "\",\"var2\":\"" + levelText + " (" + value + "%)\"}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode == 200) {
      Serial.println("SMS sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("SMS failed, code: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getString());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send SMS.");
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
