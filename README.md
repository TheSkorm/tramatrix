Overview
==

This is a simple LED matrix that displays number of minutes until next tram at a stop.

It uses an ESP8266 through the Arduino IDE and a MAX7219 LED matrix controller.

This repo also includes an Openscad box that can either be 3D printed or laser cut.

And here is a video of it working - https://drive.google.com/a/michael-wheeler.org/file/d/0B3Zh48-akWFhRnIzNjBocmtSZUE/view?usp=sharing

Hardware Requirements
==
 - NodeMCU or clone
 - LED matrix with a MAX7219 chip
 - A case to put it all in

Software Requirements
==
 - Arduino IDE
 - ESP8266 custom board for Arduino (https://github.com/esp8266/Arduino)
 - MAX72XXPanel Library (https://github.com/markruys/arduino-Max72xxPanel)
 - Adafruit Graphics Library (https://github.com/adafruit/Adafruit-GFX-Library)
