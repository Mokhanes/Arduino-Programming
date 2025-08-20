#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define TRIG_PIN 2 
#define ECHO_PIN 3 


const int buttonPin = 4;
int buttonState = 0; // Current state of the button
int lastButtonState = 0; // Previous state of the button
unsigned long buttonPressStartTime = 0; // Start time of button press
unsigned long buttonPressDuration = 0; // Duration of button press
int count = 0; // Count variable

Adafruit_SSD1306 display(128,64, &Wire, -1);

void setup() {
  Serial.begin(9600); 
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT); 
  pinMode(buttonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // Address 0x3C for 128x64
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000); 
  display.clearDisplay();
}

void loop() {
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);

  distance = (duration * 0.034) / 2; // Speed of sound is 0.034 cm/µs

  Serial.print("Distance: ");
  Serial.print(distance, 2); 
  Serial.println(" cm");

  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    buttonPressStartTime = millis();
  } 
  else if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressDuration = millis() - buttonPressStartTime;
    if (buttonPressDuration < 1700) {
      if(count == 4){
        count = 1;
      }else{
        count++; // Increment count
      }
    }
    else {
      if(count == 1){
        count = 4;
      }else{
        count--; // Decrement count
      }
    }
    Serial.println(count);
  }
  lastButtonState = buttonState;

  display.clearDisplay();
  display.setTextSize(2); 
  display.setTextColor(WHITE);
  display.setCursor(10, 5);
  display.print("Distance:");


  switch(count){
    case 1:
          oledDisplay(distance, "cm");
          break;
    case 2:
          oledDisplay(distance/100, "m");
          break;
    case 3:
          oledDisplay(distance*0.393701, "in");
          break;
    case 4:
          oledDisplay(distance*0.0328, "ft");
          break;
  }

  display.display();

  delay(500);
}

void oledDisplay(float value, String unit){

  if(value >-1 and value<10){
    display.setTextSize(3); 
    display.setCursor(20, 33);
    display.print(value,1);
    display.setCursor(80, 31);
    display.print(unit);
  }
  else if(value>=10 and value<100){
    display.setTextSize(3); 
    display.setCursor(9, 33);
    display.print(value,1);
    display.setCursor(83, 31);
    display.print(unit);
  }else if(value>=100 and value<1000){
    display.setTextSize(3); 
    display.setCursor(6, 33);
    display.print(value,1);
    display.setCursor(98, 41);
    display.setTextSize(2);
    display.print(unit);
  }else{
    display.setTextSize(3); 
    display.setCursor(15, 33);
    display.print(value,0);
    display.setCursor(86, 40);
    display.setTextSize(2);
    display.print(unit);
  }
}
