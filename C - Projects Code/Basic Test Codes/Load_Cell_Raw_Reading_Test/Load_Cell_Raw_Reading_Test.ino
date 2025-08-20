#include <HX711.h>

#define DOUT 6
#define SCK 5

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT, SCK);

  Serial.println("Remove all weight from the scale...");
  delay(3000);

  scale.tare();
  Serial.println("Tare done.");

  Serial.println("Place a known weight on the scale...");
}

void loop() {
  if (scale.is_ready()) {
    long reading = scale.read_average(10);
    Serial.print("Raw reading: ");
    Serial.println(reading);
    delay(1000);
  } else {
    Serial.println("HX711 not found.");
  }
}
