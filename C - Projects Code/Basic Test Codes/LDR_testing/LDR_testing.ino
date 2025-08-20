#define LDR_PIN D5  // Connect the DO pin of LDR module to digital pin D4

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT);
  Serial.println("LDR Sensor Digital Test Initialized...");
}

void loop() {
  int ldrStatus = digitalRead(LDR_PIN);  // Read digital value from LDR

  if (ldrStatus == HIGH) {
    Serial.println("🌙 Low Light Intensity Detected");
  } else {
    Serial.println("🌞 High Light Intensity Detected");
  }

  delay(1000); // Delay for 1 second
}
