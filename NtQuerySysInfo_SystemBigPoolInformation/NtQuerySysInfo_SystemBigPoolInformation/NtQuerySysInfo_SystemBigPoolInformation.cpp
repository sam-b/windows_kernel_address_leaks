// NtQuerySysInfo_SystemBigPoolInformation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

//from http://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/bigpool_entry.htm
typedef struct _SYSTEM_BIGPOOL_ENTRY 
{
	union {
		PVOID VirtualAddress;
		ULONG_PTR NonPaged : 1;
	};
	ULONG_PTR SizeInBytes;
	union {
		UCHAR Tag[4];
		ULONG TagUlong;
	};
} SYSTEM_BIGPOOL_ENTRY, *PSYSTEM_BIGPOOL_ENTRY;

//from http://www.geoffchappell.com/studies/windows/km/ntoskrnl/api/ex/sysinfo/bigpool.htm
typedef struct _SYSTEM_BIGPOOL_INFORMATION {
	ULONG Count;
	SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ANYSIZE_ARRAY];
} SYSTEM_BIGPOOL_INFORMATION, *PSYSTEM_BIGPOOL_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBigPoolInformation = 0x42
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS(WINAPI *PNtQuerySystemInformation)(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__inout PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);

int main()
{
	HMODULE ntdll = GetModuleHandle(TEXT("ntdll"));
	PNtQuerySystemInformation query = (PNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
	if (query == NULL) {
		printf("GetProcAddress() failed.\n");
		return 1;
	}
	unsigned int len = sizeof(SYSTEM_BIGPOOL_INFORMATION);
	unsigned long out;
	PSYSTEM_BIGPOOL_INFORMATION info = NULL;
	NTSTATUS status = ERROR;
	

	do {
		len *= 2;
		info = (PSYSTEM_BIGPOOL_INFORMATION) GlobalAlloc(GMEM_ZEROINIT, len);
		status = query(SystemBigPoolInformation, info, len, &out);
	} while (status == (NTSTATUS)0xc0000004);

	if (!SUCCEEDED(status)) {
		printf("NtQuerySystemInformation failed with error code 0x%X\n", status);
		return 1;
	}

	for (unsigned int i = 0; i < info->Count; i++) {
		SYSTEM_BIGPOOL_ENTRY poolEntry = info->AllocatedInfo[i];
#ifdef _WIN64
		printf("Tag: %.*s, Address: 0x%llx, Size: 0x%x\n", 4, poolEntry.Tag, poolEntry.VirtualAddress, poolEntry.SizeInBytes);
#else
		printf("Tag: %.*s, Address: 0x%x, Size: 0x%x\n",4, poolEntry.Tag, poolEntry.VirtualAddress, poolEntry.SizeInBytes);
#endif
	}
    return 0;
}

