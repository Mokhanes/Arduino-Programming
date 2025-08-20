#define RELAY_PIN D0  // You can change this pin if needed

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Keep relay OFF initially
}

void loop() { 
  digitalWrite(RELAY_PIN, HIGH); // Relay ON
  delay(1000);                   // Wait 1 second
  digitalWrite(RELAY_PIN, LOW);  // Relay OFF
  delay(1000);                   // Wait 1 second
}
