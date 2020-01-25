
#include "CommandParser.h"

bool CommandParser::Run()
{
	while (true)
	{
		SendChangedData();
		Parse();
	}
}

bool CommandParser::Parse()
{
	if (!uart.DataAvailable())
	{
		return true;
	}

	unsigned char command;
	bool ret = uart.Receive(&command);
	if (!ret)
	{
		LED_STOP_ON;
		return false;
	}

	switch (command)
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
	if (cr != '\r')
	{
		LED_STOP_ON;
		return false;
	}
	return true;
}

void CommandParser::SendChangedData()
{
	unsigned char packets = s88.DataAvailable();
	if (packets == 0)
	{
		return;
	}
	for (unsigned char module = 0; module < packets; ++module)
	{

	}
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
	uart.Send('s');
	unsigned char modules1 = ReadNumber();
	unsigned char modules2 = ReadNumber();
	unsigned char modules3 = ReadNumber();
	unsigned char modulesTotal = modules1 + modules2 + modules3;
	s88.SetModules(modules1, modules2, modules3);
	WriteNumber(modulesTotal);
	uart.Send('\r');
	ReturnData('i');
}

void CommandParser::ReturnData(const unsigned char command)
{
	uart.Send(command);
	unsigned char* data;
	unsigned char modules = s88.GetAllData(&data);
	WriteNumber(modules);
	for (unsigned char module = 0; module < modules; ++module)
	{
		WriteNumber(module);
		unsigned char byte = module << 1;
		WriteNumber(data[byte]);
		WriteNumber(data[byte + 1]);
	}
	uart.Send('\r');
}

