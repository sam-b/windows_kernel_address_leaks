// NtSystemDebugControl_SysDbgGetTriageDump.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "windows.h"
#include "TlHelp32.h"

//from https://crashdmp.wordpress.com/2014/08/04/livedump-1-0-is-available/
typedef struct _SYSDBG_TRIAGE_DUMP
{
	ULONG Flags;
	ULONG BugCheckCode;
	ULONG_PTR BugCheckParam1;
	ULONG_PTR BugCheckParam2;
	ULONG_PTR BugCheckParam3;
	ULONG_PTR BugCheckParam4;
	ULONG ProcessHandles;
	ULONG ThreadHandles;
	PHANDLE Handles;
} SYSDBG_TRIAGE_DUMP, *PSYSDBG_TRIAGE_DUMP;

//from https://github.com/subTee/native-nt-toolkit/blob/cc044793eaa51889c032a52a1590e376f28fa5a7/ndk/kdtypes.h
typedef enum _SYSDBG_COMMAND
{
	SysDbgGetTriageDump = 29,
} SYSDBG_COMMAND;

//from http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FDebug%2FNtSystemDebugControl.html
typedef NTSTATUS(WINAPI *PNtSystemDebugControl)(
	IN SYSDBG_COMMAND       Command,
	IN PVOID                InputBuffer OPTIONAL,
	IN ULONG                InputBufferLength,
	OUT PVOID               OutputBuffer OPTIONAL,
	IN ULONG                OutputBufferLength,
	OUT PULONG              ReturnLength OPTIONAL);

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("Target PID required\r\n");
		return 1;
	}

	DWORD dwPID = atoi(argv[1]);
	
	//based off of https://msdn.microsoft.com/en-us/library/windows/desktop/aa446619(v=vs.85).aspx

	TOKEN_PRIVILEGES tokenPrivileges = { 0x0 };
	LUID luid = { 0x0 };
	tokenPrivileges.PrivilegeCount = 1;
	
	
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		printf("Couldn't lookup SE_DEBUG_NAME privilege luid:  0x%X\r\n", GetLastError());
		return 1;
	}
	tokenPrivileges.Privileges[0].Luid = luid;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		printf("Couldn't open own token?:  0x%X\r\n", GetLastError());
		return 1;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, 0x10, NULL, NULL)) {
		printf("Failed to privesc:  0x%X\r\n", GetLastError());
		CloseHandle(hToken);
		return 1;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL) {
		printf("Failed to get a handle to target process: 0x%X\r\n", GetLastError());
		CloseHandle(hToken);
		return 1;
	}
	printf("Opened target process\r\n");
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPID);

	THREADENTRY32 threadEntry = { 0x0 };
	threadEntry.dwSize = sizeof(threadEntry);
	if (!Thread32First(hSnapshot, &threadEntry)) {
		printf("Failed to enumerate threads for pid: %d, error code: 0x%X\r\n", dwPID, GetLastError());
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return 1;
	}

	unsigned int uiCountThreadHandles = 0;
	HANDLE hThreads[16] = { 0x0 };
	HANDLE hThread = NULL;
	do {
		if (threadEntry.dwSize >= 20) {
			hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, threadEntry.th32ThreadID);

			if (hThread != INVALID_HANDLE_VALUE) {
				hThreads[uiCountThreadHandles] = hThread;
				uiCountThreadHandles++;
			}
		}

	} while ((Thread32Next(hSnapshot, &threadEntry)) && uiCountThreadHandles < 16);

	printf("Got %d process threads\r\n", uiCountThreadHandles);
	HMODULE ntdll = GetModuleHandle(TEXT("ntdll"));
	PNtSystemDebugControl NtSystemDebugControl = (PNtSystemDebugControl)GetProcAddress(ntdll, "NtSystemDebugControl");
	if (NtSystemDebugControl == NULL) {
		printf("GetProcAddress() failed.\n");
		return 1;
	}
	printf("Got pointer to NtSystemDebugControl\r\n");
	
	SYSDBG_TRIAGE_DUMP sysDbgDump = { 0 };
	ULONG ulInSize = sizeof(SYSDBG_TRIAGE_DUMP);
	sysDbgDump.ProcessHandles = 0;
	sysDbgDump.ThreadHandles = uiCountThreadHandles;
	sysDbgDump.Handles = &hThreads[0];
	
	ULONG ulOutSize = (1024 * 1024) - 1; //needs to be < 1mb
	void *pOutBuffer = HeapAlloc(GetProcessHeap(), 0, ulOutSize);
	
	ULONG ulRetLen = 0;
	
	printf("Everything setup - calling NtSystemDebugControl\r\n");
	
	NTSTATUS status = NtSystemDebugControl(SysDbgGetTriageDump, &sysDbgDump, ulInSize, pOutBuffer, ulOutSize, &ulRetLen);
	printf("Status code: 0x%llx\r\n", status);
	if (status != 0) {
		printf("Something bad happened\r\n");
		return 1;
	}

	printf("Function returned %d bytes\r\n", ulRetLen);
	HANDLE hOutFile = CreateFile(TEXT("out.dmp"), 
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hOutFile == NULL) {
		printf("CreateFileW failed: 0x%X\r\n", GetLastError());
		return 1;
	}
	
	ULONG bytesWritten = 0;
	if (!WriteFile(hOutFile, pOutBuffer, ulRetLen, &bytesWritten, NULL)) {
		printf("WriteFile failed:  0x%X\r\n", GetLastError());
		return 1;
	}
	FlushFileBuffers(hOutFile);

	printf("Wrote %d byte dump file to out.dmp\r\n", bytesWritten);
	
	CloseHandle(hOutFile);
	return 0;
}