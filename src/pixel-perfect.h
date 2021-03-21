#include <inttypes.h>
#define byte uint8_t
// #include <binary.h>

#define RS 6
#define EN 7
#define D4 8
#define D5 9
#define D6 12
#define D7 13

#define wipe if (1) {for (int i=0; i<8; i++) {lcd_write(symbol[i]); delay(10);}}

#define SET_CURSOR(col,row) lcd_command(0x80 | (col + row*0x40) )
#define START_CGRAM_WRITE(position) lcd_command(0x40 + position*0x08);

void lcd_init();

void lcd_write_empty();
void lcd_write_pixel_patch(int row, int col);

void lcd_write(byte value);
void lcd_command(byte value);
void lcd_write4bits(byte value);
void lcd_write8bits(byte value);


// lcd commands
// remains of LiquidCrystal.h

// #define LCD_CLEARDISPLAY 0x01
// #define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
// #define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
// #define LCD_SETCGRAMADDR 0x40
// #define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
// #define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
// #define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
// #define LCD_DISPLAYOFF 0x00
// #define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
// #define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
// #define LCD_DISPLAYMOVE 0x08
// #define LCD_CURSORMOVE 0x00
// #define LCD_MOVERIGHT 0x04
// #define LCD_MOVELEFT 0x00

// flags for function set
// #define LCD_8BITMODE 0x10
// #define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
// #define LCD_1LINE 0x00
// #define LCD_5x10DOTS 0x04
// #define LCD_5x8DOTS 0x00