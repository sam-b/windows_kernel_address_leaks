# NPIEP

NPIEP is a new security feature introduced by Microsoft as part of their Virtualisation Based Security (VBS). We can find this brief summary in the [Hypervisor Top-Level Functional Specification](https://github.com/Microsoft/Virtualization-Documentation/raw/master/tlfs/Hypervisor%20Top%20Level%20Functional%20Specification%20v5.0b.pdf).

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/npiep_definition.PNG)

Each of these instructions provides information about control data used by the CPU and the OS. The instructions to set this data can only be executed in kernel mode but due to historic design choices user mode software can use the read vesions. As shown below for SGDT.

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/sgdt_definition.png)


In summary the instructions have the following purposes.

|Instruction|Name|Purpose|
|-----------|----|-------|
| [SGDT](http://x86.renejeschke.de/html/file_module_x86_id_290.html) | Store [Global Descriptor Table Register](http://wiki.osdev.org/Global_Descriptor_Table) | Stores the content of the global descriptor table register (GDTR) in the destination operand. |
| [SIDT](http://x86.renejeschke.de/html/file_module_x86_id_295.html) | Store [Interrupt Descriptor Table Register](http://wiki.osdev.org/Interrupt_Descriptor_Table) | Stores the content the interrupt descriptor table register (IDTR) in the destination operand. |
| [SLDT](http://x86.renejeschke.de/html/file_module_x86_id_296.html) | Store [Local Descriptor Table Register]() | Stores the segment selector from the local descriptor table register (LDTR) in the destination operand. |
| [SMSW](http://x86.renejeschke.de/html/file_module_x86_id_297.html) | Store Machine Status Word | Stores the machine status word (bits 0 through 15 of control register [CR0](http://wiki.osdev.org/CR0#CR0)) into the destination operand. |
| [STR](http://x86.renejeschke.de/html/file_module_x86_id_307.html) | Store [Task Register](http://wiki.osdev.org/Context_Switching#Hardware_Context_Switching) | Stores the segment selector from the task register (TR) in the destination operand. |

As you can see the information revealed all of these instructions may not be critical in all cases, but still probably shouldn't be exposed to user mode. Details on abusing the Global and Local Descriptor Tables for kernel exploitation can be found [here](http://vexillium.org/dl.php?call_gate_exploitation.pdf) and obviously overwriting entries in the Interrupt Descriptor Table can be great for hooking on key presses, network packets, etc. I used the below C code + asm file in visual studio to run the instructions in windows.

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

Running the resulting binary without VBS enabled, gave me the following. 

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/test_no_vbs.PNG)

I then configured and enabled HyperV followed by Device Gaurd on the test system by following this guide: [Deploy Device Guard: enable virtualization-based security](https://docs.microsoft.com/en-us/windows/device-security/device-guard/deploy-device-guard-enable-virtualization-based-security) and using the setup powershell script that can be downloaded here: [Device Guard and Credential Guard hardware readiness tool](https://www.microsoft.com/en-us/download/details.aspx?id=53337).

Once this was setup, I ran the test binary again, getting the following output.   
![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/test_vbs.PNG)

This actually looks completely normal, but then I rebooted and the IDT and GDT addresses were identical and again and again...obviously on a normal system they aren't at static addresses. So it looks like NPIEP is enabled and working.


As a final note, it looks like Intel recently implemented an equivalent feature in hardware ([some would say after far too long](http://www.os2museum.com/wp/better-late-than-never/)): 
![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/umip_definition.PNG)

(Taken from the intel instruction set reference.)

More details pending, some intern stole my test laptop :p 