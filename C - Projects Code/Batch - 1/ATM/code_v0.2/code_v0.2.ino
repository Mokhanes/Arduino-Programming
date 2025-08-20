#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BUZZER 7
#define RELAY 3
#define TRIGGER_PIN 5  // Trigger pin connected to digital pin 4
#define ECHO_PIN 6     // Echo pin connected to digital pin 5

long duration_first;
float distance_first;

void setup() {
  Serial.begin(9600);             // Start serial communication
  pinMode(TRIGGER_PIN, OUTPUT);   // Set Trigger pin as OUTPUT
  pinMode(ECHO_PIN, INPUT);       // Set Echo pin as INPUT
  pinMode(RELAY, OUTPUT);         // Set Relay pin as OUTPUT
  pinMode(BUZZER, OUTPUT);
  digitalWrite(RELAY, LOW);       // Initialize relay to LOW (off)
  digitalWrite(BUZZER, LOW); 
  
  
  lcd.init();                     // Initialize LCD
  lcd.backlight();                // Turn on LCD backlight
  
  delayMicroseconds(200);         // Allow the LCD to initialize
  triggerUltrasonic();            // Trigger the ultrasonic sensor for the first reading
}

void loop() {
  float distance = readUltrasonic();
  
  Serial.print("Total distance: ");
  Serial.print(distance, 2);  
  Serial.println(" cm");
  
  Serial.print("First distance: ");
  Serial.print(distance_first);
  Serial.println(" cm");

  // Compare the first distance with the new distance
  if (distance_first >= distance) {
    digitalWrite(RELAY, LOW);
    digitalWrite(BUZZER, LOW);
    displayMessage("Cash is Available", "-e in ATM.");
  } else {
    digitalWrite(RELAY, HIGH);
    digitalWrite(BUZZER, HIGH);
    displayMessage("Cash is Not Avail", "-able in ATM.");
  }
  delay(1000);
}

// Function to trigger the ultrasonic sensor and get the distance
float readUltrasonic() {
  long duration;
  float distance;  
  
  digitalWrite(TRIGGER_PIN, LOW); 
  delayMicroseconds(2);           
  digitalWrite(TRIGGER_PIN, HIGH); 
  delayMicroseconds(10);          
  digitalWrite(TRIGGER_PIN, LOW); 

  duration = pulseIn(ECHO_PIN, HIGH); 
  distance = (duration * 0.0343) / 2.0;

  return distance;
}

// Function to trigger the ultrasonic sensor once
void triggerUltrasonic() {
  digitalWrite(TRIGGER_PIN, LOW); 
  delayMicroseconds(2);           
  digitalWrite(TRIGGER_PIN, HIGH); 
  delayMicroseconds(10);          
  digitalWrite(TRIGGER_PIN, LOW); 

  duration_first = pulseIn(ECHO_PIN, HIGH); 
  distance_first = ((duration_first * 0.0343) / 2.0) - 0.15;
}

// Function to display messages on the LCD
void displayMessage(String line1, String line2) {
  lcd.clear();                    // Clear the LCD
  lcd.setCursor(0, 0);           // Set cursor to the first row
  lcd.print(line1);               // Print the first line
  lcd.setCursor(0, 1);           // Set cursor to the second row
  lcd.print(line2);               // Print the second line
}
