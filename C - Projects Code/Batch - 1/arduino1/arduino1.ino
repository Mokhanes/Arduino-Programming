char Incoming_value = 0; // Variable to store the incoming data

void setup()
  {

  Serial.begin(9600);    // Start the serial communication
  pinMode(13, OUTPUT);   // Set pin 13 as output for the LED
  }

void loop() {
  if (Serial.available() > 0) {          // Check if there's incoming data
    Incoming_value = Serial.read();      // Read the incoming data
    delay(2);
    Serial.print(Incoming_value);        // Print the incoming value to the Serial Monitor
    Serial.print("\n");

    if (Incoming_value == '1') {         // If the incoming value is '1'
      digitalWrite(13, HIGH);            // Turn the LED on
    } else if (Incoming_value == '0') {  // If the incoming value is '0'
      digitalWrite(13, LOW);             // Turn the LED off
    }
  }
}
