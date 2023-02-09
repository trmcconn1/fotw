;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scroll: scrolls window defined below up or down dist rows.
; Calling sequence: void far scroll(char direction, char dist,char toprow)
; Uses BIOS int10h functins 06H and 07H
; Assumption: this routine is called while cursor is positioned within
; window to be scrolled
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Dimensions of scroll region
bottomrow equ 23
firstcol equ 0
lastcol equ 79

PUBLIC _scroll

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME cs:_TEXT,ds:_TEXT


function db 06H   ; storage for direction parameter and
distance db 01H   ; number of rows to scroll parameter
toprow db 02H


_scroll:   ; beginning of routine
        push ds
        push bp  ; setup stack frame 
	mov bp,sp
        mov ax,_TEXT ; we need to access function
        mov ds,ax
;
;  UNLOAD PARAMETERS FROM THE STACK
;
        mov al,[bp+8]       ; get parameter from C program (8 bit char)
        mov function,al     ; store scroll direction
        mov al,[bp+10]       ; store scroll distance
        mov distance,al
        mov al,[bp+12]
        mov toprow,al


; SAVE REGISTERS
        push bx
        push cx
        push dx

; Get Current display page in BH
        mov ah,0FH
        int 10H             ; service returns video status in regs

; Get Current screen attributes in ah
        mov ah,08H
        int 10H             ; Read character/attribute from screen
                            ; attribute stored in ah

; Prepare regs for scroll service
        mov bh,ah           ; attribute in bh
        mov ah,function     ; scroll up or down ?
        mov al,distance         ; scroll distance
        mov ch,toprow
        mov cl,firstcol
        mov dh,bottomrow
        mov dl, lastcol
        int 10H             ; do scroll

; Restore regs
        pop dx
        pop cx
        pop bx
        pop bp
        pop ds
        retf



_TEXT ends

sseg SEGMENT STACK 'DATA'
        dw 80 dup (?)
sseg ends

end 
