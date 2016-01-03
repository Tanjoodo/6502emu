#include "decoder.h"
#include "global_registers.h"
#include "register_procedures.h"
#include "memory.h"

#include <iostream>
#include <fstream>
#include <cstdint>

namespace reg
{
	bool pc_changed_externally = false;
	uint8_t Accumulator,
		X,
		Y,
		Status,
		PCH,
		PCL,
		SP = 0xFF;
}

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
	int t;
	cout << "Enter number of locations you wish to change: ";
	cin >> t;
	while (t--)
	{
		uint16_t location;
		int val;
		cout << "Enter address to change at followed by the value: ";
		cin >> hex >> location;
		cin >> hex >> val;
		mem[location] = val;
	}
	SetPC(0x400);
	reg::pc_changed_externally = false;
	while (true)
	{
		Instruction instruction = DecodeInstruction(mem[GetPC()]);
		if (instruction == BRK)
			break;
		int instruction_length = Decode(GetPC(), mem);
		if (!reg::pc_changed_externally)
			incPC(instruction_length);
		else
			reg::pc_changed_externally = false;
	}
	do
	{
		uint16_t location;
		cout << "Query memory location: ";
		cin >> hex >> location;
		cin.ignore();
		cout << "0x" << hex << (int)mem[location] << endl;
	}while (true);
	return 0;
}