#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);  // (I2C address, columns, rows)

void setup() {

  delay(1000);
  lcd.init();        // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  
}

void loop() {
  // Nothing in loop, display stays static
  lcd.setCursor(0, 0);
  lcd.print("LCD Test OK");
  lcd.setCursor(0, 1);
  lcd.print("Nano Ready!");
  delay(1000);
}
