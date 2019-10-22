
#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire1.begin();
}

void loop() {
  
  // put your main code here, to run repeatedly:
  Wire1.beginTransmission(0x20);
  Wire1.write(0x03);
  Wire1.endTransmission();
  Wire1.requestFrom(0x20,1);
  if (Wire1.available()) Serial.println(Wire1.read());
  else Serial.println("Read fails");

  delay(1000);
  
  
}
