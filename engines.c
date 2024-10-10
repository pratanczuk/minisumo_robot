#include "engines.h"

void engine_1_Backward( unsigned char a_ui8Speed)
{
	OCR1A = a_ui8Speed;
	PORTB |= (1<<PB0);
	PORTB &= ~(1<<PB1);
}

void engine_1_Forward( unsigned char a_ui8Speed)
{
	OCR1A = a_ui8Speed;
	PORTB |= (1<<PB1);
	PORTB &= ~(1<<PB0);
}

void engine_2_Backward( unsigned char a_ui8Speed)
{
	OCR1B = a_ui8Speed;
	PORTD |= (1<<PD6);
	PORTC &= ~(1<<PC3);
}

void engine_2_Forward( unsigned char a_ui8Speed)
{
	OCR1B = a_ui8Speed;
	PORTC |= (1<<PC3);
	PORTD &= ~(1<<PD6);
}

void engine_1_Brake( void)
{
	OCR1A=0;
	PORTB &= ~(1<<PB0);
	PORTB &= ~(1<<PB1);
}

void engine_2_Brake( void)
{
	OCR1B=0;
	PORTD &= ~(1<<PD6);
	PORTC &= ~(1<<PC3);
}

void enginesInit (void)
{
	// set logic pins as output
	// Engine 1
    DDRB |= (1<<PB0);
    DDRB |= (1<<PB1);

	//Engine 2
    DDRD |= (1<<PD6);
    DDRC |= (1<<PC3);
	
	/*
	PWM Modes atmega32 for 16 bits timers OC1A and OC1B
	
	Phase and Frequency Correct PWM Mode:
	Phase Correct PWM Mode:
	desired_frequency = (Input_frequency)/(2*Prescaler*TOP_limit)
	TOP_limit = (IF/(2*Prescaler*desired_frequency)) -1
	
	Fast PWM Mode:
	desired_frequency = (Input_frequency)/(Prescaler*(1 + TOP_limit))
	
	Clear Timer on Compare Match (CTC) Mode:
	desired_frequency = (Input_frequency)/(2 * Prescaler *(1 + TOP_limit))
		
    * Prescaler – 1, 2, 4, 8, 16 .. 1024

	*/

// TODO PWM configuration has to be configured verified
	
   	//top=100, prescaler=8, desired_frequency=10 000 hz; Input_frequency=16 000 000 Hz
	ICR1=100;
	TCCR1B|=_BV(CS11);
	
	//phase and frequency correction, icr1 = top
	TCCR1B|=_BV(WGM13);
	
	//set OC1A,OC2B on downcounting, clear on upcounting
	TCCR1A|=_BV(COM1A1)|_BV(COM1B1);
	
	//output pins, PORTD4 (motor 1) and PORTD5 (motor 2)
	DDRD|=(1<<PD4)|(1<<PD5);   //PWM Pins as Out
	
	//default
	engine_1_Brake();
	engine_2_Brake();
}
