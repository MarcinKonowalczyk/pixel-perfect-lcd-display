/*
  Macro-only fast IO functions.
  Based on code by Triffid_Hunter and modified by Kliment and Marcin
  now you can simply SET_OUTPUT(STEP); WRITE(STEP, 1); WRITE(STEP, 0);
*/

#pragma once

/*
  MACROS
  these used to sit in "macros.h", but there are just two so lets keep them here and #ifndef protect them
*/

// Disable and reenable interrupts on the AVR
#include <avr/interrupt.h> //for cli() and sei()
#ifndef CRITICAL_SECTION_START
  #define CRITICAL_SECTION_START unsigned char oldSREG = SREG; cli();
  #define CRITICAL_SECTION_END SREG = oldSREG;
#endif

#ifndef _BV // Bitmask value
  #define _BV(n) (1<<(n))
#endif

/*
  READ AND WRITE FUNCTIONALITY
  Pins > 0x100 are used. These are not converted to atomic actions. An critical section is needed.
   - Why the do {...} while (0)? see: https://stackoverflow.com/a/154264/2531987
   - What are the ##'s ? see: https://gcc.gnu.org/onlinedocs/cpp/Concatenation.html
*/

/// Read a pin
#define _READ(IO) ((bool)(DIO ## IO ## _RPORT & _BV(DIO ## IO ## _PIN)))

// Non-critical write
#define _WRITE_NC(IO, v) \
do { \
  if (v) { \
    DIO ## IO ## _WPORT |= _BV(DIO ## IO ## _PIN); \
  } else { \
    DIO ## IO ## _WPORT &= ~_BV(DIO ## IO ## _PIN); \
  }\
} while (0)

// Critical write
#define _WRITE_C(IO, v) \
do { \
  if (v) { \
    CRITICAL_SECTION_START; \
    {DIO ## IO ## _WPORT |= _BV(DIO ## IO ## _PIN);}\
    CRITICAL_SECTION_END; \
  } else { \
    CRITICAL_SECTION_START; \
    {DIO ## IO ## _WPORT &= ~_BV(DIO ## IO ## _PIN);}\
    CRITICAL_SECTION_END; \
  }\
} while (0)

#define _WRITE(IO, v) \
do { \
  if (&(DIO ## IO ## _RPORT) >= (uint8_t *)0x100) { \
    _WRITE_C(IO, v); \
  } else { \
    _WRITE_NC(IO, v); \
  }; \
} while(0)

/// toggle a pin
#define _TOGGLE(IO) \
do { \
  DIO ## IO ## _RPORT = _BV(DIO ## IO ## _PIN); \
} while (0)

/// set pin as input
#define _SET_INPUT(IO) \
do { \
  DIO ## IO ## _DDR &= ~_BV(DIO ## IO ## _PIN); \
} while (0)

/// set pin as output
#define _SET_OUTPUT(IO) \
do { \
  DIO ## IO ## _DDR |= _BV(DIO ## IO ## _PIN); \
} while (0)

/*
  WRAPPERS
  why double up on these macros?
  see https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
*/

#define READ(IO) _READ(IO)
#define WRITE(IO, v) _WRITE(IO, v)
#define TOGGLE(IO) _TOGGLE(IO)
#define SET_INPUT(IO) _SET_INPUT(IO)
#define SET_OUTPUT(IO) _SET_OUTPUT(IO)

/*
 PORTS AND FUNCTIONS
  Pinout for the AVR ATmega328P processor (Arduino UNO) only
  https://www.arduino.cc/en/Hacking/PinMapping168
*/

#ifndef __AVR_ATmega328P__
  #error This pinout works only for the AVR ATmega328P processor (Arduino UNO)
#endif

// pins
#define DIO0_PIN   PIND0
#define DIO0_RPORT PIND
#define DIO0_WPORT PORTD
#define DIO0_DDR   DDRD

#define DIO1_PIN   PIND1
#define DIO1_RPORT PIND
#define DIO1_WPORT PORTD
#define DIO1_DDR   DDRD

#define DIO2_PIN   PIND2
#define DIO2_RPORT PIND
#define DIO2_WPORT PORTD
#define DIO2_DDR   DDRD

#define DIO3_PIN   PIND3
#define DIO3_RPORT PIND
#define DIO3_WPORT PORTD
#define DIO3_DDR   DDRD

#define DIO4_PIN   PIND4
#define DIO4_RPORT PIND
#define DIO4_WPORT PORTD
#define DIO4_DDR   DDRD

#define DIO5_PIN   PIND5
#define DIO5_RPORT PIND
#define DIO5_WPORT PORTD
#define DIO5_DDR   DDRD

#define DIO6_PIN   PIND6
#define DIO6_RPORT PIND
#define DIO6_WPORT PORTD
#define DIO6_DDR   DDRD

#define DIO7_PIN   PIND7
#define DIO7_RPORT PIND
#define DIO7_WPORT PORTD
#define DIO7_DDR   DDRD

#define DIO8_PIN   PINB0
#define DIO8_RPORT PINB
#define DIO8_WPORT PORTB
#define DIO8_DDR   DDRB

#define DIO9_PIN   PINB1
#define DIO9_RPORT PINB
#define DIO9_WPORT PORTB
#define DIO9_DDR   DDRB

#define DIO10_PIN   PINB2
#define DIO10_RPORT PINB
#define DIO10_WPORT PORTB
#define DIO10_DDR   DDRB

#define DIO11_PIN   PINB3
#define DIO11_RPORT PINB
#define DIO11_WPORT PORTB
#define DIO11_DDR   DDRB

#define DIO12_PIN   PINB4
#define DIO12_RPORT PINB
#define DIO12_WPORT PORTB
#define DIO12_DDR   DDRB

#define DIO13_PIN   PINB5
#define DIO13_RPORT PINB
#define DIO13_WPORT PORTB
#define DIO13_DDR   DDRB

// Analog pins 0->5
#define DIO14_PIN   PINC0
#define DIO14_RPORT PINC
#define DIO14_WPORT PORTC
#define DIO14_DDR   DDRC

#define DIO15_PIN   PINC0
#define DIO15_RPORT PINC
#define DIO15_WPORT PORTC
#define DIO15_DDR   DDRC

#define DIO16_PIN   PINC1
#define DIO16_RPORT PINC
#define DIO16_WPORT PORTC
#define DIO16_DDR   DDRC

#define DIO17_PIN   PINC2
#define DIO17_RPORT PINC
#define DIO17_WPORT PORTC
#define DIO17_DDR   DDRC

#define DIO18_PIN   PINC3
#define DIO18_RPORT PINC
#define DIO18_WPORT PORTC
#define DIO18_DDR   DDRC

#define DIO19_PIN   PINC4
#define DIO19_RPORT PINC
#define DIO19_WPORT PORTC
#define DIO19_DDR   DDRC