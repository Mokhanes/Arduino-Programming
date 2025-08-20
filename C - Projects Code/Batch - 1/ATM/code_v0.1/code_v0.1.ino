#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RELAY 3
#define TRIGGER_PIN 4  
#define ECHO_PIN 5    

long duration_first;
float distance_first;
void setup() {
  Serial.begin(9600);             
  pinMode(TRIGGER_PIN, OUTPUT);   
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY,LOW);
  lcd.init();
  lcd.backlight();
  
  delayMicroseconds(200);
  digitalWrite(TRIGGER_PIN, LOW); 
  delayMicroseconds(2);           
  digitalWrite(TRIGGER_PIN, HIGH); 
  delayMicroseconds(10);          
  digitalWrite(TRIGGER_PIN, LOW); 
  duration_first = pulseIn(ECHO_PIN, HIGH); 
  distance_first = ((duration_first * 0.0343) / 2.0)-0.15;
}

void loop() {
  float distance =ultrea_sonic_s();
  Serial.print("Total distance: ");
  Serial.print(distance, 2);  
  Serial.println(" cm");
  Serial.print("First distance: ");
  Serial.print(distance_first);
  Serial.println(" cm");

  if(distance_first>=distance){
    digitalWrite(RELAY,LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cash is Availabl");
    lcd.setCursor(3, 1);
    lcd.print("-e in ATM.");
  }else{
    digitalWrite(RELAY,HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cash is Not Avai");
    lcd.setCursor(1, 1);
    lcd.print("-lable in ATM.");
  }
  delay(1000);
}

float ultrea_sonic_s(){
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
