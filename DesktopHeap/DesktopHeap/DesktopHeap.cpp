//Details from https://github.com/55-AA/CVE-2016-3308 and http://www.geoffchappell.com/studies/windows/win32/ntdll/structs/teb/index.htm
#include "stdafx.h"
#include <Windows.h>


typedef struct _TEB {
#ifdef _WIN64
	UCHAR ignored[0x0800];
#else
	UCHAR ignored[0x06CC];
#endif
	ULONG_PTR Win32ClientInfo[0x3E];
} TEB, *PTEB;

typedef struct _DESKTOPINFO {
	PVOID pvDesktopBase;
	PVOID pvDesktopLimit;
} DESKTOPINFO, *PDESKTOPINFO;

typedef struct _CLIENTINFO {
	ULONG_PTR CI_flags;
	ULONG_PTR cSpins;
	DWORD dwExpWinVer;
	DWORD dwCompatFlags;
	DWORD dwCompatFlags2;
	DWORD dwTIFlags;
	PDESKTOPINFO pDeskInfo;
	ULONG_PTR ulClientDelta;
} CLIENTINFO, *PCLIENTINFO;

int main() {
	LoadLibraryA("gdi32.dll");
	PTEB pTeb =  (PTEB)HeapAlloc(GetProcessHeap(), 0, sizeof(TEB));
	ReadProcessMemory(GetCurrentProcess(), NtCurrentTeb(), pTeb, sizeof(TEB), NULL);
	PCLIENTINFO clientInfo = (PCLIENTINFO) pTeb->Win32ClientInfo;
	ULONG_PTR ulClientDelta = clientInfo->ulClientDelta;
	PVOID pDeskBase = clientInfo->pDeskInfo->pvDesktopBase;
	PVOID pDeskLimit = clientInfo->pDeskInfo->pvDesktopLimit;
#ifdef _WIN64
	printf("Client delta: 0x%llx, Desktop Base: 0x%llx, Desktop Limit: 0x%llx\r\n", ulClientDelta, pDeskBase, pDeskLimit);
#else
	printf("Client delta: 0x%X, Desktop Base: 0x%X, Desktop Limit: 0x%X\r\n", ulClientDelta, pDeskBase, pDeskLimit);
#endif
	return 0;
}