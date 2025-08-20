#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HX711.h>
#define RST 2
#define BUZ 3

// Initialize the LCD display at I2C address 0x27, with a 16x2 display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the HX711 load cell
HX711 scale;

// Updated pin assignments for the HX711
const int LOADCELL_DOUT_PIN = 6; // Data pin for HX711
const int LOADCELL_SCK_PIN = 5;  // Clock pin for HX711

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);

  pinMode(BUZ,OUTPUT);
  pinMode(RST,INPUT_PULLUP);
  // Initialize the LCD with the number of columns and rows
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();


  // Display a startup message on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Calibrating...");
  
  // Initialize the HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  // Wait for the user to remove any load from the scale for calibration
  delay(2000);
  
  // Tare the scale to reset its zero point
  scale.set_scale();   // Set the scale to a default value
  scale.tare();        // Reset the scale to zero
  
  lcd.setCursor(0, 1);
  lcd.print("Tare complete");
  delay(2000);
  lcd.clear();

  // You may need to adjust the scale factor based on your load cell
  scale.set_scale(2144.5);  // Replace with your own scale factor after calibration
}

void loop() {
  Serial.println(digitalRead(RST));
  if(digitalRead(RST)==0){
    scale.set_scale();   // Set the scale to a default value
    scale.tare();        // Reset the scale to zero
    scale.set_scale(2144.5);
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("New Value Seted");
    delay(1000);
    lcd.clear();
  }
  // Read the weight from the load cell
  float weight = scale.get_units(10); // Take the average of 5 readings for stability

  if(weight <= 0.3){
    weight=0;
  }

  if(weight == 0){
    digitalWrite(BUZ,HIGH);
  }else{
    digitalWrite(BUZ,LOW);
  }
  // Print the weight to the Serial Monitor for debugging
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" kg");
  
  // Display the weight on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Weight [Kg]:");
  lcd.setCursor(0, 1);
  lcd.print(weight, 2); // Display with 2 decimal places
  lcd.print(" kg");

  // Wait a short period before updating the display again
  delay(500);
}
