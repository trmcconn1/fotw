
NAME EXTERNAL
TITLE EXTERNAL

; External.a: implementation of external monitor commands  
; the stand-alone version. 

PUBLIC _info,info_name,info_usage,info_help
PUBLIC _n_externals,_external_entry_points,_external_help,_external_usage
PUBLIC _external_names
EXTRN my_strlen:NEAR


DGROUP GROUP _TEXT,_DATA,_BSS

INCLUDE macros.asm

_DATA SEGMENT PUBLIC 'DATA' 
ASSUME DS:DGROUP

info_name db "info",00H
info_usage db "info",0DH,0AH,00H
info_help db "print author and other info",0DH,0AH,00H
info_msg db "Boot Monitor 1.2 by Terry R. McConnell (2020)",0DH,0AH,00H
_n_externals dw 1
_external_entry_points dw OFFSET DGROUP:_info + 0100H
_external_help dw OFFSET DGROUP:info_help
_external_usage dw OFFSET DGROUP:info_usage
_external_names dw OFFSET DGROUP:info_name

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:_TEXT,DS:DGROUP

_info:

	PRINT_STRING info_msg
	ret

_TEXT ENDS
_BSS SEGMENT PUBLIC 'BSS'
ASSUME DS:DGROUP
_BSS ENDS
END

