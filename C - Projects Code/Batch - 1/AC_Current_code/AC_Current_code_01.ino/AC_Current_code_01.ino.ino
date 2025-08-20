
#include <iarduino_ACS712.h>    
#include <LiquidCrystal_I2C.h>

iarduino_ACS712 sensor(A0); 

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int RELAY=2;
const int RED_LED=3;
const int BUZZER=4;
const int RST_BTN=5;

float MINI_CURRENT = 0.02;
float MAX_CURRENT = 0.10;
char Incoming_value = 0;
void setup() {
  Serial.begin(9600);   
  pinMode(RELAY, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RST_BTN, INPUT);
         
  float v=sensor.getZeroVAC(); 
  sensor.setZeroVAC(v);
  lcd.init();
  lcd.backlight();

  digitalWrite(RELAY,HIGH);
  digitalWrite(RED_LED,LOW);
  digitalWrite(BUZZER,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0) {          // Check if there's incoming data
    Incoming_value = Serial.read();      // Read the incoming data
    Serial.print(Incoming_value);        // Print the incoming value to the Serial Monitor
    Serial.print("\n");

    if (Incoming_value == '1') {         // If the incoming value is '1'
      digitalWrite(RELAY, HIGH);            // Turn the LED on
    } else if (Incoming_value == '0') {  // If the incoming value is '0'
      digitalWrite(RELAY, LOW);             // Turn the LED off
    }
  }
    
  float i = sensor.readAC();
  if(i<MINI_CURRENT){
    i=0;
  }
 // Serial.print(i);
 // Serial.println(" A.");
  lcd.setCursor(0, 0);
  lcd.print("Current:");
  lcd.print(i);
  lcd.print(" A");
  
  if(i>MAX_CURRENT){
    digitalWrite(RED_LED,HIGH);
    digitalWrite(BUZZER,HIGH);
  }else{
    digitalWrite(RED_LED,LOW);
    digitalWrite(BUZZER,LOW);
  }
  delay(300); 
}
