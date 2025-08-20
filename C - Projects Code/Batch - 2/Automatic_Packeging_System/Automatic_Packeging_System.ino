#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
const int IR_SENSOR1 = 2;
const int IR_SENSOR2 = 3;
const int MOTOR_IN1 = 8;
const int MOTOR_EN = 9;
const int POT_PIN = A1;
const int BUZZER_PIN = 4;

// Variables
int objectCount = 0;
int boxCount = 0;
bool boxFilled = false;
bool motorRunning = true;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
bool lastState1 = HIGH;
bool lastState2 = HIGH;

void setup() {
  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(" Automated Obj ");
  lcd.setCursor(0, 1);
  lcd.print("Counting System");
  delay(3000);
  lcd.clear();

  // Pin modes
  pinMode(IR_SENSOR1, INPUT);
  pinMode(IR_SENSOR2, INPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_EN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start motor
  digitalWrite(MOTOR_IN1, HIGH);
}

void loop() {
  // Read potentiometer for speed control
  int potValue = analogRead(POT_PIN);
  int motorSpeed = map(potValue, 0, 1023, 0, 255);
  analogWrite(MOTOR_EN, motorRunning ? motorSpeed : 0);

  // IR Sensor 1: Object detection
  bool currentState1 = digitalRead(IR_SENSOR1) == LOW;
  if (currentState1 && !lastState1 && (millis() - lastDebounceTime1 > 300)) {
    objectCount++;
    shortBeep();  // 🔔 Short beep for each object
    lastDebounceTime1 = millis();
  }
  lastState1 = currentState1;

  // Stop motor when box is filled
  if (objectCount >= 10 && !boxFilled) {
    motorRunning = false;
    longBeep();   // 🔔 Long beep when box is filled
    boxFilled = true;
  }

  // IR Sensor 2: Box moved out
  bool currentState2 = digitalRead(IR_SENSOR2) == LOW;
  if (boxFilled && currentState2 && !lastState2 && (millis() - lastDebounceTime2 > 300)) {
    longBeep();
    objectCount = 0;
    boxCount++;
    boxFilled = false;
    motorRunning = true;
    lastDebounceTime2 = millis();
  }
  lastState2 = currentState2;

  // LCD display
  lcd.setCursor(0, 0);
  lcd.print("Objects: ");
  lcd.print(objectCount);
  lcd.print("   ");

  lcd.setCursor(0, 1);
  lcd.print("Boxes: ");
  lcd.print(boxCount);
  lcd.print("        ");
}

// 🔊 Short beep function
void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);  // short beep duration
  digitalWrite(BUZZER_PIN, LOW);
}

// 🔊 Long beep function
void longBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(1000);  // long beep duration
  digitalWrite(BUZZER_PIN, LOW);
}
