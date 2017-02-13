# Windows Kernel Address Leaks

This repository aims to provide functioning code that demonstrated usage of various different ways to gain access to Kernel Mode pointers in Windows from User Mode.

| Technique											| Windows 7 | Windows 8 | Windows 8.1  |Windows 8.1  	| Windows 10 |
|													|			|			| Low Integrity|Medium Integrity|			 |
|---------------------------------------------------|-----------|-----------|--------------|----------------|------------|
|[NtQuerySystemInformation (SystemHandleInformation)](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation.cpp) |	![](tick.png)		|![](tick.png)			|![](cross.png)			  |	![](tick.png)		  |			|
|[NtQuerySystemInformation (SystemLockInformation)](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation/NtQuerySysInfo_SystemLockInformation.cpp)	|![](tick.png)			|![](tick.png)			|![](cross.png)			  |	![](tick.png)		  |			|
|[NtQuerySystemInformation (SystemModuleInformation)](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation/NtQuerySysInfo_SystemModuleInformation.cpp)	|![](tick.png)			|![](tick.png)			|![](cross.png)			  |	![](tick.png)		  |			|
|[NtQuerySystemInformation (SystemProcessInformation)](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation/NtQuerySysInfo_SystemProcessInformation.cpp)|![](tick.png)			|![](tick.png)			|![](cross.png)			  |	![](tick.png)		  |			|
|[System Call Return Values](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/Syscalls/Syscalls/Syscalls.cpp)							|![](tick.png)			|			|			  |			  |			|
|[Win32k Shared Info Handle Table](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/SharedInfoHandleTable/SharedInfoHandleTable/SharedInfoHandleTable.cpp)					|![](tick.png)			|![](tick.png)		|			  |			  |			|
|[Descriptor Tables](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/DescriptorTables/DescriptorTables/DescriptorTables.cpp)									|![](tick.png)			|![](tick.png)			|![](tick.png)			  |![](tick.png)			  |			|



##Attributions
I have referenced where I read about a technique and where specific structs etc have come from in the code, however these may not be the true original sources of the information :)   
A lot of the function prototypes and struct definitions are taken from [ReactOS](https://www.reactos.org/).   
Tick Icon By FatCow (http://www.fatcow.com/free-icons) [[CC BY 3.0](http://creativecommons.org/licenses/by/3.0)], via Wikimedia Commons   
Cross Icon By Cäsium137 [Public domain], via Wikimedia Commons   