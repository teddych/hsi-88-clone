#pragma once

class EEPROM
{
	public:
		EEPROM() = delete;
		static bool UpdateByte(uint16_t eepromAddress, uint8_t byte);
		static uint8_t ReadByte(uint16_t eepromAddress);

	private:
		static bool WriteByte(uint16_t eepromAddress, uint8_t byte);
};
