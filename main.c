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

//variables
unsigned char movement= 0;
unsigned char pass = 0;
unsigned char hold;
unsigned char Select_flag = 0;
unsigned char Make_Change = 0;
unsigned char Pepsi_Button;
unsigned char Coke_Button;
unsigned char Coke_Flag = 0;
unsigned char Pepsi_Flag = 0;
unsigned char Change_FLag = 0;
unsigned char Dipense_Flag = 1;
unsigned char quaters = 0;
unsigned char dimes = 0;
unsigned char nickels = 0;
uint8_t ByteOfData;
unsigned char Bank;
unsigned char admin_button = 0;



enum C_States{C_Initial, C_Wait }C_State;

void C_Tick()
{
	switch(C_State)
	{
		case C_Initial:
		C_State = C_Wait;
		break;
		
		case C_Wait:
			
			if(pass == 0)
			{
				movement = 0;
				hold = 0;
			}


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

				if(pass >= 15)
				{
					Select_flag = 1;
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
unsigned short counter = 0;
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
		if(counter < 500)
		{
			counter++;
			LCD_State = LCD_Wait;
		}
		else if(counter >= 500)
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

			if(Select_flag == 1)
			{
				LCD_DisplayString(17, "Coke or Pepsi");
				
			}
			else if(Select_flag ==0)
			{
				LCD_DisplayString(17, "GIVE ME MONEY");
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

enum S_STATES{S_INITIAL, S_WAIT, S_PEPSI, S_COKE}S_STATE;
unsigned short Motor_Counter = 0;
unsigned char test_flag =0;

void S_TICK()
{
	switch(S_STATE)
	{
		case S_INITIAL:
			S_STATE = S_WAIT;
		break;
		
		case S_WAIT:
			if(Coke_Flag == 1)
			{	
				S_STATE = S_COKE;
			}
			else if(Pepsi_Flag == 1)
			{
				S_STATE = S_PEPSI;
			}
			else
			{
				S_STATE = S_WAIT;
			}
		break;
			
		case S_PEPSI: 
			if(Motor_Counter == 0)
			{
				Motor_Counter++;
				PORTB = 0x0A;
			}
			else if(Motor_Counter == 1)
			{
				Motor_Counter++;
				PORTB = 0x06;
			}
			else if(Motor_Counter == 2)
			{
				Motor_Counter++;
				PORTB = 0x05;
			}
			else if(Motor_Counter == 3)
			{
				Motor_Counter= 0;
				PORTB = 0x09;
			}
			S_STATE = S_WAIT;
		break;

		case S_COKE:
			if(Motor_Counter == 0)
			{
				Motor_Counter++;
				PORTB = 0xA0;
			}
			else if(Motor_Counter == 1)
			{
				
				Motor_Counter++;
				PORTB = 0x60;
			}
			else if(Motor_Counter == 2)
			{
				
				Motor_Counter++;
				PORTB = 0x50;
			}
			else if(Motor_Counter == 3)
			{
				
				Motor_Counter= 0;
				PORTB = 0x90;
			}
		S_STATE = S_WAIT;
		break;
	
		default:
		S_STATE = S_INITIAL;
		break;
	}

}


enum S2_STATES{S2_INITIAL, S2_WAIT, S2_CHANGE}S2_STATE;
unsigned short Motor_Counter1 = 0;
void S2_TICK()
{


	switch(S2_STATE)
	{
		case S2_INITIAL:
		S2_STATE = S2_WAIT;
		break;
		
		case S2_WAIT:
		if(Dipense_Flag == 1)
		{
			S2_STATE = S2_CHANGE;
		}
		else
		{
			S2_STATE = S2_WAIT;
		}
		break;
		
		case S2_CHANGE:
		test_flag = 0x80;
			if(quaters > 0)
			{
				quaters--;
				if(Motor_Counter1 == 0)
				{
					Motor_Counter1++;
					PORTD = 0x12;
				}
				else if(Motor_Counter1 == 1)
				{
					Motor_Counter1++;
					PORTD = 0x06;
				}
				else if(Motor_Counter1 == 2)
				{
					Motor_Counter1++;
					PORTD = 0x05;
				}
				else if(Motor_Counter1 == 3)
				{
					Motor_Counter1= 0;
					PORTD = 0x017;
				}
				else if(quaters==0)
				{
					Dipense_Flag = 0;
				}
			}
		S2_STATE = S2_WAIT;
		break;

		default:
		S2_STATE = S2_INITIAL;
		break;
	}

}


enum CHOOSE_STATES{CHOOSE_INITIAL, CHOOSE_WAIT, CHOOSE_COKE, CHOOSE_PEPSI}CHOOSE_STATE;


void CHOOSE_TICK()
{

	Pepsi_Button = ~PINA & 0x08;
	Coke_Button = ~PINA & 0x04;
	
	switch(CHOOSE_STATE)
	{
		case CHOOSE_INITIAL:
			CHOOSE_STATE = CHOOSE_WAIT;
		break;

		case CHOOSE_WAIT:
			if(Pepsi_Button&&!Coke_Button)
			{
				CHOOSE_STATE = CHOOSE_PEPSI;
			}
			else if(Coke_Button&&!Pepsi_Button)
			{
				CHOOSE_STATE = CHOOSE_COKE;
			}
			else if(!Pepsi_Button&&!Coke_Button)
			{
				CHOOSE_STATE = CHOOSE_WAIT;
			}
		break;

		case CHOOSE_COKE:
			Coke_Flag = 1;
			Select_flag = 0;
			Change_FLag = 1;
			 CHOOSE_STATE = CHOOSE_WAIT;
		break;

		case CHOOSE_PEPSI:
			Pepsi_Flag = 1;
			Select_flag = 0;
			Change_FLag = 1;
			CHOOSE_STATE = CHOOSE_WAIT;
		break;

		default:
			CHOOSE_STATE = CHOOSE_INITIAL;
		break;
	}
}

enum CHANGE_STATES{CHANGE_INTIAL, CHANGE_WAIT}CHANGE_STATE;
void CHANGE_TICK()
{
	switch(CHANGE_STATE)
	{
		case CHANGE_INTIAL:
			Make_Change = pass - 15;
			Bank += pass - 15;
			pass = 0;
			ByteOfData += Bank;
			eeprom_update_byte (( uint8_t *) 46, ByteOfData );
			CHANGE_STATE = CHANGE_WAIT;
		break;

		case CHANGE_WAIT:

			if(Make_Change >= 5)
			{
				Make_Change = Make_Change - 5;
				quaters++;
			}
			else if(Make_Change >= 2)
			{
				Make_Change = Make_Change - 2;
				dimes++;
			}
			else if(Make_Change >= 1)
			{
				Make_Change = Make_Change - 1;
				nickels++;
			}
			else if(Make_Change==0)
			{
				Change_FLag = 0;
				Dipense_Flag=1;
			}
			CHANGE_STATE = CHANGE_WAIT;
		break;	


		default:	
			CHANGE_STATE = CHANGE_INTIAL;
		break;

	}
	
}


int main(void)
{

	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x03; PORTA = 0xFC;
	DDRD = 0xFE; PORTD = 0x01;
	DDRC = 0xFF; PORTC = 0x00;
	LCD_State = LCD_Initial;
	C_State = C_Initial;
	S_STATE = S_INITIAL;
	CHOOSE_STATE = CHOOSE_INITIAL;
	CHANGE_STATE = CHANGE_INTIAL;
	
	unsigned char C_Counter=0;
	unsigned short LCD_REFRESH = 0;
	unsigned short Stepper_counter = 0;
	unsigned short Dispense_counter = 0;
	

	//usart initialization
	initUSART(0);
	USART_Flush(0);

	TimerSet(1);
	TimerOn();
	LCD_init();

	
	
		while(1)
		{

			
			

			if(LCD_REFRESH < 1500)
			{
				LCD_REFRESH++;
			}
			else if(LCD_REFRESH >= 1500)
			{
				LCD_ClearScreen();
				LCD_REFRESH = 0l;
			}



			if(C_Counter < 25)
			{
				C_Counter++;
			}
			else if(C_Counter >= 25)
			{
				C_Tick();
				C_Counter = 0;
			}

			
			LCD_Tick();
			
			if(Select_flag == 1)
			{
				CHOOSE_TICK();
			}

			if(Change_FLag == 0)
			{	
				CHANGE_STATE = CHANGE_INTIAL;
				
			}
			else if (Change_FLag == 1)
			{
				CHANGE_TICK();
			}



			if((Coke_Flag == 1))
			{
				if(Stepper_counter < 250)
				{
					S_TICK();
					Stepper_counter++;
				}
				else if(Stepper_counter >= 250)
				{
					Stepper_counter = 0;
					Coke_Flag = 0;
					Pepsi_Flag = 0;
				}
			}

			if(Dipense_Flag == 1)
			{
				if(Dispense_counter < 500)
				{
					S2_TICK();
					Dispense_counter++;
				}
				else if(Dispense_counter >= 500)
				{
					Dispense_counter = 0;
				}
			}
			PORTD = test_flag;
			

			while(!TimerFlag); //Wait 1 sec
			TimerFlag = 0;
		}

		
   
}

