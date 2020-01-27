#pragma once

#include <stdint.h>

#include "UART.h"

class S88
{
	public:
		static S88* s88;

		struct UpdateQueueData
		{
			public:
				unsigned char module;
				unsigned char data1;
				unsigned char data2;
		};

		S88();

		void SetModules(unsigned char modules1, unsigned char modules2, unsigned char modules3);

		unsigned char DataAvailable() { return updateQueue.PacketsInQueue(); }
		UpdateQueueData GetData() { return updateQueue.Dequeue(); }
		unsigned char GetAllData(unsigned char** data) { *data = dataPublished; return modules16Total; }

		static void TimerInterruptStatic(S88* s88) { s88->TimerInterrupt(); }

	private:
		void GetModulesFromEeprom(unsigned char* modules1, unsigned char* modules2, unsigned char* modules3);
		void SetModulesToEeprom(unsigned char modules1, unsigned char modules2, unsigned char modules3);
		void InitDataMemory();
		void InitTimer();
		void TimerInterrupt();
		void ReadBit();
		void ClearDataBytes();
		void CalculateChanges();

		static const unsigned char MaxModules = 31;
		static const uint16_t EepromBaseAddress = 0x200;


		enum Status : unsigned char
		{
			Start = 0,
			Reset1,
			Reset2,
			ReadFirst,
			Read,
			Shift,
			ReadLast

		};

		unsigned char modules16_1;
		unsigned char modules16_2;
		unsigned char modules16_3;
		unsigned char modules16Max123;
		unsigned char modules16Total;
		unsigned char modules8_1;
		unsigned char modules8_2;
		unsigned char modules8_3;
		unsigned char bitsRead;
		unsigned char bitsToRead;

		unsigned char data1[MaxModules];
		unsigned char data2[MaxModules];
		unsigned char data3[MaxModules];
		unsigned char* dataPublished;
		unsigned char* dataUnpublished;
		unsigned char* dataReading;

		Status status;

		RingBuffer<UpdateQueueData, 7> updateQueue;
};
