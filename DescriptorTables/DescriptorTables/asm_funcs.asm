_DATA SEGMENT
_DATA ENDS
_TEXT SEGMENT

PUBLIC get_tss_selector
get_tss_selector PROC
	str eax
	ret
get_tss_selector ENDP

_TEXT ENDS
END