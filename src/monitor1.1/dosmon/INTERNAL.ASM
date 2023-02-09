NAME Internal
TITLE Internal

; Internal.a: implementation of internal monitor commands.  The stack on
; entry to each command looks like so:
;
;    envp    32 bit pointer to array of string pointers
;    argv    32 bit pointer to array of string pointers
;    argc    32 bit integer
;    ret addr  <---- sp
;
;    each command returns 0 if everything ok, 1 if not, in ax

PUBLIC _set,set_name,set_usage,set_help
PUBLIC shift_name,shift_usage,shift_help,exit_name,exit_usage,exit_help,if_name
PUBLIC goto_name
PUBLIC _read,read_name,read_usage,read_help
PUBLIC _help,help_name,help_usage,help_help
PUBLIC _echo,echo_name,echo_usage,echo_help
PUBLIC _status,status_name,status_usage,status_help
EXTRN my_strlen:NEAR,newline:BYTE,environment:WORD
EXTRN my_strcpy:NEAR,my_strncmp:NEAR,mygetenv:NEAR
EXTRN _linebuf1:WORD
EXTRN my_strcmp:near,my_strstr:near,_n_externals:WORD,_external_names:WORD
EXTRN _external_usage:WORD,_external_help:WORD
EXTRN _print_i:near,envc:WORD,enddata:WORD,my_data:WORD

INCLUDE macros.asm

DGROUP GROUP _TEXT,_DATA,_BSS

_DATA SEGMENT PUBLIC 'DATA' 
ASSUME DS:DGROUP

if_name db "if",00H
goto_name db "goto",00H
shift_name db "shift",00H
shift_usage db "shift [args ...]",0DH,0AH,00H
shift_help db "run args as new command after processing",0DH,0AH,00H
exit_name db "exit",00H
exit_usage db "exit",0DH,0AH,00H
exit_help db "immdiately exit from program with return value 0",0DH,0AH,00H

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; set command                                                         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



set_name db "set",00H
set_usage db "set [ FOO BAR ]",0DH,0AH,00H
set_help db "set FOO=BAR, unset FOO if BAR absent, list env if no args",0DH,0AH,00H
set_data dw 1  ; memory to keep record of number of args
set_mode dw 1  ; memory to keep record of mode of operation

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP,DS:DGROUP

_set:

	push bp       ; set up stack frame
	mov bp,sp    ;

	push si       ; store registers used on stack
	push di
	push bx
	push cx
	push dx

	mov ax,[bp+4]  ;  argc into ax
	cmp ax,3        ;  3 or more args is a usage error
	jb args_ok
	jmp set_bad_ret

args_ok:

	cmp ax,0      ;  0 args: print listing of environment
	jne not_zero_args
	jmp set_zero_args

not_zero_args:

	mov bx, [bp + 6]           ;save:  bx has  argv
	mov set_mode,ax            ;save:  value of argc

	; meaning of other values of argc:
        ; 1  set foo: remove setting for foo
        ; 2  set foo bar: copy foo=bar to data area

	cmp ax,1
	je set_one_arg                ; if only, one arg, skip copy of string

        ; stack up pointer to new env string - needed at end of 2 arg code  

	push enddata                  ; it is in data area, at end

	; copy the new environment string, piece by piece, to the end
	; of the data area

	push WORD PTR [bx]            ; pointer to string foo (*argv) 
	push enddata                  ; where to copy to
	call my_strcpy                ; copy foo
	add sp,4
	dec ax                        ; point at null char at end of string
	add enddata,ax                ; length of string copied in ax
	mov al,03DH                   ; ascii code for =
	mov bx,enddata
	mov BYTE PTR [bx],al
	inc enddata
	mov bx, [bp + 6]
	push WORD PTR [bx + 2]        ; pointer to string bar
	push enddata
	call my_strcpy
	add sp,4
	add enddata,ax   
		
set_one_arg:                         ; or two args - action here is the same

; search environment to see if foo= is already there

	push WORD PTR [bx]           ; pointer to string foo
	call mygetenv                ; See if it is set in environment
	add sp,2                     ; clean up from call
	cmp ax,00H
	je set_search1_done          ; not found, skip remove_loop 
	
remove_loop:

	; remove the foo= string from the environment by moving all
	; pointers below it up by one 
        ; BUG: should we also null out the part of data area it points to
        ; for possible re-use ? Over time, crud will build up.

	add si,2
	mov ax, environment[si] ; next pointer below to ax
	sub si,2
	mov environment[si],ax      ; and replace current pointer with it
	cmp cx,0                    ; done when cx = 0
	je set_done_shift           ; so continue on ...
	add si,2                    ; else increment index,
	dec cx                      ; decrement counter, and
	jmp remove_loop             ; restart move up pointers loop

set_done_shift:
	dec envc                    ; count reduced when env string removed
	                            ; may be increased again below
	sub si,2                    ; N.B.: si has been increased one time
                                    ; too many 
set_search1_done:             

	mov ax,set_mode             ; remember mode of operation: two args
	cmp ax,2                    ; or one, and branch accordingly
	je set_two_args
	jmp  set_good_ret           ; all done if there was only 1 arg

set_two_args:                       ; else two args 

	pop bx                      ; recall saved pointer to new string area
	mov environment[si], bx     ; and install it
	inc envc
	jmp set_good_ret 
	
; 0 args: print a listing of current environment

set_zero_args:
	mov si,0     ; initialize counter

set_prenv:
	mov cx,environment[si]   ; point cx at first env string
	cmp cx,0                     ; zero pointer marks end of env
	je set_good_ret               ; done with no problems
	PRINT_STRING environment[si]
	PRINT_CHAR 0DH
	PRINT_CHAR 0AH                ; move output to new line
	add si,2                     ; increase counter 
	jmp set_prenv                 ; restart print env loop

set_good_ret:
	mov ax,0                     ; good return value
	jmp set_ret

set_bad_ret:

	PRINT_STRING set_usage        ; usage error message
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	mov ax,1                     ; bad return value
	
set_ret:

	pop dx                       ; restore registers used
	pop cx
	pop bx
	pop di
	pop si
	pop bp
	ret

_TEXT ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; read command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP

read_name db "read",00H
read_usage db "read var",0DH,0AH,00H
read_help db "Prompt user for input and store response in env var",0DH,0AH,00H
read_prompt db "Enter your response: ",00H
read_setargs  dw 2 DUP(0)      ; char **argv for set call

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP, DS:DGROUP

_read:

	push bp                    ; set up stack frame
	mov bp,sp
	push bx

	PRINT_STRING read_prompt    ; present user with prompt for input 
	mov ax, OFFSET DGROUP:_linebuf1
	READ_LINE ax          ; read her response into buffer

; The rsponse buffer has a newline that we have to get rid of

	mov bx,ax
	dec bx
	dec bx
	mov BYTE PTR _linebuf1[bx], 00H ; kill the newline 
	
	mov ax, [bp + 4]          ; pointer to name of env var passed in
	mov read_setargs,ax 
	mov ax, OFFSET DGROUP:_linebuf1           ; pointer to value of env var
	mov read_setargs[2],ax    ; finish building argv for set
	mov ax, OFFSET DGROUP:read_setargs          ; push char **argv for set
	push ax
	mov ax,2
	push ax                    ; push argc (=2) for set call 
	call _set                   ; set var = rsponse
	add sp,4                   ; clean up from set call
	pop bx
	pop bp                     ; restore registers used
	ret 

_TEXT ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; help command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP


help_name db "help",00H
help_usage db "help [ command ]",0DH,0AH,00H
help_help db "print cmd list or help for command",0DH,0AH,00H
help_msg0 db "Available commands are:",0DH,0AH,0DH,0AH,00H
help_ptr dw 0
equalpound db 03DH,COMMENT_CHAR,00H  ; the sequence =# that identifies scripts

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP,DS:DGROUP

_help:

	push bp       ; set up stack frame
	mov bp,sp     ;

	push si       ; store registers used on stack
	push bx
	push cx
	push dx

	mov ax,4[bp]  ;  argc into ax
	cmp ax,2      ;  2 or more args is a usage error
	jb help_args_ok
	jmp help_bad_ret
help_args_ok:
	cmp ax,0      ;   
	jne one_arg
	jmp help_no_args
one_arg:

; 1 arg: print detailed help for command
	
	mov ax, 6[bp]   ; *argv in bx
	push ax      ; push *argv

	mov help_ptr,ax

	lea ax, set_name
	push ax 
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_shift
	PRINT_STRING set_usage
	PRINT_STRING set_help
	jmp help_cleanup

is_it_shift:

	lea ax, shift_name
	push ax 
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_read
	PRINT_STRING shift_usage
	PRINT_STRING shift_help
	jmp help_cleanup

is_it_read:

	lea ax, read_name
	push ax
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_help
	PRINT_STRING read_usage
	PRINT_STRING read_help
	jmp help_cleanup

is_it_help:

	lea ax, help_name
	push ax
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_echo
	PRINT_STRING help_usage
	PRINT_STRING help_help
	jmp help_cleanup

is_it_echo:

	lea ax, echo_name
	push ax
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_status
	PRINT_STRING echo_usage
	PRINT_STRING echo_help
	jmp help_cleanup

is_it_status:

	lea ax, status_name
	push ax
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_exit
	PRINT_STRING status_usage
	PRINT_STRING status_help
	jmp help_cleanup

is_it_exit:

	lea ax, exit_name
	push ax
	call my_strcmp
	add sp,2
	cmp ax,0
	jne is_it_external
	PRINT_STRING exit_usage
	PRINT_STRING exit_help
	jmp help_cleanup

is_it_external:
	
	mov cx,[_n_externals] 
	xor si,si

help_external_search:

	cmp cx,0
	jbe is_it_a_script
	push _external_names[si]
	call my_strcmp
	add sp,2
	cmp ax,0 
	je help_found_external
	add si,2
	dec cx
 	jmp help_external_search

help_found_external:

	PRINT_STRING _external_usage[si]
	PRINT_STRING _external_help[si]
	jmp help_cleanup

is_it_a_script:

	call mygetenv
	cmp ax,00H
	je help_not_found
	mov help_ptr,ax
	PRINT_PTR help_ptr
	jmp help_cleanup
	
	; clean stack and print all commands since this arg not found

help_not_found:
	add sp,2
	jmp help_no_args

help_cleanup:
	add sp,2      ; pop pushed arg
	jmp help_good_ret

help_no_args:

; 0 args: print a listing of commands

	PRINT_STRING  help_msg0
	PRINT_STRING  set_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING  help_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING  echo_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING  status_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING  shift_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING exit_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	PRINT_STRING read_name
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	

; loop over external commands, printing their names as well

	mov cx,_n_externals
	xor si,si

help_list_externals:

	cmp cx,0
	je help_list_scripts
	PRINT_STRING _external_names[si]	
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	add si,2
	dec cx
	cmp cx,0
	jmp help_list_externals

; loop over all scripts. We believe a script is the value of any environment
; variable that starts with #

help_list_scripts:

	mov cx,envc    ; loop over all env variables
	xor si,si       ; initialize env pointer offset

help_list_scripts_loop:

	cmp cx,0         ; if no env variable remain to be examined
	je help_good_ret  ; then begin the return process
	mov ax,environment[si]  ; else store address of current env string
	mov help_ptr,ax           ; in a local holding area 
	lea ax, equalpound
	push ax  ; determine whether the =# sequence occurs inside
	mov ax, help_ptr
	push ax ; current env string. If so, we deem it the start
	call my_strstr    ; of a script. (BUG: it might not be.)
	add sp,4         ; clean up from call
	cmp ax,00H      ; if strstr returned null then this var not a script
	je scripts_loop_continue ; so go on to the next env var
; else print out the name part of the current env var as the name of a script
	mov bx,ax
	mov BYTE PTR [bx], 00H ; termporarily null terminate the name part of var
	push ax          ; save address of null byte for later restore
	PRINT_PTR help_ptr
	PRINT_CHAR 0DH
	PRINT_CHAR 0AH    ; print a new line
	pop ax           ; restored saved address of byte where = removed
	mov BYTE PTR [bx],3DH  ; put = sign back in env string

scripts_loop_continue:
	
	dec cx
	add si,2
	jmp help_list_scripts_loop

help_good_ret:
	mov ax,0                     ; good return value
	jmp help_ret

help_bad_ret:
	PRINT_STRING help_usage        ; usage error message
	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	mov ax,1                     ; bad return value
	
help_ret:

	pop dx                       ; restore registers used
	pop cx
	pop bx
	pop si
	pop bp
	ret

_TEXT ENDS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; echo command                                                        ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP


echo_name db "echo",0
echo_usage db "echo [ args ... ]",0DH,0AH,00H
echo_help db "print args with one space between each",0DH,0AH,00H
echo_arg dw 0
echo_ptr dw 0

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP, DS:DGROUP

_echo:

	push bp       ; set up stack frame
	mov bp,sp    ;
	push si       ; save registers used
	push bx
	push cx
	push dx


;  print everthing that second arg points to 

	mov cx,[bp + 4] ; load argc in cx = number of words to echo
	mov bx,[bp + 6]; load argv in bx

echo_print_loop:

	cmp cx,00H       ; Any more args to echo ?
	je  echo_ret     ; No, finish up and return
	mov dx,[bx]      ;load *argv in dx
	mov echo_ptr,dx
	PRINT_PTR echo_ptr   ; print the arg
	PRINT_CHAR 20H
	add bx,2         ; increment argv
	dec cx           ; decrement counter
	jmp echo_print_loop

echo_ret:

	PRINT_CHAR 0DH	
	PRINT_CHAR 0AH	
	mov ax,0                     ; good return value
	pop dx
	pop cx                       ; restore registers used
	pop bx
	pop si
	pop bp
	ret

_TEXT ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; status command                                                       ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP

status_name db "status",00H
status_usage db "status",0DH,0AH,00H
status_help db "Print data on current program state",0DH,0AH,00H
status_str0 db 0DH,0AH,"Monitor memory available = ",00H
status_str2 db 0DH,0AH,"Number of Environment Strings = ",00H
status_str3 db 0DH,0AH,"Monitor memory Used (bytes) = ",00H
status_str4 db 0DH,0AH,"Stack used (bytes) = ",00H

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP, DS:DGROUP

_status: 

; The break address is the top of memory available to the program for
; dynamic memory allocation. We put all new environment strings in this
; area. Since we never bother to reclaim any of this memory it will get
; used up over time. Obtaining the break address is OS dependent
; and we implement it in SBRK in macros.a

	PRINT_STRING status_str0             	
	SBRK                       ; return break address in ax
	sub ax, enddata
	push ax
	call _print_i              ; print break address to screen
	pop ax                    ; clean up from call 
	PRINT_STRING status_str3
	mov ax, enddata         ; end of data we have written to data
	sub ax,OFFSET DGROUP:my_data  ; subtract from start of data area
	push ax                   ; and print the difference = bytes
	call _print_i              ; of dynamic memory used
	pop ax                    ; clean up from call
	PRINT_STRING status_str2
	mov ax, envc            ; number of environment strings
	push ax
	call _print_i              ; print it and
	pop ax                    ; and clean up from call
	PRINT_STRING status_str4
	mov ax,0FFFEH
	sub ax,sp
	push ax
	call _print_i
	pop ax
	ret
	
_TEXT ENDS
_BSS SEGMENT PUBLIC 'BSS'
_BSS ENDS
END
