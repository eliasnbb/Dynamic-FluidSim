# Dynamic-FluidSim
School Project.
Program is intended to run on Arduino Uno R4, equivalent or superior specs. Else the program will run excessively slow.
If it runs too slow on your microprocessor by default you can change the variable maksAntall to a lower number. 
I recomment to make sure the number is at most 70% of your full pixel count.

I used the Adafruit_SH1106G library and compatible Adafruit SH1106G monochrome 128x64 display, but you can easily adapt it for the Adafruit SSD1306 or similar ones. I have attempted to keep it easily scalable and easy to use if you have the same components. You may have to make some deeper changes if you want to adapt it to other displays or MPUs. 
Both the MPU 6050 3-axis accelerometer and gyroscope chip, and the SH1106G communicate on the I2C protocol. It has sufficient speed and requires only 2 wires, in addition to +5V DC and GND wires.

Mock version to give you an idea of the real thing: https://wokwi.com/projects/394332568928355329
