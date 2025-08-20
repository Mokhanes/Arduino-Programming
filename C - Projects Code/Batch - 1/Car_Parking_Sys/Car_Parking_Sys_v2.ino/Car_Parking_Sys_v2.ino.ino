#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the HEX address
#include <Servo.h> 

Servo myservo1;

// IR Sensors
int IR1 = 2;
int IR2 = 4;

// Push Buttons
int incrementButton = 8;
int decrementButton = 10;

int Slot = 4; // Total parking slots
int maxSlots = 10; // Max parking slots (modify as needed)

// Flags for IR Sensors
int flag1 = 0;
int flag2 = 0;

// Button Debounce Variables
unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 200; // 200 ms debounce delay

void setup() {
  Serial.begin(9600); // For debugging
  lcd.init();
  lcd.backlight();
  
  // IR Sensors as Input with Pull-up Resistors
  pinMode(IR1, INPUT_PULLUP);
  pinMode(IR2, INPUT_PULLUP);
  
  // Push Buttons as Input with Pull-up Resistors
  pinMode(incrementButton, INPUT_PULLUP);
  pinMode(decrementButton, INPUT_PULLUP);
  
  // Servo Motor
  myservo1.attach(6);
  myservo1.write(100); // Close gate initially
  
  // Welcome Message
  lcd.setCursor(0, 0);
  lcd.print(F("     ARDUINO    "));
  lcd.setCursor(0, 1);
  lcd.print(F(" PARKING SYSTEM "));
  delay(2000);
  lcd.clear();
}

void loop() {
  // IR Sensor Logic
  if (digitalRead(IR1) == LOW && flag1 == 0) {
    if (Slot > 0) {
      flag1 = 1;
      if (flag2 == 0) {
        myservo1.write(0); // Open gate
        delay(200);
        Slot = Slot - 1;
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print(F("    SORRY :(    "));
      lcd.setCursor(0, 1);
      lcd.print(F("  Parking Full  "));
      delay(3000);
      lcd.clear();
    }
  }

  if (digitalRead(IR2) == LOW && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      myservo1.write(0); // Open gate
      delay(200);
      Slot = Slot + 1;
    }
  }

  if (flag1 == 1 && flag2 == 1) {
    delay(1000);
    myservo1.write(100); // Close gate
    delay(200);
    flag1 = 0;
    flag2 = 0;
  }

  // Manual Slot Increment
  if (digitalRead(incrementButton) == LOW) {
    if (millis() - lastButtonPressTime > debounceDelay) {
      if (Slot < maxSlots) {
        Slot++;
        displaySlotMessage("Slot Incremented");
      } else {
        displaySlotMessage("Max Slots Reached");
      }
      lastButtonPressTime = millis(); // Update last button press time
    }
  }

  // Manual Slot Decrement
  if (digitalRead(decrementButton) == LOW) {
    if (millis() - lastButtonPressTime > debounceDelay) {
      if (Slot > 0) {
        Slot--;
        displaySlotMessage("Slot Decremented");
      } else {
        displaySlotMessage("No Slots Left");
      }
      lastButtonPressTime = millis(); // Update last button press time
    }
  }

  // Display Slot Information
  lcd.setCursor(0, 0);
  lcd.print(F("    WELCOME!    "));
  lcd.setCursor(0, 1);
  lcd.print(F("Slot Left: "));
  lcd.print(Slot);
}

// Function to Display Slot Increment/Decrement Messages
void displaySlotMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.print(F("Slots Left: "));
  lcd.print(Slot); // Show current slot count
  delay(2000); // Display message for 2 seconds
  lcd.clear();
}
