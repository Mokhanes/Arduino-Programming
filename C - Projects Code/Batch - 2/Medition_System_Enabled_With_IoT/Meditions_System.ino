#define BLYNK_TEMPLATE_ID   "TMPL3f5Uywcee"
#define BLYNK_TEMPLATE_NAME "Timed Medication Dispenser"
#define BLYNK_AUTH_TOKEN    "bRFX57tIpNsSkl4KSLdhMuZra4bGkxhC"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <Servo.h>

char ssid[] = "OPPO A7";
char pass[] = "12344321";

BlynkTimer timer;
WidgetRTC rtc;
Servo myServo;

// Switch states from Blynk
int sw1=0, sw2=0, sw3=0, sw4=0;

// Time storage
int setHour=-1, setMinute=-1;

// Hardware pins
#define BUZZER D5
#define SERVO_PIN D6
#define BUTTON_PIN D7
#define TAB1 D1
#define TAB2 D2
#define TAB3 D3
#define TAB4 D4

// ESP8266 built-in LED (LOW = ON)
#define LED_PIN LED_BUILTIN  

bool waitingForTablet = false;
unsigned long alertStartTime = 0;
unsigned long lastNotifyTime = 0;

bool boxOpen = false;
unsigned long boxOpenTime = 0;
bool allowOpen = false;  
bool cycleCompleted = false;   


bool boxOpenNotified = false;  


unsigned long lastButtonPress = 0;


enum BuzzerMode {BUZZER_OFF, BUZZER_CONTINUOUS, BUZZER_SINGLE_BEEP, BUZZER_ALERT};
BuzzerMode buzzerMode = BUZZER_OFF;
unsigned long buzzerLastToggle = 0;
int buzzerStep = 0;

void doThreeBeeps() {  
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}

void resetSystem() {
  digitalWrite(BUZZER, LOW);
  buzzerMode = BUZZER_OFF;
  myServo.write(0);
  digitalWrite(TAB1, LOW);
  digitalWrite(TAB2, LOW);
  digitalWrite(TAB3, LOW);
  digitalWrite(TAB4, LOW);
  waitingForTablet = false;
  allowOpen = false;
  cycleCompleted = false;
  boxOpen = false;
  lastNotifyTime = 0;
  boxOpenNotified = false;
  
  for(int i=0; i<2; i++){
    digitalWrite(BUZZER, HIGH);
    delay(300);
    digitalWrite(BUZZER, LOW);
    delay(300);
  }
}


BLYNK_WRITE(V0) {
  int manualState = param.asInt();
  if (manualState == 1) {
    // Open box manually, ignore schedule
    Serial.println("Manual Override: Box OPEN");
    myServo.write(180);
    digitalWrite(TAB1, sw1);
    digitalWrite(TAB2, sw2);
    digitalWrite(TAB3, sw3);
    digitalWrite(TAB4, sw4);
    buzzerMode = BUZZER_SINGLE_BEEP;  
    boxOpen = true;
  } else {
    Serial.println(" Manual Override: Box CLOSE");
    myServo.write(0);
    digitalWrite(TAB1, LOW);
    digitalWrite(TAB2, LOW);
    digitalWrite(TAB3, LOW);
    digitalWrite(TAB4, LOW);
    buzzerMode = BUZZER_OFF;          
    boxOpen = false;
  }
}

BLYNK_WRITE(V1) {
  TimeInputParam t(param);
  if (t.hasStartTime()) {
    setHour   = t.getStartHour();
    setMinute = t.getStartMinute();
    Serial.print("Medication Time Set ");
    Serial.print(setHour); Serial.print(":"); Serial.println(setMinute);
    resetSystem();
  }
}


BLYNK_WRITE(V2) { sw1 = param.asInt(); }
BLYNK_WRITE(V3) { sw2 = param.asInt(); }
BLYNK_WRITE(V4) { sw3 = param.asInt(); }
BLYNK_WRITE(V5) { sw4 = param.asInt(); }

void checkTime() {
  if (setHour < 0 || setMinute < 0) return;

  if (hour() == setHour && minute() == setMinute && !waitingForTablet && !cycleCompleted) {
    Serial.println("Time Matched Waiting for user to take tablet...");
    waitingForTablet = true;
    allowOpen = true;  
    alertStartTime = millis();
    lastNotifyTime = millis();
    buzzerMode = BUZZER_CONTINUOUS;
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonPress > 300) {  
      handleButtonPress();
      lastButtonPress = millis();
    }
  }

  if (waitingForTablet && !boxOpen && !cycleCompleted) {
    if (millis() - alertStartTime >= 30000) {
      if (millis() - lastNotifyTime >= 60000) {
        Blynk.logEvent("tablet_not_picked", "Tablet Not Picked!");
        Serial.println("Tablet not picked - Notification sent.");
        lastNotifyTime = millis();
        buzzerMode = BUZZER_ALERT;
      }
    }
  }

  if (boxOpen && !boxOpenNotified && millis() - boxOpenTime >= 60000) {
    Blynk.logEvent("box_open_warning", "Box left open!");
    Serial.println("⚠️ Box left open - Notification sent once.");
    buzzerMode = BUZZER_ALERT;
    boxOpenNotified = true;
  }
}

void handleButtonPress() {
  if (cycleCompleted) {
    Serial.println("Second cycle attempt denied in same schedule!");
    doThreeBeeps();
    return;
  }

  if (!allowOpen) {
    Serial.println("Ignored button press (Not scheduled time).");
    return;
  }

  if (!boxOpen) {
    Serial.println("Box Opened: Activating Tablets");
    myServo.write(180);
    digitalWrite(TAB1, sw1);
    digitalWrite(TAB2, sw2);
    digitalWrite(TAB3, sw3);
    digitalWrite(TAB4, sw4);
    boxOpen = true;
    boxOpenTime = millis();
    boxOpenNotified = false;  
    buzzerMode = BUZZER_SINGLE_BEEP;  
  } else {
    Serial.println("Box Closed: Turning OFF Tablets & Buzzer (Cycle Completed)");
    myServo.write(0);
    digitalWrite(TAB1, LOW);
    digitalWrite(TAB2, LOW);
    digitalWrite(TAB3, LOW);
    digitalWrite(TAB4, LOW);
    buzzerMode = BUZZER_OFF;   
    boxOpen = false;
    waitingForTablet = false;
    allowOpen = false;
    cycleCompleted = true;
  }
}

void handleBuzzer() {
  unsigned long now = millis();
  switch(buzzerMode) {
    case BUZZER_OFF:
      digitalWrite(BUZZER, LOW);
      break;

    case BUZZER_CONTINUOUS:
      digitalWrite(BUZZER, HIGH);
      break;

    case BUZZER_SINGLE_BEEP: 
      if (now - buzzerLastToggle > (buzzerStep == 0 ? 400 : 1000)) {
        buzzerStep = 1 - buzzerStep;
        digitalWrite(BUZZER, buzzerStep == 0 ? LOW : HIGH);
        buzzerLastToggle = now;
      }
      break;

    case BUZZER_ALERT: 
      if (now - buzzerLastToggle > (buzzerStep == 0 ? 400 : (buzzerStep == 1 ? 200 : (buzzerStep == 2 ? 400 : 1000)))) {
        buzzerStep = (buzzerStep + 1) % 4;
        digitalWrite(BUZZER, (buzzerStep == 0 || buzzerStep == 2));
        buzzerLastToggle = now;
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TAB1, OUTPUT);
  pinMode(TAB2, OUTPUT);
  pinMode(TAB3, OUTPUT);
  pinMode(TAB4, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(0);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); 
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  digitalWrite(LED_PIN, LOW);  

  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_TOKEN);
  if (Blynk.connect()) Serial.println("Connected to Blynk Cloud!");
  else Serial.println("Failed to connect to Blynk.");

  rtc.begin();
  timer.setInterval(200L, handleBuzzer); 
  timer.setInterval(1000L, checkTime);
}

void loop() {
  Blynk.run();
  timer.run();
}
