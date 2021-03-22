#include <Arduino.h>

#include "pixel-perfect.h"
#include "fastio.h"

void setup() {
  // SET_DEBUG_CHARACTERS();
  lcd_init();
}

const int period = 5000;
byte prev_value = 7;

void loop() {
  // Oscillation between 0 and 16
  byte value = (byte) ( (sin(2*PI*millis()/period)+1)*8 );
  
  // Shift the pixel array right
  for (int col=79; col>0; col--) {
    for (int row=0; row < 16; row++) {
      SET_PIXEL(row,col,GET_PIXEL(row,col-1));
    }
  }

  // Zero out column 0
  for (int row=0; row<16; row++) {
    SET_PIXEL(row,0,0);
  }

  // Set new pixel value
  SET_PIXEL(value,0,1);

  // Crude anti-alias
  for (int row = min(prev_value+1,value); row < max(value,prev_value); row++) {
    SET_PIXEL(row,0,1);
  }

  display_pixel_array();
  prev_value = value;
}

///////////////////
// CGRAM helpers //
///////////////////

// #define DO_OFFSET(offset) do { write_cgram_at_offset(offset); blink_characters_at_offset(offset); } while (0)

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

// LCD initialisation based on LiquidCrystal.cpp
void lcd_init() {
  SET_OUTPUT(RS); SET_OUTPUT(EN);
  SET_OUTPUT(D4); SET_OUTPUT(D5); SET_OUTPUT(D6); SET_OUTPUT(D7);
  delay(50); // this is necessary
  WRITE(RS, LOW);
  WRITE(EN, LOW);
  
  // lcd starts in 8bit mode, try to set 4 bit mode
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
  // lcd_command(0x04 | 0x02); // LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT

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