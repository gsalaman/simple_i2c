
#include <Wire.h>

bool wait=false;

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
  Wire1.begin();
  
  debug_print_and_wait("init complete"); 
}

void loop() 
{
  debug_print_and_wait("begin transmission");
  Wire1.beginTransmission(0x20);
  
  debug_print_and_wait("write address"); 
  Wire1.write(0x03);
  
  debug_print_and_wait("end transmission");
  Wire1.endTransmission();
  
  debug_print_and_wait("request 1 byte");
  Wire1.requestFrom(0x20,1);
  
  debug_print_and_wait("read");
  if (Wire1.available()) Serial.println(Wire1.read());
  else Serial.println("Read fails");

  delay(1000);
  
  
}
