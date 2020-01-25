#pragma once

#include "UART.h"

class S88
{
	public:
		S88() = delete;
		S88(UART& uart);

		bool Read();

	private:
		void InitDataMemory();
		void Reset();

		static const unsigned char Pulsduration = 100; // us
		static const unsigned char MaxModules = 31;

		unsigned char modules1;
		unsigned char modules2;
		unsigned char modules3;
		unsigned char modulesMax123;
		unsigned char modulesTotal;

		unsigned char data1[MaxModules];
		unsigned char data2[MaxModules];
		unsigned char data3[MaxModules];
		unsigned char change[MaxModules];

		UART& uart;
};

/*
void s88_clock_pulse(void);
void s88_reset(void);
void init_datamemory_s88(void);
void read_s88(unsigned char* s88_data);
unsigned char init_s88();
unsigned char my_scan(unsigned char *buffer);
void my_sprint_num(char *buffer, unsigned char num);
void my_sprint_hex(unsigned char *buffer, unsigned char num);
void hsi_send_num(unsigned char nummer);
void hsi_send_byte(unsigned char nummer);
void full_hsi_message(unsigned char* s88_data);
void copyright_message(void);
void hsi88_parse_rs232();
void hsi88_parser(void);
*/
