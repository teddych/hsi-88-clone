
#include "Hardware.h"

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "UART.h"

UART::UART()
{
	// Set baudrate with util/setbaud.h
	#define BAUD 9600UL
	#include <util/setbaud.h>
	UBRR0 = UBRR_VALUE;
	#if USE_2X
	UCSR0A = (1 << U2X0);
	#else
	UCSR0A = 0;
	#endif
	
	// UART TX, RX und RX-Interrupt on
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

	// Asynchron 8N1
	UCSR0C = (0 << UPM01) | (0 << UPM00) | (0 << UCSZ02) | (1 << UCSZ01) | (1 << UCSZ00) | (0 << USBS0);
	#undef BAUD

	// read all unused data
	while (UCSR0A & (1<<RXC0))
	{
		char __attribute__((__unused__)) dummy = UDR0;
	}

	UART::uart = this;
	sei();
}

void UART::Send(const unsigned char c)
{
	// wait until sending is possible
	while (!(UCSR0A & (1<<UDRE0)));
	
	LED_RS232_ON;
	// writes char c to usart
	UDR0 = c;
	LED_RS232_OFF;
}

void UART::Send(const unsigned char* buffer, const unsigned char size)
{
	for (unsigned char position = 0; position < size; ++position)
	{
		Send(buffer[position]);
	}
}

unsigned char UART::WaitReceive()
{
	while(!DataAvailable());
	return RxBuffer.Dequeue();
}

bool UART::Receive(unsigned char* c)
{
	if (!DataAvailable())
	{
		return false;
	}
	*c = RxBuffer.Dequeue();
	return true;
}

void UART::RxInterrupt()
{
	if (UCSR0A & ((1 << UPE0) | (1 << FE0)))
	{
		char __attribute__((__unused__)) dummy = UDR0;
		return;
	}
	unsigned char data = UDR0;
	RxBuffer.Enqueue(data);
}

UART* UART::uart;

ISR(USART0_RX_vect)
{
	UART::RxInterruptStatic(UART::uart);
}
