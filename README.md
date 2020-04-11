# Assetto-Corsa-Arduino-5-LCD
This is an Arduino Mega based LCD display with 16 RGB LEDs.

It has a Shared Memory API running in the computer, this takes the data and sends it to the Arduino throught the USB Serial, then the Arduino parses it and displays the data.

For displaying the data, a Timer is running at 32.64ms that updates the LCD data, so we can achieve a little bit less than 31 FPS.
