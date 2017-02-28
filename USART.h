#ifndef USART
#define USART

// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

	void USART_Init( unsigned int usartNum)
	{
		if (usartNum != 1) {
			// Turn on the reception circuitry of USART0
			// Turn on receiver and transmitter
			// Use 8-bit character sizes
			UCSR0B |= (1 << RXEN0)  | (1 << TXEN0);
			UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
			// Load lower 8-bits of the baud rate value into the low byte of the UBRR0 register
			UBRR0L = BAUD_PRESCALE;
			// Load upper 8-bits of the baud rate value into the high byte of the UBRR0 register
			UBRR0H = (BAUD_PRESCALE >> 8);
		}
		else {
			// Turn on the reception circuitry for USART1
			// Turn on receiver and transmitter
			// Use 8-bit character sizes
			UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);
			UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);
			// Load lower 8-bits of the baud rate value into the low byte of the UBRR1 register
			UBRR1L = BAUD_PRESCALE;
			// Load upper 8-bits of the baud rate value into the high byte of the UBRR1 register
			UBRR1H = (BAUD_PRESCALE >> 8);
		}
	}

	unsigned char USART_Receive(unsigned char usartNum)
	{
		if (usartNum != 1) {
			while ( !(UCSR0A & (1 << RXC0)) ); // Wait for data to be received
			return UDR0; // Get and return received data from buffer
		}
		else {
			while ( !(UCSR1A & (1 << RXC1)) );
			return UDR1;
		}
	}


	unsigned char USART_HasReceived(unsigned char usartNum)
	{
		return (usartNum != 1) ? (UCSR0A & (1 << RXC0)) : (UCSR1A & (1 << RXC1));
	}

	void USART_Flush(unsigned char usartNum)
	{
		static unsigned char dummy;
		if (usartNum != 1) {
			while ( UCSR0A & (1 << RXC0) ) { dummy = UDR0; }
		}
		else {
			while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
		}
	}


#endif
