/*New Blynk app with Home Automation
   Home Page
*/

//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//Define the relay pins
#define led D0
#define relay1 D1
#define relay2 D2
#define relay3 D3
#define relay4 D4

#define BLYNK_AUTH_TOKEN "UtEEBxlzt5OQG2kGaZ5aVtYJdEaHx3Vu" //Enter your blynk auth token

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "admin";//Enter your WIFI name
char pass[] = "admin12345";//Enter your WIFI password

//Get the button values
BLYNK_WRITE(V0) {
  bool value1 = param.asInt();
  // Check these values and turn the relay1 ON and OFF
  if (value1 == 1) {
    digitalWrite(relay1, LOW);
    Serial.println("V0 ON");
  } else {
    digitalWrite(relay1, HIGH);
    Serial.println("V0 OFF");
  }
}

//Get the button values
BLYNK_WRITE(V1) {
  bool value2 = param.asInt();
  // Check these values and turn the relay2 ON and OFF
  if (value2 == 1) {
    digitalWrite(relay2, LOW);
    Serial.println("V1 ON");
  } else {
    digitalWrite(relay2, HIGH);
    Serial.println("V1 OFF");
  }
}

BLYNK_WRITE(V2) {
  bool value3 = param.asInt();
  // Check these values and turn the relay2 ON and OFF
  if (value3 == 1) {
    digitalWrite(relay3, LOW);
    Serial.println("V2 ON");
  } else {
    digitalWrite(relay3, HIGH);
    Serial.println("V2 OFF");
  }
}

BLYNK_WRITE(V3) {
  bool value4 = param.asInt();
  // Check these values and turn the relay2 ON and OFF
  if (value4 == 1) {
    digitalWrite(relay4, LOW);
    Serial.println("V3 ON");
  } else {
    digitalWrite(relay4, HIGH);
    Serial.println("V3 OFF");
  }
}
void setup() {
  //Set the relay pins as output pins
  pinMode(led, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // Turn OFF the relay
  digitalWrite(led, LOW);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  //Initialize the Blynk library
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.begin(9600);
}

void loop() {
  //Run the Blynk library
  Blynk.run();
}
