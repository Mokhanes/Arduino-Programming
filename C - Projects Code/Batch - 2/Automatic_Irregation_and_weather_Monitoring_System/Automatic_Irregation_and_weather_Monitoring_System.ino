#define BLYNK_TEMPLATE_ID "TMPL3ANm4on8I"
#define BLYNK_TEMPLATE_NAME "Irrigation System"
#define BLYNK_AUTH_TOKEN "sJGSbNvKpmBvLHVl8sSwYvSPtRhr4-yn"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// WiFi Credentials
char ssid[] = "OPPO A7";
char pass[] = "12344321";

// Pin Definitions
#define DHTPIN D3        // GPIO0
#define RELAY_PIN D6     // GPIO14
#define SOIL_PIN A0
#define LED_BUILTIN_PIN D4 // GPIO2

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C LCD Address

// Blynk Virtual Pins
#define V_TEMP V0
#define V_HUMIDITY V1
#define V_SOIL V2
#define V_MOTOR V3

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_BUILTIN_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Motor OFF
  digitalWrite(LED_BUILTIN_PIN, HIGH); // LED OFF (not connected yet)

  dht.begin();
  lcd.init();
  lcd.backlight();

  // Attempt to connect to Blynk
 // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi");
  
  // Once connected to Blynk
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN_PIN, LOW);  // LED ON (blinking)
    delay(250);
    digitalWrite(LED_BUILTIN_PIN, HIGH); // LED OFF
    delay(250);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  digitalWrite(LED_BUILTIN_PIN, LOW); // LED ON (solid when connected)

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
 // timer.setInterval(1000L, weather); // 1 second interval
}

void loop() {
  Blynk.run();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilValue = analogRead(SOIL_PIN); // 0-1023
  int soilPercent = map(soilValue, 1023, 0, 0, 100); // Dry = 0%, Wet = 100%

  bool isDry = soilPercent < 40;

  // Motor Control
  digitalWrite(RELAY_PIN, isDry ? HIGH : LOW);
  String motorStatus = isDry ? "ON" : "OFF";

  // LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C  H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(soilPercent);
  lcd.print("%   ");
  lcd.setCursor(9, 1);
  lcd.print("M:");
  lcd.print(motorStatus);

  // Blynk Send
  Blynk.virtualWrite(V_TEMP, temperature);
  Blynk.virtualWrite(V_HUMIDITY, humidity);
  Blynk.virtualWrite(V_SOIL, soilPercent);
  Blynk.virtualWrite(V_MOTOR, motorStatus);

  delay(500);
}
