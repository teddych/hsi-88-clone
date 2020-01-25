#pragma once

#include "RingBuffer.h"

class UART
{
	public:
		static UART* uart;

		UART();

		void Send(const unsigned char c);
		void Send(const unsigned char* buffer, const unsigned char size);
		unsigned char WaitReceive();
		bool Receive(unsigned char* c);
		bool DataAvailable() { return !RxBuffer.IsEmpty(); }
		static void RxInterruptStatic(UART* uart) { uart->RxInterrupt(); }

	private:
		void RxInterrupt();

		RingBuffer<unsigned char, 3> RxBuffer;
		RingBuffer<unsigned char, 7> TxBuffer;
};
