* Generated by c68 5.1 (beta) 25 Apr 2002 (Dec 21 2014) from "build/_strtok.c"
* Compilation date/time: Tue Dec 23 15:09:59 2014

	.text
	.even
	.globl _strtok
_strtok:
	bra	L3
L4:
	tst.l	8(a6)
	bne	L5
	move.l	L1,8(a6)
	bne	L5
	moveq	#0,d0
	bra	L6
L5:
L2:
	move.l	8(a6),a0
	addq.l	#1,8(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-6(a6)
	move.l	12(a6),-4(a6)
	bra	L8
L7:
	cmp.w	-6(a6),d0
	beq	L2
L11:
L10:
L8:
	move.l	-4(a6),a0
	addq.l	#1,-4(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-8(a6)
	bne	L7
L9:
	tst.w	-6(a6)
	bne	L12
	clr.l	L1
	moveq	#0,d0
	bra	L6
L12:
	move.l	8(a6),a0
	subq.w	#1,a0
	move.l	a0,-12(a6)
	bra	L14
L13:
	move.l	8(a6),a0
	addq.l	#1,8(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-6(a6)
	move.l	12(a6),-4(a6)
L17:
	move.l	-4(a6),a0
	addq.l	#1,-4(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-8(a6)
	move.w	-6(a6),d1
	cmp.w	d0,d1
	bne	L20
	tst.w	-6(a6)
	bne	L21
	clr.l	8(a6)
	bra	L22
L21:
	move.l	8(a6),a0
	add.l	#-1,a0
	clr.b	(a0)
L22:
	move.l	8(a6),L1
	move.l	-12(a6),d0
	bra	L6
L20:
L18:
	tst.w	-8(a6)
	bne	L17
L19:
L16:
L14:
	bra	L13
L15:
L6:
	unlk	a6
	rts
L3:
	link	a6,#-12
	bra	L4
	.bss
	.even
L1:
	ds.b	4
	.text