#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR1 2  // IR Sensor 1 on D2
#define IR2 3  // IR Sensor 2 on D3

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD 16x2 with I2C address 0x27

unsigned long startTime = 0;
unsigned long endTime = 0;
float distance = 9.0;  // distance between sensors in cm

bool ir1Triggered = false;
bool ir2Triggered = false;
float lastSpeed = 0.0;

void setup() {
  pinMode(IR1, INPUT_PULLUP);
  pinMode(IR2, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Human Speed:");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");

  Serial.begin(9600);
}

void loop() {
  int ir1 = digitalRead(IR1);
  int ir2 = digitalRead(IR2);

  // First IR sensor triggered
  if (ir1 == LOW && !ir1Triggered) {
    startTime = millis();
    ir1Triggered = true;

    Serial.println("IR1 triggered");
  }

  // Second IR sensor triggered
  if (ir2 == LOW && ir1Triggered && !ir2Triggered) {
    endTime = millis();
    ir2Triggered = true;

    unsigned long timeTaken = endTime - startTime;  // milliseconds
    float timeInSec = timeTaken / 1000.0;           // convert to seconds

    if (timeInSec > 0) {
      lastSpeed = distance / timeInSec;             // cm/s
    } else {
      lastSpeed = 0.0;
    }

    Serial.print("Speed: ");
    Serial.print(lastSpeed);
    Serial.println(" cm/s");

    updateLCD();
    delay(2000);  // hold result before resetting
    resetFlags();
  }
}

// Display the last detected speed
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Human Speed:");
  lcd.setCursor(0, 1);
  lcd.print(lastSpeed, 2);
  lcd.print(" cm/s");
}

// Reset flags for next measurement
void resetFlags() {
  ir1Triggered = false;
  ir2Triggered = false;

  lcd.setCursor(0, 0);
  lcd.print("Human Speed:");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
}
