# Pixel-perfect LCD display

Typical 16x2 LCD allows one to define up to 8 custom characters. This project shows how to overcome that limit and, in result, be able to control each and every pixel of the LCD screen, at the price of some flickering, and the need for the LCD screen ti be updated every 20 milliseconds (which can, of course, be done concurrently with other processing).

## Instructions

Works only for Arduino UNO (but could absolutely be adapted).

```
make;
make upload;
```

Requires [Arduino IDE](https://github.com/arduino/Arduino/) to be installed. The actual compilation uses the GNU AVR Toolchain which comes with it directly.

Currently makefile works only for MacOS, but could be easily adapted to work on other operations systems.

## Explanation


CGRAM:

<p align="center"><img src="https://i.stack.imgur.com/1TKZH.gif" width="300px"></p>

## Links

 - https://www.instructables.com/Custom-Large-Font-For-16x2-LCDs/
 - https://arduino.stackexchange.com/questions/46828/how-to-show-the-º-character-in-a-lcd