#include <CheapStepper.h>
#include <Servo.h>

// Pin definitions
#define IR_SENSOR_PIN 4
#define PROXIMITY_SENSOR_PIN 5
#define BUZZER_PIN 6
#define SOIL_SENSOR_PIN A0

// Object declarations
Servo servo1;
CheapStepper stepper(8, 10, 11, 12); // IN1, IN2, IN3, IN4

// Variables
int rawSoilValue = 0;
int avgSoilMoisture = 0;

void setup() {
  Serial.begin(9600);

  // Configure pin modes
  pinMode(PROXIMITY_SENSOR_PIN, INPUT_PULLUP);
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Setup servo and stepper motor
  servo1.attach(9);
  stepper.setRpm(15);

  // 🟢 Move stepper to a known initial position (home)
  
  // Initial servo movement
  servo1.write(50);
  delay(1000);
  servo1.write(174);
  delay(1000);

  Serial.println("Initializing stepper to home position...");
  stepper.moveDegreesCCW(360); // Move 360 degrees CCW to ensure starting from known point
  delay(1000); // Allow time to settle
  Serial.println("Stepper homed.");

}

void loop() {
  avgSoilMoisture = 0;

  // Read proximity sensor
  int proximityValue = digitalRead(PROXIMITY_SENSOR_PIN);
  Serial.print("Proximity: ");
  Serial.println(proximityValue);

  if (proximityValue == 0) { // Object detected
    tone(BUZZER_PIN, 1000, 1000); // Beep buzzer for 1 second

    // Rotate stepper clockwise and back
    stepper.moveDegreesCW(240);
    delay(1000);

    servo1.write(50);
    delay(1000);
    servo1.write(174);
    delay(1000);

    stepper.moveDegreesCCW(240);
    delay(1000);
  }

  // Read IR sensor
  if (digitalRead(IR_SENSOR_PIN) == 0) {
    tone(BUZZER_PIN, 1000, 500); // Short buzzer alert
    delay(1000);

    // Take 3 soil moisture readings and average them
    for (int i = 0; i < 3; i++) {
      rawSoilValue = analogRead(SOIL_SENSOR_PIN);
      rawSoilValue = constrain(rawSoilValue, 485, 1023); // Adjust for usable range
      avgSoilMoisture += map(rawSoilValue, 485, 1023, 100, 0); // Map to % moisture
      delay(75);
    }

    avgSoilMoisture /= 3; // Average
    Serial.print("Soil Moisture: ");
    Serial.print(avgSoilMoisture);
    Serial.println("%");

    if (avgSoilMoisture > 50) {
      // Soil is dry, perform rotation and servo action
      stepper.moveDegreesCW(120);
      delay(1000);

      servo1.write(50);
      delay(1000);
      servo1.write(174);
      delay(1000);

      stepper.moveDegreesCCW(120);
      delay(1000);
    } else {
      // Soil is wet, alert and perform only servo
      tone(BUZZER_PIN, 1000, 500);
      delay(1000);

      servo1.write(50);
      delay(1000);
      servo1.write(174);
      delay(1000);
    }
  }
}
