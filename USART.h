#ifndef USART
#define USART

// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

	void USART_Init( unsigned int ubrr)
	{
		/*Set baud rate */
		UBRR0H = (unsigned char)(ubrr>>8);
		UBRR0L = (unsigned char)ubrr;
		//Enable receiver and transmitter 
		UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		/* Set frame format: 8data, 2stop bit */
		UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	}

	unsigned char USART_Receive( void )
	{
		/* Wait for data to be received */
		while ( !(UCSR0A & (1<<RXC0)) )
		;
		/* Get and return received data from buffer */
		return UDR0;
	}



#endif /* USART_ATMEGA1284_H_ */
