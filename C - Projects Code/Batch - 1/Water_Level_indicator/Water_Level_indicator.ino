#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define pins for HC-SR04
const int trigPin = 2; // Trig pin connected to digital pin 9
const int echoPin = 3; // Echo pin connected to digital pin 10

// User-defined tank distances in cm
const int tankFullDistance = 3;  // Distance (in cm) when the tank is full
const int tankLowDistance = 13; // Distance (in cm) when the tank is empty

#define BUZZER 5

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  digitalWrite(BUZZER, LOW);
}

void loop() {
  // Send a 10us pulse to trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo pin and calculate the distance in cm
  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.034) / 2; // Distance in cm
  Serial.println(distance);

  // Map the distance to water level percentage
  float waterLevelPercentage;
  if (distance >= tankLowDistance) {
    waterLevelPercentage = 0; // Tank is empty
  } else if (distance <= tankFullDistance) {
    waterLevelPercentage = 100; // Tank is full
  } else {
    waterLevelPercentage = ((tankLowDistance - distance) / 
                           (tankLowDistance - tankFullDistance)) * 100;
  }

  if(waterLevelPercentage == 100){
    digitalWrite(BUZZER,HIGH);
  }else{
    digitalWrite(BUZZER,LOW);
  }
  // Print the water level percentage to Serial Monitor
  Serial.print("Water Level: ");
  Serial.print(waterLevelPercentage);
  Serial.println("%");

  if(waterLevelPercentage == 100){
    lcd.setCursor(0, 1);
    lcd.print("  Tank is Full  ");
  }else if(waterLevelPercentage == 0){
    lcd.setCursor(0, 1);
    lcd.print(" Tank is Empty  ");
  }else{
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
  lcd.setCursor(0, 0);
  lcd.print("Water Lev:");
  lcd.print(waterLevelPercentage);
  lcd.print("% ");

  // Add a delay for readability
  delay(500);
}
