#define TRIG_PIN 2     // Ultrasonic Trigger
#define ECHO_PIN 3     // Ultrasonic Echo
#define RELAY_PIN 5    // Relay connected to Motor

long tankHeight = 0;       // Height of the tank in cm (to be measured in setup)
float distance = 0;        // Current distance of water from sensor
float levelPercentage = 0; // Current water level percentage

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Initially motor OFF

  Serial.println("Measuring tank height...");
  delay(2000);
  tankHeight = measureDistance();
  Serial.print("Tank Height (cm): ");
  Serial.println(tankHeight);
}

void loop() {
  distance = measureDistance();
  levelPercentage = ((tankHeight - distance) / (float)tankHeight) * 100.0;

  Serial.print("Water Level: ");
  Serial.print(levelPercentage);
  Serial.println(" %");

  if (levelPercentage < 10.0) {
    digitalWrite(RELAY_PIN, LOW); // Turn ON motor
    Serial.println("Water Low! Motor ON");
  } else if (levelPercentage > 86.0) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn OFF motor
    Serial.println("Tank Full! Motor OFF");
  }

  delay(500); // Delay between readings
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distanceCM = duration * 0.034 / 2; // Convert to cm
  return distanceCM;
}
