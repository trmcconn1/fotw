
NAME EXTERNAL
TITLE EXTERNAL

; External.a: implementation of external monitor commands  
; the stand-alone version. 

PUBLIC _n_externals,_external_entry_points,_external_help,_external_usage
PUBLIC _external_names
PUBLIC save_disk,save_cyl,save_sec,save_head,save_seg,save_off,save_size
EXTRN my_strlen:NEAR
EXTRN my_strcmp:NEAR
EXTRN _print_i:NEAR
EXTRN my_atoi:NEAR
EXTRN _rcscript:BYTE

INCLUDE macros.asm

DGROUP GROUP _TEXT,_DATA,_BSS

_DATA SEGMENT PUBLIC 'DATA' 
ASSUME DS:DGROUP

info_name db "info",00H
info_usage db "info",0DH,0AH,00H
info_help db "print author and other info",0DH,0AH,00H
boot_name db "boot",00H
boot_usage db "boot <cylinder head sector> | boot <a|b|c|d>",0DH,0AH,00H
boot_help db "run sector of boot drive or mbr of fd/hd as boot code",0DH,0AH,00H
partinfo_name db "partinfo",00H
partinfo_usage db "partinfo [80H | 81H]",0DH,0AH,00H
partinfo_help db "partinfo: Give partition information about boot or given drive",0DH,0AH,00H
save_name db "save",00H
save_usage db "save",0DH,0AH,00H
save_help db "save program state to disk",0DH,0AH,00H
_n_externals dw 4
_external_entry_points dw OFFSET DGROUP:_info + 0100H
dw OFFSET DGROUP:_boot + 0100H
dw OFFSET DGROUP:_partinfo + 0100H
dw OFFSET DGROUP:_save + 0100H
_external_help dw OFFSET DGROUP:info_help
dw OFFSET DGROUP:boot_help
dw OFFSET DGROUP:partinfo_help
dw OFFSET DGROUP:save_help
_external_usage dw OFFSET DGROUP:info_usage
dw OFFSET DGROUP:boot_usage
dw OFFSET DGROUP:partinfo_usage
dw OFFSET DGROUP:save_usage
_external_names dw OFFSET DGROUP:info_name
dw OFFSET DGROUP:boot_name
dw OFFSET DGROUP:partinfo_name
dw OFFSET DGROUP:save_name

; Below you can put data used by individual external commands

info_msg db "Boot Monitor 1.2 by Terry R. McConnell (2018)",0DH,0AH,00H
partinfo_heading db "Partition Table Entrys",0DH,0AH,00H
partinfo_sep db "-----------------------------------------------------------",0DH,0AH,00H
partinfo_header db "begin:   cylinder  head      sector  end: ",0DH,0AH,00H
boot_error db "volume boot failed",0DH,0AH,00H


PARTENTRY STRUC
	peBootable db ?     ; 80h = bootable, 00h = nonbootable
	peBeginHead db ?    ; beginning head
	peBeginSector db ?  ; beginning sector
	peBeginCylinder db ?; beginning cylinder
	peFileSystem db  ? ; name of file system
	peEndHead db ?
	peEndSector db ?
	peEndCylinder db ?
	peStartSectorLo dw ?
	peStartSectorHi dw ?
	peSectorsLo dw ?
	peSectorsHi dw ?
PARTENTRY ENDS

_bblock db 446 dup (0)
part_entry0 PARTENTRY <>
part_entry1 PARTENTRY <>
part_entry2 PARTENTRY <>
part_entry3 PARTENTRY <>
	

_DATA ENDS

_TEXT SEGMENT PUBLIC 'CODE'
ASSUME CS:DGROUP,DS:DGROUP

_info:

	PRINT_STRING info_msg
	ret

_boot:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;   This loader copies the boot sector at start of cylinder argv[0] 
;   into memory at offset 7c00h 
;   Then we do a far return to 00H:7c00H to run the code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

BOOTCODE	=	7c00h		; boot code entry (set by BIOS)
BOOTSEG		=	7c0h		; boot segment
CODE_SEG        =       00H             ; segment to load code into
CODE_OFF        =       7C00H           ; offset to load code into

	push bp
	mov bp,sp
	push bx
	push cx
	push dx
	push es

        mov     dl, save_disk         ; default drive number

	mov ax, [bp+4]          ; argc into ax
	cmp ax,3
	je boot_hd_sector
	cmp ax,1
	je not_boot_default_hd
	jmp error

not_boot_default_hd:

; mbr sector
	mov ch,00H              ; load track number for int 13H below
	mov dh,00H              ; load head number for int 13H below
	mov cl,01H              ; load sector number for int 13H below

	mov bx,[bp+6]           ; argv into bx
	mov ax,[bx]             ; argv[0] into ax
	mov bx,ax
	cmp BYTE PTR [bx],61H              ; Is it the letter a?
	je boot_fd_a
	cmp BYTE PTR [bx],62H
	je boot_fd_b
	cmp BYTE PTR [bx],63H
	je boot_80H
	cmp BYTE PTR [bx],64H
	je boot_81H
	jmp error



boot_81H:
	mov dl,81H
	jmp drive_reset
boot_80H:
	mov dl,80H
	jmp drive_reset
	
boot_fd_a:
	mov dl,00H              ; 00H is number of first floppy drive
	jmp drive_reset

boot_fd_b:
	mov dl,01H              ; 01H is number of second floppy drive
	jmp drive_reset


boot_hd_sector:
	mov bx,[bp+6]           ; argv into bx
	mov ax,[bx]             ; argv[0] into ax
	push ax                 ; convert string pointed to by ax to number
	call my_atoi            ; to number in ax
	add sp,2
	mov ch,al               ; load cylinder number for int 13H below
	mov ax,[bx+2]           ; argv[1] into ax           
	push ax
	call my_atoi
	add sp,2
	mov dh,al               ; load head number for int 13H below
	mov ax,[bx+4]           ; argv[2] into ax
        push ax
	call my_atoi
	add sp,2
	mov cl,al               ; load sector number for int 13H below
        
drive_reset:

        mov     ax, 0           ; reset disk controller
        int     13h             ; this is recommended by BIOS docs

read_bb:

; The BIOS disk read operation limits the transfer to one track
; at a time. We assume one track is big enough to hold everything

	mov     ax,CODE_SEG     ;
	mov     es,ax           ; segment of memory to read into
        mov     bx,CODE_OFF     ; es:bx = base of mem to read into
        mov     ah, 2           ; select int 13h read command 
	mov     al, 1           ; read 1 block
        int     13h             ; BIOS disk service
        jc      error           ; if error, jump to message

; check to make sure block has the proper signature: 55AAH at the end

	mov ax,7C0H
	mov es,ax
	mov al,es:[510]
	cmp al,55H
	jne error
	mov al,es:[511]
	cmp al,0AAH
	jne error

; Call the bootcode by doing a far return to 00H:7C00H

	mov ax,00H
	push ax
	mov ax,7C00H
	push ax
db      0CBH                    ; far return machine instruction

error:
	PRINT_STRING boot_error
	pop es
	pop dx
	pop cx
	pop bx
	pop bp
	ret

partinfo_disk db 80H
H80str db "80H",00H
H81str db "81H",00H
partinfo_msg1 db "Bios disk = ",00H

_partinfo:


	push bp
	mov bp,sp
	push bx
	push cx
	push dx
	push es

	mov al, save_disk
	mov partinfo_disk,al

	mov ax, [bp+4]          ; argc into ax
; if there is an arg, see if it is 80H or 81H 
	cmp ax,0
	je read_boot
	mov bx, [bp+6]
	mov ax, [bx] ; should be pointing to string 80H or 81H 
	push ax
	mov ax, OFFSET DGROUP:H80str
	push ax
	call my_strcmp
	add sp,4
	cmp ax,00H
	jne chk81H
	mov partinfo_disk,80H
	jmp read_boot
	 
chk81H:
	mov ax,[bx]
	push ax
	mov ax, OFFSET DGROUP:H81str
	push ax
	call my_strcmp
	add sp,4
	cmp ax,00H
	jne read_boot
	mov partinfo_disk,81H
 
; read the bootblock into memory
read_boot:

	mov     ax,DS           ;
	mov     es,ax           ; segment of memory to read into
        mov     bx,OFFSET DGROUP:_bblock     ; es:bx = base of mem to read into
        mov     dl, partinfo_disk         ; first hard drive
        mov     ah, 2           ; select int 13h read command 
	mov     ch, 00H         ; cylinder 0 
	mov     cl,1            ; sector 1
        mov     dh, 0           ; head 0
	mov     al, 1 ;         ; read 1 sector
        int     13h             ; BIOS disk service
	jnc     read_ok
        jmp     error           ; if error, jump to message

read_ok:

	PRINT_STRING partinfo_heading
	PRINT_STRING partinfo_msg1
	mov al, partinfo_disk
	cmp al,81H
	jne first_disk
	PRINT_STRING H81str
	PRINT_CHAR 0DH
	PRINT_CHAR 0AH
	jmp print_table
first_disk:
	PRINT_STRING H80str
	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

print_table:
	PRINT_STRING partinfo_sep

	PRINT_STRING partinfo_header

	xor ax,ax
	mov al,part_entry0.peBeginCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry0.peBeginHead
	push ax
	call _print_i
	add sp,2	

	xor ax,ax
	mov al,part_entry0.peBeginSector
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry0.peEndCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry0.peEndHead
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry0.peEndSector
	push ax
	call _print_i
	add sp,2

	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

	PRINT_STRING partinfo_sep


	xor ax,ax
	mov al,part_entry1.peBeginCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry1.peBeginHead
	push ax
	call _print_i
	add sp,2	

	xor ax,ax
	mov al,part_entry1.peBeginSector
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry1.peEndCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry1.peEndHead
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry1.peEndSector
	push ax
	call _print_i
	add sp,2

	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

	PRINT_STRING partinfo_sep


	xor ax,ax
	mov al,part_entry2.peBeginCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry2.peBeginHead
	push ax
	call _print_i
	add sp,2	

	xor ax,ax
	mov al,part_entry2.peBeginSector
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry2.peEndCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry2.peEndHead
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry2.peEndSector
	push ax
	call _print_i
	add sp,2

	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

	PRINT_STRING partinfo_sep


	xor ax,ax
	mov al,part_entry3.peBeginCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry3.peBeginHead
	push ax
	call _print_i
	add sp,2	

	xor ax,ax
	mov al,part_entry3.peBeginSector
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry3.peEndCylinder
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry3.peEndHead
	push ax
	call _print_i
	add sp,2

	xor ax,ax
	mov al,part_entry3.peEndSector
	push ax
	call _print_i
	add sp,2

	PRINT_CHAR 0DH
	PRINT_CHAR 0AH

	PRINT_STRING partinfo_sep

	pop es
	pop dx
	pop cx
	pop bx
	pop bp
	ret

; save: write image of monitor program back to disk whence it was loaded. This
;       allows for program state (environment, e.g.) to be preserved between
;       invocations of the program. The write to disk is done using the BIOS
;       int 13H call, and the following parameters needed by the interrupt
;       are filled in by monitor when it is first started

save_disk db ?     ; 80H or 81H = BIOS disk number
save_cyl  db ?     ; 
save_sec  db ?
save_head db ?
save_seg  db ?     ; value of cs when monitor was called
save_off  db ?     ; value of pc when monitor was called
save_size db ?     ; upper bound on number of sectors in program image

save_error_msg db "Unable to save: disk write failed",0DH,0AH,00H

_save:
	push bp    ; set up stack frame and save registers
	mov bp,sp
	push es
	push bx
	push cx
	push dx
	xor ax,ax
	mov al,save_seg
	mov es,ax
	xor bx,bx
	mov bl,save_off     ; es:bx = buffer to write from
	mov ah,03H          ; select write disk sectors 13H function
	mov ch, save_cyl    ; 3D coordinates of image on disk
	mov cl, save_sec
	mov dl, save_disk
	mov dh, save_head
	mov al, save_size   ; size of image in blocks (overestimate)
	int 13H             ; BIOS hard disk service
	jc save_error
	jmp save_return

save_error:
	PRINT_STRING save_error_msg

save_return:
	pop dx
	pop cx
	pop bx
	pop es              ; restore registers used
	pop bp
	ret

_TEXT ENDS
_BSS SEGMENT PUBLIC 'BSS'
ASSUME DS:DGROUP
_BSS ENDS
END

