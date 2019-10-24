
#include <Wire.h>

bool wait=true;

void wait_for_key( void )
{
  char c;

  Serial.println("press any key to continue");
  while (!Serial.available());
  while (Serial.available()) c = Serial.read();  
}

void debug_print_and_wait(char *str)
{
  if (wait)
  {
    Serial.println(str);
    wait_for_key();
  }
}

void setup() 
{
  Serial.println("Init");
  Serial.begin(9600);
  Wire.begin();
  
  debug_print_and_wait("init complete"); 
}

void loop() 
{
  debug_print_and_wait("begin transmission");
  Wire.beginTransmission(0x20);
  
  debug_print_and_wait("write address"); 
  Wire.write(0x03);
  
  debug_print_and_wait("end transmission");
  Wire.endTransmission();
  
  debug_print_and_wait("request 1 byte");
  Wire.requestFrom(0x20,1);
  
  debug_print_and_wait("read");
  if (Wire.available()) Serial.println(Wire.read());
  else Serial.println("Read fails");

  delay(1000);
  
  
}
