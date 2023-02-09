; monitor: very basic, generic command interpreter
; Implemented in MASM syntax for 16 Bit MSDOS platform
; Ported from an implementation for Linux in NASM syntax

NAME monitor
TITLE monitor

INCLUDE macros.asm     ; macros for DOS system calls

; symbols exported from this module

PUBLIC _monitor,newline,environment,envc,my_data,enddata,_rcscript,_linebuf1
PUBLIC _copy_env,_alone

; symbols defined externally

EXTRN my_strlen:near,my_strcpy:near,my_strcmp:near,mygetenv:near,my_strstr:near
EXTRN _set:near,set_name:word,set_usage:word,set_help:word
EXTRN _help:near,help_name:word,help_usage:word,help_help:word
EXTRN _echo:near,echo_name:word,echo_usage:word,echo_help:word
EXTRN _status:near,status_name:word,status_usage:word,status_help:word
EXTRN shift_name:word,shift_usage:word,shift_help:word
EXTRN _n_externals:word,_external_names:word,_external_entry_points:word
EXTRN exit_name:word,exit_usage:word,exit_help:word
EXTRN _read:near,read_name:word,read_usage:word,read_help:word
EXTRN _print_i:near,if_name:word,goto_name:word
EXTRN save_disk:byte,save_cyl:byte,save_sec:byte,save_head:byte,save_seg:byte
EXTRN save_off:byte,save_size:byte


;;; Monitor can be built as either a .com suitable to run
; as a stand-alone program, or incorporated into a larger application.
; The stand-alone version is tiny model - one segment for both code and
; data. Only tiny model programs can be converted to a .com.
; As distributed, monitor can also be built as a small model 16 bit program. 
; The segment name and groups
; are compatible with the ones created by bcc -c -ms  (Borland C/C++ 4.5)
; The memory map is:
; _TEXT and other code class segments
; _DATA segment
; Other data class segments (e.g, from libc)
; _BSS segment
; Other _BSS class segments
; STACK segment

; Group declares a set of segments that all use the same segment register 
DGROUP GROUP _TEXT,_DATA,_BSS

; Combine type public makes segments with same name contiguous in the map
; and class name makes segments of the same class contiguous

ORG 100H  ; The entry point for a .com is at offset 100H to allow space
; for the PSP

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP,DS:DGROUP

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Program Entry Point                                                 ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_monitor:

	mov al,CS:_alone                  ; jump around stuff that only
	cmp al,0                          ; applies in the boot monitor
	je not_stand_alone                ; version. In the .exe version
                                          ; main() sets _alone = 1

;  Stuff only done in the standalone (boot monitor) version:

	mov ax,cs                         ; load segment registers
	add ax,010H                       ; compensate for org 100H 
	mov ds,ax                         ; since it appears that DOS
                                          ; points CS:00 at the entry point
					  ; rather than the start of the
                                          ; code/data segment
	mov es,ax

;  Unstack arguments passed by the boot program - needed by save command
;  These must be coordinated with hdbblk.asm

	pop ax
	mov save_disk,al
	pop ax
	mov save_size,al
	pop ax
	mov save_sec,al
	pop ax
	mov save_head,al
	pop ax
	mov save_cyl,al
	pop ax
	mov save_seg,al
	pop ax
	mov save_off,al

;  Set up our own stack

	mov ax,cs
	mov ss,ax

	mov ax,0FFFEH 
	mov sp,ax

	mov source,OFFSET DGROUP:_rcscript; point input source to rcscript memory 
	mov source_now,OFFSET DGROUP:_rcscript 


; make a primordial environment setting: boot drive number

	mov al,save_disk
	cmp al,81H
	jne _main_loop_jmp
	mov al,31H      ; digit 1 
	mov drivetail,al

_main_loop_jmp:
	
	jmp _main_loop                    ; give prompt and get rolling
	
; code that runs when monitor() is called by main
not_stand_alone:

	push bp                 ; save base pointer 
	mov ax,sp
	mov bp,ax

; save arguments passed on stack. Right now stack frame is so:
;
;                      null word
;                      env pointers (one 16 bit word each)
;                      null word 
;                      argument pointers (one word each)
;                      argc  
;                      saved bp  <--- sp
;
	mov ax,  [bp + 2]          ; store argc
	mov argc, ax
	mov ax,  [bp + 4]          ; store argv	
	mov argv, ax
	mov si,6                   ; offset on stack of next arg ptr

cnt_args_loop:                     ; see how many args before the null ptr

	mov ax,  [bp + si]         ; store next ptr in ax
	cmp ax,0                   ; is it NULL?
	jz cnt_args_done           ; yes, found end of argv
	add si,2                   ; no, increase count by 2 bytes
	jmp cnt_args_loop          ; and restart loop
	
cnt_args_done:

	add si,2                   ; skip over the null pointer
	lea ax, [bp+si]            ; store char **envp
	mov envp, ax

	mov di,0                   ; initialize counter
	mov environment[di], 00H   ; initialize with zero pointer

	mov ax,OFFSET DGROUP:my_data      ; Initialize end of data 
	mov enddata,ax             ; to start of data area

	mov al,_copy_env           ; see whether the user wants
	cmp al,00H                 ; us to start with a copy of OS env
	je zero_env_loop           ; No, so skip next part

	mov envc,00H               ; zero count of env strings

cpy_env_loop:			   ; store environment pointers 

	mov ax, [bp + si]          ; check for null pointer on stack
	cmp ax,0                   ; that signals end of environment
	je copy_env_done           ; string pointers: quit loop if found

	push [bp + si]             ; next env string ptr as source
	push enddata               ; end of data area as destination
	call my_strcpy             ; and copy env string to data area
	add sp,4                   ; clean stack from copy call
	mov bx, enddata            ; set next env string ptr
	mov environment[di], bx 
	cmp ax,00H                 ; check for zero length string
	je copy_env_done           ; quit loop if found
	add di,2                   ; index of next env ptr for copied env
	add si,2                   ; offset on stack of next env string to copy
	add enddata,ax             ; point to where next string will go
	inc envc                   ; increase count of env strings
	jmp cpy_env_loop           ; back to start of copy loop

zero_env_loop:                      

	; Make sure remaining env pointers are NULL. Not sure this is needed
        ; (not needed if bss is zeroed, but can we rely on this?)
	
	add di,2                   ; point at next slot
	cmp di, 2*ENV_SIZ          ; have we filled up the array
	jae copy_env_done          ; 
	mov environment[di], 00H   ; initialize with zero pointer
	jmp zero_env_loop
 
copy_env_done:

; When monitor starts up it will read commands from a "startup script". These
; commands are copied into memory from a file named on the command line in
; the non-standalone version of monitor.   
; If source = 0 then monitor reads from the terminal

	mov source,OFFSET DGROUP:_rcscript; point input source to rcscript memory 
	mov source_now,OFFSET DGROUP:_rcscript 

; start of main loop: display the prompt

_main_loop:

	mov ax,source         ; don't give prompt if input is read 
	cmp ax,00H            ; from memory, as this is non-interactive use
	jne skip_prompt

	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

	PRINT_STRING prompt

skip_prompt:

; While the user is dazzled by the prompt, zero out the line buffers
; BUG: is this really needed?

	mov al,00H              ; null char to move in al
	mov cx,BUF_SIZ          ; initialize counter
	mov di,OFFSET DGROUP:_linebuf1 ; point at start of _linebuf1
	cld
rep     stosb                   ; copy al to [di]
	mov cx,BUF_SIZ          ; repeat for linebuf2
	mov di,OFFSET DGROUP:linebuf2
rep     stosb
 

; read a line from terminal or from memory

	mov ax,source           ; source for read into ax
	cmp ax,00H              ; if it is zero, then
 	je read_terminal        ; read from terminal
	
	mov ax, OFFSET DGROUP:_linebuf1            ; otherwise call read_mem routine (below)
	push ax
	call read_mem            ; to read from memory
	add sp,2	         ; clean up from call
	jmp read_terminal_done

read_terminal:

	mov ax, OFFSET DGROUP:_linebuf1
	READ_LINE ax        ; read from canonical mode terminal

read_terminal_done:

	cmp ax,00H             ; actual bytes read returned in ax
	jne properly_terminate   
	
; 0 bytes read signals EOF (^Z). When read from a script, returns control to
; the terminal. When read from the terminal, exits the program.

	mov ax,source
	cmp ax,00H
	jne stay_in_game
	jmp _exit

stay_in_game:

	mov  source,00H  ; signal that input will now be from terminal
	jmp _main_loop

properly_terminate:


; properly terminate string read into _linebuf1
	cmp ax,2
	ja _something_there          
	jmp _main_loop         ; nothing on line but \n\r
_something_there:
	mov si,ax              ; number of bytes read in si
	mov _linebuf1[si],00H

; see if the first character is a # (comment character)    
	mov al,_linebuf1
	cmp al,COMMENT_CHAR      ; 0x23 is # sign
	jne not_a_comment_char
	jmp _main_loop            ; skip line since it is a comment

not_a_comment_char:

; see if the first character is : (colon character) since then this
; line is a label and should be skipped
	cmp al,3AH              ; 0x3A is : sign
	jne not_a_colon
	jmp _main_loop

not_a_colon:

; Figure out the first word on the line (in _linebuf1)
	mov si,00H             ; initialize index

first_arg_loop:

	mov al,_linebuf1[si] ; See if current char value is > 20H
	cmp al,20H              ; i.e. not whitspace
	jbe first_arg_found     ; first whitspace so end of arg found
	inc si                 ; not whitspace, so go to next char
	jmp first_arg_loop  

first_arg_found:

	; Properly terminate first argument by inserting null char in buffer
	mov al,00H
	mov _linebuf1[si], al 
	inc si 

; Make a processed copy of cmd_tail in linebuf2. The processing takes care
; of variable expansion/escaping and separates each resulting arg by a
; null character in linebuf2

	xor di,di             ; Zero destination index. Source is ongoing
	xor cx,cx             ; Prepare to use cx as escape flag
	xor dx,dx             ; Prepare to use dx as whitespace flag
	xor bx,bx             ; Prepare to use bx as comment flag

expand_line:    

	mov al,_linebuf1[si]     ; next char of cmd_tail
	cmp al,00H             ; is it null ?
	jne line_expand_not_done
	jmp line_expand_done     ; all done if so
line_expand_not_done:
	cmp al,0DH              ; is it a NL ?
	jne not_newline         ; no, so do not-NL stuff
	xor bx,bx               ; it's a new line, cancel pending comment
	cmp cx,01H            ; is there a pending escape?
	jne not_add_to_linebuf
	jmp add_to_linebuf       ; yes, there is, so continuation line
not_add_to_linebuf:
	jmp line_expand_done    ; all done if not escaped newline char

not_newline:

 	cmp bx,00H
	jne comment
	cmp cx, 01H           ; Is there a pending escape?
	je line_expand_copy_char; Yes, so just copy character literally
	cmp al,COMMENT_CHAR     ; Is it the # sign ?
	jne no_comment
	mov bx,01H            ; turn on comment flag
	jmp comment

no_comment:

	cmp al,ESCAPE_CHAR      ; See if the new char is the escape char
	jne escape_jumparound   ; It is not, so skip over next steps
	mov cx, 01H           ; it is escape char, so mark escape flag
	inc si                 ; increase copy index to skip copying char 
	jmp expand_line         ; and fetch next char

comment:
	inc si
	jmp expand_line

escape_jumparound:

	cmp al,20H             ; Is the current char whitspace?
	ja not_ws;              ; not whitspace
				; now it must be whitepace
	cmp dx,01H            ; Was the previous char whitspace?
        jne new_ws		; No, so mark and copy a null in its stead
	inc si                 ; yes, so skip copy fetch
	jmp expand_line         ; next char
	
new_ws:
	mov dx,01H            ; mark that it was whitspace
	mov al,00H             ; and replace it with a null in the copy
	jmp line_expand_copy_char

not_ws:
	xor dx,dx             ; flag as not whitspace

	cmp al,VARIABLE_CHAR   ; Is it the $ char ?
	je substitution        ; If so, jump to code that substitutes env var

line_expand_copy_char:

	mov linebuf2[di],al ; copy current char to linebuf2
	xor cx,cx             ; cancel any pending escape
	inc si                 ; increment indices 
	inc di                 ; and
	jmp expand_line         ; back to start of copy loop

substitution:                   ; copy value of env var instead of name
	
	push di                ; save di on stack
	xor di,di             ; so we can use as a local index into name

subs_var_name_loop:             ; put everything from here to next ws into
		                ; expandbuf as env string name

	inc si
	mov al,_linebuf1[si] ; next char from input buffer 
	cmp al,20H             ; is it white space
	jbe subs_var_name_fnd   ; if so complete name now in expandbuf
	mov expandbuf[di],al; else add char to expandbuf
	inc di                 ; increment index for next char
	jmp subs_var_name_loop

subs_var_name_fnd:
	mov expandbuf[di], 00H  ; properly terminate name string
	pop di                           ; restore index into copy area
	push si                          ; save si on stack
	push bx
	mov ax, OFFSET DGROUP:expandbuf            ; prepare for call
	push ax
	call mygetenv                     ; see if name=foo is in env
	add sp,2                         ; clean up from call
	cmp ax,00H                      ; null returned if not found
	je subs_not_found                      ; so copy nothing
	mov bx,ax                       ; else save address of foo 
	xor si,si                       ; prepare counter for copy of foo

subs_copy_value:           ; copy value of environment string into output

	mov al,[bx + si] ; fetch next char of foo
	cmp al,00H        ; if it is null we have reached the end
	je subs_done
	mov linebuf2[di],al           ; else copy new byte into position
	inc si                           ; increment both counters 
	inc di
	jmp subs_copy_value               ; and restart copy loop

subs_done:
	pop bx
	pop si                           ; restore index into input string
	jmp new_ws                        ; pretend we have just gotten new ws

subs_not_found:
	pop bx
	pop si
	inc si
	jmp expand_line

add_to_linebuf:                 ; escaped newline, so get more input from user
	xor cx,cx             ; cancel escape that got us here
	mov linebuf2[di],0DH  ; copy literal newline to destination
	inc di                 ; increment indices
	mov linebuf2[di],0AH
	inc di
	mov ax, OFFSET DGROUP:_linebuf1  
	add ax,si
;	lea ax,_linebuf1[si]  ; store pointer to where new input goes
	mov my_ptr,ax
	mov ax,source
	cmp ax,00H
	je add_from_terminal
	push my_ptr
	call read_mem
	add sp,2
	jmp add_read_done

add_from_terminal:

	mov ax,my_ptr
	READ_LINE ax      ; returns with number of bytes read in ax

add_read_done:
	push bx
	mov bx,ax                   ; Cannot use ax as index in MASM
	mov _linebuf1[bx + si], 00H ; properly terminate string 
	pop bx
	jmp expand_line
	

line_expand_done:
	mov linebuf2[di],00H  ; make sure we properly terminate

; set pointers in lineargs array to null terminated strings in linebuf2
; the address of lineargs to be passed to commands, both internal and external,
; as char **argv

	mov si,00H
	mov di,00H
	mov cx,00H

find_all_args:

	mov al,linebuf2[si]     ; next char into al
	cmp al,00H                 ; if it is null, then we are
	je all_args_found           ; at end of buffer: no more args
	lea bx,linebuf2[si]    ; set current char address as ptr to 
	mov lineargs[di], bx   ; next line argument. Use bx to save
                                    ; char in al

plow_through:                       ; plow through the next arg 

	inc si                     ; increment index
	mov al,linebuf2[si]     ; next char into al
	cmp al,00H	            ; is it null? 
	je next_arg_found           ; yes, so branch to properly end new arg
	jmp plow_through            ; back to beginning of plow through loop

next_arg_found:

	inc cx                     ; increment arg count
	add di,2                   ; increment pointer index
	inc si                     ; and char counter
	jmp find_all_args           ; and back to loop start for remaining args

all_args_found:
	inc si
	mov linebuf2[si],00H    ; mark end of args in linebuf2
	mov nargs,cx             ; record how many args were found
	 
; Fall through blocks of code, one for each internal function. 
; Match arg0 against the name of each
; function. If a match, call the command implemented in internal.a and start
; new loop. If no match, print a helpful message and start a new prompt loop


	mov ax,source
	cmp ax,00H
	jne dont_skip_keywords
	jmp skip_keywords            ; skip over programming keywords when
	                            ; used interactively (source = 0). 
dont_skip_keywords:
	lea ax, if_name                ; see whether this is an if construct
	push ax
	mov ax, OFFSET DGROUP:_linebuf1               ; by calling strcmp with the name "if"
	push ax
	call my_strcmp              ; and argv[0]
	add sp,4                   ; clean up from call
	cmp ax,0                   ; do the comparison
	jne goto_section            ; not an if construct, so ...
	mov ax,lineargs
	push ax                     ; foo, in if foo bar
	call mygetenv               ; see if foo=* in env
	add sp,2                   ; clean up from call
	cmp ax,00H                ; if not, then restart the main loop
	jne dont_do_main_loop
	jmp _main_loop      
dont_do_main_loop:
	push ax                    ; value bar if foo=bar in env
	mov ax,lineargs[2]
	push ax
	call my_strcmp
	add sp,4
	cmp ax,00H
	je dont_do_main_loop2
	jmp _main_loop      
dont_do_main_loop2:
	mov ax,lineargs[4]
	push ax
	mov ax, OFFSET DGROUP:_linebuf1        ; where to copy belch as new argv[0]
	push ax
	call my_strcpy              ; do the copy
	add sp,4                   ; clean up from copy call
	mov cx,nargs             ; reduce number of args by 3
	sub cx,3                   ; we drop if foo bar 
	mov nargs,cx        
	xor si,si                 ; initialize index for shift args loop

shift_args:  ; move belch ... into position as if that had been the command

	cmp cx,00H               ; all args counted ?
	jbe goto_section           ; yes, so continue on to look for belch
	add si,6
	mov ax,lineargs[si] ; next arg to get moved
	sub si,6
	mov lineargs[si],ax
	dec cx
	add si,2
	jmp shift_args
	
goto_section:

	lea ax, goto_name              ; see whether this is a goto construct
	push ax
	mov ax, OFFSET DGROUP:_linebuf1               ; by calling strcmp with the name "goto"
	push ax
	call my_strcmp              ; and argv[0]
	add sp,4                   ; clean up from call
	cmp ax,0                   ; do the comparison
	jne skip_keywords           ; not an if construct, so ...
	mov ax, lineargs       ; copy goto label (sans colon) to buffer 
	push ax
	mov ax, OFFSET DGROUP:_linebuf1
	inc ax
	push ax
	call my_strcpy              ; do the call
	add sp,4                   ; clean up from call
	mov _linebuf1,3AH    ; colon character on front of buffer
	mov ax, OFFSET DGROUP:_linebuf1               ; push label after goto as arg for
	push ax
	push source         ; strstr. Also push script start
	call my_strstr              ; search for label in script
	add sp,4                   ; clean up from call
	cmp ax,00H                ; if not found do nothing
	je skip_keywords
	mov source_now,ax        ; else branch by switching next read
	jmp _main_loop              ; to label location
	
skip_keywords:

;;;; set section  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	lea ax, set_name
	push ax                  ; stack args to my_strcmp
	lea ax, _linebuf1
	push ax
	call my_strcmp           ; call string compare routine
	add sp,4                 ; clean up from call
	cmp ax,0                 ; is arg0 equal to name of set command? 
	jne read_section         ; if not, go on to check for next command
	mov ax, OFFSET DGROUP:lineargs            ; char **argv      
	push ax
	push nargs               ; int argc      
	call _set                ; call set command (internal.a)
	add sp,4                 ; clean up from call
	jmp _main_loop           ; return to beginning of main loop

;;;;; read section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
read_section:

	lea ax, read_name
	push ax
	lea ax, _linebuf1
	push ax
	call my_strcmp           ; call string compare routine
	add sp,4
	cmp ax,0                ; is arg0 equal to name of read command?
	jne help_section         ; no, so go on to check for next command
	mov ax,lineargs
	push ax    ; stack one (and only) char *arg for read
	call _read               ; make the call
	add sp,2                ; clean up from call
	jmp _main_loop           ; return to beginning of main loop

;;;;; help section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 
help_section:  

	lea ax, help_name           ; stack args to help
	push ax
	lea ax, _linebuf1
	push ax
	call my_strcmp           ; call string compare routine
	add sp,4                ; clean up from call
	cmp ax,0                ; is arg0 equal to name of help command? 
	jnz  shift_section         ; if not, go on to check for next command
	mov ax,lineargs
	push ax
	mov ax, nargs
	push ax
	call _help               ; call help command (internal.a)
	add sp,4                ; clean up from call
	jmp _main_loop           ; return to beginning of main loop

;;;;;;; shift section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this is the only internal command implemented in line here
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
shift_section: 

	lea ax, shift_name          ; call string compare routine
	push ax
	lea ax, _linebuf1
	push ax
	call my_strcmp
	add sp,4                ; clean up from call
	cmp ax,0                ; is arg0 equal to name of shift command?
	jne echo_section         ; no, so look at next command

;copy current args to _linebuf1, and jump back into main loop at point
;just after getting user input- remember to set si to 0 for that jump

	xor si,si              ; initialize copy index	

copy_args_back_loop:
	
	mov al,linebuf2[si]  ; next char from source
	jnz nonzero_last_char    ; we detect two nulls in a row by seeing
                                 ; if the zero flag is still set from the
                                 ; previous loop interation. This is delicate
                                 ; because many instructions (not mov or jmp)
                                 ; affect the zero flag (inc does!)
	cmp al,00H              ; if flag already set then a second null
	jz copy_args_back_done   ; means we are done

nonzero_last_char:               ; the last char was not a null

	inc si
	cmp al,00H              ; Are we at end of an arg?
	jnz copy_char_back
	mov al,20H              ; replace null with space char

copy_char_back:

	mov _linebuf1[si-1], al ; copy char to line buf 1	
	jmp copy_args_back_loop           ; and restart copy loop

copy_args_back_done:

	mov _linebuf1[si],00H ; properly terminate _linebuf1
	xor si,si                  ; prepare for rentry to main loop	
	jmp first_arg_loop           ; rejoin main loop after user input

;;;;;;; echo section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

echo_section:

	lea ax, echo_name
	push ax           ; stack args to set
	lea ax, _linebuf1            
	push ax
	call my_strcmp           ; call string compare routine
	add sp,4                ; clean up from call
	cmp ax,0                ; is arg0 equal to name of help command? 
	jne status_section       ; if not, go on to check for next command
	mov ax, OFFSET DGROUP:lineargs      ; char **argv      
	push ax
	push nargs       ; argc      
	call _echo               ; call echo command (internal.a)
	add sp,4                ; clean up from call
	jmp _main_loop           ; return to beginning of main loop

;;;;;; status section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

status_section:

	lea ax, status_name
	push ax
	lea ax, _linebuf1            
	push ax
	call my_strcmp           ; call string compare routine
	add sp,4                ; clean up from call
	cmp ax,0                ; is arg0 equal to name of status command? 
	jne exit_section     ; if not, go on to check for next command
	call _status             ; call status command (internal.a)
	jmp _main_loop           ; return to beginning of main loop

exit_section:
	lea ax, exit_name
	push ax
	lea ax, _linebuf1
	push ax
	call my_strcmp
	add sp,4
	cmp ax,0
	jnz external_section
	jmp _exit

;;;;;; external commands section ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

external_section:

; Search  through names of external commands (implemented in external.c) 

	mov si, 00H            ; initialize index
	mov cx, 00H            ; initialize counter
	
external_search:

	cmp cx,_n_externals    ; have we reached end of externals array?
	jae scripts_section      ; yes, so go to next section
	push _external_names[si] ; load name of next external cmd
	lea ax, _linebuf1            ; and use strcmp to see if it agrees with
	push ax
	call my_strcmp           ; the user's first argument
	add sp,4                ; clean up from call
	cmp ax,0                ; Have we found a match?
	je found_external        ; so branch to code that makes the call
	add si,2                ; no, so increment pointer index and
	inc cx;
 	jmp external_search      ; and look at name of next external command

found_external:                  ; actually call the matching external command
	mov ax, OFFSET DGROUP:environment         ; push its char **envp
	push ax
	mov ax, OFFSET DGROUP:lineargs            ; push its char **argv
	push ax
	push nargs       ; push its int argc
	call _external_entry_points[si] ; call entry point of command
	add sp,6               ; clean up from call
	jmp _main_loop           ; and restart main prompt loop`

scripts_section:    ; if all else fails, see if the command is a script
                    ; i.e., there is an env variable by this name

	lea ax, _linebuf1
	push ax
	call mygetenv 
	add sp,2
	cmp ax,00H
	je not_found_section
	mov source,ax
	mov source_now,ax
	jmp _main_loop

not_found_section:

	mov ax,source         ; in non-interactive use we do not print
	cmp ax,00H             ; the helpful message
	je dont_do_main_loop3
	jmp _main_loop  
dont_do_main_loop3:

	PRINT_STRING msg0        ; default action: print helpful message
	jmp _main_loop           ; return to beginning of main loop

_exit:
	PRINT_CHAR 0AH           ; Get shell prompt back on a new line
	PRINT_CHAR 0DH
	
	pop bp
	; Terminate program with appropriate system call
	TERMINATE 0

;  Auxiliary routine
;  read bytes from source_now pointer and copy to string ptr arg,
;  up to and including first NEW_LINE. Return number of bytes read
;  in ax, and position source_now for next read

read_mem: 
	push bp
	mov bp,sp
	push dx            ; save reg
	mov dx,4[bp]   ; save char *arg       
	mov bp,dx      ; reuse bp as base of memory to be read
	push bx 
	push si            ; save all regs
	xor si,si         ; initialize char counter
	mov bx,source_now

read_mem_copy_loop:

	mov al,[bx + si] ; copy next byte from source
	cmp al,00H           ; Is it a null ?
	je read_mem_nl        ; Yes, so branch to prepare to return
	mov DS:[bp + si],al    ; copy char to output buffer
	inc si               ; increment counter
	cmp al,0AH            ; Was it a newline ?
	je read_mem_nl        ; Yes, so branch to prepare to return
	jmp read_mem_copy_loop; otherwise continue copy loop

read_mem_nl:

	add bx,si          ; position pointer
	mov source_now,bx     ; for next read
	mov ax,si          ; return number of chars copied in ax
	pop si              ; restore regs used
	pop bx
	pop dx
	pop bp
	ret	

;;;;;;;;;;;;;;;;;;;;;;;;;;; End of Code ;;;;;;;;;;;;;;;;;;;;;;;;;;; 

_TEXT ENDS

;;;;;;;;;;;;;;;;;;;;;;;;;; Initialized Data ;;;;;;;;;;;;;;;;;;;;;;;
_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP

	_rcscript  DB   "info",0DH,0AH,"menu",0DH,0AH,"set BOOT_DRIVE 8"   
	drivetail  db   "0"
	aitch      db   "H",00H
	prompt     DB PROMPT_STRING,00H
	newline    DB 0DH,0AH     ; space used by PRINT_CHAR (not always NL)
	msg0       DB "Enter help for a list of commands, ^Z to exit",0DH,0AH,00H
	_alone     DB 1           ; = 1 for standalone mode ("boot monitor")
	_copy_env  DB 0           ; -e flag set
	envc       DW ?           ; number of env strings
	source     DW 0           ; =0 for stdin, memory reference else
	source_now DW 0           ; moved as source is read
	enddata    DW OFFSET DGROUP:my_data  ; ptr to start of free data area
_DATA ENDS
;;;;;;;;;;;;;;;;;;;;;;;;; End of Initialized Data ;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;; Uninitialized Data ;;;;;;;;;;;;;;;;;;;;;
_BSS SEGMENT PUBLIC 'BSS'
ASSUME DS:DGROUP
	argc        DW ?                     ; 32 bit int for argc
	argv        DW ?                     ; ptr to first argument pointer
	envp        DW ?                     ; ptr to environment pointers 
	nargs       DW ?                     ; number of args from user  
	my_ptr      DW ?                     ; pointer to whatever
	expandbuf   DB BUF_SIZ DUP(?)
	_linebuf1   DB BUF_SIZ DUP(?)        ; buffer area to read into
	linebuf2    DB BUF_SIZ DUP(?)        ; extra storage for parsing, etc
	lineargs    DW BUF_SIZ DUP(?)        ; ptrs to parsed line strings
	environment DW ENV_SIZ DUP(?)        ; environment ptr storage
	my_data     DB ? 

_BSS ENDS

END

