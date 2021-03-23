#include <inttypes.h>
#define byte uint8_t

// Row x Column x Line
byte pixels[2][16][9] = {0};

// LCD pins
#define RS 4
#define EN 6
#define D4 7
#define D5 8
#define D6 9
#define D7 10

// CGRAM helper function headers
void display_pixel_array();
void write_cgram_at_offset(int offset);
void blink_characters_at_offset(int offset);

// LCD function headers
void lcd_init();
void lcd_write_empty();
void lcd_write_pixel_patch(int row, int col);
void lcd_write(byte value);
void lcd_command(byte value);
void lcd_write8bits(byte value);
void lcd_write4bits(byte value);

// LCD command macros
#define SET_CURSOR(_row,_col) lcd_command(0x80 | ((_col) + (_row)*0x40) )
#define START_CGRAM_WRITE(position) lcd_command(0x40 + position*0x08);

// Pulse enable pin macro
// Apparently (according to LiquidCrysta.cpp) commands need > 37us to settle,
// but I had it workign even without any delay. Nevertheless delay left at 50us // because not everyone may have such a forgiving LCD.
#define PULSE_ENABLE do {WRITE(EN, HIGH); delayMicroseconds(1); WRITE(EN, LOW);delayMicroseconds(100);} while (0)

// Helper macros to addres the pixel array per-pixel
// Here 'row' and 'col' are the indices of the individual pixels
// #define GET_LINE(_row,_col) pixels[_row/8][_col/5][_row%8]
#define GET_PIXEL(_row,_col) pixels[(_row)/9][(_col)/6][(_row)%9] >> (5-((_col)%6)) & 0x1
#define SET_PIXEL(_row,_col,_value) do {\
  pixels[(_row)/9][(_col)/6][(_row)%9] &= ~(0x1 << (5-((_col)%6)));\
  pixels[(_row)/9][(_col)/6][(_row)%9] |= ((_value) & 0x1) << (5-((_col)%6));\
} while (0)

// Set the pixel array to all zeros
#define ALL_ZEROS(...) memset(pixels, 0, sizeof pixels)

// Set character at (character index) row, col in the pixel array
#define SET_CHARACTER(_row,_col, B0,B1,B2,B3,B4,B5,B6,B7,B8) do {\
  pixels[_row][_col][0] = B0;\
  pixels[_row][_col][1] = B1;\
  pixels[_row][_col][2] = B2;\
  pixels[_row][_col][3] = B3;\
  pixels[_row][_col][4] = B4;\
  pixels[_row][_col][5] = B5;\
  pixels[_row][_col][6] = B6;\
  pixels[_row][_col][7] = B7;\
  pixels[_row][_col][8] = B8;\
} while (0)

// Debug character set
//
//  row | description
//  --------------------------------------------------------
//   0  |  Character row index in binary (right aligned)
//   1  |  Character column index in binary (right aligned)
//   2  |  empty
//   3  |  increments on each quartet in a line
//   4  |  increments on each line
//   5  |  increments on each set of characters
//   6  |  empty
//   7  |  empty

#define DEBUG_CHARACTER_SET_1(...) do {\
  /* Debug charater set 1 at positions 0, 4, 8 and 12 */\
  SET_CHARACTER(0,  0, 0,  0, 0, B10000,B10000,B10000,0,0);\
  SET_CHARACTER(0,  4, 0,  4, 0, B01000,B10000,B10000,0,0);\
  SET_CHARACTER(0,  8, 0,  8, 0, B00100,B10000,B10000,0,0);\
  SET_CHARACTER(0, 12, 0, 12, 0, B00010,B10000,B10000,0,0);\
  SET_CHARACTER(1,  0, 1,  0, 0, B10000,B01000,B10000,0,0);\
  SET_CHARACTER(1,  4, 1,  4, 0, B01000,B01000,B10000,0,0);\
  SET_CHARACTER(1,  8, 1,  8, 0, B00100,B01000,B10000,0,0);\
  SET_CHARACTER(1, 12, 1, 12, 0, B00010,B01000,B10000,0,0);\
  /* Debug charater set 2 at positions 1, 5, 8 and 13 */\
  SET_CHARACTER(0,  1, 0,  1, 0, B10000,B10000,B01000,0,0);\
  SET_CHARACTER(0,  5, 0,  5, 0, B01000,B10000,B01000,0,0);\
  SET_CHARACTER(0,  9, 0,  9, 0, B00100,B10000,B01000,0,0);\
  SET_CHARACTER(0, 13, 0, 13, 0, B00010,B10000,B01000,0,0);\
  SET_CHARACTER(1,  1, 1,  1, 0, B10000,B01000,B01000,0,0);\
  SET_CHARACTER(1,  5, 1,  5, 0, B01000,B01000,B01000,0,0);\
  SET_CHARACTER(1,  9, 1,  9, 0, B00100,B01000,B01000,0,0);\
  SET_CHARACTER(1, 13, 1, 13, 0, B00010,B01000,B01000,0,0);\
  /* Debug charater set 3 at positions 2, 6, 10 and 14 */\
  SET_CHARACTER(0,  2, 0,  2, 0, B10000,B10000,B00100,0,0);\
  SET_CHARACTER(0,  6, 0,  6, 0, B01000,B10000,B00100,0,0);\
  SET_CHARACTER(0, 10, 0, 10, 0, B00100,B10000,B00100,0,0);\
  SET_CHARACTER(0, 14, 0, 14, 0, B00010,B10000,B00100,0,0);\
  SET_CHARACTER(1,  2, 1,  2, 0, B10000,B01000,B00100,0,0);\
  SET_CHARACTER(1,  6, 1,  6, 0, B01000,B01000,B00100,0,0);\
  SET_CHARACTER(1, 10, 1, 10, 0, B00100,B01000,B00100,0,0);\
  SET_CHARACTER(1, 14, 1, 14, 0, B00010,B01000,B00100,0,0);\
  /* Debug charater set 4 at positions 3, 7, 11 and 15 */\
  SET_CHARACTER(0,  3, 0,  3, 0, B10000,B10000,B00010,0,0);\
  SET_CHARACTER(0,  7, 0,  7, 0, B01000,B10000,B00010,0,0);\
  SET_CHARACTER(0, 11, 0, 11, 0, B00100,B10000,B00010,0,0);\
  SET_CHARACTER(0, 15, 0, 15, 0, B00010,B10000,B00010,0,0);\
  SET_CHARACTER(1,  3, 1,  3, 0, B10000,B01000,B00010,0,0);\
  SET_CHARACTER(1,  7, 1,  7, 0, B01000,B01000,B00010,0,0);\
  SET_CHARACTER(1, 11, 1, 11, 0, B00100,B01000,B00010,0,0);\
  SET_CHARACTER(1, 15, 1, 15, 0, B00010,B01000,B00010,0,0);\
} while (0)

#define DEBUG_CHARACTER_SET_2(...) do {\
  /* Debug charater set 1 at positions 0, 4, 8 and 12 */\
  SET_CHARACTER(0,  0, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(0,  4, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(0,  8, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(0, 12, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(1,  0, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(1,  4, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(1,  8, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  SET_CHARACTER(1, 12, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B000001, B111111);\
  /* Debug charater set 2 at positions 1, 5, 8 and 13 */\
  SET_CHARACTER(0,  1, 0,  1, 0, B10000,B10000,B01000,0,0,0);\
  SET_CHARACTER(0,  5, 0,  5, 0, B01000,B10000,B01000,0,0,0);\
  SET_CHARACTER(0,  9, 0,  9, 0, B00100,B10000,B01000,0,0,0);\
  SET_CHARACTER(0, 13, 0, 13, 0, B00010,B10000,B01000,0,0,0);\
  SET_CHARACTER(1,  1, 1,  1, 0, B10000,B01000,B01000,0,0,0);\
  SET_CHARACTER(1,  5, 1,  5, 0, B01000,B01000,B01000,0,0,0);\
  SET_CHARACTER(1,  9, 1,  9, 0, B00100,B01000,B01000,0,0,0);\
  SET_CHARACTER(1, 13, 1, 13, 0, B00010,B01000,B01000,0,0,0);\
  /* Debug charater set 3 at positions 2, 6, 10 and 14 */\
  SET_CHARACTER(0,  2, 0,  2, 0, B10000,B10000,B00100,0,0,0);\
  SET_CHARACTER(0,  6, 0,  6, 0, B01000,B10000,B00100,0,0,0);\
  SET_CHARACTER(0, 10, 0, 10, 0, B00100,B10000,B00100,0,0,0);\
  SET_CHARACTER(0, 14, 0, 14, 0, B00010,B10000,B00100,0,0,0);\
  SET_CHARACTER(1,  2, 1,  2, 0, B10000,B01000,B00100,0,0,0);\
  SET_CHARACTER(1,  6, 1,  6, 0, B01000,B01000,B00100,0,0,0);\
  SET_CHARACTER(1, 10, 1, 10, 0, B00100,B01000,B00100,0,0,0);\
  SET_CHARACTER(1, 14, 1, 14, 0, B00010,B01000,B00100,0,0,0);\
  /* Debug charater set 4 at positions 3, 7, 11 and 15 */\
  SET_CHARACTER(0,  3, 0,  3, 0, B10000,B10000,B00010,0,0,0);\
  SET_CHARACTER(0,  7, 0,  7, 0, B01000,B10000,B00010,0,0,0);\
  SET_CHARACTER(0, 11, 0, 11, 0, B00100,B10000,B00010,0,0,0);\
  SET_CHARACTER(0, 15, 0, 15, 0, B00010,B10000,B00010,0,0,0);\
  SET_CHARACTER(1,  3, 1,  3, 0, B10000,B01000,B00010,0,0,0);\
  SET_CHARACTER(1,  7, 1,  7, 0, B01000,B01000,B00010,0,0,0);\
  SET_CHARACTER(1, 11, 1, 11, 0, B00100,B01000,B00010,0,0,0);\
  SET_CHARACTER(1, 15, 1, 15, 0, B00010,B01000,B00010,0,0,0);\
} while (0)