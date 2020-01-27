#include <avr/interrupt.h>
#include <stdint.h>
#include "Eeprom.h"

bool EEPROM::WriteByte(uint16_t eepromAddress, uint8_t byte)
{
	while((EECR & (1 << EEPE)) != 0); // wait for EEPE to be cleared after previous EEPROM write

	EEAR = eepromAddress;	// load EEPROM address
	EEDR = byte;			// load EEPROM data byte
	
	cli();					// disable global interrupts to prevent EEPROM write cycle fail
	
	EECR |= (1 << EEMPE);   // set EEPROM Master Write Enable
	EECR |= (1 << EEPE);    // start EEPROM write by setting EEPROM Write Enable
	
	sei();					// re-activate global interrupts
	
	return true;
}

bool EEPROM::UpdateByte(uint16_t eepromAddress, uint8_t byte)
{
	if (ReadByte(eepromAddress) != byte) // update only if EEPROM value != RAM value
	{
		WriteByte(eepromAddress, byte); // write one byte from RAM to EEPROM
	}	
	
	return true;
}

uint8_t EEPROM::ReadByte(uint16_t eepromAddress)
{
	while((EECR & (1 << EEPE)) != 0);  // wait for EEPE to be cleared in case of previous EEPROM write

	EEAR = eepromAddress;		// load the address from where to read EEPROM.
	
	cli();						// disable global interrupts to prevent EEPROM write cycle fail
	
	EECR |= (1 << EERE);		// start EEPROM read by setting EERE
	
	sei();						// re-activate global interrupts
	
	return EEDR;				// return data from EEPROM data register
}
