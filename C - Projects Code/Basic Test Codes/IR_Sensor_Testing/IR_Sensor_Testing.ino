// Define IR sensor pin
const int irSensorPin = 3;  // Connect IR sensor OUT pin to Arduino digital pin 2

void setup() {
  pinMode(irSensorPin, INPUT);     // Set pin as input
  Serial.begin(9600);              // Start serial communication at 9600 baud
}

void loop() {
  int sensorValue = digitalRead(irSensorPin); // Read the digital value

  if (sensorValue == LOW) {
    Serial.println("Object Detected!");
  } else {
    Serial.println("No Object.");
  }

  delay(500); // Delay to avoid flooding the serial monitor
}
