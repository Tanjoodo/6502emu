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

uint8_t _get_low(uint16_t address)
{
	return  address & 0x00FF;
}

uint8_t _get_high(uint16_t address)
{
	 return (address >> 8) & 0x00FF;
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

void SetFlagN(bool state)
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

bool GetFlagN()
{
	return _getBit(7);
}

void IncrementPC(uint8_t val)
{
	uint16_t pc = (uint16_t)reg::PCH << 8 | reg::PCL;
	pc += val;
	reg::PCH = (pc >> 8) & 0x00FF;
	reg::PCL = pc & 0x00FF;
}

void DecrementPC(uint8_t val)
{
	uint16_t pc = (uint16_t)reg::PCH << 8 | reg::PCL;
	pc -= val;
	reg::PCH = (pc >> 8) & 0x00FF;
	reg::PCL = pc & 0x00FF;
}

void SetPC(uint16_t val)
{
	reg::PCH = _get_high(val);
	reg::PCL = _get_low(val);
}