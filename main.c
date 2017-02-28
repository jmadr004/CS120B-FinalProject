/*
 * jmadr004_Final_Project.c
 *
 * Created: 2/28/2017 10:26:58 AM
 * Author : user
 */ 

#include <avr/io.h>
#include "USART.h"


int main(void)
{

	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;

	unsigned char coinage;
	//unsigned char hold;
	USART_Init(0);
	coinage = USART_Receive();

		

	//unsigned char button;


    while (1) 
    {
		
		PORTB = coinage;

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
}

