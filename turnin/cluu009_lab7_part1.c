/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/flF3JnYC-KU
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {START, INIT, INC_ON, INC_OFF, MIN_ON, MIN_OFF, RESET} state;

volatile unsigned char TimerFlag = 0;
unsigned char timer = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void Tick(){
    switch(state){
        case START:
            state = INIT;
            PORTB = 0x00;
            break;
        case INIT:
            if((~PINA & 0x03) == 0x03){
                state = RESET;
            }
            else if((~PINA & 0x03) == 0x01){
                state = INC_ON;
            }
            else if((~PINA & 0x03) == 0x02){
                state = MIN_ON;
            }
            else{
                state = INIT;
            }
            break;
        case INC_ON:
            break;
        case INC_OFF:
            if((~PINA & 0x03) == 0x01){
                timer++;
            }
            else if((~PINA & 0x03) == 0x02){
                state = MIN_ON;
                timer = 0;
            }
            else if((~PINA & 0x03) == 0x03){
                state = RESET;
                timer = 0;
            }
            else if((~PINA & 0x03) == 0x00){
                state = INIT;
                timer = 0;
            }
            if(timer == 10){
                state = INC_ON;
                timer = 0;
            }
            break;
        case MIN_ON:
            break;
        case MIN_OFF:
            if((~PINA & 0x03) == 0x02){
                timer++;
            }
            else if((~PINA & 0x03) == 0x01){
                state = INC_ON;
                timer = 0;
            }
            else if((~PINA & 0x03) == 0x03){
                state = RESET;
                timer = 0;
            }
            else if((~PINA & 0x03) == 0x00){
                state = INIT;
                timer = 0;
            }
            if(timer == 10){
                state = MIN_ON;
                timer = 0;
            }
            break;
        case RESET:
            state = INIT;
            break;
        default:
            break;
    }

    switch(state){
        case START:
            break;
        case INIT:
            break;
        case INC_ON:
            if(PORTB < 9){
                PORTB = PORTB + 1;
            }
            state = INC_OFF;
            break;
        case INC_OFF:
            break;
        case MIN_ON:
            if(PORTB > 0){
                PORTB = PORTB - 1;
            }
            state = MIN_OFF;
            break;
        case MIN_OFF:
            break;
        case RESET:
                PORTB = 0x00;
            break;
        default:
            break;
    }
}

int main(void) {
    
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */
    TimerSet(100);
    TimerOn();
    LCD_init();
    
    while (1) {
        Tick();
        if(PORTB == 0x00) LCD_DisplayString(1, "0");
        else if(PORTB == 0x01) LCD_DisplayString(1, "1");
        else if(PORTB == 0x02) LCD_DisplayString(1, "2");
        else if(PORTB == 0x03) LCD_DisplayString(1, "3");
        else if(PORTB == 0x04) LCD_DisplayString(1, "4");
        else if(PORTB == 0x05) LCD_DisplayString(1, "5");
        else if(PORTB == 0x06) LCD_DisplayString(1, "6");
        else if(PORTB == 0x07) LCD_DisplayString(1, "7");
        else if(PORTB == 0x08) LCD_DisplayString(1, "8");
        else LCD_DisplayString(1, "9");

        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
