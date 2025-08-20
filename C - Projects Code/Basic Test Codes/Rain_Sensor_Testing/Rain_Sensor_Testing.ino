// Define the analog pin connected to the rain sensor
#define RAIN_SENSOR A0  // A0 is the only analog pin on NodeMCU

void setup() {
  Serial.begin(9600);       // Start Serial communication
  pinMode(RAIN_SENSOR, INPUT);
  Serial.println("Rain Sensor Test Initialized...");
}

void loop() {
  int rainValue = analogRead(RAIN_SENSOR); // Read analog value

  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);

  if (rainValue < 800) {
    Serial.println("⚠️ Rain Detected!");
  } else {
    Serial.println("☀️ No Rain");
  }

  delay(1000); // Read every 1 second
}
