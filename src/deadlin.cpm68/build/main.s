* Generated by c68 5.1 (beta) 25 Apr 2002 (Dec 21 2014) from "build/_main.c"
* Compilation date/time: Tue Dec 23 15:09:58 2014

	.data
	.even
	.globl _BrkVal
_BrkVal:
	dc.l	65535
	.globl _MaxLines
_MaxLines:
	dc.w	8191
	.globl _CurrentLine
_CurrentLine:
	dc.w	0
	.globl _LastLine
_LastLine:
	dc.w	-1
	.text
L13:
	dc.b	$1a,$0
L12:
	dc.b	$61,$0
L11:
	dc.b	$2e,$24,$24,$24,$0
L10:
	dc.b	$25,$73,$0
L9:
	dc.b	$77,$0
L8:
	dc.b	$2e,$62,$61,$6b,$0
L7:
	dc.b	$25,$39,$64,$3a,$20,$0
L6:
	dc.b	$25,$39,$64,$3a,$2a,$0
L5:
	dc.b	$45,$6e,$64,$20,$6f,$66,$20,$49,$6e,$70,$75,$74
	dc.b	$20,$46,$69,$6c,$65,$a,$0
L4:
	dc.b	$4e,$65,$77,$20,$46,$69,$6c,$65,$a,$0
L3:
	dc.b	$46,$69,$6c,$65,$20,$6e,$61,$6d,$65,$20,$6d,$75
	dc.b	$73,$74,$20,$62,$65,$20,$73,$70,$65,$63,$69,$66
	dc.b	$69,$65,$64,$a,$0
L2:
	dc.b	$45,$72,$72,$6f,$72,$20,$4f,$70,$65,$6e,$69,$6e
	dc.b	$67,$20,$25,$73,$a,$0
L1:
	dc.b	$49,$6e,$76,$61,$6c,$69,$64,$20,$50,$61,$72,$61
	dc.b	$6d,$65,$74,$65,$72,$a,$0
	.even
	.globl _main
_main:
	bra	L14
L15:
	move.l	#__iob,-4(a6)
	clr.w	-18(a6)
	moveq	#1,d0
	cmp.w	8(a6),d0
	bge	L16
	bra	L18
L17:
	addq.l	#4,10(a6)
	move.l	10(a6),a0
	move.l	(a0),a0
	moveq	#45,d0
	cmp.b	(a0),d0
	bne	L20
	move.l	10(a6),a0
	move.l	(a0),a0
	move.b	1(a0),d0
	sub.b	#98,d0
	beq	L23
	add.b	#32,d0
	beq	L24
	sub.b	#18,d0
	beq	L25
	sub.b	#32,d0
	beq	L26
	addq.b	#7,d0
	beq	L27
	add.b	#32,d0
	beq	L28
	bra	L22
L23:
L24:
	or.w	#16,_Status
	subq.w	#1,8(a6)
	bra	L21
L25:
L26:
	addq.l	#4,10(a6)
	move.l	10(a6),a0
	move.l	(a0),-(a7)
	jsr	_atoi
	move.w	d0,_TabSize
	move.w	8(a6),d0
	subq.w	#2,d0
	move.w	d0,8(a6)
	addq.w	#4,a7
	bra	L21
L27:
L28:
	addq.l	#4,10(a6)
	move.l	10(a6),a0
	move.l	(a0),-(a7)
	jsr	_atoi
	ext.l	d0
	move.l	d0,_BrkVal
	move.w	8(a6),d0
	subq.w	#2,d0
	move.w	d0,8(a6)
	addq.w	#4,a7
	bra	L21
L22:
	pea	L1
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	move.w	#1,-(a7)
	jsr	_exit
	addq.w	#2,a7
L21:
	bra	L29
L20:
	move.w	_Status,d0
	and.w	#64,d0
	beq	L30
	pea	L1
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	move.w	#1,-(a7)
	jsr	_exit
	addq.w	#2,a7
L30:
	or.w	#64,_Status
	pea	-4(a6)
	move.l	10(a6),a0
	move.l	(a0),-(a7)
	jsr	_OpenFile
	tst.w	d0
	addq.w	#8,a7
	bne	L31
	or.w	#4,_Status
	pea	_FilePath
	pea	L2
	pea	__iob+28
	jsr	_fprintf
	lea	12(a7),a7
L31:
	subq.w	#1,8(a6)
L29:
L18:
	moveq	#1,d0
	cmp.w	8(a6),d0
	blt	L17
L19:
	bra	L32
L16:
	pea	L3
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	move.w	#1,-(a7)
	jsr	_exit
	addq.w	#2,a7
L32:
	move.w	_Status,d0
	and.w	#64,d0
	bne	L33
	pea	L3
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	move.w	#1,-(a7)
	jsr	_exit
	addq.w	#2,a7
L33:
	clr.w	-(a7)
	jsr	_init_tty
	addq.w	#2,a7
	jsr	_init_mem
	move.l	_MemAvail,_MemStart
	move.w	_Status,d0
	and.w	#4,d0
	beq	L34
	pea	L4
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	bra	L35
L34:
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	bra	L37
L36:
	move.l	_LineBuf,-(a7)
	jsr	_strlen
	addq.w	#4,a7
	addq.w	#1,d0
	move.w	d0,-(a7)
	jsr	_MyMalloc
	move.l	d0,-16(a6)
	addq.w	#2,a7
	beq	L39
	move.w	-18(a6),d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	-16(a6),(a0)
	move.l	_LineBuf,-(a7)
	move.w	-18(a6),d0
	addq.w	#1,-18(a6)
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	(a0),-(a7)
	jsr	_strcpy
	addq.w	#8,a7
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	addq.w	#4,a7
	bra	L40
L39:
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	or.w	#8,_Status
	addq.w	#4,a7
	bra	L38
L40:
L37:
	move.l	_LineBuf,-(a7)
	move.l	-4(a6),-(a7)
	jsr	_ReadLine
	tst.w	d0
	addq.w	#8,a7
	bne	L36
L38:
	move.w	_Status,d0
	and.w	#8,d0
	bne	L41
	pea	L5
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
L41:
	move.w	-18(a6),d0
	subq.w	#1,d0
	move.w	d0,_LastLine
	addq.w	#4,a7
L35:
	bra	L43
L42:
	move.w	_Status,d0
	and.w	#2,d0
	beq	L45
	move.w	_CurrentLine,d0
	cmp.w	_CurrentLine,d0
	bne	L46
	addq.w	#1,d0
	move.w	d0,-(a7)
	pea	L6
	jsr	_printf
	addq.w	#6,a7
	bra	L47
L46:
	addq.w	#1,d0
	move.w	d0,-(a7)
	pea	L7
	jsr	_printf
	addq.w	#6,a7
L47:
	bra	L49
L48:
	move.l	_LineBuf,a0
	moveq	#4,d0
	cmp.b	(a0),d0
	beq	L50
L51:
	moveq	#22,d0
	cmp.b	(a0),d0
	bne	L52
	moveq	#68,d0
	cmp.b	1(a0),d0
	bne	L52
	move.b	#4,(a0)
	move.l	_LineBuf,a0
	clr.b	1(a0)
	addq.w	#2,a0
	pea	(a0)
	move.l	_LineBuf,-(a7)
	jsr	_strcat
	addq.w	#8,a7
L52:
	move.l	_LineBuf,-(a7)
	jsr	_strlen
	addq.w	#4,a7
	addq.w	#1,d0
	move.w	d0,-(a7)
	jsr	_MyMalloc
	move.l	d0,-16(a6)
	addq.w	#2,a7
	beq	L53
	move.w	_LastLine,d0
	addq.w	#1,_LastLine
	move.w	d0,-18(a6)
	bra	L55
L54:
	move.w	-18(a6),d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.w	-18(a6),d0
	addq.w	#1,d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a1
	add.l	_Lines,a1
	move.l	(a0),(a1)
L57:
	subq.w	#1,-18(a6)
L55:
	move.w	_CurrentLine,d0
	cmp.w	-18(a6),d0
	ble	L54
L56:
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	-16(a6),(a0)
	move.l	_LineBuf,-(a7)
	move.w	_CurrentLine,d0
	addq.w	#1,_CurrentLine
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	(a0),-(a7)
	jsr	_strcpy
	addq.w	#8,a7
	bra	L58
L53:
	or.w	#8,_Status
	bra	L50
L58:
	move.w	_CurrentLine,d0
	cmp.w	_CurrentLine,d0
	bne	L59
	addq.w	#1,d0
	move.w	d0,-(a7)
	pea	L6
	jsr	_printf
	addq.w	#6,a7
	bra	L60
L59:
	addq.w	#1,d0
	move.w	d0,-(a7)
	pea	L7
	jsr	_printf
	addq.w	#6,a7
L60:
L49:
	move.l	_LineBuf,-(a7)
	pea	__iob
	jsr	_ReadLine
	tst.w	d0
	addq.w	#8,a7
	bne	L48
L50:
	and.w	#-3,_Status
	bra	L61
L45:
	pea	__iob
	jsr	_Parse
	sub.w	#9,d0
	addq.w	#4,a7
	beq	L64
	subq.w	#1,d0
	beq	L65
	add.w	#10,d0
	beq	L66
	bra	L63
L64:
	move.w	_Status,d0
	and.w	#4,d0
	bne	L67
	move.l	-4(a6),-(a7)
	jsr	_rewind
	move.l	#L8,(a7)
	pea	_BaseName
	jsr	_strcat
	addq.w	#8,a7
	pea	L9
	pea	_BaseName
	jsr	_fopen
	move.l	d0,-8(a6)
	bra	L69
L68:
	move.l	_LineBuf,-(a7)
	pea	L10
	move.l	-8(a6),-(a7)
	jsr	_fprintf
	lea	12(a7),a7
L69:
	move.l	_LineBuf,-(a7)
	move.l	-4(a6),-(a7)
	jsr	_ReadLine
	tst.w	d0
	addq.w	#8,a7
	bne	L68
L70:
	addq.w	#8,a7
	move.l	-4(a6),-(a7)
	jsr	_fclose
	move.l	-8(a6),(a7)
	jsr	_fclose
	move.l	#_BaseName,(a7)
	jsr	_strlen
	subq.w	#4,d0
	move.w	d0,a0
	add.l	#_BaseName,a0
	clr.b	(a0)
	addq.w	#4,a7
L67:
	pea	L11
	pea	_BaseName
	jsr	_strcat
	addq.w	#8,a7
	pea	L12
	pea	_BaseName
	jsr	_fopen
	move.l	d0,-8(a6)
	clr.w	_CurrentLine
	addq.w	#8,a7
	bra	L72
L71:
	move.w	_CurrentLine,d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	(a0),-(a7)
	pea	L10
	move.l	-8(a6),-(a7)
	jsr	_fprintf
	lea	12(a7),a7
L74:
	addq.w	#1,_CurrentLine
L72:
	move.w	_LastLine,d0
	cmp.w	_CurrentLine,d0
	bge	L71
L73:
	moveq	#0,d0
	cmp.w	_LastLine,d0
	bgt	L75
	move.w	_LastLine,d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	(a0),a0
	move.l	a0,-(a7)
	move.w	_LastLine,d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a1
	add.l	_Lines,a1
	move.l	(a1),-(a7)
	jsr	_strlen
	ext.l	d0
	addq.w	#4,a7
	move.l	(a7)+,a0
	add.l	d0,a0
	subq.w	#1,a0
	moveq	#10,d0
	cmp.b	(a0),d0
	beq	L75
	pea	L1+17
	move.l	-8(a6),-(a7)
	jsr	_fprintf
	addq.w	#8,a7
L75:
	pea	L13
	move.l	-8(a6),-(a7)
	jsr	_fprintf
	addq.w	#8,a7
	move.l	-8(a6),-(a7)
	jsr	_fclose
	move.l	#_FilePath,(a7)
	jsr	_unlink
	move.l	#_FilePath,(a7)
	pea	_BaseName
	jsr	_rename
	addq.w	#8,a7
	pea	_BaseName
	jsr	_strlen
	subq.w	#4,d0
	move.w	d0,a0
	add.l	#_BaseName,a0
	clr.b	(a0)
	jsr	_cleanup
	moveq	#0,d0
	bra	L76
L65:
	move.w	#1,-18(a6)
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	and.w	#-9,_Status
	addq.w	#4,a7
	bra	L78
L77:
	move.w	_LinesToAppend,d0
	cmp.w	-18(a6),d0
	bge	L80
	or.w	#8,_Status
	bra	L79
L80:
	move.l	_LineBuf,-(a7)
	jsr	_strlen
	addq.w	#4,a7
	addq.w	#1,d0
	move.w	d0,-(a7)
	jsr	_MyMalloc
	move.l	d0,-16(a6)
	addq.w	#2,a7
	beq	L81
	move.w	_LastLine,d0
	add.w	-18(a6),d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	-16(a6),(a0)
	move.l	_LineBuf,-(a7)
	move.w	_LastLine,d0
	move.w	-18(a6),d1
	addq.w	#1,-18(a6)
	add.w	d1,d0
	ext.l	d0
	asl.l	#2,d0
	move.l	d0,a0
	add.l	_Lines,a0
	move.l	(a0),-(a7)
	jsr	_strcpy
	addq.w	#8,a7
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	addq.w	#4,a7
	bra	L82
L81:
	move.l	-4(a6),-(a7)
	jsr	_ftell
	move.l	d0,-12(a6)
	or.w	#8,_Status
	addq.w	#4,a7
	bra	L79
L82:
L78:
	move.l	_LineBuf,-(a7)
	move.l	-4(a6),-(a7)
	jsr	_ReadLine
	tst.w	d0
	addq.w	#8,a7
	bne	L77
L79:
	move.w	_Status,d0
	and.w	#8,d0
	bne	L83
	pea	L5
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
L83:
	move.w	-18(a6),d0
	subq.w	#1,d0
	add.w	d0,_LastLine
L66:
L63:
L62:
L61:
L43:
	bra	L42
L44:
L76:
	unlk	a6
	rts
L14:
	link	a6,#-18
	bra	L15
L85:
	dc.b	$64,$65,$61,$64,$6c,$69,$6e,$3a,$20,$43,$6f,$75
	dc.b	$6c,$64,$20,$6e,$6f,$74,$20,$61,$6c,$6c,$6f,$63
	dc.b	$20,$25,$64,$20,$62,$79,$74,$65,$73,$a,$0
	.even
	.globl _MyMalloc
_MyMalloc:
	bra	L86
L87:
	move.w	8(a6),-(a7)
	jsr	_malloc
	move.l	d0,-4(a6)
	bne	L88
	move.w	8(a6),-(a7)
	pea	L85
	pea	__iob+28
	jsr	_fprintf
	lea	10(a7),a7
	move.w	#1,-(a7)
	jsr	_exit
L88:
	move.w	8(a6),d0
	ext.l	d0
	sub.l	d0,_MemAvail
	move.l	-4(a6),d0
L89:
	unlk	a6
	rts
L86:
	link	a6,#-4
	bra	L87
	.even
	.globl _StringFree
_StringFree:
	bra	L91
L92:
	move.l	8(a6),-(a7)
	jsr	_strlen
	ext.l	d0
	move.l	d0,-4(a6)
	addq.l	#1,d0
	add.l	d0,_MemAvail
	move.l	8(a6),(a7)
	jsr	_free
	move.l	-4(a6),d0
L93:
	unlk	a6
	rts
L91:
	link	a6,#-4
	bra	L92
L96:
	dc.b	$52,$65,$71,$75,$65,$73,$74,$65,$64,$20,$25,$6c
	dc.b	$64,$20,$62,$79,$74,$65,$73,$2c,$20,$41,$76,$61
	dc.b	$69,$6c,$61,$62,$6c,$65,$20,$3d,$20,$33,$2f,$34
	dc.b	$20,$6f,$66,$20,$25,$6c,$64,$20,$62,$79,$74,$65
	dc.b	$73,$2c,$20,$4d,$61,$78,$4c,$69,$6e,$65,$73,$20
	dc.b	$3d,$20,$25,$64,$a,$0
L95:
	dc.b	$46,$61,$74,$61,$6c,$3a,$20,$75,$6e,$61,$62,$6c
	dc.b	$65,$20,$74,$6f,$20,$61,$6c,$6c,$6f,$63,$61,$74
	dc.b	$65,$20,$65,$6e,$6f,$75,$67,$68,$20,$6d,$65,$6d
	dc.b	$6f,$72,$79,$20,$74,$6f,$20,$72,$75,$6e,$21,$a
	dc.b	$0
	.even
	.globl _init_mem
_init_mem:
	bra	L97
L98:
	move.w	_BrkVal+2,-6(a6)
	bra	L100
L99:
	move.w	-6(a6),d0
	ext.l	d0
	divs	#2,d0
	move.w	d0,-6(a6)
	beq	L101
L102:
L100:
	move.w	-6(a6),-(a7)
	jsr	_malloc
	move.l	d0,-4(a6)
	addq.w	#2,a7
	beq	L99
L101:
	move.w	-6(a6),d0
	ext.l	d0
	move.l	d0,_MemAvail
	move.l	-4(a6),-(a7)
	jsr	_free
	addq.w	#4,a7
	move.w	#255,-(a7)
	jsr	_MyMalloc
	move.l	d0,_LineBuf
	bne	L103
	pea	L95
	pea	__iob+28
	jsr	_fprintf
	addq.w	#8,a7
	move.w	#1,-(a7)
	jsr	_exit
	addq.w	#2,a7
L103:
	move.w	#32764,-6(a6)
	bra	L105
L104:
	move.w	-6(a6),d0
	ext.l	d0
	divs	#2,d0
	move.w	d0,-6(a6)
	beq	L106
L107:
L105:
	move.w	-6(a6),-(a7)
	jsr	_MyMalloc
	move.l	d0,_Lines
	addq.w	#2,a7
	beq	L104
L106:
	move.w	-6(a6),d0
	ext.l	d0
	lsr.l	#2,d0
	move.w	d0,_MaxLines
	move.l	#65535,d0
	cmp.l	_BrkVal,d0
	beq	L108
	move.w	_MaxLines,-(a7)
	move.l	_MemAvail,-(a7)
	move.l	_BrkVal,-(a7)
	pea	L96
	pea	__iob+28
	jsr	_fprintf
L108:
	moveq	#1,d0
L109:
	unlk	a6
	rts
L97:
	link	a6,#-6
	bra	L98
	.even
	.globl _init_printer
_init_printer:
	bra	L111
L112:
	moveq	#1,d0
L113:
	unlk	a6
	rts
L111:
	link	a6,#0
	bra	L112
	.even
	.globl _flush_printer
_flush_printer:
	bra	L115
L116:
	moveq	#1,d0
L117:
	unlk	a6
	rts
L115:
	link	a6,#0
	bra	L116
	.even
	.globl _cleanup
_cleanup:
	bra	L119
L120:
	pea	L11
	pea	_BaseName
	jsr	_strcat
	addq.w	#8,a7
	pea	_BaseName
	jsr	_unlink
	unlk	a6
	rts
L119:
	link	a6,#0
	bra	L120
	.bss
	.even
	.globl _Lines
_Lines:
	ds.b	4
	.globl _LineBuf
_LineBuf:
	ds.b	4
	.globl _Status
_Status:
	ds.b	2
	.globl _LinesToAppend
_LinesToAppend:
	ds.b	2
	.globl _MemAvail
_MemAvail:
	ds.b	4
	.globl _MemStart
_MemStart:
	ds.b	4
	.globl _CutStart
_CutStart:
	ds.b	2
	.globl _CutEnd
_CutEnd:
	ds.b	2
	.globl _DestLine
_DestLine:
	ds.b	2
	.globl _Count
_Count:
	ds.b	2
	.text