# Windows Kernel Address Leaks

This repository aims to provide functioning code that demonstrated usage of various different ways to gain access to Kernel Mode pointers in Windows from User Mode. A green ticket indicates a leak which works from a low integrity process and an organge tick a leak which requires a medium integrity process.

| Technique											| Windows 7 | Windows 8 | Windows 8.1 | Windows 10 - 1511 | Windows 10 - 1607 | Windows 10 - 1703 | 
|---------------------------------------------------|-----------|-----------|--------------|----------------|----------------|----------------|
| NtQuerySystemInformation: <br>[SystemHandleInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation.cpp) |	![](icons/tick.png)		|![](icons/tick.png)			|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|
|[SystemLockInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation.cpp)	|![](icons/tick.png)			|![](icons/tick.png)			|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|
|[SystemModuleInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation.cpp)	|![](icons/tick.png)			|![](icons/tick.png)			|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|
|[SystemProcessInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation.cpp)|![](icons/tick.png)			|![](icons/tick.png)			|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|
|[SystemBigPoolInformation](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemBigPoolInformation/NtQuerySysInfo_SystemBigPoolInformation/NtQuerySysInfo_SystemBigPoolInformation.cpp)|![](icons/tick.png)			|![](icons/tick.png)			|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|![](icons/amber.png)|
|[System Call Return Values](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/Syscalls/Syscalls/Syscalls.cpp)							|![](icons/tick.png)			|![](icons/cross.png)			|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|![](icons/cross.png)|
|[Win32k Shared Info User Handle Table](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/SharedInfoHandleTable/SharedInfoHandleTable/SharedInfoHandleTable.cpp)	|![](icons/tick.png)			|![](icons/tick.png)		|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|
|[Descriptor Tables](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/DescriptorTables/DescriptorTables/DescriptorTables.cpp)								|![](icons/tick.png)			|![](icons/tick.png)		|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|
|[HMValidateHandle](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/HMValidateHandle/HMValidateHandle/HMValidateHandle.cpp) |![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|
|[GdiSharedHandleTable](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/GdiSharedHandleTable/GdiSharedHandleTable/GdiSharedHandleTable.cpp)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|![](icons/cross.png)|
|[DesktopHeap](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/DesktopHeap/DesktopHeap/DesktopHeap.cpp)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/tick.png)|![](icons/cross.png)|

## Further Details
Some more details on techniques which no longer work and what was changed:
### NtQuerySystemInformation/System Call Return Values:
[https://samdb.xyz/revisiting-windows-security-hardening-through-kernel-address-protection/](https://samdb.xyz/revisiting-windows-security-hardening-through-kernel-address-protection/)
### Win32k Shared Info User Handle Table
[notes/gSharedInfo.md](notes/gSharedInfo.md) - A brief look at the changes made in the Creators Update/1703. Not very concrete or detailed, I might revisit it and create something more detailed or maybe someone else will.
### GdiSharedHandleTable / Desktop Heap
Pending
## Caveats
The Descriptor Table pointer leak will work on a standard Windows 10 (or possibly 8.1: https://twitter.com/JosephBialek/status/768954635570679808) machine but a Windows 10 Enterprise machine with HyperV enabled will trap on the sidt/sgdt instructions thanks to Intel Non-Privileged Instruction Execution Prevention (NPIEP) and return false values (see: [https://www.blackhat.com/docs/us-16/materials/us-16-Weston-Windows-10-Mitigation-Improvements.pdf](https://www.blackhat.com/docs/us-16/materials/us-16-Weston-Windows-10-Mitigation-Improvements.pdf), Windows Kernel 64-bit ASLR Improvements).   
## Attributions
I have referenced where I read about a technique and where specific structs etc have come from in the code, however these may not be the true original sources of the information :)   
A lot of the function prototypes and struct definitions are taken from [ReactOS](https://www.reactos.org/).   
Tick Icon By FatCow (http://www.fatcow.com/free-icons) [[CC BY 3.0](http://creativecommons.org/licenses/by/3.0)], via Wikimedia Commons   
Cross Icon By Cäsium137 [Public domain], via Wikimedia Commons   
