#pragma once
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "instruction_procedures.h"
#include "decoder.h"
#include "memory.h"
#include "global_registers.h"
#include "register_procedures.h"

using namespace std;

void Format(AddressingMode addressingMode)
{
	switch (addressingMode)
	{
	case Immediate:
		cout << "#$" << setw(2) << setfill('0') << uppercase << hex;
		break;
	case ZeroPage:
	case ZeroPageX:
	case ZeroPageY:
	case Relative:
		cout << "$" << setw(2) << setfill('0') << uppercase << hex;
		break;
	case Absolute:
	case AbsoluteX:
	case AbsoluteY:
		cout << "$" << setw(4) << setfill('0') << uppercase << hex;
		break;
	case ZeroPageIndirectX:
	case ZeroPageIndirectY:
		cout << '(' << '$' << setw(4) << setfill('0') << uppercase << hex;
		break;
	}

	//Anything else does not need special formatting
}

void PrintOperands(AddressingMode addressingMode, uint8_t operands[])
{
	Format(addressingMode);

	uint16_t operand;
	switch (addressingMode)
	{
	case Immediate:
		cout << (int)operands[0];
		break;
	case ZeroPage:
		cout << (int)operands[0];
		break;
	case ZeroPageX:
		cout << (int)operands[0] << ",X";
		break;
	case ZeroPageY:
		cout << (int)operands[0] << ",Y";
	case Relative:
		cout << (int)operands[0];
		break;
	case Absolute:
		operand = (uint16_t)operands[1] << 8 | operands[0];
		cout << operand;
		break;
	case AbsoluteX:
		operand = (uint16_t)operands[1] << 8 | operands[0];
		cout << operand << ",X";
		break;
	case AbsoluteY:
		operand = (uint16_t)operands[1] << 8 | operands[0];
		cout << operand << ",Y";
		break;
	case ZeroPageIndirectX:
		cout << (int)operands[0] << ",X)";
		break;
	case ZeroPageIndirectY:
		cout << (int)operands[0] << "),Y";
		break;
	}

	cout << endl;

}

uint16_t _8_to_16(uint8_t high, uint8_t low)
{
	return (uint16_t)high << 8 | low;
}

uint16_t _calculate_address(AddressingMode addressingMode, uint8_t operands[])
{
	switch (addressingMode)
	{
	case Absolute:
		return _8_to_16(operands[1], operands[0]);
	case ZeroPage:
		return operands[0];
	case Relative:
		return _8_to_16(reg::PCH, reg::PCL) + (int8_t)operands[0];
	case AbsoluteX:
		return reg::X + (int16_t)_8_to_16(operands[1], operands[0]);
	case AbsoluteY:
		return reg::Y + (int16_t)_8_to_16(operands[1], operands[0]);
	case ZeroPageX:
		return (uint16_t)((reg::X + operands[0]) % 0xFF);
	case ZeroPageY:
		return (uint16_t)((reg::Y + operands[0]) % 0xFF);
	case ZeroPageIndirectX:
		return _8_to_16(mem[reg::X + operands[0] + 1], mem[reg::X + operands[0]]);
	case ZeroPageIndirectY:
		return _8_to_16(mem[reg::Y + operands[0] + 1], mem[reg::Y + operands[0]]);
	default:
		return 0;
	}
}

// Should not be used with addressing modes Accumulator and Implied.
uint8_t* _fetch_operand(AddressingMode addressingMode, uint8_t operands[])
{
	switch (addressingMode)
	{
	case Immediate:
		return &operands[0];
	default:
		return &mem[_calculate_address(addressingMode, operands)];
	}
}

void _push_onto_stack(uint8_t operand)
{
	mem[reg::SP + 0x100] = operand;
	reg::SP--;
}

uint8_t _pull_from_stack()
{
	return mem[++reg::SP];
}

void ProcADC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagD())
	{
		bool bit7 = (reg::Accumulator & (1 << 7)) != 0;
		uint8_t oldAcc = reg::Accumulator;
		if (reg::Accumulator + *operand > 255)
			SetFlagC(true);
		else
			SetFlagC(false);

		reg::Accumulator += *operand;

		SetFlagN((reg::Accumulator & (1 << 7)) != 0); // Put bit 7 in S flag
		SetFlagZ(!reg::Accumulator);
		//TODO: Implement overflow flag setting
	}
}

void ProcAND(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = reg::Accumulator & *operand;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcASL(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = (addressingMode == Accumulator) ? &reg::Accumulator : _fetch_operand(addressingMode, operands);

	SetFlagC((*operand & (1 << 7)) != 0);
	SetFlagN((*operand & (1 << 6)) != 0);

	*operand <<= 1;

	SetFlagZ(*operand == 0);
}

void ProcBCC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagC())
		IncrementPC(2 + *operand);
}

void ProcBCS(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (GetFlagC())
		IncrementPC(2 + *operand);
}

void ProcBEQ(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (GetFlagZ())
		IncrementPC(2 + *operand);
}

void ProcBIT(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	uint8_t res = *operand & reg::Accumulator;

	SetFlagN((res & (1 << 7)) != 0);
	SetFlagV((res & (1 << 6)) != 0);
	SetFlagZ(res == 0);
}

void ProcBMI(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (GetFlagN())
		IncrementPC(2 + *operand);
}

void ProcBNE(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagZ())
		IncrementPC(2 + *operand);
}

void ProcBPL(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagN())
		IncrementPC(2 + *operand);
}

void ProcBRK(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BRK ";
	PrintOperands(addressingMode, operands);
}

void ProcBVC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagV())
		IncrementPC(2 + *operand);
}

void ProcBVS(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	if (GetFlagV())
		IncrementPC(2 + *operand);
}

void ProcCLC(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagC(false);
}

void ProcCLD(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagD(false);
}

void ProcCLI(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagI(false);
}

void ProcCLV(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagV(false);
}

void ProcCMP(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	SetFlagZ(*operand == reg::Accumulator);
	SetFlagN(reg::Accumulator < *operand);
	SetFlagC(reg::Accumulator >= *operand);
}

void ProcCPX(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	SetFlagZ(*operand == reg::X);
	SetFlagC(*operand < reg::X);
	SetFlagN(*operand > reg::X);
}

void ProcCPY(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	uint8_t sub_result = reg::Y + ~(*operand) + 1;
	SetFlagC(reg::Y >= *operand);
	SetFlagN((sub_result & (1 << 7)) != 0);
	SetFlagZ(reg::Y == *operand);
}

void ProcDEC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	*operand--;
	SetFlagN((*operand & (1 << 7)) != 0);
	SetFlagZ(*operand == 0);
}

void ProcDEX(AddressingMode addressingMode, uint8_t operands[])
{
	reg::X--;
	SetFlagN((reg::X & (1 << 7)) != 0);
	SetFlagZ(reg::X == 0);
}

void ProcDEY(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Y--;
	SetFlagN((reg::Y & (1 << 7)) != 0);
	SetFlagZ(reg::Y == 0);
}

void ProcEOR(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = reg::Accumulator ^ *operand;

	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcINC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	*operand++;
	SetFlagZ(*operand == 0);
	SetFlagN((*operand & (1 << 7)) != 0);
}

void ProcINX(AddressingMode addressingMode, uint8_t operands[])
{
	reg::X++;
	SetFlagZ(reg::X == 0);
	SetFlagN((reg::X & (1 << 7)) != 0);;
}

void ProcINY(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Y++;
	SetFlagZ(reg::Y == 0);
	SetFlagN((reg::Y & (1 << 7)) != 0);;
}

void ProcJMP(AddressingMode addressingMode, uint8_t operands[])
{
	uint16_t address = _calculate_address(addressingMode, operands);
	SetPC(address);
}

void ProcJSR(AddressingMode addressingMode, uint8_t operands[])
{
	IncrementPC(2);
	_push_onto_stack(reg::PCH);
	_push_onto_stack(reg::PCL);
	SetPC(_calculate_address(addressingMode, operands));
}

void ProcLDA(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = *operand;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcLDX(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::X = *operand;
	SetFlagZ(reg::X == 0);
	SetFlagN((reg::X & (1 << 7)) != 0);
}

void ProcLDY(AddressingMode addressingMode, uint8_t operands[])
{
    uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::Y = *operand;
	SetFlagN((reg::Y & (1 << 7)) != 0);
	SetFlagZ(reg::Y == 0);
}

void ProcLSR(AddressingMode addressingMode, uint8_t operands[])
{
	if (addressingMode == Accumulator)
	{
		SetFlagC((Accumulator & 1) != 0);
		reg::Accumulator >>= 1;
		SetFlagZ(reg::Accumulator == 0);
	}
	else
	{
		uint8_t* operand = _fetch_operand(addressingMode, operands);
		SetFlagC((*operand & 1) != 0);
		*operand >>= 1;
		SetFlagZ(*operand == 0);
	}

	SetFlagN(false);
}

void ProcNOP(AddressingMode addressingMode, uint8_t operands[])
{
}

void ProcORA(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = reg::Accumulator | *operand;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);

}

void ProcPHA(AddressingMode addressingMode, uint8_t operands[])
{
	_push_onto_stack(reg::Accumulator);
}

void ProcPHP(AddressingMode addressingMode, uint8_t operands[])
{
	_push_onto_stack(reg::Status);
}

void ProcPLA(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Accumulator = _pull_from_stack();

	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcPLP(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Status = _pull_from_stack();
}

void ProcROL(AddressingMode addressingMode, uint8_t operands[])
{
	if (addressingMode == Accumulator)
	{
		reg::Accumulator <<= 1;
		reg::Accumulator |= (GetFlagC()) ? 1 : 0;
		SetFlagZ(reg::Accumulator == 0);
		SetFlagN((reg::Accumulator & (1 << 7)) != 0);
	}
	else
	{
		uint8_t* operand = _fetch_operand(addressingMode, operands);
		*operand <<= 1;
		*operand |= (GetFlagC()) ? 1 : 0;
		SetFlagZ(*operand == 0);
		SetFlagN((*operand & (1 << 7)) != 0);
	}
}

void ProcROR(AddressingMode addressingMode, uint8_t operands[])
{
	if (addressingMode == Accumulator)
	{
		reg::Accumulator >>= 1;
		reg::Accumulator |= (GetFlagC()) ? (1 << 7) : 0;
		SetFlagZ(reg::Accumulator == 0);
	}
	else
	{
		uint8_t* operand = _fetch_operand(addressingMode, operands);
		*operand >>= 1;
		*operand |= (GetFlagC()) ? (1 << 7) : 0;
		SetFlagZ(*operand == 0);
	}

	SetFlagN(GetFlagC());
}

void ProcRTI(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "RTI ";
	PrintOperands(addressingMode, operands);
}

void ProcRTS(AddressingMode addressingMode, uint8_t operands[])
{
	reg::PCL = _pull_from_stack();
	reg::PCH = _pull_from_stack();
	IncrementPC(1);
}

void ProcSBC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	uint8_t twos_comp = ~(*operand) + 1;
	SetFlagV((reg::Accumulator <= 127 && reg::Accumulator + twos_comp > 127) ||
			 (reg::Accumulator >= 127 && reg::Accumulator + twos_comp < 127));

	reg::Accumulator = twos_comp + reg::Accumulator;

	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
	SetFlagC(!(reg::Accumulator & (1 << 7)) != 0);

}

void ProcSEC(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagC(true);
}

void ProcSED(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagD(true);
}

void ProcSEI(AddressingMode addressingMode, uint8_t operands[])
{
	SetFlagI(true);
}

void ProcSTA(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	*operand = reg::Accumulator;
}

void ProcSTX(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	*operand = reg::X;
}

void ProcSTY(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t* operand = _fetch_operand(addressingMode, operands);
	*operand = reg::Y;
}

void ProcTAX(AddressingMode addressingMode, uint8_t operands[])
{
	reg::X = reg::Accumulator;
	SetFlagZ(reg::X == 0);
	SetFlagN((reg::X & (1 << 7)) != 0);
}

void ProcTAY(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Y = reg::Accumulator;
	SetFlagZ(reg::Y == 0);
	SetFlagN((reg::Y & (1 << 7)) != 0);
}

void ProcTSX(AddressingMode addressingMode, uint8_t operands[])
{
	reg::X = reg::SP;

	SetFlagZ(reg::X == 0);
	SetFlagN((reg::X & (1 << 7)) != 0);
}

void ProcTXA(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Accumulator = reg::X;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcTXS(AddressingMode addressingMode, uint8_t operands[])
{
	reg::SP = reg::X;

	SetFlagZ(reg::SP == 0);
	SetFlagN((reg::SP & (1 << 7)) != 0);
}

void ProcTYA(AddressingMode addressingMode, uint8_t operands[])
{
	reg::Accumulator = reg::Y;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);

}
