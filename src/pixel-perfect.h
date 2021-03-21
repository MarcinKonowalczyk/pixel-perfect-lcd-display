#include <inttypes.h>
#define byte uint8_t

#define RS 6
#define EN 7
#define D4 8
#define D5 9
#define D6 12
#define D7 13

#define SET_CURSOR(row,col) lcd_command(0x80 | (col + row*0x40) )
#define START_CGRAM_WRITE(position) lcd_command(0x40 + position*0x08);

void lcd_init();

void lcd_write_empty();
void lcd_write_pixel_patch(int row, int col);

void lcd_write(byte value);
void lcd_command(byte value);
void lcd_write4bits(byte value);
void lcd_write8bits(byte value);

void set_debug_characters();
void write_cgram_at_offset(int offset);
void blink_characters_at_offset(int offset);
void display_pixel_array();

// lcd commands
// remains of LiquidCrystal.h

// #define LCD_ENTRYMODESET 0x04
// #define LCD_DISPLAYCONTROL 0x08
// #define LCD_FUNCTIONSET 0x20

// #define LCD_ENTRYLEFT 0x02
// #define LCD_ENTRYSHIFTINCREMENT 0x01

// #define LCD_DISPLAYON 0x04
// #define LCD_CURSOROFF 0x00
// #define LCD_BLINKOFF 0x00

// #define LCD_2LINE 0x08

// LCD pins
#define RS 6
#define EN 7
#define D4 8
#define D5 9
#define D6 12
#define D7 13
#define IN 19 // A5 input