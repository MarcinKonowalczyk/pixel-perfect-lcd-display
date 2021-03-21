
#include "pixel-perfect.h"
#include <Arduino.h>
#include "fastio.h"

byte pixels[2][16][8] = {0};

#define RS 6
#define EN 7
#define D4 8
#define D5 9
#define D6 12
#define D7 13
const uint8_t data_pins[4] = {D4,D5,D6,D7};

const int n = 16;
int h[16] = {0}; // history

#define symbol full
#define wipe if (1) {for (int i=0; i<8; i++) {lcd_write(symbol[i]); delay(10);}}

#define SET_CURSOR(col,row) lcd_command(0x80 | (col + row*0x40) )
#define START_CGRAM_WRITE(position) lcd_command(0x40 + position*0x08);

// LCD initialisation from LiquidCrystal.cpp
void lcd_init() {

  pixels[0][0][0]  = B10000;
  pixels[0][4][0]  = B01000;
  pixels[0][8][0]  = B00100;
  pixels[0][12][0] = B00010;
  pixels[1][0][0]  = B10001;
  pixels[1][4][0]  = B01001;
  pixels[1][8][0]  = B00101;
  pixels[1][12][0] = B00011;

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
  lcd_command(LCD_FUNCTIONSET | LCD_2LINE);
  lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
  // lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);

  lcd_command(0x01); // Clear display
  delay(2); // Useful after the clear to reduce screen glitches

  for (int j=0; j < 8; j++) {
    START_CGRAM_WRITE(j);
    lcd_write_empty();
  }
}

void setup() {
  pinMode(A5,INPUT);

  lcd_init();

  // Blank both positions
  SET_CURSOR(0,1); lcd_write(byte(0x20));
  SET_CURSOR(1,1); lcd_write(byte(0x20));
  SET_CURSOR(2,1); lcd_write(byte(0x20));
  SET_CURSOR(3,1); lcd_write(byte(0x20));
}

void loop() {
  // Characters in positions 0, 4, 8, 12, 16, 20, 24 and 28
  int offset = 0;
  for (int j=0; j < 4; j++) {
    START_CGRAM_WRITE(j);
    lcd_write_pixel_patch(0, 4*j+offset);
  }
  // for (int j=0; j < 4; j++) {
  //   START_CGRAM_WRITE(j+4);
  //   lcd_write_pixel_patch(1, 4*j+offset);
  // }

  // DOESN'T:
  // #pragma unroll (4)

  for (int j=0; j < 4; j++ ) {
    START_CGRAM_WRITE(j+4);
    lcd_write_pixel_patch(1, 4*j);
  }

  // WORKS:
  // START_CGRAM_WRITE(4);
  // lcd_write_pixel_patch(1, 0);

  // START_CGRAM_WRITE(5);
  // lcd_write_pixel_patch(1, 4);

  // START_CGRAM_WRITE(6);
  // lcd_write_pixel_patch(1, 8);

  // START_CGRAM_WRITE(7);
  // lcd_write_pixel_patch(1, 12);


  // for (int j=0; j < 4; j++) {
  //   SET_CURSOR(4*j+offset,0); lcd_write(byte(j));
  // }
  // // for (int j=0; j < 4; j++) {
  // //   SET_CURSOR(4*j+offset,1); lcd_write(byte(j+4));
  // // }

  // delay(20);

  // for (int j=0; j < 4; j++) {
  //   SET_CURSOR(4*j+offset,0); lcd_write(byte(0x20));
  // }
  // for (int j=0; j < 4; j++) {
  //   SET_CURSOR(4*j+offset,1); lcd_write(byte(0x20));
  // }

  SET_CURSOR(0,1); lcd_write(byte(0));
  SET_CURSOR(1,1); lcd_write(byte(1));
  SET_CURSOR(2,1); lcd_write(byte(2));
  SET_CURSOR(3,1); lcd_write(byte(3));
  SET_CURSOR(4,1); lcd_write(byte(4));
  SET_CURSOR(5,1); lcd_write(byte(5));
  SET_CURSOR(6,1); lcd_write(byte(6));
  SET_CURSOR(7,1); lcd_write(byte(7));
}

void lcd_write_empty() {
  WRITE(RS,HIGH);
  for (int k=0;k<8;k++) {
    byte value = B00000;
    lcd_write4bits(value>>4);
    lcd_write4bits(value);
  }
}

void lcd_write_pixel_patch(int row, int col) {
  WRITE(RS,HIGH);
  for (int line=0;line<8;line++) {
    byte value = pixels[row][col][line];
    lcd_write4bits(value>>4);
    lcd_write4bits(value);
  }
}

void lcd_write(byte value) {
  WRITE(RS,HIGH);
  lcd_write4bits(value>>4);
  lcd_write4bits(value);
}

void lcd_command(byte value) {
  WRITE(RS,LOW);
  lcd_write4bits(value>>4);
  lcd_write4bits(value);
}

void lcd_write4bits(byte value) {
  // TODO: faster to and with 0x1, 0x2, 0x4, 0x8 ?
  // TODO: write_8_bits version simpler?
  WRITE(D4,(value >> 0) & 0x01);
  WRITE(D5,(value >> 1) & 0x01);
  WRITE(D6,(value >> 2) & 0x01);
  WRITE(D7,(value >> 3) & 0x01);

  // Pulse enable pin
  WRITE(EN, HIGH);
  delayMicroseconds(1); // enable pulse must be >450ns
  WRITE(EN, LOW);
  delayMicroseconds(50); // commands need > 37us to settle
}





  // // Characters in positions 2 and 6
  // START_CGRAM_WRITE(0);
  // for (int i=0; i<8; i++) {lcd_write(two[i]);}
  // START_CGRAM_WRITE(1);
  // for (int i=0; i<8; i++) {lcd_write(six[i]);}

  // SET_CURSOR(1,0); lcd_write(byte(0x0));
  // SET_CURSOR(5,0); lcd_write(byte(0x1));
  // delay(20);
  // SET_CURSOR(1,0); lcd_write(byte(0x20));
  // SET_CURSOR(5,0); lcd_write(byte(0x20));

  // // Characters in positions 3 and 7
  // START_CGRAM_WRITE(0);
  // for (int i=0; i<8; i++) {lcd_write(three[i]);}
  // START_CGRAM_WRITE(1);
  // for (int i=0; i<8; i++) {lcd_write(seven[i]);}

  // SET_CURSOR(2,0); lcd_write(byte(0x0));
  // SET_CURSOR(6,0); lcd_write(byte(0x1));
  // delay(20);
  // SET_CURSOR(2,0); lcd_write(byte(0x20));
  // SET_CURSOR(6,0); lcd_write(byte(0x20));

  // // Characters in positions 4 and 8
  // START_CGRAM_WRITE(0);
  // for (int i=0; i<8; i++) {lcd_write(four[i]);}
  // START_CGRAM_WRITE(1);
  // for (int i=0; i<8; i++) {lcd_write(eight[i]);}

  // SET_CURSOR(3,0); lcd_write(byte(0x0));
  // SET_CURSOR(7,0); lcd_write(byte(0x1));
  // delay(20);
  // SET_CURSOR(3,0); lcd_write(byte(0x20));
  // SET_CURSOR(7,0); lcd_write(byte(0x20));