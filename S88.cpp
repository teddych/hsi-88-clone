
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

S88::S88(UART& uart)
:	uart(uart)
{
	InitDataMemory();
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

	for (unsigned char i = 0; i < sizeof(change); ++i)
	{
		change[i]=0;
	}
}

bool S88::Read()
{
	//Reset();
	return true;
}

void S88::Reset()
{
	S88_LOAD_HIGH;
	_delay_us(Pulsduration);
	S88_LOAD_LOW;
	S88_CLOCK_HIGH;
	_delay_us(Pulsduration);
	S88_CLOCK_LOW;
	S88_RESET_HIGH;
	_delay_us(Pulsduration);
	S88_RESET_LOW;
}

/*
void s88_clock_pulse(void)
{
	S88_CLOCK_HIGH;
	_delay_us(s88_pulsduration);
	S88_CLOCK_LOW;
	_delay_us(s88_pulsduration);
}

void read_s88(unsigned char* s88_data)
{
	s88_reset();
	for (unsigned char module = 0; module < s88_size_max; ++module)
	{
		for(unsigned char bitshifter = 1; bitshifter != 0; bitshifter = bitshifter << 1)
		{
			if (module < s88_size1 && READ_S88_D1)
			{
				s88_data[module] |= bitshifter;
			}
			if (module < s88_size2 && READ_S88_D2)
			{
				s88_data[module + s88_size1] |= bitshifter;
			}
			if (module < s88_size3 && READ_S88_D3)
			{
				s88_data[module + s88_size1 + s88_size2] |= bitshifter;
			}

			s88_clock_pulse();
		}
	}
}

unsigned char init_s88()
{
	init_datamemory_s88();

	S88_RESET_LOW;
	S88_CLOCK_LOW;
	S88_LOAD_LOW;
	
	s88_size1 = 2;
	s88_size2 = 2;
	s88_size3 = 2;

	s88_size_max = s88_size1;
	if (s88_size2 > s88_size_max)
	{
		s88_size_max = s88_size2;
	}
	if (s88_size3 > s88_size_max)
	{
		s88_size_max = s88_size3;
	}

	s88_size_total = s88_size1 + s88_size2 + s88_size3;
	return (s88_size_total <= SIZE_S88_MAX);
}


unsigned char terminalmode = 0; // 0 = binary answers/ 1 = ASCII

unsigned char my_scan(unsigned char *buffer)
{
	unsigned char retval;
	retval = (*buffer++ & 0x0f) * 10;
	retval += (*buffer & 0x0f);
	return(retval);
}

void my_sprint_num(char *buffer, unsigned char num)
{
	*buffer++ = 0x30 + (num / 10);
	*buffer++ = 0x30 + (num % 10);
	*buffer   = 0;
}

void my_sprint_hex(unsigned char *buffer, unsigned char num)
{
	unsigned char hex;
	hex = num >> 4;
	hex += 0x30;
	if (hex >= 10)
	{
		hex += 0x07;   // +offset f�r ABCDEF
	}
	*buffer++ = hex;
	hex = num & 0x0f;
	hex += 0x30;
	if (hex >= 10)
	{
		hex += 0x07;   // +offset f�r ABCDEF
	}
	*buffer++ = hex;
	*buffer   = 0;
}


void hsi_send_num(unsigned char nummer)
{
	if (terminalmode)
	{
		while (tx_fifo_ready() == 0);
		char buffer[3];  // 2+1 f�r sprintf + 0
		my_sprint_num(buffer, nummer);
		tx_fifo_write(buffer[0]);
		tx_fifo_write(buffer[1]);
	}
	else
	{
		while (tx_fifo_ready() == 0);
		tx_fifo_write(nummer);
	}
}

void hsi_send_byte(unsigned char nummer)
{
	if (terminalmode)
	{
		while (tx_fifo_ready() == 0);
		unsigned char buffer[3];  // 2+1 f�r sprintf + 0
		my_sprint_hex(buffer, nummer);
		tx_fifo_write(buffer[0]);
		tx_fifo_write(buffer[1]);
	}
	else
	{
		while (tx_fifo_ready() == 0);
		tx_fifo_write(nummer);
	}
}


void full_hsi_message(unsigned char* s88_data)
{
	unsigned char no_of_modules = s88_size_total >> 1;
	hsi_send_num(no_of_modules);
	for (unsigned char module = 0; module < no_of_modules; ++module)
	{
		hsi_send_num(module + 1);
		hsi_send_byte(s88_data[2 * module + 1]);          // High byte first
		hsi_send_byte(s88_data[2 * module ]);
	}
	while (tx_fifo_ready() == 0);
	tx_fifo_write(0x0d);
}




// const char copyright[] PROGMEM = "Ver. x.xx / dd.mm.jj / HSI-88 / (c) LDT\r";
const char* copyright = "Ver. 1.00 / 22.01.20 / HSI-88-Clone / Teddy\r";

void copyright_message(void)
{
	char zeichen;
	unsigned char i=0;

	while ( (zeichen = copyright[i++]) )
	{
		while (tx_fifo_ready() == 0);
		tx_fifo_write(zeichen);
	}
}

void hsi88_parse_rs232()
{
	return;
	unsigned char cmd = rx_fifo_read();
	switch(cmd)
	{
		case 't':                                         // Command: toggle terminal mode
		{
			if (rx_fifo_read() == 0x0d)                   // discard terminating <cr>
			{
				tx_fifo_write('t');
				if (terminalmode)
				{
					terminalmode = 0;
					tx_fifo_write('0');
				}
				else
				{
					terminalmode = 1;
					tx_fifo_write('1');
				}
				LED_STOP_OFF;
				tx_fifo_write(0x0d);
			}
			else
			{
				LED_STOP_ON;
			}
			break;
		}

		case 's':                                          // Command: set modules
		{
			unsigned int no_of_modules_links;
			unsigned int no_of_modules_mitte;
			unsigned int no_of_modules_rechts;
			unsigned int no_of_modules;
			if (terminalmode)
			{
				unsigned char myzahl[3];  // 2+1 f�r sprintf + 0

				myzahl[0] = rx_fifo_read();
				myzahl[1] = rx_fifo_read();
				myzahl[2] = 0;
				no_of_modules_links = my_scan(myzahl);
				
				myzahl[0] = rx_fifo_read();
				myzahl[1] = rx_fifo_read();
				myzahl[2] = 0;
				no_of_modules_mitte = my_scan(myzahl);
				
				myzahl[0] = rx_fifo_read();
				myzahl[1] = rx_fifo_read();
				myzahl[2] = 0;
				no_of_modules_rechts = my_scan(myzahl);
				
				no_of_modules = no_of_modules_links + no_of_modules_mitte + no_of_modules_rechts;
				if (no_of_modules > SIZE_S88_MAX)
				{
					no_of_modules_links = 2;
					no_of_modules_mitte = 2;
					no_of_modules_rechts = 2;
					no_of_modules = 6;
					LED_STOP_ON;
				}
				if (rx_fifo_read() == 0x0d)                 // discard terminating <cr>
				{
					tx_fifo_write('s');
					hsi_send_num(no_of_modules);
					tx_fifo_write(0x0d);
				}
				else
				{
					LED_STOP_ON;
				}
			}
			else  // terminalmode = 1;
			{
				no_of_modules_links = rx_fifo_read();
				no_of_modules_mitte = rx_fifo_read();
				no_of_modules_rechts = rx_fifo_read();
				no_of_modules = no_of_modules_links + no_of_modules_mitte + no_of_modules_rechts;
				if (no_of_modules > SIZE_S88_MAX)
				{
					no_of_modules_links = 2;
					no_of_modules_mitte = 2;
					no_of_modules_rechts = 2;
					no_of_modules = 6;
					LED_STOP_ON;
				}
				if (rx_fifo_read() == 0x0d)                   // discard terminating <cr>
				{
					tx_fifo_write('s');
					tx_fifo_write((char) no_of_modules);
					tx_fifo_write(0x0d);
				}
				else
				{
					LED_STOP_ON;
				}
			}
			// init set modules and performs the first read
			LED_CTRL_ON;
			s88_size1 = no_of_modules_links * 2;
			s88_size2 = no_of_modules_mitte * 2;
			s88_size3 = no_of_modules_rechts * 2;
			init_s88();
			LED_CTRL_OFF;
			// now send answer 2
			tx_fifo_write('i');
			
			full_hsi_message(s88_data1);
			break;
		}

		case 'm':                                              // Command: full read
		{
			if (rx_fifo_read() == 0x0d)                        // discard terminating <cr>
			{
				tx_fifo_write('m');
				LED_STOP_OFF;
				full_hsi_message(s88_data1);
			}
			else
			{
				LED_STOP_ON;
			}
			break;
		}
		
		case 'v':                                               // Command: Version
		{
			if (rx_fifo_read() == 0x0d)
			{
				copyright_message();
				LED_STOP_OFF;
			}
			else
			{
				LED_STOP_ON;
			}
			break;
		}
	}
}

void hsi88_parser(void)
{
	while (1)
	{
		unsigned char c = rx_fifo_read();
		tx_fifo_write(c);
	}

	if (rx_fifo_ready())
	{
		hsi88_parse_rs232();
	}
	else
	{
		LED_GO_ON;
		read_s88(s88_data1);
		LED_GO_OFF;
	}
}

*/
