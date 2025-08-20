#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

#define TRIG_PIN 2
#define ECHO_PIN 3
#define RELAY_PIN 4
#define BUTTON_PIN 5

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;

float tankHeight = 0; // Measured once at startup
bool motorRunning = false;
unsigned long motorStartTime = 0;
bool autoStartedToday = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  rtc.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(RELAY_PIN, HIGH); // Relay OFF (inverted logic)

  delay(2000); // Sensor settle

  // Measure tank height assuming empty at boot
  tankHeight = getUltrasonicDistance();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tank Calibrated");
  delay(1000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  float currentDistance = getUltrasonicDistance();
  float waterHeight = tankHeight - currentDistance;
  float waterPercent = (waterHeight / tankHeight) * 100.0;
  waterPercent = constrain(waterPercent, 0, 100);

  // Display Time or Countdown
  lcd.setCursor(0, 0);
  if (motorRunning) {
    unsigned long timeLeft = 60 - (millis() - motorStartTime) / 1000;
    lcd.print("Running: ");
    lcd.print(timeLeft);
    lcd.print("s   ");
  } else {
    lcd.print("Time: ");
    print2Digit(now.hour());
    lcd.print(":");
    print2Digit(now.minute());
    lcd.print(":");
    print2Digit(now.second());
  }

  // Display Water Level
  lcd.setCursor(0, 1);
  lcd.print("Level: ");
  lcd.print((int)waterPercent);
  lcd.print("%    ");

  // Motor Control Logic
  if (motorRunning) {
    if ((millis() - motorStartTime) > 60000 || waterPercent < 30) {
      digitalWrite(RELAY_PIN, HIGH); // Turn OFF motor
      motorRunning = false;

      if (waterPercent < 30) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Water Level Low");
        delay(2000);
        lcd.clear();
      }
    }
  }

  // Push button demo trigger
  if (!motorRunning && digitalRead(BUTTON_PIN) == LOW && waterPercent > 30) {
    startMotor();
  }

  // Auto-start at 6:00 AM
  if (now.hour() == 6 && now.minute() == 0 && !autoStartedToday && waterPercent > 30) {
    startMotor();
    autoStartedToday = true;
  }

  // Reset daily flag after 6:01
  if (now.hour() == 6 && now.minute() > 1) {
    autoStartedToday = false;
  }

  delay(500);
}

void startMotor() {
  motorRunning = true;
  motorStartTime = millis();
  digitalWrite(RELAY_PIN, LOW); // Turn ON motor (inverted logic)
}

float getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout = 30ms
  float distance = duration * 0.034 / 2.0;
  return distance;
}

void print2Digit(int number) {
  if (number < 10) lcd.print("0");
  lcd.print(number);
}
