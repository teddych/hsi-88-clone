
#include "Hardware.h"
#include "CommandParser.h"
#include "UART.h"
#include "S88.h"

void InitHardware()
{
	// Input/Output Ports initialization
	PORTA = (1<<S88DIN1)       // high = pull up
	| (1<<S88DIN2)       // high = pull up
	| (1<<S88DIN3)       // high = pull up
	| (1<<EXT_STOP)      // high = pull up
	| (1<<LED_GO)        // low = turn on
	| (1<<LED_STOP)      // low = turn on
	| (1<<KEY_STOP)      // high = pull up
	| (1<<KEY_GO);       // high = pull up
	DDRA  = (0<<S88DIN1)       // in
	| (0<<S88DIN2)       // in
	| (0<<S88DIN3)       // in
	| (0<<EXT_STOP)      // in
	| (1<<LED_GO)        // out
	| (1<<LED_STOP)      // out
	| (0<<KEY_STOP)      // in
	| (0<<KEY_GO);       // in
	// Port B
	PORTB = (0<<S88_CLOCK)       // 0 clk = low
	| (0<<S88_RESET)     // 1 reset = low
	| (1<<ACK_DETECT)    // 2 pullup
	| (1<<S88_LOAD)      // 3 load = high
	| (1<<DMX_OUT)       // 4 high = inactiv
	// 5 MOSI
	| (0<<MY_CTS)        // 6
	| (1<<MY_RTS);       // 7 high = pullup

	DDRB  = (1<<S88_CLOCK)       // out
	| (1<<S88_RESET)     // out
	| (0<<ACK_DETECT)    // in
	| (1<<S88_LOAD)      // out
	| (1<<DMX_OUT)       // out
	| (1<<MY_CTS)        // out
	| (0<<MY_RTS);       // in
	// Port C
	PORTC = (1<<SHORT_MAIN)    // pull up
	| (1<<SHORT_PROG)    // pull up
	| (1<<LED_CTRL)      // low = turn on
	| (1<<LED_RS232);    // low = turn on

	DDRC  = (0<<SHORT_MAIN)    // in
	| (0<<SHORT_PROG)    // in
	| (1<<LED_CTRL)      // out
	| (1<<LED_RS232);    // out
	// Port D
	PORTD = (1<<MY_RXD)        // in, pullup
	| (1<<MY_TXD)        // out
	| (1<<JUMPER1)       // pullup
	| (0<<JUMPER2)       // in
	| (1<<NDCC)          // out
	| (1<<DCC)           // out
	| (1<<ENA_PROG)      // out
	| (1<<ENA_MAIN);     // out

	ACSR=0x80;
}

int main()
{
	InitHardware();

	UART uart;
	/*
	while (true)
	{
		unsigned char c;
		if (uart.Receive(&c))
		{
			uart.Send(c);
			_delay_ms(1000);
		}
	}
	*/

	S88 s88(uart);
	CommandParser parser(uart, s88);

	while (true)
	{
		if (uart.DataAvailable())
		{
			parser.Parse();
		}
	}
	return 0;
}
