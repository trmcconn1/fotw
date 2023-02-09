;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   Hard Drive Partition Boot loader for monitor program
;               By Terry R. McConnell
;
;   In the current version, 
;   The monitor program is assumed to be about 20k beginning at offset 1024
;   bytes from the beginning of the very first partition of the drive.
;   This loader copies the
;   program into memory at offset 1024 (This leaves two 512 byte blocks below
;   it in memory. The first is the vector table and the second is the BIOS
;   data area.) Monitor is called by a far jump (actually a far return) to
;   0DFH:100H. It is written this way so that the monitor can be called
;   the same way DOS would call a .com: with CS pointing to the start of the
;   PSP, and the entry point at offset 100H
;   The actual location of the monitor is configurable by changing the
;   setting below. However we cannot go beyond cylinder 255 (if there is one)
;   since we use only the ch register for the cylinder number. (Our BIOS
;   allows the two bits of cl to be used as high bits of the cylinder number,
;   but the parition table entries only allow one byte for starting and
;   ending cylinder.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        name    hdbblk
        title   'hdblk.com'
 
cseg    segment para    public  'CODE'
        org 0h
        assume  cs:cseg,ds:cseg,es:cseg,ss:cseg

hdbblk  proc near


; These don't depend on the disk. They determine where the monitor program
; is loaded into memory

CODE_SEG        =       0EFH            ; segment to load code into
CODE_OFF        =       00H             ; offset to load code into

; This needs to be at least as large as the current monitor size
CODE_BLOCKS     =       40              ; blocks (512 byte) of code to load
				        ; needs to less than SECTORS

; These need to be tailored to the disk partition used:

FIRST_CYLINDER  =       00H             ; cylinder where code starts
FIRST_SECTOR    =       3               ; sector where code starts
FIRST_HEAD      =       1               ; head where code starts
BOOT_DISK       =       80H             ; disk number (80H or 81H)

; Recall that hard drives are organized in sectors, heads, and cylinders. 
; The numbering runs through the sectors on the first cylinder and head, then
; to the next head, etc, and then to the next cylinder. This loader assumes that
; everything to be loaded is in the first cylinder. The concept of track does
; not really apply to hard disks, but can be thought of as the collection of
; sectors running "around" the disk on a given cylinder/head. (cylinder numbers
; count "inward" from the outer edge of the disk, and head numbers count
; "down" from the top platter of the disk to the bottom platter.)

;		Bootstrap Entrypoint
;

bb1:
        mov     ax, cs          ; resets segments registers
        mov     es, ax
	mov	ss, ax
        mov     ax, 7c0h        ; data segment
        mov     ds, ax
	mov	sp,7c00h	; set up a stack
        
        mov     ax, 0           ; reset disk controller
	mov     dl,BOOT_DISK    ; disk number, 80H or 81H
        int     13h             ; this is recommended by BIOS docs
        
        push    es              ; save es

; The BIOS disk read operation limits the transfer to one track
; at a time. We assume one track is big enough to hold everything

	mov     ax,CODE_SEG     ;
	mov     es,ax           ; segment of memory to read into
        mov     bx,CODE_OFF     ; es:bx = base of mem to read into
        mov     dl, 80H         ; first hard drive
        mov     ah, 2           ; select int 13h read command 
	mov     ch, FIRST_CYLINDER
	mov     cl,FIRST_SECTOR ;
        mov     dh, FIRST_HEAD  ; 
	mov     al, CODE_BLOCKS ;
        int     13h             ; BIOS disk service
        jc      error           ; if error, jump to message

        pop     es              ; restore es

        mov     bx, offset strmsg  
        call    print           ; print a reassuring message

;       pass arguments to monitor on the stack
;       if this is changed, corresponding changes need to be made in
;       monitor.asm

	xor ax,ax
	mov al,CODE_OFF
	push ax
	mov al,CODE_SEG
	push ax
	mov al,FIRST_CYLINDER
	push ax
	mov al,FIRST_HEAD
	push ax
	mov al,FIRST_SECTOR
	push ax
	mov al,CODE_BLOCKS
	push ax
	mov al,BOOT_DISK
	push ax

	
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

hdbblk  endp

strmsg:  db    'Monitor bootloader...',0dh,0ah,0
errmsg:  db    'There is something wrong',0dh,0ah,0

cseg    ends

end hdbblk
