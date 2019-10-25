# simple_i2c, or, test code for debugging I2C on the 8266 Thing
## Background
While working on an MQTT joystick, I ran into a roadblock with the 8266's I2C.  The basic overview:  you've got an 8266 talking w
Wifi to an MQTT server, and polling an I2C joystick to send direction changes to that server...which then forwards them on to
whichever game is listening.  Worked through all the nasty Wifi and MQTT stuff (which I *thought* would be the hard part), 
and then hooked up I2c...which was failing the reads of the joystick!

## First Debug Step:  RPI
The first thing I needed to check was whether the joystick was working.  Since I'm in arduino-land, my first thought was to 
tweak the driver I was using on an Uno for an I2C touchpot and see if it would talk to the joystick...but the Uno ruins at 5v and
the joystick I have is 3.3.  While I *could* translated the levels, I'd rather do this with a 3.3v system.

Next thought:  use the Pi.  It's 3.3v, and it's got decent command line tools for detecting addresses & doing reads and writes.
So, hook that up, verify that it sees the device (default address 0x20), and I can do reads and writes of the horizontal and 
vertical control registers.  Good news...it's not a broken joystick.

## Next debug step:  Artemis
I had a Sparkfun Artemis Thing lying around that I hadn't played with...checked the specs, and it was 3.3v and had two I2C mechanisms:
Sparkfun's QWIIC connect system, and standard I2C.  The joystick *also* has a QWIIC connetor, so that was my first thought...but I 
didn't have the cable.   Okay, just breadboard it to std I2C pins.

Ran some of the default examples, and noticed that if I wanted to use the I2C pins in Artemis, I needed to use "Wire1" calls rather than
just "Wire".  Make the change, and I'm seeing reads and writes from the Artemis.  Hmmm...if I can use Wifi on the Artemis, then I
don't even need the 8266...yes, it's a little more expensive, but it's got other cool stuff on it.  Problem:  this Artemis module
only talks Bluetooth...not Wifi.  :(

## Back to the 8266
So maybe it's Wire1 instead of Wire to get the 8266 working.  Nope...while the Artemis has 2 different I2C addressing schemes, the 
8266 only has Wire...Wire1 doesn't even compile.  Okay, so there's something different between the I2C between the two.  Time
to dig into the signals and drag out the scope.  Means I needed to refresh my I2C knowlege...

## I2C software side
Sparkfun has a good primer at
https://learn.sparkfun.com/tutorials/i2c/all

If you look at my simple code, you'll notice we've got two main "blocks"...this one tells which I2C device address and which register in that device we're interested in:
```
  Wire1.beginTransmission(0x20);
  Wire1.write(0x03);
  Wire1.endTransmission();
```
Basically, I want to talk to the device at I2C address 0x20, and I'm interested in register 0x03 from that device.

Note my actual code has some debug constructs around these...but this is really the core of what's needed.  Also, note this snippet is "Wire1" rather than "Wire"...meaning it's for Artemis, not 8266.

Then, you need to request a read:
```
  Wire1.requestFrom(0x20,1);
  if (Wire1.available()) Serial.println(Wire1.read());
  else Serial.println("Read fails");
```
The "requestFrom" actually does the I2C bus write...it's requesting one byte from address 0x20.  That device knows the byte comes from it's 0x03 register...and if everything is working, that byte is returned to the I2C driver.  More on how that happens down below.  

The "Wire1.available" is a software call that checks that hardware transaction...it returns the number of bytes available.  In our example (and if everything is good), it'll return a "1"...but if the transaction has failed, it will return a "0", meaning no bytes are available.

## Artemis "write" signal analysis.
Now lets look at this in hardware.  Here's a snapshot of that first "setup" block:

![working write_25us](https://user-images.githubusercontent.com/43499190/67578581-f93b0980-f6ff-11e9-9224-e4eb8ceff015.jpg)

The top signal is SDA; the bottom SCL.

Note the first seven bits (working your way left-to-right on SDA, matching each bit when SCL goes high): 010 0000, or 0x20.
We then have another 0, meaning the master (8266) wants to write.  We then have an Ack pulse, where the slave pulls SDA low when SCL goes high.

This Ack was important to check...it meant that the joystick was actually seeing and matching it's address (0x20).

Next, the master writes the data it wants to write...in this case, it's the register of interest (0x03).  Those 8 bits follow, followed by another ack pulse (which the joystick also acks by pulling SDA low).  All good.

## 8266 "write" analysis
Now, let's look at the same thing on the 8266.

![8266_write_50us](https://user-images.githubusercontent.com/43499190/67580181-2210ce00-f703-11e9-8816-ac9dd84ef116.jpg)

We see the same bits being sent, and also see both acks.  Note the clock rate is a little different...in the Artemis picture, we're 25us per division...if we do the math, that looks like ~100 KHz.  In the 8266, we're at 50us per division, with a clock rate ~60 KHz.  Something to keep my eye on...

## Artemis "read" analysis
Here's the "read" request from the artemis (master).

After a little bit of time, the joystick will put it's data onto the bus:


Here's the whole thing, zoomed out (100us per division)
![working_read_100us](https://user-images.githubusercontent.com/43499190/67583889-bed66a00-f709-11e9-9621-81effc516960.jpg)

Note that it takes a little more than 400us for the joystick to actually write the byte onto the bus.
![working_read_100us](https://user-images.githubusercontent.com/43499190/67583889-bed66a00-f709-11e9-9621-81effc516960.jpg)
