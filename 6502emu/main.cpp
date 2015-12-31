#include "decoder.h"
#include "global_registers.h"
#include "register_procedures.h"
#include "memory.h"

#include <iostream>
#include <fstream>
#include <cstdint>


uint8_t mem[0x10000] = {};
using namespace std;
void incPC(uint8_t number)
{
	IncrementPC(number);
	reg::pc_changed_externally = false;
}

void decPC(uint8_t number)
{
	DecrementPC(number);
	reg::pc_changed_externally = false;
}
int main(int argc, char **argv)
{
	char *bytes;
	uint8_t *bytes8;
	int index = 0;
	size_t size = 0;
	if (argc < 2)
	{
		cout << "Please enter a file name" << endl;
		return 1;
	}
	else
	{
		ifstream inFile;
		inFile.open(argv[1], ios::in | ios::binary | ios::ate);
		if (inFile.is_open())
		{
			inFile.seekg(0, ios::end);
			size = inFile.tellg();
			inFile.seekg(0, ios::beg);

			bytes = new char[size];
			inFile.read(bytes, size);

			bytes8 = new uint8_t[size];
			std::copy(bytes, bytes + size, mem);
			delete[] bytes;
			delete[] bytes8;
		}
		else
		{
			cout << "File input failed" << endl;
		}
	}

	SetPC(0x400);
	reg::pc_changed_externally = false;
	while (true)
	{
		Instruction instruction = DecodeInstruction(mem[GetPC()]);
		int instruction_length = Decode(GetPC(), mem);
		if (instruction == BRK)
			break;
		if (!reg::pc_changed_externally)
			incPC(instruction_length);
		else
			reg::pc_changed_externally = false;
	}
	cin.ignore();

	do
	{
		uint16_t location;
		cin >> location;
		cout << hex << (int)mem[location];
	}while (true);
	return 0;
}