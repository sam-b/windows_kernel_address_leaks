// DescriptorTables.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>


int main()
{
	unsigned char idtr[6] = { 0 };
	__asm {
		sidt idtr;
	}
	unsigned int idtrBase = (unsigned int)idtr[0] << 24 |
		(unsigned int)idtr[1] << 16 |
		(unsigned int)idtr[2] << 8 |
		(unsigned int)idtr[3];
	unsigned short idtrLimit = (unsigned int) idtr[4] << 8 |
		(unsigned int)idtr[5];
	printf("Interrupt Descriptor Table Register base: 0x%X, limit: 0x%X\r\n", idtrBase, idtrLimit);
	unsigned char gdtr[6] = { 0 };
	__asm {
		sgdt gdtr;
	}
	unsigned int gdtrBase = (unsigned int)gdtr[0] << 24 |
		(unsigned int)gdtr[1] << 16 |
		(unsigned int)gdtr[2] << 8 |
		(unsigned int)gdtr[3];
	unsigned short gdtrLimit = (unsigned int)gdtr[4] << 8 |
		(unsigned int)gdtr[5];
	printf("Global Descriptor Table Register base: 0x%X, limit: 0x%X\r\n", gdtrBase, gdtrLimit);

	LDT_ENTRY tss;
	WORD tr;

	__asm str tr

	GetThreadSelectorEntry(GetCurrentThread(), tr, &tss);

	unsigned int  tssBase = (tss.HighWord.Bits.BaseHi << 24) +
		(tss.HighWord.Bits.BaseMid << 16) +
		tss.BaseLow;
	unsigned int tssLimit = tss.LimitLow;
	printf("TSS base: 0x%X, limit: 0x%X\r\n", tssBase, tssLimit);

	return 0;
}

