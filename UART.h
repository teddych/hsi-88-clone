#pragma once

#include "RingBuffer.h"

class UART
{
	public:
		static RingBuffer<unsigned char> RxBuffer;

		UART();

		void Send(const unsigned char c);
		void Send(const unsigned char* buffer, const unsigned char size);
		unsigned char WaitReceive();
		bool Receive(unsigned char* c);
		bool DataAvailable() { return !RxBuffer.IsEmpty(); }

	private:
};
