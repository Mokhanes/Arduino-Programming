#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> // Required for HTTPS connections

// Replace with your WiFi credentials
const char* ssid = "OPPO A7";
const char* password = "12344321";

// Replace with your API Key
const char* apiKey = "4j2NlwJjHxZP"; //get api key from circuitdigest.cloud

// SMS details (users can easily modify these)
const char* templateID = "103";                  // Template ID //moreinfo: https://circuitdigest.com/article/free-sms-api-for-arduino-esp32-esp8266-nodemcu-raspberry-pi
const char* mobileNumber = "917373303165";       // Mobile number (with country code)
const char* var1 = "Fall Sensor 1";            // Variable 1
const char* var2 = "the Grandfather's Hand";                 // Variable 2

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Send SMS
  sendSMS();
}

void sendSMS() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client; // Use WiFiClientSecure for HTTPS connections
    client.setInsecure();    // Skip certificate validation (not secure but works for development)
    HTTPClient http;

    // Build the API URL with the template ID
    String apiUrl = "https://www.circuitdigest.cloud/send_sms?ID=" + String(templateID);

    // Start the HTTPS connection with WiFiClientSecure
    http.begin(client, apiUrl);
    http.addHeader("Authorization", apiKey);
    http.addHeader("Content-Type", "application/json");

    // Create the JSON payload with SMS details
    String payload = "{\"mobiles\":\"" + String(mobileNumber) + "\",\"var1\":\"" + String(var1) + "\",\"var2\":\"" + String(var2) + "\"}";

    // Send POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode == 200) {
      Serial.println("SMS sent successfully!");
      Serial.println(http.getString());
    } else {
      Serial.print("Failed to send SMS. Error code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + http.getString());
    }

    http.end(); // End connection
  } else {
    Serial.println("WiFi not connected!");
  }
}

void loop() {
  // Nothing to do here
}
