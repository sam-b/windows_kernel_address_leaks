// DescriptorTables.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <Windows.h>
#include <intrin.h>

int main()
{

#ifdef _WIN64
	unsigned char idtr[10] = { 0 };
	__sidt(idtr);
	unsigned long long idtrBase = (unsigned long long)idtr[9] << 56 |
		(unsigned long long)idtr[8] << 48 |
		(unsigned long long)idtr[7] << 40 |
		(unsigned long long)idtr[6] << 32 |
		(unsigned long long)idtr[5] << 24 |
		(unsigned long long)idtr[4] << 16 |
		(unsigned long long)idtr[3] << 8 |
		(unsigned long long)idtr[2];
	unsigned short idtrLimit = (unsigned int)idtr[1] << 8 |
		(unsigned int)idtr[0];
	printf("Interrupt Descriptor Table Register base: 0x%llx, limit: 0x%X\r\n", idtrBase, idtrLimit);
#else
	unsigned char idtr[6] = { 0 };
	__asm {
		sidt idtr;
	}
	unsigned int idtrBase = (unsigned int)idtr[5] << 24 |
		(unsigned int)idtr[4] << 16 |
		(unsigned int)idtr[3] << 8 |
		(unsigned int)idtr[2];
	unsigned short idtrLimit = (unsigned int)idtr[1] << 8 |
		(unsigned int)idtr[0];
	printf("Interrupt Descriptor Table Register base: 0x%X, limit: 0x%X\r\n", idtrBase, idtrLimit);
#endif



#ifdef _WIN64
	unsigned char gdtr[10] = { 0 };
	_sgdt(gdtr);
	unsigned long long gdtrBase = (unsigned long long)gdtr[9] << 56 |
		(unsigned long long)gdtr[8] << 48 |
		(unsigned long long)gdtr[7] << 40 |
		(unsigned long long)gdtr[6] << 32 |
		(unsigned long long)gdtr[5] << 24 |
		(unsigned long long)gdtr[4] << 16 |
		(unsigned long long)gdtr[3] << 8 |
		(unsigned long long)gdtr[2];
	unsigned short gdtrLimit = (unsigned int)gdtr[1] << 8 |
		(unsigned int)gdtr[0];
	printf("Global Descriptor Table Register base: 0x%llx, limit: 0x%X\r\n", gdtrBase, gdtrLimit);
#else
	unsigned char gdtr[6] = { 0 };
	__asm {
		sgdt gdtr;
	}
	unsigned int gdtrBase = (unsigned int)gdtr[5] << 24 |
		(unsigned int)gdtr[4] << 16 |
		(unsigned int)gdtr[3] << 8 |
		(unsigned int)gdtr[2];
	unsigned short gdtrLimit = (unsigned int)gdtr[1] << 8 |
		(unsigned int)gdtr[0];
	printf("Global Descriptor Table Register base: 0x%X, limit: 0x%X\r\n", gdtrBase, gdtrLimit);
#endif


#ifndef _WIN64
	WORD tr;
	__asm str tr
	LDT_ENTRY tss;
	GetThreadSelectorEntry(GetCurrentThread(), tr, &tss);

	unsigned int  tssBase = (tss.HighWord.Bits.BaseHi << 24) +
		(tss.HighWord.Bits.BaseMid << 16) +
		tss.BaseLow;
	unsigned int tssLimit = tss.LimitLow;
	printf("TSS base: 0x%X, limit: 0x%X\r\n", tssBase, tssLimit);

	return 0;
#endif
}

