#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gps.h"
#include "lcd.h"
#include "map.h"


#define FOSC 7372800            // Clock frequency
#define BAUD 9600               // Baud rate used
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

#define Buffer_Size 150
#define degrees_buffer_size 20

#define MAINMENU 0
#define PATHMENU 1
#define SENDLOC 2
#define NAVDISPLAY 3
#define RETURN 4
void serial_init();
void button_init();
void init_timer(unsigned short);
void reverse(char*, int);
int intToStr(int, char[], int);
void ftoa(float, char*, int);



char degrees_buffer[degrees_buffer_size];     /* save latitude or longitude in degree */
char GGA_Buffer[Buffer_Size];         		  /* save GGA string */
char GGA_CODE[3];

volatile uint16_t GGA_Index;

bool  IsItGGAString = false;

volatile bool end_string = false;

// Button/Menu Navigation flags
volatile char select = 0;
volatile char screen = MAINMENU;
char refresh_count = 0;
char refresh_rate = 7;


struct Map map; //struct for Map
float pointsA[7][2] = {{34.020331, -118.289703}, {34.020201, -118.289425}, {34.020403, -118.289213},
  {34.020560, -118.289106}, {34.020458, -118.288893}, {34.020331, -118.288690}, {34.020331, -118.288494}};
float pointsB[9][2] = {{34.020360, -118.285336}, {34.020655, -118.286029}, {34.021209, -118.286114},
  {34.021448, -118.285556}, {34.021779, -118.285118}, {34.022275, -118.284795}, {34.022400, -118.285506},
  {34.022776, -118.286293}, {34.023539, -118.288026}};
float pointsC[8][2] = {{34.020360, -118.285336}, {34.020524, -118.284555}, {34.020789, -118.283933},
  {34.020674, -118.283602}, {34.020926, -118.283128}, {34.021194, -118.282637}, {34.020771, -118.281816},
  {34.020457, -118.281062}};

int main(void){
  GGA_Index = 0;
  memset(GGA_Buffer, 0, 150); //set GGA_Buffer to beginning
  memset(GGA_CODE, 0, 5);     //set GGA_code to beginning
  lcd_init();                 // Initialize the LCD display
  lcd_moveto(0, 0);

  serial_init(); //initialize interrupt
  button_init();
  sei(); // Enable Global Interrupt Enable Flag
  lcd_clear();

  struct GPS gps;  //struct for GPS
  double dist;     //distance to next location
  char* direction; //direction to next location
  char res[5];     //hold test values to print out
  char distDisplay[10];

  // Splash Screen
  lcd_moveto(0,7);
  lcd_stringout("Welcome");
  lcd_moveto(1,9);
  lcd_stringout("to");
  lcd_moveto(2,3);
  lcd_stringout("Jurassic Park");
  _delay_ms(3000);
  lcd_clear();
  bool done = false;

  while(1){
    if (screen == MAINMENU){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      //main menu
      lcd_moveto(0,0);
      lcd_stringout("1. Navigate");
      lcd_moveto(1,0);
      lcd_stringout("2. Help, I'm Lost!");
      lcd_moveto(2,0);
      lcd_stringout("3. Return to Start");
    }
    else if (screen == PATHMENU){
      //ensures screen isn't constantly refreshed
      //must be refreshed occasionally to get rid of excess chararacters
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      //list of trails
      lcd_moveto(0,0);
      lcd_stringout("1. T-Rex Trail");
      lcd_moveto(1,0);
      lcd_stringout("2. Pterodactyl Path");
      lcd_moveto(2,0);
      lcd_stringout("3. Old Town Road");
      lcd_moveto(3,0);
      lcd_stringout("4. Back");
    }
    //sends location data to ranger stations
    //not yet implemented
    else if (screen == SENDLOC){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      lcd_moveto(0,2);
      lcd_stringout("Sending Location");
      lcd_moveto(1,1);
      lcd_stringout("to Ranger Station");
      lcd_moveto(3,7);
      lcd_stringout("Cancel");
    }
    //displays navigation guide
    else if (screen == NAVDISPLAY){
      if (refresh_count == refresh_rate){
        lcd_clear();
        refresh_count = 0;
      }
      else{
        refresh_count++;
      }
      if(done == false){
        struct Point* pt = map.curr;

        //parse gps data buffer into gps struct
        parse(GGA_Buffer, &gps);
        lcd_moveto(0,0);

        //tally of check points
        lcd_stringout("Checkpoint ");
        memset(res, 0, 3);
        sprintf(res, "%d", map.index);
        lcd_stringout(res);
        lcd_stringout(" of ");
        memset(res, 0, 3);
        sprintf(res, "%d", map.totalPoints);
        lcd_stringout(res);

        //Details about next point
        lcd_moveto(1,0);
        lcd_stringout("Next: ");
        memset(degrees_buffer,0,degrees_buffer_size);
        dist =  distanceNext(&gps, pt);
        dtostrf(dist, 6, 2, degrees_buffer);
        lcd_stringout(degrees_buffer);
        direction = directionNext(&gps, pt);
        lcd_stringout(" ");
        lcd_stringout(direction);
        updateNext(&gps, &map);
        if(map.index == map.totalPoints){done = true;}
        if(!(calcDistance(&gps, pt->next->gps) < 50)){
          lcd_redon();
          lcd_moveto(2, 5);
          lcd_stringout("CAUTION");
          lcd_moveto(3, 2);
          lcd_stringout("Return to trail");
        }else{
          lcd_moveto(2,0);
          lcd_stringout("Remaining: ");
          memset(distDisplay, 0, 10);
          sprintf(distDisplay, "%f", map.distRemaining);
          lcd_moveto(3,0);
          lcd_stringout("Total Dist: ");
          memset(distDisplay, 0, 10);
          sprintf(distDisplay, "%f", map.totalDist);
        }
      }else{
        if (refresh_count == refresh_rate){
          lcd_clear();
          refresh_count = 0;
        }
        else{
          refresh_count++;
        }
        //show after finished with path
        lcd_moveto(1, 5);
        lcd_stringout("Congrats");
        lcd_moveto(2, 1);
        lcd_stringout("You've finished!");
      }
    }else if(screen == RETURN){//displays return to start navigation guide
        if (refresh_count == refresh_rate){
          lcd_clear();
          refresh_count = 0;
        }
        else{
            refresh_count++;
        }
        if(map.index == 1){
          lcd_moveto(1, 0);
          lcd_stringout("Arrived at start");
          continue;
        }
        struct Point* pt = map.curr;

        parse(GGA_Buffer, &gps);
        lcd_moveto(0,0);

        //tally of check points
        lcd_stringout("Checkpoint ");
        memset(res, 0, 3);
        sprintf(res, "%d", map.index);
        lcd_stringout(res);
        lcd_stringout(" of ");
        memset(res, 0, 3);
        sprintf(res, "%d", map.totalPoints);
        lcd_stringout(res);

        //Details about next point
        lcd_moveto(1,0);
        lcd_stringout("Next: ");
        memset(degrees_buffer,0,degrees_buffer_size);
        dist =  distancePrev(&gps, pt);
        dtostrf(dist, 6, 2, degrees_buffer);
        lcd_stringout(degrees_buffer);
        direction = directionPrev(&gps, pt);
        lcd_stringout(" ");
        lcd_stringout(direction);
        updatePrev(&gps, &map);
        //show when too far from trail
        if(!(calcDistance(&gps, pt->next->gps) < 50)){
          lcd_redon();
          lcd_moveto(2, 5);
          lcd_stringout("CAUTION");
          lcd_moveto(3, 2);
          lcd_stringout("Return to trail");
        }
    }



	}

  return 0;
}

//receive data from GPS
ISR(USART_RX_vect){
  uint8_t oldsrg = SREG;
  cli();
  char received_char = UDR0;

  if(received_char =='$'){                                                    /* check for '$' */
    GGA_Index = 0;
    end_string = false;
    if(IsItGGAString){
      end_string = true;
    }
    IsItGGAString = false;
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(IsItGGAString == true){                                             /* if true save GGA info. into buffer */
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else if(GGA_CODE[0] == 'G' && GGA_CODE[1] == 'G' && GGA_CODE[2] == 'A'){    /* check for GGA string */
    IsItGGAString = true;
    GGA_CODE[0] = 0; GGA_CODE[1] = 0; GGA_CODE[2] = 0;
    GGA_Buffer[GGA_Index++] = received_char;
  }
  else{
    GGA_CODE[0] = GGA_CODE[1];  GGA_CODE[1] = GGA_CODE[2]; GGA_CODE[2] = received_char;
  }
  SREG = oldsrg;
}
//receive data from buttons
ISR(PCINT2_vect){
  if ((PIND & (1<<PD1)) == 0){ // Button #1 pressed
    if (screen == MAINMENU){
      screen = PATHMENU;
      _delay_ms(1000);
    }
    else if (screen == PATHMENU){
      init_map(&map, pointsA, (int)(sizeof(pointsA)/sizeof(pointsA[0]))); //initialize map
      screen = NAVDISPLAY;
      _delay_ms(1000);
    }
  }
  else if ((PIND & (1<<PD2)) == 0){ // Button #2 pressed
    if (screen == MAINMENU){
      screen = SENDLOC;
      _delay_ms(1000);
    }
    else if (screen == PATHMENU){
      init_map(&map, pointsB, (int)(sizeof(pointsB)/sizeof(pointsB[0]))); //initialize map
      screen = NAVDISPLAY;
      _delay_ms(1000);
    }
  }
  else if ((PIND & (1<<PD3)) == 0){  // Button #3 pressed
    if (screen == MAINMENU){
      screen = RETURN;
    }
    else if (screen == PATHMENU){
      init_map(&map, pointsC, (int)(sizeof(pointsC)/sizeof(pointsC[0]))); //initialize map
      screen = NAVDISPLAY;
      _delay_ms(1000);
    }
  }
}

ISR(PCINT1_vect){
  // Button #4 pressed
  if ((PINC & (1<<PC1)) == 0){
    screen = MAINMENU;
    _delay_ms(1000);
    sei();
  }
}

void serial_init(){
  // initialize USART (must call this before using it)
  uint8_t oldsrg = SREG;
  cli();
  UBRR0=MYUBRR; // set baud rate
  UCSR0B|=(1<<RXEN0); //enable RX
  UCSR0B|=(1<<RXCIE0); //RX complete interrupt
  UCSR0C|=(1<<UCSZ01)|(1<<UCSZ01); // no parity, 1 stop bit, 8-bit data
  SREG = oldsrg;
}

void button_init(){
  DDRD &= ~((1 << PD1) | (1 << PD2) | (1 << PD3));      // Clear the PD1, PD2, and PD3 pin
  PORTD |= ((1 << PD1) | (1 << PD2) | (1 << PD3));      // turn On the Pull-up
  PCICR |= ((1 << PCIE2) | (1 << PCIE1));				// Enable interrupts for Port C and D
	PCMSK2 |= ((1 << PCINT17) | (1 << PCINT18) | (1 << PCINT19));	// Set mask register for PD1, PD2, and PD3

  DDRC &= ~(1 << PC1);      // Clear the PC1 pin
  PORTC |= (1 << PC1);      // turn On the Pull-up
  PCMSK1 |= (1 << PCINT9);	// Set mask register for PC1
}
