#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Motor control pins
const int motorIn1 = 8;
const int motorEnable = 9;

// User buttons
const int incBtn = 6;
const int decBtn = 7;

// RF zone inputs (Active LOW)
const int rf40 = 2;
const int rf60 = 4;
const int rf80 = 5;
const int rf100 = 3;

// Buzzer & LED
const int buzzer = 11;
const int redLED = 12;

// Speed control
int speedPWM = 0; // 0–255
const int step = 10;
int currentLimit = 0; // 0 if no limit zone

// Debounce
unsigned long lastDebounce = 0;
const unsigned long debounceDelay = 200;

void setup() {
  // Pin modes
  pinMode(motorIn1, OUTPUT);
  pinMode(motorEnable, OUTPUT);
  pinMode(incBtn, INPUT_PULLUP);
  pinMode(decBtn, INPUT_PULLUP);
  pinMode(rf40, INPUT_PULLUP);
  pinMode(rf60, INPUT_PULLUP);
  pinMode(rf80, INPUT_PULLUP);
  pinMode(rf100, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(redLED, OUTPUT);

  digitalWrite(motorIn1, HIGH); // set direction

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Motor Speed Sys ");
  delay(1000);
  lcd.clear();
}

void loop() {
  readRFZone();
  handleButtons();
  applySpeedLimit();
  analogWrite(motorEnable, speedPWM);
  updateLCD();
}

// Read zone limit inputs
void readRFZone() {
  if (digitalRead(rf40) == HIGH) currentLimit = 40;
  else if (digitalRead(rf60) == HIGH) currentLimit = 60;
  else if (digitalRead(rf80) == HIGH) currentLimit = 80;
  else if (digitalRead(rf100) == HIGH) currentLimit = 100;
  else currentLimit = 0;
}

// Button-based manual speed control
void handleButtons() {
  if (millis() - lastDebounce > debounceDelay) {
    if (digitalRead(incBtn) == LOW) {
      speedPWM += step;
      if (speedPWM > 255) speedPWM = 255;
      lastDebounce = millis();
    }
    if (digitalRead(decBtn) == LOW) {
      speedPWM -= step;
      if (speedPWM < 0) speedPWM = 0;
      lastDebounce = millis();
    }
  }
}

// Limit and warning logic
void applySpeedLimit() {
  int currentSpeed = map(speedPWM, 0, 255, 0, 160);

  if (currentLimit > 0 && currentSpeed > currentLimit) {
    // Danger level
    int diff = currentSpeed - currentLimit;
    int blinkDelay = constrain(300 - (diff * 15), 50, 300);

    digitalWrite(buzzer, HIGH);
    digitalWrite(redLED, HIGH);
    delay(blinkDelay);
    digitalWrite(buzzer, LOW);
    digitalWrite(redLED, LOW);
    delay(blinkDelay);

    // Auto reduce speed
    speedPWM -= 5;
    if (speedPWM < 0) speedPWM = 0;
  } else {
    // Safe zone
    digitalWrite(buzzer, LOW);
    digitalWrite(redLED, LOW);
  }
}

// Display current & limit speed
void updateLCD() {
  int kmSpeed = map(speedPWM, 0, 255, 0, 160);

  lcd.setCursor(0, 0);
  lcd.print("Speed: ");
  lcd.print(kmSpeed);
  lcd.print(" km   ");

  lcd.setCursor(0, 1);
  lcd.print("Limit: ");
  if (currentLimit == 0)
    lcd.print("None     ");
  else {
    lcd.print(currentLimit);
    lcd.print(" km     ");
  }
}
