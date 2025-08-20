#include <iarduino_ACS712.h>    
#include <LiquidCrystal_I2C.h>

iarduino_ACS712 sensor1(A0); 
iarduino_ACS712 sensor2(A1); 

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RED_LED 2
#define BUZZER 4

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  float v1=sensor1.getZeroVAC(); 
  sensor1.setZeroVAC(v1);
  float v2=sensor2.getZeroVAC(); 
  sensor2.setZeroVAC(v2);
  lcd.init();
  lcd.backlight();
}

void loop() {

  
  float tolerance = 0.09;
  float average1 = 0;
  float average2 = 0;
  for(int i = 0; i < 12; i++) 
  {
    average1 = average1 + sensor1.readAC() ;
    average2 = average2 + sensor2.readAC() ;
  }
  average1 /=12;
  average2 /=12;

  //average2 +=0.05;

  if(average1 < 0.07){
    average1 = 0;
  }
  
  if(average2 < 0.07){
    average2 = 0;
  }
  
  Serial.print("Avg1 = ");
  Serial.println(average1);  
  Serial.print("Avg2 = ");
  Serial.println(average2);

  
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(average1);
  lcd.print("A R:");
  lcd.print(average2);
  lcd.print("A");

  if (abs(average1 - average2) <= tolerance) {
    Serial.println("No Current Loss ");
    lcd.setCursor(0, 1);
    lcd.print("No Current Loss  ");
    digitalWrite(RED_LED,LOW);
    digitalWrite(BUZZER,LOW);
  } else {
    Serial.println("Current Loss..! ");
    lcd.setCursor(0, 1);
    lcd.print("Current Loss..!");
    digitalWrite(RED_LED,HIGH);
    digitalWrite(BUZZER,HIGH);
  }
  
}
