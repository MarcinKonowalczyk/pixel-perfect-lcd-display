#include <Arduino.h>

#include "pixel-perfect.h"
#include "fastio.h"

// Row x Column x Line
byte pixels[2][16][8] = {0};

// Helper macros to addres the pixel array per-pixel
// Here 'row' and 'col' are the indices of the individual pixels
#define GET_PIXEL(row,col) (pixels[row/8][col/5][row%8] >> 4-(col%5)) & 0x1
#define SET_PIXEL(row,col,value) pixels[row/8][col/5][row%8] = (value & 0x1) << (4-(col%5))

void setup() {
  set_debug_characters();

  // To test whether SET_ and GET_PIXEL macros work
  // SET_PIXEL(8,5,1);
  // SET_PIXEL(8,4,GET_PIXEL(8,5));

  SET_INPUT(IN);
  // pinMode(A5,INPUT);

  lcd_init();

  // Blank both positions
  SET_CURSOR(1,0); lcd_write(byte(0x20));
  SET_CURSOR(1,1); lcd_write(byte(0x20));
  SET_CURSOR(1,2); lcd_write(byte(0x20));
  SET_CURSOR(1,3); lcd_write(byte(0x20));
}

void loop() {
  display_pixel_array();
}

// Debug character set

void set_debug_characters() {
  // Debug charater set 1
  pixels[0][0][2]  = B10000;
  pixels[0][4][2]  = B01000;
  pixels[0][8][2]  = B00100;
  pixels[0][12][2] = B00010;
  pixels[0][0][3]  = B10000;
  pixels[0][4][3]  = B10000;
  pixels[0][8][3]  = B10000;
  pixels[0][12][3] = B10000;
  pixels[0][0][4]  = B10000;
  pixels[0][4][4]  = B10000;
  pixels[0][8][4]  = B10000;
  pixels[0][12][4] = B10000;

  pixels[1][0][2]  = B10000;
  pixels[1][4][2]  = B01000;
  pixels[1][8][2]  = B00100;
  pixels[1][12][2] = B00010;
  pixels[1][0][3]  = B01000;
  pixels[1][4][3]  = B01000;
  pixels[1][8][3]  = B01000;
  pixels[1][12][3] = B01000;
  pixels[1][0][4]  = B10000;
  pixels[1][4][4]  = B10000;
  pixels[1][8][4]  = B10000;
  pixels[1][12][4] = B10000;

  // Debug charater set 2
  pixels[0][1][2]  = B10000;
  pixels[0][5][2]  = B01000;
  pixels[0][9][2]  = B00100;
  pixels[0][13][2] = B00010;
  pixels[0][1][3]  = B10000;
  pixels[0][5][3]  = B10000;
  pixels[0][9][3]  = B10000;
  pixels[0][13][3] = B10000;
  pixels[0][1][4]  = B01000;
  pixels[0][5][4]  = B01000;
  pixels[0][9][4]  = B01000;
  pixels[0][13][4] = B01000;

  pixels[1][1][2]  = B10000;
  pixels[1][5][2]  = B01000;
  pixels[1][9][2]  = B00100;
  pixels[1][13][2] = B00010;
  pixels[1][1][3]  = B01000;
  pixels[1][5][3]  = B01000;
  pixels[1][9][3]  = B01000;
  pixels[1][13][3] = B01000;
  pixels[1][1][4]  = B01000;
  pixels[1][5][4]  = B01000;
  pixels[1][9][4]  = B01000;
  pixels[1][13][4] = B01000;

  // Debug charater set 3
  pixels[0][2][2]  = B10000;
  pixels[0][6][2]  = B01000;
  pixels[0][10][2] = B00100;
  pixels[0][14][2] = B00010;
  pixels[0][2][3]  = B10000;
  pixels[0][6][3]  = B10000;
  pixels[0][10][3] = B10000;
  pixels[0][14][3] = B10000;
  pixels[0][2][4]  = B00100;
  pixels[0][6][4]  = B00100;
  pixels[0][10][4] = B00100;
  pixels[0][14][4] = B00100;

  pixels[1][2][2]  = B10000;
  pixels[1][6][2]  = B01000;
  pixels[1][10][2] = B00100;
  pixels[1][14][2] = B00010;
  pixels[1][2][3]  = B01000;
  pixels[1][6][3]  = B01000;
  pixels[1][10][3] = B01000;
  pixels[1][14][3] = B01000;
  pixels[1][2][4]  = B00100;
  pixels[1][6][4]  = B00100;
  pixels[1][10][4] = B00100;
  pixels[1][14][4] = B00100;

  // Debug charater set 4
  pixels[0][3][2]  = B10000;
  pixels[0][7][2]  = B01000;
  pixels[0][11][2] = B00100;
  pixels[0][15][2] = B00010;
  pixels[0][3][3]  = B10000;
  pixels[0][7][3]  = B10000;
  pixels[0][11][3] = B10000;
  pixels[0][15][3] = B10000;
  pixels[0][3][4]  = B00010;
  pixels[0][7][4]  = B00010;
  pixels[0][11][4] = B00010;
  pixels[0][15][4] = B00010;

  pixels[1][3][2]  = B10000;
  pixels[1][7][2]  = B01000;
  pixels[1][11][2] = B00100;
  pixels[1][15][2] = B00010;
  pixels[1][3][3]  = B01000;
  pixels[1][7][3]  = B01000;
  pixels[1][11][3] = B01000;
  pixels[1][15][3] = B01000;
  pixels[1][3][4]  = B00010;
  pixels[1][7][4]  = B00010;
  pixels[1][11][4] = B00010;
  pixels[1][15][4] = B00010;
}

///////////////////
// CGRAM helpers //
///////////////////

void display_pixel_array() {
  for (int offset = 0; offset < 4; offset++) {
    write_cgram_at_offset(offset);
    blink_characters_at_offset(offset);
  }
}

void write_cgram_at_offset(int offset) {
  for (int j=0; j < 8; j++) {
    START_CGRAM_WRITE(j);
    lcd_write_pixel_patch( j/4, (4*j+offset)%16 );
  }
}

void blink_characters_at_offset(int offset) {
  for (int j=0; j < 8; j++) {
    SET_CURSOR( j/4, (4*j+offset)%16 );
    lcd_write(byte(j));
  }
  delay(20);
  for (int j=0; j < 8; j++) {
    SET_CURSOR( j/4, (4*j+offset)%16 );
    lcd_write(byte(0x20));
  }
}

///////////////////
// LCD functions //
///////////////////

// LCD initialisation from LiquidCrystal.cpp
void lcd_init() {
  SET_OUTPUT(RS); SET_OUTPUT(EN);
  SET_OUTPUT(D4); SET_OUTPUT(D5); SET_OUTPUT(D6); SET_OUTPUT(D7);
  delay(50); // This is necessary
  WRITE(RS, LOW);
  WRITE(EN, LOW);
  
  // we start in 8bit mode, try to set 4 bit mode
  lcd_write4bits(0x03);
  delayMicroseconds(4500); // wait min 4.1ms

  // second try
  lcd_write4bits(0x03);
  delayMicroseconds(4500); // wait min 4.1ms
  
  // third go!
  lcd_write4bits(0x03); 
  delayMicroseconds(150);

  // finally, set to 4-bit interface
  lcd_write4bits(0x02); 

  // // finally, set # lines, font size, etc.
  lcd_command(0x20 | 0x08); // LCD_FUNCTIONSET | LCD_2LINE
  lcd_command(0x08 | 0x04); // LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF

  lcd_command(0x01); // Clear display
  delay(2); // Useful after the clear to reduce screen glitches

  for (int j=0; j < 8; j++) {
    START_CGRAM_WRITE(j);
    lcd_write_empty();
  }
}

void lcd_write_empty() {
  WRITE(RS,HIGH);
  for (int k=0;k<8;k++) {
    byte value = B00000;
    lcd_write8bits(value);
  }
}

void lcd_write_pixel_patch(int row, int col) {
  WRITE(RS,HIGH);
  for (int line=0;line<8;line++) {
    lcd_write8bits(pixels[row][col][line]);
  }
}

void lcd_write(byte value) {
  WRITE(RS,HIGH);
  lcd_write8bits(value);
}

void lcd_command(byte value) {
  WRITE(RS,LOW);
  lcd_write8bits(value);
}

// Pulse enable pin
// commands need > 37us to settle
# define PULSE_ENABLE do {WRITE(EN, HIGH); delayMicroseconds(1); WRITE(EN, LOW); delayMicroseconds(50);} while (0)

void lcd_write8bits(byte value) {
  WRITE(D4,(value >> 4) & 0x1);
  WRITE(D5,(value >> 5) & 0x1);
  WRITE(D6,(value >> 6) & 0x1);
  WRITE(D7,(value >> 7) & 0x1);

  PULSE_ENABLE;

  WRITE(D4,(value >> 0) & 0x1);
  WRITE(D5,(value >> 1) & 0x1);
  WRITE(D6,(value >> 2) & 0x1);
  WRITE(D7,(value >> 3) & 0x1);

  PULSE_ENABLE;
}

void lcd_write4bits(byte value) {
  // TODO: faster to and with 0x1, 0x2, 0x4, 0x8 ?
  // TODO: write_8_bits version simpler?
  WRITE(D4,(value >> 0) & 0x1);
  WRITE(D5,(value >> 1) & 0x1);
  WRITE(D6,(value >> 2) & 0x1);
  WRITE(D7,(value >> 3) & 0x1);

  // Pulse enable pin
  WRITE(EN, HIGH);
  delayMicroseconds(1); // enable pulse must be >450ns
  WRITE(EN, LOW);
  delayMicroseconds(50); // commands need > 37us to settle
}