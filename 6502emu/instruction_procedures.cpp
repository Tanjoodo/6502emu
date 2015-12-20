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
uint8_t& _fetch_operand(AddressingMode addressingMode, uint8_t operands[])
{
	switch (addressingMode)
	{
	case Immediate:
		return operands[0];
	default:
		return mem[_calculate_address(addressingMode, operands)];
	}
}

void ProcADC(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t& operand = _fetch_operand(addressingMode, operands);
	if (!GetFlagD())
	{
		bool bit7 = (reg::Accumulator & (1 << 7)) != 0;
		uint8_t oldAcc = reg::Accumulator;
		if (reg::Accumulator + operand > 255)
			SetFlagC(true);
		else
			SetFlagC(false);

		reg::Accumulator += operand;

		SetFlagN((reg::Accumulator & (1 << 7)) != 0); // Put bit 7 in S flag
		SetFlagZ(!reg::Accumulator);
		//TODO: Implement overflow flag setting
	}
}

void ProcAND(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t& operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = reg::Accumulator & operand;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN((reg::Accumulator & (1 << 7)) != 0);
}

void ProcASL(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t& operand = (addressingMode == Accumulator) ? reg::Accumulator : _fetch_operand(addressingMode, operands);

	SetFlagC((operand & (1 << 7)) != 0);
	SetFlagN((operand & (1 << 6)) != 0);

	operand <<= 1;

	SetFlagZ(operand == 0);
}

void ProcBCC(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BCC ";
	PrintOperands(addressingMode, operands);
}

void ProcBCS(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BCS ";
	PrintOperands(addressingMode, operands);
}

void ProcBEQ(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BEQ ";
	PrintOperands(addressingMode, operands);
}

void ProcBIT(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BIT ";
	PrintOperands(addressingMode, operands);
}

void ProcBMI(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BMI ";
	PrintOperands(addressingMode, operands);
}

void ProcBNE(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BNE ";
	PrintOperands(addressingMode, operands);
}

void ProcBPL(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BPL ";
	PrintOperands(addressingMode, operands);
}

void ProcBRK(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BRK ";
	PrintOperands(addressingMode, operands);
}

void ProcBVC(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BVC ";
	PrintOperands(addressingMode, operands);
}

void ProcBVS(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "BVS ";
	PrintOperands(addressingMode, operands);
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
	uint8_t &operand = _fetch_operand(addressingMode, operands);
	SetFlagZ(operand == reg::Accumulator);
	SetFlagN(reg::Accumulator < operand);
	SetFlagC(reg::Accumulator >= operand);
}

void ProcCPX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "CPX ";
	PrintOperands(addressingMode, operands);
}

void ProcCPY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "CPY ";
	PrintOperands(addressingMode, operands);
}

void ProcDEC(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "DEC ";
	PrintOperands(addressingMode, operands);
}

void ProcDEX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "DEX ";
	PrintOperands(addressingMode, operands);
}

void ProcDEY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "DEY ";
	PrintOperands(addressingMode, operands);
}

void ProcEOR(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "EOR ";
	PrintOperands(addressingMode, operands);
}

void ProcINC(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "INC ";
	PrintOperands(addressingMode, operands);
}

void ProcINX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "INX ";
	PrintOperands(addressingMode, operands);
}

void ProcINY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "INY ";
	PrintOperands(addressingMode, operands);
}

void ProcJMP(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "JMP ";
	PrintOperands(addressingMode, operands);
}

void ProcJSR(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "JSR ";
	PrintOperands(addressingMode, operands);
}

void ProcLDA(AddressingMode addressingMode, uint8_t operands[])
{
	uint8_t& operand = _fetch_operand(addressingMode, operands);
	reg::Accumulator = operand;
	SetFlagZ(reg::Accumulator == 0);
	SetFlagN(reg::Accumulator & (1 << 7) != 0);
}

void ProcLDX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "LDX ";
	PrintOperands(addressingMode, operands);
}

void ProcLDY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "LDY ";
	PrintOperands(addressingMode, operands);
}

void ProcLSR(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "LSR ";
	PrintOperands(addressingMode, operands);
}

void ProcNOP(AddressingMode addressingMode, uint8_t operands[])
{
}

void ProcORA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "ORA ";
	PrintOperands(addressingMode, operands);
}

void ProcPHA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "PHA ";
	PrintOperands(addressingMode, operands);
}

void ProcPHP(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "PHP ";
	PrintOperands(addressingMode, operands);
}

void ProcPLA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "PLA ";
	PrintOperands(addressingMode, operands);
}

void ProcPLP(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "PLP ";
	PrintOperands(addressingMode, operands);
}

void ProcROL(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "ROL ";
	PrintOperands(addressingMode, operands);
}

void ProcROR(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "ROR ";
	PrintOperands(addressingMode, operands);
}

void ProcRTI(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "RTI ";
	PrintOperands(addressingMode, operands);
}

void ProcRTS(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "RTS ";
	PrintOperands(addressingMode, operands);
}

void ProcSBC(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "SBC ";
	PrintOperands(addressingMode, operands);
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
	cout << "SEI ";
	PrintOperands(addressingMode, operands);
}

void ProcSTA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "STA ";
	PrintOperands(addressingMode, operands);
}

void ProcSTX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "STX ";
	PrintOperands(addressingMode, operands);
}

void ProcSTY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "STY ";
	PrintOperands(addressingMode, operands);
}

void ProcTAX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TAX ";
	PrintOperands(addressingMode, operands);
}

void ProcTAY(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TAY ";
	PrintOperands(addressingMode, operands);
}

void ProcTSX(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TSX ";
	PrintOperands(addressingMode, operands);
}

void ProcTXA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TXA ";
	PrintOperands(addressingMode, operands);
}

void ProcTXS(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TXS ";
	PrintOperands(addressingMode, operands);
}

void ProcTYA(AddressingMode addressingMode, uint8_t operands[])
{
	cout << "TYA ";
	PrintOperands(addressingMode, operands);
}
