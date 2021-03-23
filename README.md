# Pixel-perfect LCD display

Typical 16x2 LCD allows one to define up to 8 custom characters. This project shows how to overcome that limit and, in result, be able to control each and every pixel of the LCD screen, at the price of some flickering, and the need for the LCD screen ti be updated every 20 milliseconds (which can, of course, be done concurrently with other processing).

<img align=right width=300 src="./img/movie.gif" style="margin:10px">

## Word of caution

Doing this *shouldn't* break your LCD because CGRAM *should* be read-only and if things get messed-up, LCD *should* always start the same way after power cycle. Having said that, my LCD now does not accept normal `lcd.print("...")`'s from `LiquidCrystal` library, even though the CGRAM seems fine when I print it out the library. I can still do anything I want with it, but I have to manually move cursor to the new line and use `lcd.write(...)` to put character on the screen (cursor still advances fine). If you're ok *potentially* making your LCD screen much more... fun to use, read on.

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

## ToDos

- [ ] Different scanning pattern ?
- [x] Fix spacing by accounting for inter-character gaps
- [ ] Write explanation section above
- [ ] Wiring diagram