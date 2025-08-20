#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// WiFi Credentials
const char* ssid = "OPPO A7";
const char* password = "12344321";

// SMS API Details
const char* apiKey = "LdSDcJsK0bpP";
const char* templateID = "101";
const char* mobileNumber = "919677623314";

String wrongOTP = "";

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pins
const int relayPin = 4; // Relay control
const int ledPin = 2;   // Built-in LED (D2)

String generatedOTP = "";
String enteredOTP = "";
bool waitingForOTP = false;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  // Blink LED while connecting
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(500);
  }
  digitalWrite(ledPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1000);
  lcd.clear();
  lcd.print("Press A to Start");
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (!waitingForOTP) {
      if (key == 'A') {
        generatedOTP = String(random(1000, 9999));
        sendSMS(generatedOTP);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("OTP Sent!");
        lcd.setCursor(0, 1);
        lcd.print("Enter OTP:");
        waitingForOTP = true;
        enteredOTP = "";
      }
    } else {
      if (key == 'B') {
        // Backspace
        if (enteredOTP.length() > 0) {
          enteredOTP.remove(enteredOTP.length() - 1);
        }
      } else if (isdigit(key)) {
        enteredOTP += key;
      }

      lcd.setCursor(0, 1);
      lcd.print("OTP: " + enteredOTP + "    ");

      if (enteredOTP.length() == 4) {
        if (enteredOTP == generatedOTP) {
          lcd.clear();
          lcd.print("Access Granted");
          digitalWrite(relayPin, HIGH);
          delay(4000);
          digitalWrite(relayPin, LOW);
          lcd.clear();
          lcd.print("Press A to Start");
        } else {
          wrongOTP = enteredOTP;
          sendWrongPasswordSMS();
          lcd.clear();
          lcd.print("Wrong OTP!");
          delay(2000);
          lcd.clear();
          lcd.print("Press A to Start");
        }
        waitingForOTP = false;
      }
    }
  }
}

void sendSMS(String otp) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);
    http.begin(apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");
    
    String var1 = "OTP";
    String var2 = otp + " (Door Unlock)";
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + var1 + "\",\"var2\":\"" + var2 + "\"}";
    
    int code = http.POST(payload);
    Serial.println("HTTP Response code: " + String(code));
    http.end();
  }
}

void sendWrongPasswordSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);
    http.begin(apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");
    
    String var1 = "Wrong OTP";
    String var2 = wrongOTP + " (Door Lock)";
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + var1 + "\",\"var2\":\"" + var2 + "\"}";
    
    int code = http.POST(payload);
    Serial.println("Wrong Pass SMS Code: " + String(code));
    http.end();
  }
}
