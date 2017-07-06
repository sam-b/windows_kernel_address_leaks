# NPIEP

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/npiep_definition.PNG)

```
// InstructionProtection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <intrin.h>

extern "C" void sgdt_wrapper(void *);
extern "C" void sidt_wrapper(void *);
extern "C" unsigned long long sldt_wrapper(void);
extern "C" unsigned short smsw_wrapper(void);
extern "C" unsigned long long str_wrapper(void);

#pragma pack(1)
typedef struct _DESCRIPTOR {
	unsigned short limit;
	unsigned long long base;
} DESCRIPTOR, *PDESCRIPTOR;

int main()
{
	printf("SGDT:\n");
	PDESCRIPTOR gdtr = (PDESCRIPTOR) malloc(sizeof(DESCRIPTOR));
	memset(gdtr, 0x41, sizeof(DESCRIPTOR));
	sgdt_wrapper((void *) gdtr);
	printf("\tGDTR base: 0x%llx, limit: 0x%X\n", gdtr->base, gdtr->limit);
	printf("SIDT:\n");
	PDESCRIPTOR idtr = (PDESCRIPTOR) malloc(sizeof(DESCRIPTOR));
	memset(idtr, 0x41, sizeof(DESCRIPTOR));
	sidt_wrapper((void *) idtr);
	printf("\tIDTR base: 0x%llx, limit: 0x%X\n", idtr->base, idtr->limit);
	free(idtr);
	printf("SLDT:\n");
	unsigned long long ldtr = sldt_wrapper();
	printf("\tLDTR selector: 0x%llx\n", ldtr);
	printf("SMSW:\n");
	unsigned short cr0 = smsw_wrapper();
	printf("\tCR0.PE = %d\n", cr0 & 1);
	printf("\tCR0.MP = %d\n", (cr0 >> 1) & 1);
	printf("\tCR0.EM = %d\n", (cr0 >> 2) & 1);
	printf("\tCR0.TS = %d\n", (cr0 >> 3) & 1);
	printf("\tCR0.ET = %d\n", (cr0 >> 4) & 1);
	printf("\tCR0.NE = %d\n", (cr0 >> 5) & 1);
	printf("STR:\n");
	unsigned long long tr = str_wrapper();
	printf("\tTR segment selector: 0x%llx\n", tr);
    return 0;
}
```


```
_DATA SEGMENT
_DATA ENDS
_TEXT SEGMENT

PUBLIC sgdt_wrapper
PUBLIC sidt_wrapper
PUBLIC sldt_wrapper
PUBLIC smsw_wrapper
PUBLIC str_wrapper

sgdt_wrapper PROC
sgdt [rcx]
ret
sgdt_wrapper ENDP

sidt_wrapper PROC
sidt [rcx]
ret
sidt_wrapper ENDP

sldt_wrapper PROC
sldt rax
ret
sldt_wrapper ENDP

smsw_wrapper PROC
smsw rax
ret
smsw_wrapper ENDP

str_wrapper PROC
str rax
ret
str_wrapper ENDP

_TEXT ENDS
END
```

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/test_no_vbs.PNG)


[Deploy Device Guard: enable virtualization-based security](https://docs.microsoft.com/en-us/windows/device-security/device-guard/deploy-device-guard-enable-virtualization-based-security)
[Device Guard and Credential Guard hardware readiness tool](https://www.microsoft.com/en-us/download/details.aspx?id=53337)
![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/test_vbs.PNG)

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/umip_definition.PNG)