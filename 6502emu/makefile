CC = g++
CFLAGS = -c -Wall -std=c++11

all: 6502emu

6502emu: main.o instruction_procedures.o register_procedures.o decoder.o
	$(CC) main.o instruction_procedures.o register_procedures.o decoder.o -o 6502emu

register_procedures.o: register_procedures.cpp
	$(CC) $(CFLAGS) register_procedures.cpp -o register_procedures.o

instruction_procedures.o: instruction_procedures.cpp
	$(CC) $(CFLAGS) instruction_procedures.cpp -o instruction_procedures.o

decoder.o: decoder.cpp
	$(CC) $(CFLAGS) decoder.cpp -o decoder.o

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o main.o

clean:
	rm *o 6502emu
