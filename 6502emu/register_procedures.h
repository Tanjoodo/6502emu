#pragma once
//#include "global_registers.h"
#include <cstdint>
namespace reg
{
	static uint8_t Accumulator,
		X,
		Y,
		Status,
		PC,
		SP;
}
void SetFlagC(bool);
void SetFlagZ(bool);
void SetFlagI(bool);
void SetFlagD(bool);
void SetFlagB(bool);
void SetFlagV(bool);
void SetFlagS(bool);

bool GetFlagC();
bool GetFlagZ();
bool GetFlagI();
bool GetFlagD();
bool GetFlagB();
bool GetFlagV();
bool GetFlagS();