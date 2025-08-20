/* Weather monitoring system with NodeMCU
 * Optimized by ChatGPT
 */

#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// LCD and DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(D3, DHT11); // DHT pin, Type

// Blynk credentials
char auth[] = "n7o_UvkuUTHNd2bTh__5b2Qp8IH08a6G";
char ssid[] = "OPPO A7";
char pass[] = "12344321";

// Timer and LED Widget
BlynkTimer timer;
WidgetLED led1(V3); // Defined globally once

void weather() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int r = analogRead(A0);
  bool l = digitalRead(D5); // LDR status

  r = map(r, 0, 1023, 100, 0); // Invert value for rainfall display

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send to Blynk
  Blynk.virtualWrite(V0, t);  // Temperature
  Blynk.virtualWrite(V1, h);  // Humidity
  Blynk.virtualWrite(V2, r);  // Rainfall

  // LCD Display
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(t); lcd.print("C ");
  lcd.setCursor(9, 0);
  lcd.print("R:"); lcd.print(r); lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.print("H:"); lcd.print(h); lcd.print("% ");

  lcd.setCursor(9, 1);
  if (l == 0) {
    lcd.print("L:Light");
    led1.on();
  } else {
    lcd.print("L:Dark ");
    led1.off();
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  dht.begin();

  weather();
  pinMode(LED_BUILTIN, OUTPUT); // Setup built-in LED

  // Connect to WiFi and blink LED while connecting
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);  // LED ON (blinking)
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH); // LED OFF
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  digitalWrite(LED_BUILTIN, LOW); // LED ON (solid when connected)

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, weather); // 1 second interval
}

void loop() {
  Blynk.run();
  timer.run();
}
