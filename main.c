#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "notes.h"
#include "lcd.h"
#include "printf.h"

#define RCH PB5
#define LCH PC6
#define LED PB7

#define SWW PC5
#define SWS PC4
#define SWE PC3
#define SWN PC2
#define SWC PE7

#define F_CPU 8000000UL

void init(void);
void read_buttons(void);
void init_draw(void);

volatile int button = 127;

volatile rectangle w,w2,n,n2,e,e2,s,s2,c,c2;

void main()
{
  init_lcd();
  init();
  
  sei();
  
  for (;;)
  {

  }
  
}

void init()
{
  
  init_draw();
  
  /* set buttons as input with pull ups */
  DDRC &= ~_BV(SWW) | ~_BV(SWE) | ~_BV(SWS) | ~_BV(SWN);
  PORTC |= _BV(SWW) | _BV(SWE) | _BV(SWS) | _BV(SWN);
  
  DDRE &= ~_BV(SWC);
  PORTE |= _BV(SWC);

  /* set LED to output for visual feedback */
  DDRB |= _BV(LED);
  PORTB &= ~_BV(LED);
  
  /* set right channel to output turned off */
  DDRB |= _BV(RCH);
  PORTB &= ~_BV(RCH);
  /* set left channel to output turned off */
  DDRC |= _BV(LCH);
  PORTC &= ~_BV(LCH);
  
  /* set up timer 0 for playing tone */
  TCCR0A = _BV(WGM01);
  TCCR0B = _BV(CS01) | _BV(CS00); /* /64 prescalar */
  
  OCR0A = C;
  
  TIMSK0 |= _BV(OCIE0A);
  
  /* set up timer 2 for reading input */
  TCCR2A = _BV(WGM21);
  TCCR2B = _BV(CS21) | _BV(CS20); /* /64 prescalar */
  
  OCR2A = 255;
  
  TIMSK2 |= _BV(OCIE2A);

}

void fill_rects()
{
  fill_rectangle(w, LIGHT_GRAY);
  fill_rectangle(w2, LIGHT_GRAY);  
  
  fill_rectangle(n, LIGHT_GRAY);
  fill_rectangle(n2, LIGHT_GRAY);
  
  fill_rectangle(e, LIGHT_GRAY);
  fill_rectangle(e2, LIGHT_GRAY);
  
  fill_rectangle(s, LIGHT_GRAY);
  fill_rectangle(s2, LIGHT_GRAY);
  
  fill_rectangle(c, LIGHT_GRAY);
  fill_rectangle(c2, LIGHT_GRAY);
}

void init_draw()
{
  /* draw ring */
  rectangle r = {100,LCDHEIGHT-100,10,LCDWIDTH-10};
  rectangle r2 = {75,LCDHEIGHT-75,35,LCDWIDTH-35};
  rectangle r3 = {50,LCDHEIGHT-50, 60,LCDWIDTH-60};
  fill_rectangle(r, WHITE);
  fill_rectangle(r2, WHITE);
  fill_rectangle(r3, WHITE);
  
  /* draw buttons */
  w.left=55;
  w.right=65;
  w.top=LCDWIDTH/2-5;
  w.bottom=LCDWIDTH/2+5;
  
  w2.left=65;
  w2.right=75;
  w2.top=LCDWIDTH/2-15;
  w2.bottom=LCDWIDTH/2+15;
  
  n.left=LCDHEIGHT/2-5;
  n.right=LCDHEIGHT/2+5;
  n.top=15;
  n.bottom=25;
  
  n2.left=LCDHEIGHT/2-15;
  n2.right=LCDHEIGHT/2+15;
  n2.top=25;
  n2.bottom=35;
  
  e.left=LCDHEIGHT-65;
  e.right=LCDHEIGHT-55;
  e.top=LCDWIDTH/2-5;
  e.bottom=LCDWIDTH/2+5;
  
  e2.left=LCDHEIGHT-75;
  e2.right=LCDHEIGHT-65;
  e2.top=LCDWIDTH/2-15;
  e2.bottom=LCDWIDTH/2+15;
  
  s.left=LCDHEIGHT/2-5;
  s.right=LCDHEIGHT/2+5;
  s.top=LCDWIDTH-25;
  s.bottom=LCDWIDTH-15;
  
  s2.left=LCDHEIGHT/2-15;
  s2.right=LCDHEIGHT/2+15;
  s2.top=LCDWIDTH-35;
  s2.bottom=LCDWIDTH-25;
  
  c.left=LCDHEIGHT/2-15;
  c.right=LCDHEIGHT/2+15;
  c.top=LCDWIDTH/2-5;
  c.bottom=LCDWIDTH/2+5;
  
  c2.left=LCDHEIGHT/2-5;
  c2.right=LCDHEIGHT/2+5;
  c2.top=LCDWIDTH/2-15;
  c2.bottom=LCDWIDTH/2+15;
  
  fill_rects();
}

ISR( TIMER0_COMPA_vect )
{
  cli();
  if (!(button > 4))
  {
    PINB |= _BV(RCH);
    PINC |= _BV(LCH);
  }
  sei();
}

ISR( TIMER2_COMPA_vect )
{
  cli();
  
  read_buttons();
  switch (button)
  {
    case 0: {
      OCR0A = C;
      fill_rectangle(w,RED);
      fill_rectangle(w2,RED);
      break;
    }
    case 1: {
      OCR0A = D;
      fill_rectangle(n,RED);
      fill_rectangle(n2,RED);
      break;
    }
    case 2: {
      OCR0A = E;
      fill_rectangle(e,RED);
      fill_rectangle(e2,RED);
      break;
    }
    case 3: {
      OCR0A = G;
      fill_rectangle(s,RED);
      fill_rectangle(s2,RED);
      break;
    }
    case 4: {
      OCR0A = A;
      fill_rectangle(c,RED);
      fill_rectangle(c2,RED);
      break;
    }
    default: fill_rects(); break;
  }
  
  sei();
}

void read_buttons()
{  
  cli();
  
  uint8_t in = PINC;
  uint8_t in_ = PINE;
  
  
  if (!(in & _BV(SWW))) {
    button = 0;
  } else if (!(in & _BV(SWN))) {
    button = 1;
  } else if (!(in & _BV(SWE))) {
    button = 2;
  } else if (!(in & _BV(SWS))) {
    button = 3;
  } else if (!(in_ & _BV(SWC))) {
    button = 4;
  } else {
    button = 127;
  }
  
  sei();
  
}