# Windows Kernel Address Leaks

This repository aims to provide functioning code that demonstrated usage of various different ways to gain access to Kernel Mode pointers in Windows from User Mode. A green ticket indicates a leak which works from a low integrity process and a blue tick indicates a leak which requires a medium integrity process.

| Technique											| 7 | 8 | 8.1 | 10 - 1511 | 10 - 1607 | 10 - 1703 | 10 - 1703 + VBS | 
|---------------------------------------------------|-----------|-----------|--------------|----------------|----------------|----------------|----------------|
| NtQuerySystemInformation: <br>&nbsp;&nbsp;&nbsp;&nbsp;[SystemHandleInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation.cpp) <br>&nbsp;&nbsp;&nbsp;&nbsp;[SystemLockInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation.cpp)<br>&nbsp;&nbsp;&nbsp;&nbsp;[SystemModuleInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation.cpp)<br>&nbsp;&nbsp;&nbsp;&nbsp;[SystemProcessInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation.cpp)<br>&nbsp;&nbsp;&nbsp;&nbsp;[SystemBigPoolInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemBigPoolInformation/NtQuerySysInfo_SystemBigPoolInformation/NtQuerySysInfo_SystemBigPoolInformation.cpp)|	![](icons/tick.png)		|![](icons/tick.png)			|![](icons/blue.png)|![](icons/blue.png)|![](icons/blue.png)|![](icons/blue.png)|![](icons/blue.png)|
|[System Call Return Values](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/Syscalls/Syscalls/Syscalls.cpp)							|![](icons/tick.png)			|![](icons/cross.png)			|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|
|[Win32k Shared Info User Handle Table](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/SharedInfoHandleTable/SharedInfoHandleTable/SharedInfoHandleTable.cpp)	|![](icons/tick.png)			|![](icons/tick.png)		|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|![](icons/cross.png)|
|[Descriptor Tables](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/DescriptorTables/DescriptorTables/DescriptorTables.cpp)								|![](icons/tick.png)			|![](icons/tick.png)		|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|
|[HMValidateHandle](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/HMValidateHandle/HMValidateHandle/HMValidateHandle.cpp) |![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|
|[GdiSharedHandleTable](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/GdiSharedHandleTable/GdiSharedHandleTable/GdiSharedHandleTable.cpp)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|
|[DesktopHeap](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/DesktopHeap/DesktopHeap/DesktopHeap.cpp)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|![](icons/cross.png)|

The following techniques requiring non-standard permissions.

| Technique											| Permission Needed | 7 | 8 | 8.1 | 10 - 1511 | 10 - 1607 | 10 - 1703 | 10 - 1703 + VBS | 
|---------------------------------------------------|-------------------|-----------|-----------|--------------|----------------|----------------|----------------|----------------|
| NtSystemDebugControl: <br>&nbsp;&nbsp;&nbsp;&nbsp;[SysDbgGetTriageDump](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtSystemDebugControl_SysDbgGetTriageDump/NtSystemDebugControl_SysDbgGetTriageDump/NtSystemDebugControl_SysDbgGetTriageDump.cpp) | [SeDebugPrivilege](https://blogs.msdn.microsoft.com/oldnewthing/20080314-00/?p=23113) |![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/question.png)|
|  | SeSystemProfilePrivilege |

## Further Details
Some more details on techniques which no longer work and what was changed:
### NtQuerySystemInformation/System Call Return Values:
[https://samdb.xyz/revisiting-windows-security-hardening-through-kernel-address-protection/](https://samdb.xyz/revisiting-windows-security-hardening-through-kernel-address-protection/)
### Win32k Shared Info User Handle Table
[notes/gSharedInfo.md](notes/gSharedInfo.md) - A brief look at the changes made in the Creators Update/1703. Not very concrete or detailed, I might revisit it and create something more detailed or maybe someone else will.
### GdiSharedHandleTable / Desktop Heap
Pending
### NPIEP 
[notes/NPIEP.md](notes/NPIEP.md) - A very brief "it's a thing" write up, more details pending on me getting a test laptop back when the summer interns are gone...
## Attributions
I have referenced where I read about a technique and where specific structs etc have come from in the code, however these may not be the true original sources of the information :)   
A lot of the function prototypes and struct definitions are taken from [ReactOS](https://www.reactos.org/).   
Green Tick Icon By FatCow (http://www.fatcow.com/free-icons) [[CC BY 3.0](http://creativecommons.org/licenses/by/3.0)], via Wikimedia Commons   
Cross Icon By Cäsium137 [Public domain], via Wikimedia Commons   
Blue Tick By Gregory Maxwell, User:David Levy, Wart Dark (en:Image:Blue check.png) [GFDL 1.2 (http://www.gnu.org/licenses/old-licenses/fdl-1.2.html)], via Wikimedia Commons