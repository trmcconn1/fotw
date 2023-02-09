.model small  
DOSSEG

.data?
color dw ?

.CODE
	PUBLIC  _border

_border PROC    near

        push bp
	mov bp,sp ; make bp link to stack
	mov ax, [bp + 4]  ;get passed color value from stack
	mov color,ax
; set border color
	mov     ah,0Bh  ; BIOS service number
	mov     bh,0   ; subservice number

	mov     bl,byte ptr color             ; and put in bl
	int     10h
        pop     bp
        ret
_border ENDP
END
