#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C address: change 0x27 to your LCD address if different
LiquidCrystal_I2C lcd(0x27, 16, 2);

// IR sensor pins
const int sensorA = 2;  // IR Sensor A
const int sensorB = 3;  // IR Sensor B
const int relay = 4;
const int buzzer = 5;
const int rst = 6;

boolean sences = HIGH;

int count = 0; // People inside

void setup() {
  pinMode(sensorA, INPUT);
  pinMode(sensorB, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(rst, INPUT_PULLUP);

  digitalWrite(relay, HIGH);
  digitalWrite(buzzer, LOW);

  lcd.init();          // Initialize LCD
  lcd.backlight();     // Turn on backlight

  lcd.setCursor(0, 0);
  lcd.print("People inside:");
  lcd.print(count);
  lcd.setCursor(0, 1);
  lcd.print("You Can Close");

  Serial.begin(9600);
}

void loop() {
  Serial.println(digitalRead(sensorA));
  Serial.println(digitalRead(sensorB));
  Serial.println(digitalRead(rst));
  if(digitalRead(rst)== LOW){
    count=0;
    updateLCD();
  }
  
  // Check for Entry (A then B)
  if (digitalRead(sensorA) == sences) {
    delay(300); // debounce
    if (digitalRead(sensorB) == sences) {
      count++;
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      Serial.println("Entry Detected");
      updateLCD();
      waitForSensorsToReset();
    }
  }

  // Check for Exit (B then A)
  if (digitalRead(sensorB) == sences) {
    delay(300); // debounce
    if (digitalRead(sensorA) == sences) {
      if (count > 0){
        count--;
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
      }
      Serial.println("Exit Detected");
      updateLCD();
      waitForSensorsToReset();
    }
  }
}

void waitForSensorsToReset() {
  // Wait until both sensors are unblocked
  while (digitalRead(sensorA) == sences || digitalRead(sensorB) == sences);
  delay(200);
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("People inside:");
  lcd.print(count);
  lcd.setCursor(0, 1);
  if(count==0){
    lcd.print("You Can Close");
    digitalWrite(relay, HIGH);
  }else{
    lcd.print("Don't Close..!");
    digitalWrite(relay, LOW);
  }
}
