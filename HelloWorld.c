/*
 * CPE 329 - Fall 2013
 * Project 1: Hello World
 *
 * Displays "Hello World!" on the LCD display, and when the button is pressed
 * shifts it over to the second message "I am HAL"
 *
 * Data = Port D
 * RS = Pin B2
 * RW = Pin B1
 * E = Pin B0
 *
 * Created: 4/12/2013 10:12:03 AM
 *  Author: tsaadus
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

void startLCD();
void pulseEnable();
void writeChar(char out);
void writeStr(char* str);
void clearDisp();
void returnHome();
void displayShift(int dir);
int buttonPressed();

/*
 * Initialize the LCD, write the two messages, and then infinite loop polling
 * the button state and shift if necessary.
 */
int main(void){
   int button, pos = 0;

   DDRD = 0xFF; //Set all of Port D to output
   DDRB = 0x07; //Set Port B 0-2 to output
   PORTB |= (1<<3); //Port B3 as input w/ pullup

   DDRB |= (1<<5); //Output to LED (debugging purposes)

   startLCD();

   writeStr("Hello World!    I am HAL"); //Move both messages into LCD memory

   while (1) {
      button = buttonPressed(); //Get the current state of the button

      if (!button && (pos < 16)) { //If button is pressed and not shifted left entirely, move
         displayShift(1);
         pos++;
         _delay_ms(80);
      } else if (button && (pos > 0)) { //Same as above for right shift
         displayShift(0);
         pos--;
         _delay_ms(80);
      }
   }

   return 1;
}

 /*
  * Checks if the button input on Pin B3 is pressed or not.
  * Polls the button state every 1ms, 4 consecutive reads means that it's stable.
  * Returns 1 if the button is up, 0 if it is pressed.
  */
int buttonPressed() {
   int previous, next, i = 0;
   previous = PINB & (1<<3);

   while (i < 4) {
      _delay_ms(1);
      next = PINB & (1<<3);
      if (previous == next) {
         i++;
      } else {
         i = 0;
      }
      previous = next;
   }

   if (next >= 1) {
      PORTB |= (1<<5);
      return 1;
   } else {
      PORTB &= ~(1<<5);
      return 0;
   }
}

/*
 * Takes in a string of chars, and prints them one by one to the display.
 */
void writeStr(char* str) {
   int i = 0;

   clearDisp();

   while (str[i] != '\0') {
      writeChar(str[i]);
      i++;
   }
}

/*
 * Writes a single character to the display.
 */
void writeChar(char out) {
   PORTB |= (1<<2);
   PORTB &= ~(1<<1);
   _delay_us(5);
   PORTD = out;
   _delay_us(5);
   pulseEnable();
   _delay_ms(1);
}

/*
 * Shifts the entire display to the right or left, using extra memory spaces off of the display.
 * dir equal to 0 is a right shift, otherwise shift left.
 */
void displayShift(int dir) {
   PORTB &= ~(6);
   if (dir >= 1) {
      PORTD = 0x18;
   } else {
      PORTD = 0x1C;
   }
   pulseEnable();

   _delay_ms(2);
}

/*
 * CLears the display and waits 2 ms
 */
void clearDisp() {
   PORTD = 0x01;
   PORTB &= ~(0x07);
   pulseEnable();
   _delay_ms(2);
}

/*
 * Sends the return home function and waits 1 ms
 */
void returnHome() {
   PORTD = 0x02;
   PORTB &= ~(0x07);
   pulseEnable();
   _delay_ms(1);
}

/*
 * Initializes the LCD to the desired settings.
 * 8 bit data-length, 2 lines, 5x8 dots.
 * Display on, no cursor.
 * Right moving cursor, no shift
 */
void startLCD() {
   _delay_ms(40); //Startup delay

   //Function set
   PORTD = 0x38;
   PORTB &= ~(6);
   pulseEnable();

   //Diplay ON/OFF control
   _delay_us(50);
   PORTD = 0x0C;
   pulseEnable();

   //Clear Display
   _delay_us(50);
   PORTD = 0x01;
   pulseEnable();

   //Entry Mode Set
   _delay_ms(2);
   PORTD = 0x06;
   pulseEnable();

   _delay_ms(10);
}

/*
 * Forces enable to go Low->High->Low, so that the current output is read by the LCD.
 */
void pulseEnable() {
   PORTB &= ~(1); //Set E to 0
   _delay_us(4);
   PORTB |= (1); //Set E to 1
   _delay_us(4);
   PORTB &= ~(1); //Set E to 0
}
