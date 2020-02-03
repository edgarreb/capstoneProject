/*************************************************************
*       at328-5.c - Demonstrate interface to a parallel LCD display
*
*       This program will print a message on an LCD display
*       using the 4-bit wide interface method
*
*       PORTB, bit 4 (0x10) - output to RS (Register Select) input of display
*              bit 3 (0x08) - output to R/W (Read/Write) input of display
*              bit 2 (0x04) - output to E (Enable) input of display
*       PORTB, bits 0-1, PORTD, bits 2-7 - Outputs to DB0-DB7 inputs of display.
*
*       The second line of the display starts at address 0x40.
*
* Revision History
* Date     Author      Description
* 11/17/05 A. Weber    First Release for 8-bit interface
* 01/13/06 A. Weber    Modified for CodeWarrior 5.0
* 08/25/06 A. Weber    Modified for JL16 processor
* 05/08/07 A. Weber    Some editing changes for clarification
* 06/25/07 A. Weber    Updated name of direct page segment
* 08/17/07 A. Weber    Incorporated changes to demo board
* 01/15/08 A. Weber    More changes to the demo board
* 02/12/08 A. Weber    Changed 2nd LCD line address
* 04/22/08 A. Weber    Added "one" variable to make warning go away
* 04/15/11 A. Weber    Adapted for ATmega168
* 01/30/12 A. Weber    Moved the LCD strings into ROM
* 02/27/12 A. Weber    Corrected port bit assignments above
* 11/18/13 A. Weber    Renamed for ATmega328P
* 04/10/15 A. Weber    Extended "E" pulse, renamed strout to strout_P
* 05/06/17 A. Weber    Change to use new LCD routines
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#ifndef LCD_H
#define LCD_H

/*
  The NIBBLE_HIGH condition determines which PORT bits are used to
  transfer data to data bits 4-7 of the LCD.
  If LCD data bits 4-7 are connected to PORTD, bits 4-7, define NIBBLE_HIGH
  If LCD data bits 4-7 are connected to PORTD, bits 2-3 and PORTB bits 0-1,
  don't define NIBBLE_HIGH.
*/
#define NIBBLE_HIGH
// Use bits 4-7 for talking to LCD

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(unsigned char);
void lcd_writedata(unsigned char);
void lcd_writebyte(unsigned char);
void lcd_writenibble(unsigned char);
void lcd_wait(void);
void lcd_stringout_P(char *);
void lcd_clear(void);
void lcd_redon(void);
void lcd_redoff(void);
void lcd_yellowon(void);
void lcd_yellowoff(void);
/* Use the "PROGMEM" attribute to store the strings in the ROM
  instead of in RAM. */

#define LCD_RS          (1 << PB0)
#define LCD_RW          (1 << PB1)
#define LCD_E           (1 << PB2)
#define LCD_Bits        (LCD_RS|LCD_RW|LCD_E)


#define LCD_Data_D     (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7)
// Bits in Port D for LCD data
#define LCD_Status     (1<< PD7)    // Bit in Port D for LCD busy status
#endif
