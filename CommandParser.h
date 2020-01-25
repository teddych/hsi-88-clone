#pragma once

#include "Hardware.h"
#include "S88.h"
#include "UART.h"

class CommandParser
{
	public:
		CommandParser() = delete;
		CommandParser(UART& uart, S88& s88)
		:	uart(uart),
		 	s88(s88),
		 	terminalMode(false)
		{}

		bool Parse();

	private:
		unsigned char ReadNumber();
		void WriteNumber(const unsigned char number);
		void Version();
		void TerminalMode();
		void SetModules();
		void ReturnData(const unsigned char command);

		UART& uart;
		S88& s88;

		bool terminalMode;
};
