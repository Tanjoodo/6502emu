#include "register_procedures.h"
#include "global_registers.h"
using namespace reg;

void _setBit(bool value, int bit_num)
{
	if (value)
		Status = Status | (1 << bit_num);
	else
		Status = Status & ~(1 << bit_num);
}

bool _getBit(int bit_num)
{
	uint8_t temp_s = Status;
	temp_s = temp_s >> bit_num;
	return temp_s & 1;
}

void SetFlagC(bool state)
{
	_setBit(state, 0);
}

void SetFlagZ(bool state)
{
	_setBit(state, 1);
}

void SetFlagI(bool state)
{
	_setBit(state, 2);
}

void SetFlagD(bool state)
{
	_setBit(state, 3);
}

void SetFlagB(bool state)
{
	_setBit(state, 4);
}

void SetFlagV(bool state)
{
	_setBit(state, 6);
}

void SetFlagS(bool state)
{
	_setBit(state, 7);
}

bool GetFlagC()
{
	return _getBit(0);
}

bool GetFlagZ()
{
	return _getBit(1);
}

bool GetFlagI()
{
	return _getBit(2);
}

bool GetFlagD()
{
	return _getBit(3);
}

bool GetFlagB()
{
	return _getBit(4);
}

bool GetFlagV()
{
	return _getBit(6);
}

bool GetFlagS()
{
	return _getBit(7);
}