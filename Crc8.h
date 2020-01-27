#pragma once

#include <stdint.h>

class CRC8
{
	public:
		static uint8_t CalcString(const uint8_t* const str, const uint8_t size);
		
		CRC8() : actualValue(InitalValue) {};

		void CalcChar(const uint8_t c) { actualValue = CrcTable[actualValue ^ c]; }
		uint8_t Value() const { return actualValue; }
		void Reset() { actualValue = InitalValue; }
		
	private:
		static const uint8_t CrcTable[256];
		static const uint8_t InitalValue = 0x07;

		uint8_t actualValue;
};

