// NtQuerySysInfo_SystemModuleInformation.cpp : Attempts to use the NtQuerySystemInformation to find the base addresses if loaded modules.
//

#include "stdafx.h"
#include <windows.h>

#define MAXIMUM_FILENAME_LENGTH 255 

typedef struct SYSTEM_MODULE {
	ULONG                Reserved1;
	ULONG                Reserved2;
	PVOID                ImageBaseAddress;
	ULONG                ImageSize;
	ULONG                Flags;
	WORD                 Id;
	WORD                 Rank;
	WORD                 w018;
	WORD                 NameOffset;
	BYTE                 Name[MAXIMUM_FILENAME_LENGTH];
}SYSTEM_MODULE, *PSYSTEM_MODULE;

typedef struct SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemModuleInformation = 11
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
	ULONG len = 0;
	query(SystemModuleInformation, NULL, 0, &len);

	PSYSTEM_MODULE_INFORMATION pModuleInfo = (PSYSTEM_MODULE_INFORMATION)GlobalAlloc(GMEM_ZEROINIT, len);
	if (pModuleInfo == NULL) {
		printf("Could not allocate memory for module info.\n");
		return 1;
	}
	query(SystemModuleInformation, pModuleInfo, len, &len);
	if (len == 0) {
		printf("Failed to retrieve system module information.\r\n");
		return 1;
	}
	for (int i = 0; i < pModuleInfo->ModulesCount; i++) {
		PVOID kernelImageBase = pModuleInfo->Modules[i].ImageBaseAddress;
		PCHAR kernelImage = (PCHAR)pModuleInfo->Modules[i].Name;
		printf("Module name %s\t", kernelImage);
		printf("Base Address 0x%X\r\n", kernelImageBase);
	}
    return 0;
}

