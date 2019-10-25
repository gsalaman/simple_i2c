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

## I2C specifis
Sparkfun has a good primer at
https://learn.sparkfun.com/tutorials/i2c/all

If you look at my simple code, you'll notice we've got two main "blocks"...this one tells which I2C device address and which register in that device we're interested in:
```
```

## Artemis Signal analysis.
https://github.com/gsalaman/simple_i2c/issues/1#issuecomment-546373295
