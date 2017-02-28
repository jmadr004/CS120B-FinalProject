/*
 * jmadr004_Final_Project.c
 *
 * Created: 2/28/2017 10:26:58 AM
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "USART.h"

volatile unsigned char TimerFlag = 0; //TimerISR() sets this to 1. c programmer should clear to 0

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; //start count from here down to 0. default 1ms.
unsigned long _avr_timer_cntcurr =0; //current internal count of 1ms ticks

unsigned char LEDS;


void TimerOn()
{
	//AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; //bit3 = 0; CTC mode (clear timer on compare)
	//bit2bit1bit0=011: pre-scaler /64
	//00001011: 0x0B
	//SO, 8 MHz clock or 8,000,000 / 64 = 125,000 ticks/s
	//Thus, TCNT1 register will count at 125,000 ticks/s

	//AVR output compare register OCR1A
	OCR1A = 125; // timer interrupt will be generated when TCTN1 == OCR1A
	//We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125;
	//so when TCHTN1 register equals 125,
	//1 ms has passed. Thus, we compare to 125
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1 =0;

	_avr_timer_cntcurr = _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr milliseconds

	//enable global interrupts
	//SREG |= 0x80;// 0x80 : 1000000
	sei();
}


void TimerOFF()
{
	TCCR1B = 0x00; //bit3bit1bit0: timer off

}
void TimerISR()
{
	TimerFlag = 1;
}

//in our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect)
{
	//CPU automattically calls when TCNT1==OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; //Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr==0)//results in a more efficient compare
	{
		TimerISR(); //call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;

}



int main(void)
{

	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	DDRD = 0x00; PORTD = 0xFF;

	unsigned char coinage = 0;
	//unsigned char hold;
	TimerSet(25);
	TimerOn();
	USART_Init(0);
	USART_Flush(0);
	

		while(1)
		{
			
			
			if(USART_HasReceived(0))
			{
				coinage = USART_Receive(0);
			}
			
			if(coinage != 0)
			{
				PORTB = coinage;
			}
			
			
			while(!TimerFlag); //Wait 1 sec
			TimerFlag = 0;

		}

	//unsigned char button;


		

// write received data to B output
		
		
		//button = PINA & 0x01;
		//if(button)
		//{
			//PORTB = button;
		//}	
		//else
		//{
			//PORTB = 0;
		//}

   
}

