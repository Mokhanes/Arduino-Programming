#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int limitSwitchPin = 2;
const int mq2Pin = 3;
const int relay1Pin = 6;
const int relay2Pin = 7;
const int buzzerPin = 8;

// Variables
bool helmetUsedOnce = false;
bool countdownActive = false;
bool rideDenied = false;
int countdown = 10;
unsigned long previousMillis = 0;

// Function to display two-line message
void showMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(limitSwitchPin, INPUT);
  pinMode(mq2Pin, INPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(relay1Pin, HIGH);  // Relays OFF
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(buzzerPin, LOW);   // Buzzer OFF

  showMessage("Smart Helmet ", "");
  delay(2000);
  lcd.clear();
}

void loop() {
  bool helmetOn = digitalRead(limitSwitchPin) == LOW;
  bool alcoholDetected = digitalRead(mq2Pin) == LOW;

  // --- Alcohol Detection ---
  if (alcoholDetected) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(200);
      digitalWrite(buzzerPin, LOW);
      delay(200);
    }

    digitalWrite(relay1Pin, HIGH);
    digitalWrite(relay2Pin, HIGH);
    digitalWrite(buzzerPin, LOW);
    countdownActive = false;
    countdown = 10;
    rideDenied = false;

    showMessage("Alcohol Content", "Detected..!");
    delay(1000);
    return;
  }

  // --- Helmet is ON ---
  if (helmetOn) {
    helmetUsedOnce = true;
    rideDenied = false;
    digitalWrite(relay1Pin, LOW);   // Relays ON
    digitalWrite(relay2Pin, LOW);
    digitalWrite(buzzerPin, LOW);   // Buzzer OFF

    if (countdownActive) {
      countdownActive = false;
      countdown = 10;
    }

    showMessage("Happy Ride..!", "");
    delay(1000);
    return;
  }

  // --- Helmet NOT worn ---
  if (!helmetUsedOnce) {
    showMessage("Wear the Helmet", "to Start Ride..!");
    delay(1000);
    return;
  }

  // After ride denied, just display and wait
  if (rideDenied) {
    digitalWrite(buzzerPin, LOW);
    showMessage("Wear the Helmet", "to Start Ride..!");
    delay(1000);
    return;
  }

  // Helmet removed during ride — start countdown
  if (!countdownActive) {
    countdownActive = true;
    previousMillis = millis();
    showMessage("Helmet Removed!", "");
    delay(500);
  }

  // Countdown logic
  if (countdownActive && millis() - previousMillis >= 1000) {
    previousMillis = millis();

    if (countdown > 0) {
      showMessage("Wear the Helmet", "Time Left: " + String(countdown));
      digitalWrite(buzzerPin, HIGH);
      countdown--;
    } else {
      // Final buzzer peep x2
      digitalWrite(buzzerPin, LOW);
      delay(200);
      for (int i = 0; i < 2; i++) {
        digitalWrite(buzzerPin, HIGH);
        delay(200);
        digitalWrite(buzzerPin, LOW);
        delay(200);
      }

      // Stop Ride
      digitalWrite(relay1Pin, HIGH);
      digitalWrite(relay2Pin, HIGH);
      digitalWrite(buzzerPin, LOW);

      countdownActive = false;
      rideDenied = true;
      countdown = 10;

      showMessage("Ride Denied!", "Wear Helmet..!");
      delay(2000);
    }
  }
}
