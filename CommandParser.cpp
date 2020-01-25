
#include "CommandParser.h"

bool CommandParser::Parse()
{
	unsigned char command;
	bool ret = uart.Receive(&command);
	if (!ret)
	{
		return false;
	}
	switch(command)
	{
		case 'v':
			Version();
			break;

		case 't':
			TerminalMode();
			break;

		case 's':
			SetModules();
			break;

		case 'm':
			ReturnData('m');
			break;

		default:
			return false;
	}
	unsigned char cr = uart.WaitReceive();
	return (ret && cr == 0x0D);
}

unsigned char CommandParser::ReadNumber()
{
	if (terminalMode)
	{
		unsigned char nibble1 = uart.WaitReceive();
		nibble1 -= '0';
		if (nibble1 >= 10)
		{
			nibble1 -= 7;
		}

		unsigned char nibble2 = uart.WaitReceive();
		nibble2 -= '0';
		if (nibble2 >= 10)
		{
			nibble2 -= 7;
		}

		unsigned char number = (nibble1 << 4) + nibble2;
		return number;
	}
	else
	{
		unsigned char number = uart.WaitReceive();
		return number;
	}
}

void CommandParser::WriteNumber(const unsigned char number)
{
	if (terminalMode)
	{
		unsigned char nibble1 = number >> 4;
		if (nibble1 >= 10)
		{
			nibble1 += 7;
		}
		unsigned char nibble2 = number & 0x0F;
		if (nibble2 >= 10)
		{
			nibble2 += 7;
		}
		uart.Send(nibble1);
		uart.Send(nibble2);
	}
	else
	{
		uart.Send(number);
	}
}

void CommandParser::Version()
{
	const unsigned char* data = reinterpret_cast<const unsigned char*>("1.0.0 / 24.01.20 / HSI-88-Clone by Teddy\r");
	uart.Send(data, 41);
}

void CommandParser::TerminalMode()
{
	terminalMode = !terminalMode;
	uart.Send('t');
	uart.Send(terminalMode + '0');
	uart.Send('\r');
}

void CommandParser::SetModules()
{
	//uart.Send(*buffer);
	unsigned char modules1 = ReadNumber();
	unsigned char modules2 = ReadNumber();
	unsigned char modules3 = ReadNumber();
	unsigned char modulesTotal = modules1 + modules2 + modules3;
	uart.Send(modules1);
	uart.Send(modules2);
	uart.Send(modules3);
	WriteNumber(modulesTotal);
	uart.Send('\r');
	ReturnData('i');
}

void CommandParser::ReturnData(const unsigned char command)
{
	uart.Send(command);
	WriteNumber(0);
	uart.Send('\r');
}

