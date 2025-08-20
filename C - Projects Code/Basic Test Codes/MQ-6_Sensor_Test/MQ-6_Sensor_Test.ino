const int mq6Pin = A0; // Analog pin connected to A0 of MQ-6

void setup() {
  Serial.begin(9600);    // Start serial communication
  pinMode(mq6Pin, INPUT);
  Serial.println("MQ-6 Gas Sensor Test Starting...");
}

void loop() {
  int sensorValue = analogRead(mq6Pin); // Read analog value from sensor
  float voltage = sensorValue * (5.0 / 1023.0); // Convert to voltage (0-5V)

  Serial.print("Analog Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  delay(1000); // Read every second
}
