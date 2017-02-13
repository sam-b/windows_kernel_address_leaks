// NtQuerySysInfo_SystemModuleInformation.cpp : Attempts to use the NtQuerySystemInformation to find the base addresses if loaded modules.
//

#include "stdafx.h"
#include <windows.h>

typedef struct _SYSTEM_LOCK {
	PVOID	Address;
	USHORT  Type;
	USHORT  Reserved1;
	ULONG	ExclusiveOwnerThreadId;
	ULONG	ActiveCount;
	ULONG	ContentionCount;
	ULONG	Reserved2[2];
#ifdef _WIN64
	ULONG	Reserved3;
#endif
	ULONG	NumberOfSharedWaiters;
	ULONG	NumberOfExclusiveWaiters;
} SYSTEM_LOCK, *PSYSTEM_LOCK;

typedef struct SYSTEM_LOCK_INFORMATION {
	ULONG              LocksCount;
	SYSTEM_LOCK        Locks[1];
} SYSTEM_LOCK_INFORMATION, *PSYSTEM_LOCK_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemLockInformation = 12
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

	PSYSTEM_LOCK_INFORMATION pLockInfo = NULL;
	ULONG len = 20;
	NTSTATUS status = (NTSTATUS)0xc0000004;

	do {
		len *= 2;
		pLockInfo = (PSYSTEM_LOCK_INFORMATION)GlobalAlloc(GMEM_ZEROINIT, len);
		status = query(SystemLockInformation, pLockInfo, len, &len);
	} while (status == (NTSTATUS)0xc0000004);
	if (status != (NTSTATUS)0x0) {
		printf("NtQuerySystemInformation failed with error code 0x%X\n", status);
		return 1;
	}
	for (unsigned int i = 0; i < pLockInfo->LocksCount; i++) {
		PVOID lockAddress = pLockInfo->Locks[i].Address;
		USHORT lockType = (USHORT)pLockInfo->Locks[i].Type;
#ifdef _WIN64
		printf("Lock Address 0x%llx\t", lockAddress);
#else
		printf("Lock Address 0x%X\t", lockAddress);
#endif
		printf("Lock Type 0x%X\r\n", lockType);
	}
	return 0;
}

