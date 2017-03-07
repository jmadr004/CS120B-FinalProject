/*
 * jmadr004_Final_Project.c
 *
 * Created: 2/28/2017 10:26:58 AM
 * Author : user
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "USART.h"
#include "io.c"



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

enum C_States{C_Initial, C_Wait }C_State;

unsigned char movement= 0;
unsigned char pass = 0;
unsigned char hold;




void C_Tick()
{
	switch(C_State)
	{
		case C_Initial:
		C_State = C_Wait;
		break;
		
		case C_Wait:
			if(USART_HasReceived(0))
			{
				pass++;

				if(pass >=21 && pass <= 39)
				{
					movement++;	
				}
				else if(pass==40)
				{
					movement = 0;
				}
				else if(pass >= 41 && pass <= 59)
				{
					movement++;
				}
				else if(pass==60)
				{
					movement = 0;
				}
				else if(pass >= 61 && pass <= 79)
				{
					movement++;
				}
				else if(pass==80)
				{
					movement = 0;
				}

				USART_Flush(0);
				C_State = C_Wait;
			}
			else
			{
				C_State = C_Wait;
			}

		break;

		default:
		C_State = C_Initial;
		break;
	}


	switch(C_State)
	{
		case C_Initial:
		break;
		
		case C_Wait:
		break;

		default:
		break;
	}

}

enum LCD_States{LCD_Initial, LCD_Wait }LCD_State;
unsigned char counter = 0;
const unsigned char* Test[20]={"00","05","10","15","20","25","30","35","40","45","50","55","60","65","70","75","80","85","90","95"};
const unsigned char* Test2[20]={"1.", "2.","3.","4.","5.","6.","7.","8.","9.","10.","11.","12.","13.","14.","15.","16.","17.","18.","19.","20."};

void LCD_Tick()
{

	switch(LCD_State)
	{
		case LCD_Initial:
			LCD_State = LCD_Wait;
		break;
		
		case LCD_Wait:
		if(counter < 20)
		{
			counter++;
			LCD_State = LCD_Wait;
		}
		else if(counter >= 20)
		{
			if(pass <= 19)
			{
				LCD_DisplayString(1, "Change:.");
				LCD_DisplayString(9, Test[(int)pass]);

			}
			else if(pass >= 20)
			{
				
				if(pass==20)
				{
					hold=0;
				}
				else if(pass==40)
				{
					hold=1;
				}
				else if(pass==60)
				{
					hold=2;
				}
				LCD_DisplayString(1, "Change:");
				LCD_DisplayString(8, Test2[(int)hold]);
				LCD_DisplayString(10, Test[(int)movement]);
			}
			
			LCD_State = LCD_Wait;
			counter = 0;
		}
		break;

		default:
		LCD_State = LCD_Initial;
		break;
	}

switch(LCD_State)
{
	case LCD_Initial:
	break;
	
	case LCD_Wait:
	break;

	default:
	break;
}

}

int main(void)
{

	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x03; PORTA = 0xFC;
	DDRD = 0x00; PORTD = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	LCD_State = LCD_Initial;
	C_State = C_Initial;
	

	//usart initialization
	initUSART(0);
	USART_Flush(0);

	TimerSet(25);
	TimerOn();
	LCD_init();
	
		while(1)
		{
			
			C_Tick();
			LCD_Tick();
			
			//if(USART_HasReceived(0))
			//{
				//pass++;
				//USART_Flush(0);
			//}
			//
			//if(counter	< 20 )
			//{
				//counter++;
			//}
			//else if(counter >=20 )
			//{
				//LCD_DisplayString(1, "Change:");
				//LCD_DisplayString(7, Test[(int)pass]);
				//counter = 0;
//
			//}

			
		
				
			//if(USART_HasReceived(0))
			//{
				//pass++;
				//coinage = USART_Receive(0);
				//
			//
			//
			

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

