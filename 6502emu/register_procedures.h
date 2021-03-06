#pragma once
//#include "global_registers.h"
#include <cstdint>

void SetFlagC(bool);
void SetFlagZ(bool);
void SetFlagI(bool);
void SetFlagD(bool);
void SetFlagB(bool);
void SetFlagV(bool);
void SetFlagN(bool);

bool GetFlagC();
bool GetFlagZ();
bool GetFlagI();
bool GetFlagD();
bool GetFlagB();
bool GetFlagV();
bool GetFlagN();

void IncrementPC(uint8_t);
void DecrementPC(uint8_t);
void SetPC(uint16_t);
uint16_t GetPC();