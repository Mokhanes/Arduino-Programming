


#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int RELAY=3;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY, OUTPUT);
  lcd.init();
  lcd.backlight();
}

void loop() {
  
    lcd.setCursor(0, 0);
    lcd.print("Mokhanes A");
    digitalWrite(RELAY,HIGH);
    delay(1000);
    digitalWrite(RELAY,LOW);
    delay(1000);
}
