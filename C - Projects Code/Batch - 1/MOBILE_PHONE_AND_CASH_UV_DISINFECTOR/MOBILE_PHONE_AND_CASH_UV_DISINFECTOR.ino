#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin definitions
#define BUZZER 2
#define RELAY 3
#define DECREMENT 4
#define INCREMENT 5
#define START 6

// Variables
int setTime = 0;   // Time in seconds
int countdownTime = 0;
int bz=0;
bool started = false;

void setup() {
  Serial.begin(9600);
  
  // Initialize pin modes
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(DECREMENT, INPUT_PULLUP);
  pinMode(INCREMENT, INPUT_PULLUP);
  pinMode(START, INPUT_PULLUP);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Initial state
  digitalWrite(RELAY, LOW);  // UV light OFF
  digitalWrite(BUZZER, LOW); // Buzzer OFF
  
  // Display the initial set time
  lcd.setCursor(0, 0);
  lcd.print("Set Time: ");
  lcd.setCursor(10, 0);
  lcd.print(setTime);
  lcd.setCursor(12, 0);
  lcd.print("s");
}

void loop() {
  // Increment time if increment button is pressed
  
  if (digitalRead(INCREMENT) == LOW) {
    setTime++;
    lcd.setCursor(10, 0);
    lcd.print("    "); // Clear old time
    lcd.setCursor(10, 0);
    lcd.print(setTime);
    lcd.print("s");
    delay(300); // Debouncing delay
  }

  // Decrement time if decrement button is pressed
  if (digitalRead(DECREMENT) == LOW && setTime > 0) {
    setTime--;
    lcd.setCursor(10, 0);
    lcd.print("    "); // Clear old time
    lcd.setCursor(10, 0);
    lcd.print(setTime);
    lcd.print("s");
    delay(300); // Debouncing delay
  }

  // Start countdown if start button is pressed
  if (digitalRead(START) == LOW && !started) {
    bz=0;
    countdownTime = 0;
    started = true;
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Running: ");
    delay(300); // Debouncing delay
    
    // Turn ON relay (start disinfection)
    digitalWrite(RELAY, HIGH);
    
    // Countdown loop
    while (countdownTime <= setTime) {
      lcd.setCursor(10, 1);
      lcd.print(countdownTime);
      lcd.print("s  "); // Space for clearing old value
      delay(1000);  // 1-second delay
      countdownTime++;
      if(digitalRead(START) == LOW){
        bz++;
        countdownTime = setTime;
      }
    }
    
    // Turn OFF relay when countdown reaches zero
    digitalWrite(RELAY, LOW);
    
    // Buzzer ON for 3 seconds
    if(bz==0){
      digitalWrite(BUZZER, HIGH);
      delay(3000);
      digitalWrite(BUZZER, LOW);
    }
    
    
    // Reset display
    lcd.setCursor(0, 1);
    lcd.print("Process Completed");  // Clear the second line
    started = false; // Reset started flag
  }
}
