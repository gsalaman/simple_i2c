
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
  Wire.begin();
  Wire.setClockStretchLimit(2000);
  
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
  
  debug_print_and_wait("request 4 bytes");
  Wire.requestFrom(0x20,4);
  
  debug_print_and_wait("read");
  
  if (Wire.available() == 4) 
  {
    // The 4 bytes should be:
    //    1) Horizontal MSB
    //    2) Horizontal LSB
    //    3) Vertical MSB
    //    4) Vertical LSB
    //  I only care about the MSBs, so the LSB reads will be ignored.
    Serial.print("horiz: ");
    Serial.println(Wire.read());
    Wire.read();
    Serial.print("vert:");
    Serial.println(Wire.read());
  }
  else Serial.println("Read fails");

  delay(1000);
  
  
}
