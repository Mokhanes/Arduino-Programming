#define BLYNK_TEMPLATE_ID "TMPL3GeR96eml"
#define BLYNK_DEVICE_NAME "Smart Car Parking System"
#define BLYNK_AUTH_TOKEN "nZXp6O887Kc4P1lAyCrFHw70pgZzEfCn"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
char ssid[] = "OPPO A7";
char pass[] = "12344321";

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // address 0x27, 16 chars, 2 lines

// IR sensor pins (using GPIO numbers)
const int slot1 = 0;   // GPIO 0 (D3)
const int slot2 = 13;  // GPIO 13 (D7)
const int slot3 = 14;  // GPIO 14 (D5)
const int slot4 = 12;  // GPIO 12 (D6)

const byte LED_WIFI = LED_BUILTIN;

void setup() {
  Serial.begin(9600);

  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, HIGH); // Start with LED OFF (active LOW)

  // LCD initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Car ");
  lcd.setCursor(0, 1);
  lcd.print("Parking System");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // Start WiFi connection
  WiFi.begin(ssid, pass);

  // Blink built-in LED until connected
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_WIFI, LOW);  // ON (active LOW)
    delay(300);
    digitalWrite(LED_WIFI, HIGH); // OFF
    delay(300);
    Serial.print(".");
  }

  // Connected to WiFi
  digitalWrite(LED_WIFI, LOW); // Turn ON solid
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connected ");
  delay(1000);
  lcd.clear();

  // Start Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Sensor pin setup
  pinMode(slot1, INPUT);
  pinMode(slot2, INPUT);
  pinMode(slot3, INPUT);
  pinMode(slot4, INPUT);
}

void loop() {
  Blynk.run();

  // Read sensors (LOW = occupied)
  bool s1 = digitalRead(slot1);
  bool s2 = digitalRead(slot2);
  bool s3 = digitalRead(slot3);
  bool s4 = digitalRead(slot4);

  // Send to Blynk (V0–V3)
  Blynk.virtualWrite(V0, !s1);
  Blynk.virtualWrite(V1, !s2);
  Blynk.virtualWrite(V2, !s3);
  Blynk.virtualWrite(V3, !s4);

  // Count available slots
  int available = s1 + s2 + s3 + s4;

  // Update LCD – first part (S1, S2)
  lcd.setCursor(0, 0);
  lcd.print("Avail: ");
  lcd.print(available);
  lcd.print("   ");  // Clear extra chars

  lcd.setCursor(0, 1);
  lcd.print("S1:");
  lcd.print(s1 ? "Free " : "Full ");
  lcd.print("S2:");
  lcd.print(s2 ? "Free" : "Full");

  delay(500);

  // Update LCD – second part (S3, S4)
  lcd.setCursor(0, 1);
  lcd.print("S3:");
  lcd.print(s3 ? "Free " : "Full ");
  lcd.print("S4:");
  lcd.print(s4 ? "Free" : "Full");

  delay(500);
}
