_DATA SEGMENT
_DATA ENDS
_TEXT SEGMENT

PUBLIC get_idtr
get_idtr PROC
	sidt [rcx]
	ret
get_idtr ENDP

PUBLIC get_tss_selector
get_tss_selector PROC
	str eax
	ret
get_tss_selector ENDP

_TEXT ENDS
END