#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>

#define RST 2
#define BUZ 3

// Initialize the LCD display at I2C address 0x27, with a 16x2 display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the HX711 load cell
HX711 scale;

// Pin definitions for the HX711
const int LOADCELL_DOUT_PIN = 6;
const int LOADCELL_SCK_PIN = 5;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);

  pinMode(BUZ, OUTPUT);
  pinMode(RST, INPUT_PULLUP);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Display a startup message on the LCD
  lcd.setCursor(0, 0);
  //lcd.print("Calibrating...");

  // Initialize the HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Wait for the scale to stabilize
  delay(100);

  // Tare the scale to reset its zero point
  scale.set_scale();
  scale.tare();

  // Display calibration completed message
  lcd.clear();
  lcd.setCursor(0, 0);
 // lcd.print("Tare Complete");
  //delay(1500);
  lcd.clear();

  // Set the scale factor (your value: 467440.0)
  scale.set_scale(824280.0);  // Replace with your own scale factor
}

void loop() {
  if (digitalRead(RST) == LOW) {
    scale.set_scale();
    scale.tare();
    scale.set_scale(824280.0); // Reset scale factor
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tare Reset");
    delay(1000);
    lcd.clear();
  }

  // Get the weight in kg from the load cell (average of 10 readings)
  float weightKg = scale.get_units(10);  // Average of 10 readings for stability

  if (weightKg <= 0.001) { // Below 1g, consider as 0
    weightKg = 0.0;
  }

  // Convert weight to Kg and Grams
  int kgPart = int(weightKg);            // Integer part for Kg
  int gramPart = int((weightKg - kgPart) * 1000); // Grams part

  // Serial output for debugging
  Serial.print("Weight: ");
  Serial.print(kgPart);
  Serial.print(" Kg ");
  Serial.print(gramPart);
  Serial.println(" g");

  // Display weight on the LCD
  //lcd.clear(); // Clear the LCD to prevent overlapping of values
  lcd.setCursor(0, 0); // Set cursor to top row
  lcd.print("Weight: ");
  lcd.print("        ");
  lcd.setCursor(8, 0);
  lcd.print(weightKg,2);
  lcd.print(" Kg");

  lcd.setCursor(0, 1); // Set cursor to bottom row
  lcd.print("Grams: ");
  lcd.print("         ");
  lcd.setCursor(7, 1);
  lcd.print(gramPart);
  lcd.print(" g"); // Print grams

  delay(500); // Delay before the next reading
}
