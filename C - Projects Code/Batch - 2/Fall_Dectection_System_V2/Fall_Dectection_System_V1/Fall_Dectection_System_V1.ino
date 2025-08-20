/*****************************************************************
 *  ESP8266 (NodeMCU) – MPU‑6050 Fall Detector
 *  Sends SMS via CircuitDigest and drives LED + Buzzer steady
 *****************************************************************/
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

/* ================= USER SETTINGS ============================ */
const char* WIFI_SSID     = "OPPO A7";
const char* WIFI_PASS     = "12344321";

const char* API_KEY       = "boXSrW3fEOxe";
const char* TEMPLATE_ID   = "103";
const char* PHONE_NUMBER  = "917708114889";
const char* VAR1          = "Fall Sensor 1";
const char* VAR2          = "Grandfather's Hand";

/* Fall‑detection thresholds (*10 for g) */
const int  TRIG1_LOWER = 3;    // 0.3 g dip
const int  TRIG2_UPPER = 7;    // ≥1.7 g impact
const int  ORIENT_MIN  = 20;   // deg/s
const int  ORIENT_MAX  = 300;  // deg/s
/* ============================================================ */

/* --- GPIO map --- */
const byte LED_WIFI = LED_BUILTIN; // D4, active‑LOW
const byte LED_FALL = D5;          // alarm LED
const byte BUZZER   = D6;          // buzzer behaves like LED
/* ---------------- */

const byte MPU_ADDR = 0x68;

/* IMU raw variables */
int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;
float   ax,ay,az,gx,gy,gz;

/* FSM flags & counters */
bool t1=false,t2=false,t3=false;
bool smsPending=false;
byte c1=0,c2=0,c3=0;

/* timing helper for Wi‑Fi LED blink */
unsigned long lastBlink=0;
bool wifiBlinkState=false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_WIFI, OUTPUT);
  pinMode(LED_FALL, OUTPUT);
  pinMode(BUZZER,   OUTPUT);
  digitalWrite(LED_WIFI, HIGH);   // off (active‑LOW)
  digitalWrite(LED_FALL, LOW);    // off
  digitalWrite(BUZZER,   LOW);    // off

  /* Wake MPU‑6050 */
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); Wire.write(0); Wire.endTransmission();

  /* Wi‑Fi connect with blink */
  Serial.print("Wi‑Fi: ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    blinkWifiLED();
    delay(100);
  }
  digitalWrite(LED_WIFI, LOW);    // solid ON
  Serial.println(" connected");
}

void loop() {
  readMPU();
  int amp = 10 * sqrt(ax*ax + ay*ay + az*az);
  int ang =      sqrt(gx*gx + gy*gy + gz*gz);

  /* -------- 3‑stage fall detector -------- */
  if (amp <= TRIG1_LOWER && !t2) t1 = true;
  if (t1 && ++c1 >= 1) {
    if (amp >= TRIG2_UPPER) { t2=true; t1=false; c1=0; }
  }
  if (t2 && ++c2 >= 1) {
    if (ang >= ORIENT_MIN && ang <= ORIENT_MAX) { t3=true; t2=false; c2=0; }
  }
  if (t3 && ++c3 >= 10) {                 // 1 s settle
    ang = sqrt(gx*gx + gy*gy + gz*gz);
    if (ang <= 10) {                      // lying still
      Serial.println("FALL DETECTED");
      digitalWrite(LED_FALL, HIGH);       // LED ON
      digitalWrite(BUZZER,   HIGH);       // buzzer ON
      smsPending = true;                  // queue SMS
    }
    t3=false; c3=0;
  }
  if (c1 > 6) { t1=false; c1=0; }
  if (c2 > 6) { t2=false; c2=0; }

  /* ---------- send SMS once per fall ---------- */
  if (smsPending) {
    if (sendSMS()) Serial.println("SMS sent.");
    else           Serial.println("SMS failed.");
    digitalWrite(LED_FALL, LOW);          // LED OFF
    digitalWrite(BUZZER,   LOW);          // buzzer OFF
    smsPending = false;
  }

  delay(100);   // loop @10 Hz
}

/* ============ helper functions ============ */
void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
          Wire.read(); Wire.read();     // skip temp
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  ax = (AcX - 2050) / 16384.0;
  ay = (AcY -   77) / 16384.0;
  az = (AcZ - 1947) / 16384.0;
  gx = (GyX +  270) / 131.07;
  gy = (GyY -  351) / 131.07;
  gz = (GyZ +  136) / 131.07;
}

bool sendSMS() {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client; client.setInsecure();  // dev only
  HTTPClient http;
  String url = "https://www.circuitdigest.cloud/send_sms?ID=" + String(TEMPLATE_ID);
  if (!http.begin(client, url)) return false;

  http.addHeader("Authorization", API_KEY);
  http.addHeader("Content-Type", "application/json");
  String body = "{\"mobiles\":\"" + String(PHONE_NUMBER) +
                "\",\"var1\":\""   + String(VAR1) +
                "\",\"var2\":\""   + String(VAR2) + "\"}";
  int code = http.POST(body);
  Serial.printf("HTTP %d\n", code);
  Serial.println(http.getString());
  http.end();
  return code == 200;
}

/* blink on‑board LED @2 Hz while joining Wi‑Fi */
void blinkWifiLED() {
  unsigned long now = millis();
  if (now - lastBlink >= 250) {
    wifiBlinkState = !wifiBlinkState;
    digitalWrite(LED_WIFI, wifiBlinkState ? LOW : HIGH); // active‑LOW
    lastBlink = now;
  }
}
