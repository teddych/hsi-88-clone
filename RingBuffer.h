#pragma once

template <class T>
class RingBuffer
{
	public:
		static const unsigned char size = 1 << 4;
		static const unsigned char mask = size - 1;

		RingBuffer()
		:	write(1),
			read(0)
		{
			buffer;
		}

		bool Enqueue(T data)
		{
			if (IsFull())
			{
				return false;
			}
			buffer[write] = data;
			write = (write + 1) & mask;
			LED_RS232_ON;
			_delay_ms(10);
			LED_RS232_OFF;
			return true;
		}

		T Dequeue()
		{
			unsigned char nextRead = (read + 1) & mask;
			T data = buffer[nextRead];
			read = nextRead;
			return data;
		}

		bool IsEmpty()
		{
			unsigned char nextRead = (read + 1) & mask;
			return (nextRead == write);
		}

		bool IsFull()
		{
			return write == read;
		}

	private:
		volatile T buffer[size];
		volatile unsigned char write;
		volatile unsigned char read;
};
