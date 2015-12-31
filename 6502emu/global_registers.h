#pragma once
namespace reg
{
	static bool pc_changed_externally = false;
	static uint8_t Accumulator,
		X,
		Y,
		Status,
		PCH,
		PCL,
		SP = 0xFF;
}