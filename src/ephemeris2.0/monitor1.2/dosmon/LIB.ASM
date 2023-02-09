NAME mylib
TITLE mylib

; Library routines for monitor. We could just link to stdlib.o, but where's
; the fun in that?  All routines assume the stack top has the return address
; (32 bit) with arguments pushed in reverse order above it. All routines
; save all registers used except for eax. Standard lib names are changed
; in case we include c code that links to stdlib
 
INCLUDE macros.asm

PUBLIC my_strlen,my_strcpy,my_strcmp,my_strncmp,_print_i,mygetenv,my_strstr,my_atoi
EXTRN environment:WORD,envc:WORD

DGROUP GROUP _DATA,_BSS

_TEXT SEGMENT PUBLIC 'CODE' 
ASSUME CS:_TEXT,DS:DGROUP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; strlen (char *string) - return via ax the number of characters in the    ;
;                         null terminated string. Address of the first char ;
;                         of string has been pushed before the call         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

my_strlen:

	push bp
	mov bp,sp
	push bx
	push si
	mov si,0 	        ; initialize count
	mov bx,4[bp]            ; point at first char of string

strlen_loop:                    ; entry point of loop to scan string

	mov al,[bx + si]        ; Is the next byte of string
	cmp al,0                ; equal to a zero byte (NULL)  ?
	jz strlen_ret           ; yes it is, so exit scanning loop
	inc si                  ; no, so increment counter and
	jmp strlen_loop         ; begin next loop iteration

strlen_ret:                     ; label for begin return to caller 

	mov ax,si             ; return char count in ax
	pop si
	pop bx
	pop bp
	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  strcpy( char *dest, char *src )                                     ;
;                                                                      ;
;  Unlike the C std lib routine, this one returns the number of bytes  ;
;  copied through ax                                                  ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
my_strcpy: 

	push bp
	mov bp,sp
	push dx                ; save registers used
	push bx
	push si
	mov si,0    		; initialize count
	mov dx,4[bp]        ; point at first char of dest string
	mov bx,6[bp]        ; point at first char of src string
	mov bp,dx


	cmp bx,0
	jz strcpy_ret           ; Null ptr to src string

strcpy_loop:                    ; entry point of loop to scan string

	mov al,[bx + si]      ; copy next byte of src 
	mov DS:[bp + si],al      ; to dest. 
	inc si                 ; add 1 to count of bytes copied
	cmp al,0                ; Is byte equal to a zero byte (NULL)  ?
	jz strcpy_ret           ; yes it is, so exit scanning loop
	jmp strcpy_loop         ; begin next loop iteration

strcpy_ret:                     ; label for begin return to caller 

	mov ax,si               ; return char count in ax
	pop si
	pop bx
	pop dx
	pop bp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; my_atoi (char *string) - return via eax the integer value of the given     ; 
; string of decimal digits.   Address of the first char                   ; 
; of string has been pushed before the call i                               ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

my_atoi:

	push bp
	mov bp,sp
	push bx
	push cx                ; holds raw digits read 
	push dx                ; holds base or radix (10)
	push si
	xor si,si    	       ; initialize index
	mov ax,4[bp]
	mov bx,ax              ; point bx at base address of string
	xor ax,ax              ; zero out accumulator. result will accrue
			       ; in ax
	mov dx,10              ; store radix (multiplier) in dx

myatoi_loop:                   ; entry point of loop to scan string

	mov cl,[bx + si]        ; Is the next byte of string
	cmp cl,0                ; equal to a zero byte (NULL)  ?
	jz myatoi_ret           ; yes it is, so exit scanning loop
	                        ; no, so shift and add next nigit to total
	inc si                  ; increment counter 
	mul dl                  ; mutiply ret val by 10 (shift)
	sub cl,48               ; ascii -> digit value: subtract 48
	add al,cl               ; add value of next digit
	jmp myatoi_loop         ; begin next loop iteration

myatoi_ret:                     ; label for begin return to caller 

	                        ; return value in eax
	pop si
	pop dx
	pop cx
	pop bx
	pop bp
	ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; strcmp (char *s, char *t) - return via ax 0 if s and t are identical,  ;
;                             -1 if s < t and 1 if s > t                  ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

my_strcmp:

	push bp
	mov bp,sp
	push bx
	push si
	mov bx,[bp+4]           ; addr of first char of t --> bx
	mov ax,[bp+6]           ; addr of first char of s --> bp
	mov bp,ax
	mov si,0      		; initialize counter

strcmp_loop:                    ; loop to scan strings

	mov al,[bx + si]        ; How does the next byte of s
	mov ah,DS:[bp + si]     ; compare with the 
	cmp ah,al               ; the next byte of t?
	jb strcmp_below         ; s < t so jump to -1 return 
	ja strcmp_above         ; s > t so jump to  1 return
	cmp ah,0                ; Have we reached end of s (hence also t)?
	jz strcmp_equal         ; If so then s = t
	inc si                  ; increment counter and
	jmp strcmp_loop         ; begin next loop iteration

strcmp_below:
	mov ax,0FFFFH           ; unsigned form of -1
	jmp strcmp_ret

strcmp_above:
	mov ax,1
	jmp strcmp_ret

strcmp_equal:
	mov ax,0

strcmp_ret:                     ; label for begin return to caller 

	pop si                 ; restore registers used
	pop bx
	pop bp
	ret
	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; strncmp (char *s, char *t, int n)                                       ;
; return via ax 0 if first n chars of s and t are identical,             ;     ; -1 if s < t and 1 if s > t                                              ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

my_strncmp:

	push bp
	mov bp,sp
	push bx
	push cx
	push dx
	push si
	mov cx,[bp+8]        ; initialize counter
	mov dx,[bp+6]        ; addr of first char of t --> dx
	mov bx,[bp+4]        ; addr of first char of s --> bx
	mov bp,dx

	xor si,si             ; zero out index

strncmp_loop:                    ; loop to scan strings

	cmp cx,0               ; Have we checked all necessary chars ?
	je strncmp_equal        ; if so then s = t

	mov al,[bx + si]      ; How does the next byte of s
	mov bx,dx
	mov ah,DS:[bp + si]      ; compare with the 
	cmp ah,al               ; the next byte of t?
	jb strncmp_below        ; s < t so jump to -1 return 
	ja strncmp_above        ; s > t so jump to  1 return
	cmp ah,0                ; Have we reached end of s (hence also t)?
	je strncmp_equal        ; If so then s = t
	inc si                 ; increment index 
	dec cx                 ; decrement counter
	jmp strncmp_loop        ; begin next loop iteration

strncmp_below:
	mov ax,0FFFFH      ; unsigned form of -1
	jmp strncmp_ret

strncmp_above:
	mov ax,1
	jmp strncmp_ret

strncmp_equal:
	mov ax,0

strncmp_ret:                    ; label for begin return to caller 

	pop si                 ; restore registers used
	pop dx
	pop cx
	pop bx
	pop bp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  char *mygetenv(char *): return pointer to bar when called with       ;
;  pointer to foo and foo=bar is in the environment. Return null pointer;
;  when foo is not defined in the environment                           ;
;  Also return in si the index of the found environment string         ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

mygetenv: 
	push bp
	mov bp,sp       ; set up stack frame

	push bx          ; preserve all registers used
	push cx
	push di

	mov cx,envc    ; load counter for search loop over environment
	xor si,si       ; initialize index to dword pointer

mygetenv_loop:

	cmp cx,00H      ; have we searched the whole environment yet?
	je mygetenv_nfd  ; jump to not found code if so

	mov bx,environment[si] ; load pointer to current env string in bx

; we need to find the byte offset of the "=" sign in the env string

	xor di,di       ; zero out byte counter for search for = sign

mygetenv_sch:             ; the = search loop start

	mov al,[bx + di]  ; fetch di-th byte
	cmp al,3DH        ; is it ascii code for = sign
	je mygetenv_efnd  ; yes, so break out loop with found bit offset in di 
	inc di            ; else increase bit offset and 
	jmp mygetenv_sch  ; back to start of loop

mygetenv_efnd:            ; we have found the = offset by di

	mov BYTE PTR [bx + di], 00H ; temporarily change = to null char	
	push bx           ; pointer to before = part of current env string 
	push [bp + 4]           ; pointer to foo string
	call my_strcmp     ; call compare strings 
	add sp,4          ; clean up from call
	mov BYTE PTR [bx + di], 3DH  ; yes, I did remember to put = back!
	cmp ax,0          ; did we find foo= in env?
	je mygetenv_fnd    ; yes, so break out of main loop with found result
	add si,2          ; else increment env string pointer offset
	dec cx            ; decrement environment counter
	jmp mygetenv_loop  ; and restart main search loop
	
mygetenv_fnd:              ; when found, return address of bar in ax
	inc di
	lea ax,[bx + di]; store address of bar part in ax 
        jmp mygetenv_rtr   ; and prepare to return

mygetenv_nfd:
	mov ax,00H       ; return zero address when not found

mygetenv_rtr:
			   ; prepare to return: restore registers
	pop di
	pop cx
	pop bx
	pop bp 

	ret                ; result in ax
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  print_i( unsigned ) convert 32 bit to ascii string and print it   ;
;  in decimal format                                                 ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 


_print_i: 
	
	push bp               ; set up stack frame
	mov bp,sp
	push bx
	push cx
	push dx

	mov bx,9

buffer_clear_loop:

	mov BYTE PTR my_buf[bx],020H 
	dec bx
	cmp bx,0
	je  buffer_clear_done
	jmp buffer_clear_loop

buffer_clear_done:
	
	mov bx,9              ; set up bx for byte position
        mov cx,10             ; divisor for div by 10   	
	mov ax, [bp + 4]     ; load divisor into ax

digit_loop:
	
	xor dx,dx            ; clear dx for remainder
	div cx                ; divide by 10
	add dl,48              ; convert remainder in dl into digit
	mov my_buf[bx],dl  ; copy digit into place
	dec bx                ; adjust byte position
	cmp bx,0
	je convert_done 
	cmp ax,0              ; see if anything left of binary number
	jne digit_loop         ; back to start of loop if so 
	

convert_done:

        PRINT_STRING my_buf

	pop dx
	pop cx
	pop bx
	pop bp
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;    char *my_strstr(char *haystack,char *needle): return pointer of first;
; occurrence of string needle in string haystack, or null if not found    ;
; return haystack if needle is null                                       ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

my_strstr:

	push bp                ; set up a stack frame 
	mov bp,sp
	push di                ; save registers used
	push bx
	push cx
	push dx

	xor ax,ax             ; default return value is null
	mov bx,[bp + 4]       ; store pointer to haystack in bx 
	mov dx,[bp + 6]      ; store pointer to needle in dx
	mov bp,dx
	cmp bx,00H            ; if haystack is null return null since 
	je my_strstr_ret        ; nothing can be found in a null haystack
	mov ax,bx             ; new default return value is haystack
	cmp dx,00H            ; if needle is null return haystack, since you
	je my_strstr_ret        ; will find nothing if you look for nothing
	
	xor di,di             ; initialize loop index

little_loop:                    ; this loops over needle
	mov cl,[bx+di]        ; load next byte of haystack in cl
	mov al,DS:[bp+di]        ; load next byte of needle in al
	cmp al,00H             ; if we have reached null at end of needle
	je my_strstr_found      ; then needle has been found!
	cmp cl,00H             ; if we have reached null at end of haystack
	je my_strstr_notfound   ; then needle has not been found
	cmp al,cl               ; does needle char = haystack char?
	jne big_loop            ; no, so move to next haystack char
	inc di                 ; yes, so move to next needle char
	jmp little_loop
big_loop:                      ; this loops over haystack
	inc bx                 ; move to next haystack char
	xor di,di             ; and reset needle index
	jmp little_loop	        ; restart loop over needle

my_strstr_found:

	mov ax,bx             ; bx points at character in haystack where
	jmp my_strstr_ret       ; needle matched, so return this value in ax

my_strstr_notfound:

	xor ax,ax             ; needle not found, so return null pointer

my_strstr_ret:

	pop dx                 ; restore registers used
	pop cx
	pop bx
	pop di
	pop bp 
	ret     ; Twizzle, Twazzle, Twuzzle, Twom. Time for this one to come
	        ; home   -- Mr. Wizard

_TEXT ENDS
_DATA SEGMENT PUBLIC 'DATA'
ASSUME DS:DGROUP
my_buf db "          ",00H      ; leave room for up to 10 digits
_DATA ENDS
_BSS SEGMENT PUBLIC 'BSS'
_BSS ENDS
END
