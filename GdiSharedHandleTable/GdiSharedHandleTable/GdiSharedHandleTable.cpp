// GdiSharedHandleTable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

//Mostly from http://www.geoffchappell.com/studies/windows/win32/ntdll/structs/peb/index.htm, other structures etc...
typedef struct _PEB {
#ifdef _WIN64
	UCHAR ignored[0xf8];
#else
	UCHAR ignored[0x94];
#endif
	PVOID GdiSharedHandleTable;
} PEB, *PPEB;

typedef struct _GDICELL
{
	PVOID pKernelAddress;
	USHORT wProcessId;
	USHORT wCount;
	USHORT wUpper;
	USHORT wType;
	PVOID pUserAddress;
} GDICELL, *PGDICELL;

typedef struct _PROCESS_BASIC_INFORMATION {
	NTSTATUS  ExitStatus;
	PVOID     PebBaseAddress;
	ULONG_PTR AffinityMask;
	LONG      BasePriority;
	HANDLE    UniqueProcessId;
	HANDLE    InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef enum _PROCESSINFOCLASS {
	SystemProcessBasicInformation = 0
} PROCESSINFOCLASS;

typedef NTSTATUS(WINAPI *PNtQueryInformationProcess)(
	_In_      HANDLE           ProcessHandle,
	_In_      PROCESSINFOCLASS ProcessInformationClass,
	_Out_     PVOID            ProcessInformation,
	_In_      ULONG            ProcessInformationLength,
	_Out_opt_ PULONG           ReturnLength
	);

int main() {
	HMODULE ntdll = GetModuleHandle(TEXT("ntdll"));
	PNtQueryInformationProcess query = (PNtQueryInformationProcess)GetProcAddress(ntdll, "NtQueryInformationProcess");
	if (query == NULL) {
		printf("GetProcAddress() failed.\n");
		return 1;
	}
	LoadLibraryA("gdi32.dll");
	ULONG dwReturned = 0;
	PROCESS_BASIC_INFORMATION processBasicInfo = { 0x0 };
	NTSTATUS status = query(GetCurrentProcess(), SystemProcessBasicInformation, &processBasicInfo, sizeof(PROCESS_BASIC_INFORMATION), &dwReturned);
	PPEB peb = (PPEB)HeapAlloc(GetProcessHeap(), 0, sizeof(PEB));
	ReadProcessMemory(GetCurrentProcess(), processBasicInfo.PebBaseAddress, peb, sizeof(PEB), NULL);
	DWORD count = 0x8fff; //That should do it...
	PGDICELL gdiTable = (PGDICELL)HeapAlloc(GetProcessHeap(), 0, sizeof(GDICELL) * count);
	ReadProcessMemory(GetCurrentProcess(), peb->GdiSharedHandleTable, gdiTable, sizeof(GDICELL) * count, NULL);
	for (unsigned int i = 0; i < count; i++) {
		GDICELL cell = gdiTable[i];
		if (cell.pKernelAddress == (void *)0xCDCDCDCD) {
			break;
		}
		if (cell.pKernelAddress == 0x00000000) {
			continue;
		}
		HANDLE gdiHandle = (HANDLE)((cell.wUpper << 16) + i);
#ifdef _WIN64
		printf("Kernel address: 0x%llx, GDI Handle: 0x%llx, Process ID: %d\r\n", cell.pKernelAddress, gdiHandle, cell.wProcessId);
#else
		printf("Kernel address: 0x%X, GDI Handle: 0x%X, Process ID: %d\r\n", cell.pKernelAddress, gdiHandle, cell.wProcessId);
#endif
	}
	return 0;
}