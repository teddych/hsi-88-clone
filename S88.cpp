#include <avr/interrupt.h>

#include "Hardware.h"
#include "S88.h"

//-----------------------------------------------------------------------
// timing to read s88
//
// Load  _______/---------------------\______________________________________
//
// Clk   ___________/---\_____________________/---\___/---\___/---\___/---\__
//
// Reset ____________________/---\___________________________________________
//
// Data  ___________/x1.x1.x1.x1.x1.x1.x1.x1../\x2.x2./\x3.x3./\x3.x3./\x4.x4.
//
// Durations:
// with 30m of cable: t(pd)=200ns;
// with 44pF/m and Rout=250Ohm: t(rc)=11ns/m;
// Note: 20us are 320 cycles with a 16 MHz CPU

S88::S88()
:	modules16_1(2),
	modules16_2(2),
	modules16_3(2),
	modules16Max123(2),
	modules16Total(6),
	modules8_1(4),
	modules8_2(4),
	modules8_3(4),
	bitsRead(0),
	bitsToRead(modules16Max123 * 16),
	dataPublished(data1),
	dataUnpublished(data2),
	dataReading(data3),
	status(Start)
{
	InitDataMemory();
	S88::s88 = this;
	InitTimer();
}

void S88::InitDataMemory()
{
	for (unsigned char i = 0; i < sizeof(data1); ++i)
	{
		data1[i]=0;
	}

	for (unsigned char i = 0; i < sizeof(data2); ++i)
	{
		data2[i]=0;
	}

	for (unsigned char i = 0; i < sizeof(data3); ++i)
	{
		data3[i]=0;
	}
}

void S88::SetModules(unsigned char modules1, unsigned char modules2, unsigned char modules3)
{
	this->modules16_1 = modules1;
	this->modules16_2 = modules2;
	this->modules16_3 = modules3;
	this->modules8_1 = modules1 << 1;
	this->modules8_2 = modules2 << 1;
	this->modules8_3 = modules3 << 1;
	modules16Max123 = modules1;
	if (modules2 > modules16Max123)
	{
		modules16Max123 = modules2;
	}
	if (modules3 > modules16Max123)
	{
		modules16Max123 = modules3;
	}
	modules16Total = modules1 + modules2 + modules3;
	bitsToRead = modules16Max123 * 16;
}

void S88::InitTimer()
{
	TCCR2A = (1 << WGM21) | (0 << WGM20);
	TCCR2B = (1 << FOC2A) | (0 << FOC2B) | (0 << WGM22) | (0 << CS22) | (1 << CS21) | (1 << CS20);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 12;
	sei();
}

void S88::TimerInterrupt()
{
	LED_CTRL_ON;
	switch(status)
	{
		case Start:
			S88_RESET_LOW;
			S88_LOAD_HIGH;
			S88_CLOCK_LOW;
			status = Reset1;
			break;

		case Reset1:
			S88_CLOCK_HIGH;
			status = Reset2;
			bitsRead = 0;
			break;

		case Reset2:
			S88_LOAD_LOW;
			S88_RESET_HIGH;
			status = ReadFirst;
			break;

		case ReadFirst:
			S88_RESET_LOW;
			S88_CLOCK_LOW;
			ReadBit();
			status = Shift;
			break;

		case Read:
			S88_CLOCK_LOW;
			ReadBit();
			status = Shift;
			break;

		case Shift:
			S88_CLOCK_HIGH;
			status = (bitsRead + 1 < bitsToRead ? Read : ReadLast);
			break;

		case ReadLast:
			S88_CLOCK_LOW;
			ReadBit();
			CalculateChanges();
			status = Start;
			break;

		default:
			S88_RESET_LOW;
			S88_LOAD_LOW;
			S88_CLOCK_LOW;
			status = Start;
			break;
	}
	LED_CTRL_OFF;
}

void S88::ReadBit()
{
	unsigned char module = bitsRead >> 3;
	unsigned char shiftInDataByte = bitsRead & 0x07;
	if (shiftInDataByte == 0)
	{
		ClearDataBytes();
	}
	if (module < modules8_1)
	{
		unsigned char dataBitBus1 = READ_S88_D1;
		dataReading[module] |= (dataBitBus1 << shiftInDataByte);
	}
	if (module < modules8_2)
	{
		unsigned char dataBitBus2 = READ_S88_D2;
		dataReading[modules8_1 + module] |= (dataBitBus2 << shiftInDataByte);
	}
	if (module < modules8_3)
	{
		unsigned char dataBitBus3 = READ_S88_D3;
		dataReading[modules8_1 + modules8_2 + module] |= (dataBitBus3 << shiftInDataByte);
	}
	++bitsRead;
}

void S88::ClearDataBytes()
{
	unsigned char module = bitsRead >> 3;
	if (module < modules8_1)
	{
		dataReading[module] = 0x00;
	}
	if (module < modules8_2)
	{
		dataReading[modules8_1 + module] = 0x00;
	}
	if (module < modules8_3)
	{
		dataReading[modules8_1 + modules8_2 + module] = 0x00;
	}
}

void S88::CalculateChanges()
{
	for (unsigned char module = 0; module < modules16Total; ++module)
	{
		unsigned char module8_1 = module << 1;
		unsigned char module8_2 = module8_1 + 1;
		bool changeModule1 = (dataReading[module8_1] != dataPublished[module8_1]) && (dataReading[module8_1] == dataUnpublished[module8_1]);
		bool changeModule2 = (dataReading[module8_2] != dataPublished[module8_2]) && (dataReading[module8_2] == dataUnpublished[module8_2]);
		if (!changeModule1 && !changeModule2)
		{
			continue;
		}
		UpdateQueueData queueData;
		queueData.module = module;
		if (changeModule1 && changeModule2)
		{
			queueData.data1 = dataReading[module8_1];
			queueData.data2 = dataReading[module8_2];
		}
		else if (changeModule1)
		{
			queueData.data1 = dataReading[module8_1];
			queueData.data2 = dataPublished[module8_2];
		}
		else // changeModule2
		{
			queueData.data1 = dataPublished[module8_1];
			queueData.data2 = dataReading[module8_2];
		}
		if (!updateQueue.Enqueue(queueData))
		{
			LED_STOP_ON;
			continue;
		}
	}
	unsigned char* temp = dataPublished;
	dataPublished = dataUnpublished;
	dataUnpublished = dataReading;
	dataReading = temp;
}

S88* S88::s88;

ISR(TIMER2_COMPA_vect)
{
	S88::TimerInterruptStatic(S88::s88);
}
