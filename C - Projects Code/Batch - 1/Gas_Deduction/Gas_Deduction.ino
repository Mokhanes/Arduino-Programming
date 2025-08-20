#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo myServo;

#define MQ6_PIN A0 // Analog pin for MQ-6 sensor
#define BUZZER 2
#define LED 3
#define RELAY_PIN 4 // Digital pin for relay control
#define RESET 5


int res = 0;

// Initialize the LCD (set the address 0x27; adjust if your LCD has a different address)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  myServo.attach(9);
  pinMode(MQ6_PIN, INPUT);      // Set MQ-6 sensor pin as input
  pinMode(RESET, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);   // Set relay pin as output
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Start with relay off
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED, LOW);
  
  lcd.init();                   // Initialize the LCD
  lcd.backlight();              // Turn on the LCD backlight
  
  Serial.begin(9600);           // Start serial communication
  lcd.setCursor(0, 0);
  lcd.print("MQ-6 Gas Sensor");
  delay(2000);                  // Allow sensor to stabilize
  lcd.clear();
}

void loop() {
  int sensorValue = analogRead(MQ6_PIN); // Read the analog output value
  int gasLevel = map(sensorValue, 0, 1023, 0, 100); // Map the sensor value to a range of 0-100%

  if (digitalRead(RESET) == LOW){
    res = 0;
  }
  
  if (gasLevel > 50 || res >= 2) {
    lcd.setCursor(0, 0);
    lcd.print("Gas Level: High ");
    lcd.setCursor(0, 1);
    lcd.print("Gas Conc: ");
    lcd.print(gasLevel);
    lcd.print("%   ");
    myServo.write(80);
    if (digitalRead(RELAY_PIN)){
      lcd.setCursor(0, 0);
      lcd.print("Exhaust Fan: ON ");
      res++;
    }
    myServo.write(180);
    digitalWrite(RELAY_PIN, LOW); // Turn on the relay
    digitalWrite(BUZZER, HIGH);
    digitalWrite(LED, HIGH);
    delay(1000);
   
  } else if(res<2){
    lcd.setCursor(0, 0);
    lcd.print("Gas Level:Normal");
    lcd.setCursor(0, 1);
    lcd.print("Gas Conc: ");
    lcd.print(gasLevel);
    lcd.print("%   ");
    if(!digitalRead(RELAY_PIN)){
      lcd.setCursor(0, 0);
      lcd.print("Exhaust Fan: OFF ");
    }
    myServo.write(40);
    digitalWrite(RELAY_PIN, HIGH); // Turn off the relay
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED, LOW);
    delay(1000);
  }

  // Debugging via Serial Monitor
  Serial.print("Gas Level: ");
  Serial.print(gasLevel);
  Serial.println("%");

  delay(100); // Update every second
}
