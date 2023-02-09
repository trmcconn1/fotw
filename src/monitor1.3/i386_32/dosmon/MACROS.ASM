
BUF_SIZ EQU 512
ENV_SIZ EQU 512
SCRIPT_SIZ EQU 4096
PROMPT_STRING EQU "Monitor>"
ESCAPE_CHAR EQU 05EH
COMMENT_CHAR EQU 023H
VARIABLE_CHAR EQU 024H
HELP_COLS EQU 3

; If the bios is used to print strings to the console, the following sets
; the attribute, with high nibble = rgb value of background, low nibble =
; rgb value of foreground. I like white on blue background
; Standard white on black is 07H

CHAR_ATTR EQU 17H     

;; Note: all system calls should be called from inside macros, for portability

; Every system call is implemented in a separate macro. There are two 
; implementations of each one - one using only BIOS interrupts so that
; monitor could be run in the absence of an operating system, say, as a 
; boot monitor. This is selected by defining BIOS on the assember command
; line. Otherwise, DOS int21H system calls are used

; BE VERY CAREFUL USING REGISTERS AS MACRO ARGUMENTS!!!

IFDEF BIOS

; Read a line of text from the terminal using Bios 00H int 16H service
; Leaves number of characters read in ax. 

READ_LINE MACRO mybuf
LOCAL loop,done,do_backsp
	push si
	push bx
	push bp
	mov ax,mybuf
	mov bp,ax
	mov ah,0FH
        int 10H
        mov bl,CHAR_ATTR
	xor si,si
	jmp loop

do_backsp:

	cmp si,0
	jbe loop
	dec si
	mov ah,0EH
	int 10H
	mov al,20H
	int 10H
	mov al,08H
	int 10H

loop:
	mov ax,00H
	int 16H
	cmp al,1AH
	je done
	cmp al,08H
	je do_backsp 
	mov BYTE PTR DS:[bp+si],al
	inc si
        mov ah,0EH
        int 10H
	cmp al,0DH
	jne loop

done:

	mov al,0AH
	mov BYTE PTR DS:[bp+si],al
	mov ah,0EH
	int 10H
	inc si
	mov ax,si
	pop bp
	pop bx
	pop si
ENDM

; Print a single character to the terminal using int 10H service 0EH

PRINT_CHAR MACRO mychar 
		     push ax
	             push bx
		     mov ah,0FH
		     int 10H
	             mov bl,CHAR_ATTR
		     mov ah,0EH
	             mov al,mychar
	             int 10H
		     pop bx
	             pop ax
ENDM

; Macro: use BIOS 10H service to write a string to the screen
; string must be null terminated

PRINT_STRING MACRO mystring 
	push ax
	push bx
	push cx
	push dx
	push es
	push bp
	mov ah,0FH
	int 10H
	mov ah,03H
	int 10H
	mov ax,OFFSET DGROUP:mystring
	push ax
	call my_strlen
	add sp,2
	mov cx,ax
	mov ax,ds
	mov es,ax
	mov ax,OFFSET DGROUP:mystring
	mov bp,ax
	mov bl,CHAR_ATTR
	mov al,01H
	mov ah,13H
	int 10H
	pop bp
	pop es
	pop dx
	pop cx
	pop bx
	pop ax
ENDM

; A version of the previous using a register passed as argument

PRINT_PTR MACRO my_ptr 
	push ax
	push bx
	push cx
	push dx
	push es
	push bp
	mov ah,0FH
	int 10H
	mov ah,03H
	int 10H
	mov ax,my_ptr
	push ax
	call my_strlen
	add sp,2
	mov cx,ax
	mov ax,ds
	mov es,ax
	mov ax,my_ptr
	mov bp,ax
	mov bl,CHAR_ATTR
	mov al,01H
	mov ah,13H
	int 10H
	pop bp
	pop es
	pop dx
	pop cx
	pop bx
	pop ax
ENDM

; Try to return to whatever might have called us. For a true boot monitor
; we might want to try int 19H, which does a warm reboot

TERMINATE MACRO rval
	mov al,rval	      
	ret
	;int 19H   
ENDM

; SBRK  return in ax the number of K of total memory as returned
; by the BIOS. It is not clear this is meaningful   

SBRK MACRO  
	int 12H
ENDM

ELSE

; Macro:  write a single char to stdout 

PRINT_CHAR MACRO mychar 
		     push ax
		     push dx
		     mov dl,mychar
		     mov ah,02H
		     int 21H
		     pop dx
                     pop ax
ENDM 

; Macro: use DOS 21H AH = 40H call to write a string to stdout  
; string must be null terminated

PRINT_STRING MACRO mystring 
	push ax
	push bx
	push cx
	push dx
	mov ax,OFFSET DGROUP:mystring
	push ax
	call my_strlen
	add sp,2
	mov cx,ax
	mov bx,1
	mov dx,OFFSET DGROUP:mystring
	mov ah,40H
	int 21H
	pop dx
	pop cx
	pop bx
	pop ax
ENDM

PRINT_PTR MACRO my_ptr 
	push ax
	push bx
	push cx
	push dx
	mov ax,my_ptr
	push ax
	call my_strlen
	add sp,2
	mov cx,ax
	mov bx,1
	mov dx,my_ptr
	mov ah,40H
	int 21H
	pop dx
	pop cx
	pop bx
	pop ax
ENDM

TERMINATE MACRO rval
	mov al,rval	      
	mov ah,4CH      
	int 21H   
ENDM

;; SBRK  return in ax value of current BRK   
SBRK MACRO  
	push bx
	push es
	mov ax,ds
	mov es,ax
	mov bx,0FFFFH
	mov ah,4AH
	int 21H
	mov ax,bx
	pop es
	pop bx
ENDM

READ_LINE MACRO mybuf

	push bx
	push cx
	push dx
	mov bx,0
	mov cx,BUF_SIZ
	mov dx, mybuf
	mov ah,3FH                
	int 21H                 
	pop dx
	pop cx
	pop bx
ENDM

	 
ENDIF
