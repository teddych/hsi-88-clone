
MMCU=atmega644p
#CPPFLAGS=-Os -gdwarf-2 -MD -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -Wextra -Werror -c
CPPFLAGS=-Os -Wall -Wextra -fno-exceptions -std=c++11
LDFLAGS=

OBJECTS= \
	CommandParser.o \
	HSI-88-Clone.o \
	S88.o \
	UART.o

%.o: %.cpp *.h
	avr-g++ -mmcu=$(MMCU) $(CPPFLAGS) -c -o $@ $<

all: $(OBJECTS)
	avr-g++ -mmcu=$(MMCU) $(LDFLAGS) -o HSI-88-Clone.elf $(OBJECTS)

program: HSI-88-Clone.elf
	sudo avrdude -V -p m644p -c atmelice_isp -U flash:w:HSI-88-Clone.elf:e

clean:
	rm *.o
	rm *.elf
