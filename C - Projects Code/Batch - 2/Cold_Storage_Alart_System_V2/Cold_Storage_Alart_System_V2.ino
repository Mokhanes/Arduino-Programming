#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// IR sensor pins
const int sensorA = 2;
const int sensorB = 3;
const int relay = 4;
const int buzzer = 5;
const int rst = 6;

const boolean sences = HIGH;
int count = 0;

unsigned long lastTriggerTime = 0;
const unsigned long debounceDelay = 200; // Debounce time
bool personDetected = false;

void setup() {
  pinMode(sensorA, INPUT);
  pinMode(sensorB, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(rst, INPUT_PULLUP);

  digitalWrite(relay, HIGH);
  digitalWrite(buzzer, LOW);

  lcd.init();
  lcd.backlight();
  updateLCD();

  Serial.begin(9600);
}

void loop() {
  if (digitalRead(rst) == LOW) {
    count = 0;
    updateLCD();
    delay(300); // debounce for reset button
  }

  detectMovement();
}

void detectMovement() {
  static int state = 0;
  static unsigned long timer = 0;

  bool a = digitalRead(sensorA) == sences;
  bool b = digitalRead(sensorB) == sences;

  switch (state) {
    case 0:
      if (a) {
        state = 1;
        timer = millis();
      } else if (b) {
        state = 4;
        timer = millis();
      }
      break;

    case 1:
      if (b && millis() - timer < 1000) {
        state = 2;
      } else if (millis() - timer > 1000) {
        state = 0;
      }
      break;

    case 2: // Entry detected
      count++;
      buzzerBeep(1);
      Serial.println("Entry Detected");
      updateLCD();
      waitForSensorsToReset();
      state = 0;
      break;

    case 4:
      if (a && millis() - timer < 1000) {
        state = 5;
      } else if (millis() - timer > 1000) {
        state = 0;
      }
      break;

    case 5: // Exit detected
      if (count > 0) {
        count--;
        buzzerBeep(2);
      }
      Serial.println("Exit Detected");
      updateLCD();
      waitForSensorsToReset();
      state = 0;
      break;
  }
}

void waitForSensorsToReset() {
  while (digitalRead(sensorA) == sences || digitalRead(sensorB) == sences);
  delay(100);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("People inside:");
  lcd.print(count);
  lcd.setCursor(0, 1);
  if (count == 0) {
    lcd.print("You Can Close");
    digitalWrite(relay, HIGH);
  } else {
    lcd.print("Don't Close..!");
    digitalWrite(relay, LOW);
  }
}

void buzzerBeep(int type) {
  for (int i = 0; i < type; i++) {
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
}
