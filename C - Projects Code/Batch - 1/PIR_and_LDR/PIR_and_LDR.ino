
#define LDR 2
#define PIR 3
#define RELAY 4

void setup() {
  // put your setup code here, to run once:
  pinMode(PIR, INPUT);
  pinMode(LDR, INPUT);
  pinMode(RELAY, OUTPUT);
  Serial.begin(9600);

  digitalWrite(RELAY, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("PIR = ");
  Serial.println(digitalRead(PIR));
  Serial.println("LDR = ");
  Serial.println(digitalRead(LDR));
  if(digitalRead(LDR) and digitalRead(PIR)){
    digitalWrite(RELAY, HIGH);
  }else{
    digitalWrite(RELAY, LOW);
  }
  delay(500);
}
