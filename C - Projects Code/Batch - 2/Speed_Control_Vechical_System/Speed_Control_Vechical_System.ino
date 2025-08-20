#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR_SENSOR_PIN 2
#define RELAY_PIN 3
#define WHEEL_DIAMETER_CM 20.0
#define PULSES_PER_REV 5
#define AVERAGE_COUNT 5

LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int pulseCount = 0;
volatile unsigned long lastPulseTime = 0;

unsigned long lastUpdateTime = 0;
float rpmBuffer[AVERAGE_COUNT] = {0};
int bufferIndex = 0;
float rpm = 0;
float speedKmph = 0;

bool relayActivated = false;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), countPulse, FALLING);

  lcd.setCursor(0, 0);
  lcd.print("RPM:     Speed:");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdateTime >= 1000 && !relayActivated) {
    detachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN));

    float revsPerSec = pulseCount / float(PULSES_PER_REV);
    float currentRpm = revsPerSec * 60.0;
    pulseCount = 0;
    lastUpdateTime = currentMillis;

    rpmBuffer[bufferIndex] = currentRpm;
    bufferIndex = (bufferIndex + 1) % AVERAGE_COUNT;

    float rpmSum = 0;
    for (int i = 0; i < AVERAGE_COUNT; i++) {
      rpmSum += rpmBuffer[i];
    }
    rpm = rpmSum / AVERAGE_COUNT;

    float wheelCircumference = 3.1416 * WHEEL_DIAMETER_CM;
    speedKmph = (rpm * wheelCircumference * 60.0) / 100000.0;

    if (speedKmph > 5.0) {
      digitalWrite(RELAY_PIN, LOW);
      relayActivated = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("The Vehicle");
      lcd.setCursor(0, 1);
      lcd.print("Stopped..!");
    } else {
      // Show RPM and speed
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.setCursor(0, 1);
      lcd.print(int(rpm));

      lcd.setCursor(9, 1);
      lcd.print("      ");
      lcd.setCursor(9, 1);
      lcd.print(speedKmph, 1);
      lcd.print("km");
    }

    attachInterrupt(digitalPinToInterrupt(IR_SENSOR_PIN), countPulse, FALLING);
  }
}

void countPulse() {
  unsigned long currentTime = micros();
  if (currentTime - lastPulseTime > 5000) {
    pulseCount++;
    lastPulseTime = currentTime;
  }
}
