#pragma once

#include <avr/io.h>

#define F_CPU 16000000UL

#include <util/delay.h>

#define EEPROM_BASE  0x810000L

//------------------------------------------------------------------------
// Fuses of ATmega32 (just for documentation)
//------------------------------------------------------------------------
// CKSEL3...1 = 1111 (not blown = 16 MHz external crystal)
// CKOPT = 0 (blown = high output swing)
// SUT1..0 = 01 (blown + not blown)
// BODLEVEL = 0 (blown = reset at 4V)
//------------------------------------------------------------------------
// Fuses of ATmega644p (just for documentation)
//------------------------------------------------------------------------
// Lock Bit Byte
//  7       6       5       4       3       2       1       0 
//  ------  ------  ------  ------  ------  ------  ------  ------
//  1       1       1       1       1       1       1       1       default, not locked
//
// Extended Fuse Byte:
//  7       6       5       4       3       2       1       0 
//  ------  ------  ------  ------  ------  ------  ------  ------
//                                          BOD2    BOD1    BOD0    111=no BOD (*), 101=2,7V, 100=4,3V
//  1       1       1       1       1       1       0       0
//
// Fuse High Byte:
//  7       6       5       4       3       2       1       0 
//  ------  ------  ------  ------  ------  ------  ------  ------
//  OCDEN   JTAGEN  SPIEN   WDTON   EESAVE  BOOTSZ1 BOOTSZ0 BOOTRST
//  1       1       0       1       1       1       0       0
//  disabl  disabl  enabl   default no save =0x7C00         enter boot
//
// Fuse Low Byte:
//  7       6       5       4       3       2       1       0 
//  ------  ------  ------  ------  ------  ------  ------  ------
//  CKDIV8  CKOUT   SUT1    SUT0    CKSEL3  CKSEL2  CKSEL1  CKSEL0
//  1       1       0       1       0       1       1       1
//  no div  no clk  crystal, BOD ena, 16 MHz    



// Port A
#define S88DIN1    0     // in
#define S88DIN2    1     // in
#define S88DIN3    2     // in
#define EXT_STOP   3     // in
#define LED_GO     4     // out
#define LED_STOP   5     // out
#define KEY_STOP   6     // in
#define KEY_GO     7     // in
// Port B
#define S88_CLOCK  0     // out
#define S88_RESET  1     // out
#define ACK_DETECT 2     // in    this is only a short pulse -> use int!
#define S88_LOAD   3     // out
#define DMX_OUT    4     // out
#define MY_CTS     6     // out
#define MY_RTS     7     // in
// Port C
#define SHORT_MAIN 0     // in
#define SHORT_PROG 1     // in
#define LED_CTRL   6     // out
#define LED_RS232  7     // out
// Port D
#define MY_RXD     0     // in
#define MY_TXD     1     // out
#define JUMPER1    2     // in
#define JUMPER2    3     // in
#define NDCC       4     // out
#define DCC        5     // out
#define ENA_PROG   6     // out  high = enable programming
#define ENA_MAIN   7     // out  high = enable main

//------------------------------------------------------------------------
// LED-Control and IO-Macros

#define LED_CTRL_ON      PORTC &= ~(1<<LED_CTRL)
#define LED_CTRL_OFF     PORTC |= (1<<LED_CTRL)
#define LED_RS232_ON     PORTC &= ~(1<<LED_RS232)
#define LED_RS232_OFF    PORTC |= (1<<LED_RS232)
#define LED_GO_ON        PORTA &= ~(1<<LED_GO)
#define LED_GO_OFF       PORTA |= (1<<LED_GO)
#define LED_STOP_ON      PORTA &= ~(1<<LED_STOP)
#define LED_STOP_OFF     PORTA |= (1<<LED_STOP)

// Note LEDs are active low -> state on == pin low!
#define LED_CTRL_STATE   (!(PINC & (1<<LED_CTRL)))
#define LED_RS232_STATE  (!(PINC & (1<<LED_RS232)))
#define LED_GO_STATE     (!(PINA & (1<<LED_GO)))
#define LED_STOP_STATE   (!(PINA & (1<<LED_STOP)))

#define MY_CTS_HIGH      PORTB |= (1<<MY_CTS)
#define MY_CTS_LOW       PORTB &= ~(1<<MY_CTS)
#define MY_CTS_STATE     (PORTB & (1<<MY_CTS))

#define DMX_OUT_HIGH     PORTB |= (1<<DMX_OUT)
#define DMX_OUT_LOW      PORTB &= ~(1<<DMX_OUT)

// this results after compile: sbi(PORTB,MY_CTS)

#define MAIN_TRACK_ON    PORTD |= (1<<ENA_MAIN)
#define MAIN_TRACK_OFF   PORTD &= ~(1<<ENA_MAIN)
#define MAIN_TRACK_STATE (PIND & (1<<ENA_MAIN))
#define PROG_TRACK_ON    PORTD |= (1<<ENA_PROG)
#define PROG_TRACK_OFF   PORTD &= ~(1<<ENA_PROG)
#define PROG_TRACK_STATE (PIND & (1<<ENA_PROG))

// Abfragen:
#define MAIN_IS_SHORT    (!(PINC & (1<<SHORT_MAIN)))
#define PROG_IS_SHORT    (!(PINC & (1<<SHORT_PROG)))
#define ACK_IS_DETECTED  ( (PINB & (1<<ACK_DETECT)))
#define KEY_GO_PRESSED   (!(PINA & (1<<KEY_GO)))
#define KEY_STOP_PRESSED (!(PINA & (1<<KEY_STOP)))
#define EXT_STOP_PRESSED (!(PINA & (1<<EXT_STOP)))

// RTS is aktive low
#define MY_RTS_DETECT    (!(PINB & (1<<MY_RTS)))

#define CHECK_JUMPER1    (!(PIND & (1<<JUMPER1)))
#define CHECK_JUMPER2    (!(PIND & (1<<JUMPER2)))

// S88:
#define S88_LOAD_HIGH    PORTB |= (1<<S88_LOAD)
#define S88_LOAD_LOW     PORTB &= ~(1<<S88_LOAD)
#define S88_RESET_HIGH   PORTB |= (1<<S88_RESET)
#define S88_RESET_LOW    PORTB &= ~(1<<S88_RESET)
#define S88_CLOCK_HIGH   PORTB |= (1<<S88_CLOCK)
#define S88_CLOCK_LOW    PORTB &= ~(1<<S88_CLOCK)

#define READ_S88_D1      ((PINA >> S88DIN1) & 0x01)
#define READ_S88_D2      ((PINA >> S88DIN2) & 0x01)
#define READ_S88_D3      ((PINA >> S88DIN3) & 0x01)
