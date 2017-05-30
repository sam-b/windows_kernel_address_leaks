# gSharedInfo
## Background

While going through and validating the status of the kernel leaks on versions 1607/1703 of Windows 10, I found that the gSharedInfo table no longer gave us kernel pointers and the table structure had clearly been changed.

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/gSharedInfo_win_1703.png)

Which is very different to what used to be returned on 1607:   

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/gSharedInfo_win_1607.png)

Turns out this shouldn't have been much of a surprise, as people had been discussing it during the [preview build releases](https://twitter.com/Blomster81/status/847571032521273345).
So I set out to atleast get a basic idea of what the changes entailed.

## Initial Structure 'Reversing'
With some trial and error purely using Visual Studios debugger it was easy enough to work out the size of the new structure. Increasing the structures size to 256 bits meant that the pHead and pOwner fields both contained what looked like HANDLEs and various fields contained repeating values. 
A big help in working this out was 32 bits worth of null bytes which appears at an offset of 224 bits in each entry, this is likely padding to bring the structure up to 256 bits which fits on a 64 bit alignment nicely.
<pre> 
typedef struct _HANDLEENTRY {
	PVOID	pHead;
	PVOID	pOwner;
	BYTE	bType;
	BYTE	bFlags;
	WORD	wUniq;
	WORD	unknown;
	WORD	unknown1; 
	WORD	unknown2;
	WORD	unknown3;
	INT     padding;
}HANDLEENTRY, *PHANDLEENTRY;
</pre>

I also modfiied the printf statement used to log values, as shown below.

<pre>
printf("Head: 0x%llp, Owner: 0x%llp, flags: 0x%X, uniq: 0x%X Type: 0x%X, unknown: 0x%x, unknown1: 0x%X, unknown2: 0x%X, unknown3: 0x%X\r\n", entry.phead, entry.pOwner, entry.bFlags, entry.wUniq ,entry.bType, entry.unknown, entry.unknown1, entry.unknown2, entry.unknown3);
</pre>

Running the updated code gives much saner looking output:

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/gSharedInfo_win_1703_struct_size_example.png)

At this point the phead and pOwner fields very much look like handles. Additionally a lot of the new fields have recurring values (for example unknown's value is constant for each object type (bType)) so I figured I might be able to get some insight into those at the same time.

## pHead/pOwner Verification

Working with the assumption the pHead and pOwner values are pointers now I decided to validate they were atleast valid pointers. To do this I selected a few entries, for example the one shown next.
<pre>
Head: 0x00000000000009D0, Owner: 0x0000000000000328, flags: 0x0, uniq: 0x0 Type: 0x3, unknown: 0xac39, unknown1: 0x3697, unknown2: 0x1, unknown3: 0x1
</pre>

I then used [NtQuerySystemInformation (SystemHandleInformation)](https://github.com/sam-b/windows_kernel_address_leaks/blob/master/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation/NtQuerySysInfo_SystemHandleInformation.cpp) to dump all the Handles on the system and the corresponding objects addresses. 
Looking up the handles I'd picked proved they were all atleast valid handles. Although not knowing what process they corresponded to made proper validation hard. For example I found:   
<pre>
PID: 1592	Object 0xffffa103d2b11d50	Handle 0x328
...
PID: 1592	Object 0xffffa103d366d080	Handle 0x9d0
</pre>
Aswell as results for a number of other processes. Looking into win32kfull.sys in IDA and viewing references to gSharedInfo shows it is still heavily in use, with most uses looking similar to the below from 'LockDesktopMenu':

![](https://github.com/sam-b/windows_kernel_address_leaks/raw/master/notes/screenshots/LockDesktopMenu.png)

This shows the table being used for handle management, as all of the other uses, atleast a glance do. I have no idea about the unknown fields, unknown1 atleast changes across boots.

So in conclusion pHead and pOwner are probably Handles for the corresponding objects now, rather than addresses as they were pre-Creators update. I have no idea about the other stuff, I might look into this more in the future but I'm not sure there's much point :)
