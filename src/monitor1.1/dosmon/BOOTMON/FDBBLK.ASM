;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Floppy boot loader for monitor program
;
;   The monitor program is assumed to be about 17k beginning at offset 1024
;   bytes from the beginning of the floppy drive. This loader copies the
;   program into memory at offset 1024 (This leaves two 512 byte blocks below
;   it in memory. The first is the vector table and the second is the BIOS
;   data area.) Monitor is called by a far jump (actually a far return) to
;   0DFH:100H. 1024 is written this way so that the monitor can be called
;   the same way DOS would call a .com: with CS pointing to the start of the
;   PSP, and the entry point at offset 100H
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        name    bblock
        title   'bblock.com'
 
cseg    segment para    public  'CODE'
        org 0h
        assume  cs:cseg,ds:cseg,es:cseg,ss:cseg

bblock  proc near

BOOTCODE	=	7c00h		; boot code entry (set by BIOS)
BOOTSEG		=	7c0h		; boot segment
CODE_SEG        =       0EFH            ; segment to load code into
CODE_OFF        =       00H             ; offset to load code into
CODE_BLOCKS     =       36              ; blocks (512 byte) of code to load
DISK_OFFSET     =       2               ; blocks offset to code on
 				        ; disk to load into memory 
SECTOR_BLOCKS   =       1               ; blocks per sector on floppy
TRACK_SECTORS   =       18              ; sectors per track on floppy
FIRST_SECTOR    =       (DISK_OFFSET MOD TRACK_SECTORS) + 1
FIRST_TRACK     =       DISK_OFFSET / TRACK_SECTORS 

;		Bootstrap Entrypoint
;

bb1:
        mov     ax, cs          ; resets segments registers
        mov     es, ax
	mov	ss, ax
        mov     ax, 7c0h        ; data segment
        mov     ds, ax
	mov	sp,7c00h	; set up a stack
        
        mov     ax, 0           ; reset floppy disk controller
        int     13h             ; this is recommended by BIOS docs
        
        push    es              ; save es

; The BIOS floppy disk read operation limits the transfer to 18 sectors
; at a time. Load registers for read operation that do not change during the
; copy loop:

	mov     ax,CODE_SEG     ;
	mov     es,ax           ; segment of memory to read into
        mov     bx,CODE_OFF     ; es:bx = base of mem to read into
        mov     dl, 0           ; first floppy drive
        mov     dh, 0           ; head 0
        mov     ah, 2           ; select int 13h read command 

; Read initial part of code from floppy. Note the sectors count from 1, tracks
; count from 0 (go figure.) 
; use si as counter of blocks copied

        mov     al, TRACK_SECTORS - FIRST_SECTOR + 1 
        mov     si, (TRACK_SECTORS - FIRST_SECTOR + 1)*SECTOR_BLOCKS 
	mov     ch, FIRST_TRACK 
        mov     cl, FIRST_SECTOR 
        int     13h             ; BIOS disk service
        jc      error           ; if error, jump to message


; Update the offset of memory to read into 

	add     bx,(TRACK_SECTORS - FIRST_SECTOR + 1)*512   

; Loop to read the rest of code 
; read 18 sectors of track 1 on floppy drive a to buffer at 0EFH:16x512

copy_loop:

	cmp     si,CODE_BLOCKS
	jae     copy_done
	mov     al,TRACK_SECTORS; read a full track 
	inc 	ch              ; next track number
	mov     cl,1            ; sector 1
	int     13h             ; BIOS disk service
	jc      error
	add     si,TRACK_SECTORS*SECTOR_BLOCKS
	add     bx,TRACK_SECTORS*512
	jmp     copy_loop

copy_done:

        pop     es              ; restore es

        mov     bx, offset strmsg  
        call    print           ; print a reassuring message

; Call the monitor by doing a far return to 0DFH:100H

	mov ax,0DFH
	push ax
	mov ax,100H
	push ax
db      0CBH                    ; far return machine instruction

error:
        mov bx, offset errmsg
        call print
err1:                           ; loops forever (shrug)
        jmp     err1

; code for printing a string to console with BIOS 10H service

print:  push    ax              ; prints string, bx has buffer
        push    bx              ; and string ends with 0
        push    cx
        push    dx
pr1:    push    bx              ; push pointer
        mov     al, ds:[bx]     ; get char
        or      al, al          ; check for end
        jz      pr2             ; jump to finish
        mov     ah, 0eh         ; print code
        mov     bh,0            ; page
        int     10h             ; int to BIOS
        pop     bx              ; restore pointer to bx
        inc     bx              ; next char
        jmp     pr1             ; loop
pr2:
        pop     bx
        pop     dx
        pop     cx
        pop     bx
        pop     ax
        ret        

bblock  endp

strmsg:  db    'Monitor bootloader...',0dh,0ah,0
errmsg:  db    'There is something wrong',0dh,0ah,0

cseg    ends

end bblock
