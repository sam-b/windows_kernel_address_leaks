// SharedInfoHandleTable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>


typedef struct _HANDLEENTRY {
	PVOID	phead;
	PVOID	pOwner;
	BYTE	bType;
	BYTE	bFlags;
	WORD	wUniq;
}HANDLEENTRY, *PHANDLEENTRY;

typedef struct _SERVERINFO {
#ifdef _WIN64
	UINT64 dwSRVIFlags;
	UINT64 cHandleEntries;
#else
	DWORD dwSRVIFlags;
	DWORD cHandleEntries;
#endif
	WORD	wSRVIFlags;
	WORD	wRIPPID;
	WORD	wRIPError;
}SERVERINFO, *PSERVERINFO;

typedef struct _SHAREDINFO {
	PSERVERINFO		psi;
	PHANDLEENTRY	aheList;
	ULONG			HeEntrySize; 
	ULONG_PTR		pDispInfo;
	ULONG_PTR		ulSharedDelta;
	ULONG_PTR		awmControl; 
	ULONG_PTR		DefWindowMsgs; 
	ULONG_PTR		DefWindowSpecMsgs; 
}SHAREDINFO, *PSHAREDINFO;

int main()
{
	HMODULE hUser32 = LoadLibraryA("user32.dll");
	PSHAREDINFO gSharedInfo = (PSHAREDINFO)GetProcAddress(hUser32, "gSharedInfo");
	for (unsigned int i = 0; i < gSharedInfo->psi->cHandleEntries; i++) {
		HANDLEENTRY entry = gSharedInfo->aheList[i];
		if (entry.bType != 0) { //ignore free entries
#ifdef _WIN64
			printf("Head: 0x%llx, Owner: 0x%llx, Type: 0x%X\r\n", entry.phead, entry.pOwner, entry.bType);
#else
			printf("Head: 0x%X, Owner: 0x%X, Type: 0x%X\r\n", entry.phead, entry.pOwner, entry.bType);
#endif
		}
	}
    return 0;
}

