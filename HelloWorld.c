/*
 * HelloWorld.c
 *
 * RS = PB2
 * RW = PB1
 * E = PB0
 *
 * Created: 4/12/2013 10:12:03 AM
 *  Author: tsaadus
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define STATE_HELLO 0
#define STATE_OTHER 1

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

int main(void){
   int button, pos = 0;

   DDRD = 0xFF; //Set all of Port D to output
   DDRB = 0x07; //Set Port B 0-2 to output
   PORTB |= (1<<3); //Port B3 as input w/ pullup

   DDRB |= (1<<5); //Output to LED (debugging purposes)

   startLCD();

   _delay_ms(10);

   writeStr("Hello World!    Other message..."); //32 chars

   while (1) {
      button = buttonPressed();

      if (!button && (pos < 16)) {
         displayShift(1);
         pos++;
         _delay_ms(80);
      } else if (button && (pos > 0)) {
         displayShift(0);
         pos--;
         _delay_ms(80);
      }
   }

   return 1;
}

int buttonPressed() {
   int previous, next, i = 0;
   previous = PINB & (1<<3);

   //Poll button state every 1 ms.
   //If constant for 4 checks, assume that the button is stable
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

void writeStr(char* str) {
   int i = 0;

   clearDisp();

   while (str[i] != '\0') {
      writeChar(str[i]);
      i++;
   }
}

void writeChar(char out) {
   PORTB |= (1<<2);
   PORTB &= ~(1<<1);
   _delay_us(5);
   PORTD = out;
   _delay_us(5);
   pulseEnable();
   _delay_ms(1);
}

//0 for right shift, else left
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

void clearDisp() {
   PORTD = 0x01;
   PORTB &= ~(0x07);
   pulseEnable();
   _delay_ms(2);
}

void returnHome() {
   PORTD = 0x02;
   PORTB &= ~(0x07);
   pulseEnable();
   _delay_ms(1);
}

void startLCD() {
   _delay_ms(40);
   PORTD = 0x38; //DL = 1, N = 1, F = 0
   PORTB &= ~(6); // Set RS, R/W to 0
   pulseEnable();

   _delay_us(50);
   PORTD = 0x0C; //Display on, no cursor
   pulseEnable();

   _delay_us(50);
   PORTD = 0x01;
   pulseEnable();

   _delay_ms(2);
   PORTD = 0x06;
   pulseEnable();
}

void pulseEnable() {
   PORTB &= ~(1); //Set E to 0
   _delay_us(4);
   PORTB |= (1); //Set E to 1
   _delay_us(4);
   PORTB &= ~(1); //Set E to 0
}
