#pragma once

template <class T, unsigned char BitSize>
class RingBuffer
{
	public:
		static const unsigned char size = 1 << BitSize;
		static const unsigned char mask = size - 1;

		RingBuffer()
		:	write(1),
			read(0)
		{
		}

		bool Enqueue(const T& data)
		{
			if (IsFull())
			{
				return false;
			}
			buffer[write] = data;
			write = (write + 1) & mask;
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

		unsigned char PacketsInQueue()
		{
			if (write > read)
			{
				return write - read - 1;
			}
			return size - (read - write) - 1;
		}

	private:
		T buffer[size];
		volatile unsigned char write;
		volatile unsigned char read;
};
