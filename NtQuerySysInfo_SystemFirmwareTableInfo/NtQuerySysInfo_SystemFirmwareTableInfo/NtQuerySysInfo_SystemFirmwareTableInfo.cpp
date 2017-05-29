// NtQuerySysInfo_SystemFirmwareTableInfoGet.cpp : Defines the entry point for the console application.
//

// NtQuerySysInfo_SystemFirmwareTableInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

//from https://github.com/hfiref0x/VMDE
typedef enum _SYSTEM_FIRMWARE_TABLE_ACTION
{
	SystemFirmwareTable_Enumerate,
	SystemFirmwareTable_Get
} SYSTEM_FIRMWARE_TABLE_ACTION, *PSYSTEM_FIRMWARE_TABLE_ACTION;

typedef struct _SYSTEM_FIRMWARE_TABLE_INFORMATION {
	ULONG ProviderSignature;
	SYSTEM_FIRMWARE_TABLE_ACTION Action;
	ULONG TableID;
	ULONG TableBufferLength;
	UCHAR TableBuffer[ANYSIZE_ARRAY];
} SYSTEM_FIRMWARE_TABLE_INFORMATION, *PSYSTEM_FIRMWARE_TABLE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemFirmwareTableInformation = 76
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS(WINAPI *PNtQuerySystemInformation)(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);

//from http://grapsus.net/blog/post/Hexadecimal-dump-in-C
#define HEXDUMP_COLS 16

void hexdump(unsigned char *mem, unsigned int len)
{
	unsigned int i, j;

	for (i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if (i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}

		/* print hex data */
		if (i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}

		/* print ASCII dump */
		if (i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for (j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if (j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if (isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}

int main()
{
	HMODULE ntdll = GetModuleHandle(TEXT("ntdll"));
	PNtQuerySystemInformation query = (PNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (query == NULL) {
		printf("GetProcAddress() failed.\n");
		return 1;
	}

	ULONG Length = 0x1000;
	SYSTEM_FIRMWARE_TABLE_INFORMATION *sfti = (PSYSTEM_FIRMWARE_TABLE_INFORMATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
	if (sfti == NULL) {
		return 1;
	}
	sfti->Action = SystemFirmwareTable_Get;
	sfti->ProviderSignature = 'RSMB';
	sfti->TableID = 0;
	sfti->TableBufferLength = Length;

	// Query if info class available and if how many memory we need.
	NTSTATUS Status = query(SystemFirmwareTableInformation, sfti, Length, &Length);
	if (Status != 0xC0000023 && Status != 0) { //0xC0000023 == Buffer too small
		HeapFree(GetProcessHeap(), 0, sfti);
		printf("Failed to query SystemFirmwareTableInformation: 0x%X\r\n", Status);
		return 1;
	}

	HeapFree(GetProcessHeap(), 0, sfti);

	sfti = (PSYSTEM_FIRMWARE_TABLE_INFORMATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Length);
	if (sfti == NULL) {
		printf("Failed to allocate memory for sfti 0x%X\r\n", Status);
		return 1;
	}
	sfti->Action = SystemFirmwareTable_Get;
	sfti->ProviderSignature = 'RSMB';
	sfti->TableID = 0;
	sfti->TableBufferLength = Length;
	Status = query(SystemFirmwareTableInformation, sfti, Length, &Length);
	if (Status != 0) {
		HeapFree(GetProcessHeap(), 0, sfti);
		printf("Failed to query SystemFirmwareTableInformation: 0x%X\r\n", Status);
		return 1;
	}

	PUCHAR buf = sfti->TableBuffer;
	hexdump(buf, Length);

	return 0;
}


