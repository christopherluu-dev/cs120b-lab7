/*	Author: christopherluu
 *  Partner(s) Name: 
 *	Lab Section: 22
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/GRE9Z13I5RA
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States {START, INIT, LED, BUTTON_ON, BUTTON_RESET} state;
unsigned char LEDstate[3] = {0x01, 0x02, 0x04};
unsigned char counter = 0;
unsigned char reverse = 0;
unsigned char score = 5;
volatile unsigned char TimerFlag = 0;

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
            break;
        case INIT:
            counter = 0;
            reverse = 0;
            state = LED;
            break;
        case LED:
            if((~PINA & 0x01) == 0x01){
                if(counter != 1 && score == 9){
                    score = 5;
                }
                else if(counter != 1 && score != 9){
                    score++;
                }
                else if(counter == 1 && score == 0){
                    score = 0;
                }
                else{
                    score--;
                }

                state = BUTTON_ON;
            }
            else{
                state = LED;
            }
            break;
        case BUTTON_ON:
            if((~PINA & 0x01) == 0x01){
                state = BUTTON_ON;
            }
            else{
                state = BUTTON_RESET;
            }
            break;
        case BUTTON_RESET:
            if((~PINA & 0x01) == 0x00){
                state = BUTTON_RESET;
            }
            else{
                if(score == 9){
                    score = 5;
                }
                state = INIT;
            }
            break;
        default:
            break;
    }

    switch(state){
        case START:
            break;
        case INIT:
            PORTB = LEDstate[0];
            break;
        case LED:
            PORTB = LEDstate[counter];
            if(!reverse && counter < 2){
                counter++;
                if(counter == 2){
                    reverse = 1;
                }
            }
            
            else if(reverse && counter > 0){
                counter--;
                if(counter == 0){
                    reverse = 0;
                }
            }
            break;
        case BUTTON_ON:
            break;
        case BUTTON_RESET:
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
    TimerSet(300);
    TimerOn();
    LCD_init();
    while (1) {
        Tick();
        if(score == 0) LCD_DisplayString(1, "0");
        else if(score == 1) LCD_DisplayString(1, "1");
        else if(score == 2) LCD_DisplayString(1, "2");
        else if(score == 3) LCD_DisplayString(1, "3");
        else if(score == 4) LCD_DisplayString(1, "4");
        else if(score == 5) LCD_DisplayString(1, "5");
        else if(score == 6) LCD_DisplayString(1, "6");
        else if(score == 7) LCD_DisplayString(1, "7");
        else if(score == 8) LCD_DisplayString(1, "8");
        else if(score == 9) {LCD_DisplayString(1, "9"); LCD_DisplayString(2, "VICTORY!");}

        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}
