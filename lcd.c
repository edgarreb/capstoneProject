#include "lcd.h"

/* lcd_stringout_P - Print the contents of the character string "s" starting at LCD
 RAM location "x" where the string is stored in ROM.  The string must be
 terminated by a zero byte. */
void lcd_stringout_P(char *s)
{
	 unsigned char ch;

	 /* Use the "pgm_read_byte()" routine to read the date from ROM */
	 while ((ch = pgm_read_byte(s++)) != '\0') {
			 lcd_writedata(ch);             // Output the next character
	 }
}



/* lcd_init - Do various things to force a initialization of the LCD
 display by instructions, and then set up the display parameters and
 turn the display on. */
void lcd_init(void)
{
	 DDRC |= (1 << PC2);				 // For lcd_redon
	 DDRD |= LCD_Data_D;         // Set PORTD bits 4-7 for output
	 DDRB |= LCD_Bits;           // Set PORTB bits 0, 1 and 2 for output

	 PORTB &= ~(LCD_RS);           // Clear RS for command write

	 _delay_ms(15);              // Delay at least 15ms

	 lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
	 _delay_ms(5);               // Delay at least 4msec

	 lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
	 _delay_us(120);             // Delay at least 100usec

	 lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
	 _delay_ms(2);

	 lcd_writenibble(0x20);      // Send 00100000, set for 4-bit interface
	 _delay_ms(2);

	 lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

	 lcd_writecommand(0x0f);     // Display and cursor on
}



/* lcd_moveto - Move the cursor to the row and column given by the arguments.
 Row is 0 or 1, column is 0 - 15. */
void lcd_moveto(unsigned char row, unsigned char col)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > 4 ) {
	  row = 3;    // we count rows starting w/0
	}

  lcd_writecommand(0x80 | (col + row_offsets[row]));
}



/* lcd_stringout - Print the contents of the character string "str"
 at the current cursor position. */
void lcd_stringout(char *str)
{
	 char ch;
	 while ((ch = *str++) != '\0')
		 lcd_writedata(ch);
}



/* lcd_writecommand - Output a byte to the LCD command register. */
void lcd_writecommand(unsigned char x)
{
	 PORTB &= ~(LCD_RS);       // Clear RS for command write
	 lcd_writebyte(x);
	 lcd_wait();
}



/* lcd_writedata - Output a byte to the LCD data register */
void lcd_writedata(unsigned char x)
{
	 PORTB |= LCD_RS;	// Set RS for data write
	 lcd_writebyte(x);
	 lcd_wait();
}



/* lcd_writebyte - Output a byte to the LCD */
void lcd_writebyte(unsigned char x)
{
	 lcd_writenibble(x);
 //_delay_ms(5);
	 lcd_writenibble(x << 4);
 //_delay_ms(5);
}



/* lcd_writenibble - Output a 4-bit nibble to the LCD */
void lcd_writenibble(unsigned char x)
{

	 PORTD &= ~(LCD_Data_D);
	 PORTD |= (x & (LCD_Data_D));  // Put high 4 bits of data in PORTD
	 PORTB &= ~(LCD_RW|LCD_E);     // Set R/W=0, E=0
	 PORTB |= LCD_E;               // Set E to 1
	 PORTB |= LCD_E;               // Extend E pulse > 230ns
	 PORTB &= ~(LCD_E);            // Set E to 0
}



/* lcd_wait - Wait for the BUSY flag to reset */
void lcd_wait()
{
#ifdef USE_BUSY_FLAG
	 unsigned char bf;

	 PORTD &= ~(LCD_Data_D);       // Set for no pull ups
	 DDRD &= ~(LCD_Data_D);        // Set for input

	 PORTB &= ~(LCD_E|LCD_RS);     // Set E=0, R/W=1, RS=0
	 PORTB |= LCD_RW;

	 do {
			 PORTB |= LCD_E;         // Set E=1
			 _delay_us(1);           // Wait for signal to appear
			 bf = PIND & LCD_Status; // Read status register high bits
			 PORTB &= ~(LCD_E);      // Set E=0
 PORTB |= LCD_E;                     // Need to clock E a second time to fake
 PORTB &= ~(LCD_E);                  //getting the status register low bits
	 } while (bf != 0);              // If Busy (PORTD, bit 7 = 1), loop

	 DDRD |= LCD_Data_D;             // Set PORTD bits for output
#else
	 _delay_ms(2);		             // Delay for 2ms
#endif
}



void lcd_clear(){
		lcd_writecommand(1);
}



void lcd_redon(){
	PORTC &= ~(1 << PC2);
}



void lcd_redoff(){
	PORTC |= (1 << PC2);
}



void lcd_yellowon(){
	PORTC &= ~(1 << PC3);
}



void lcd_yellowoff(){
	PORTC |= (1 << PC3);
}
