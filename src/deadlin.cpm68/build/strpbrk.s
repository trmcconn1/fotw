* Generated by c68 5.1 (beta) 25 Apr 2002 (Dec 21 2014) from "build/_strpbrk.c"
* Compilation date/time: Tue Dec 23 15:09:59 2014

	.text
	.even
	.globl _strpbrk
_strpbrk:
	bra	L1
L2:
	bra	L4
L3:
	move.l	12(a6),-4(a6)
	bra	L7
L6:
	move.w	-6(a6),d0
	cmp.w	-8(a6),d0
	bne	L10
	move.l	8(a6),a0
	subq.w	#1,a0
	move.l	a0,d0
	bra	L11
L10:
L9:
L7:
	move.l	-4(a6),a0
	addq.l	#1,-4(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-8(a6)
	bne	L6
L8:
L4:
	move.l	8(a6),a0
	addq.l	#1,8(a6)
	move.b	(a0),d0
	ext.w	d0
	move.w	d0,-6(a6)
	bne	L3
L5:
	moveq	#0,d0
L11:
	unlk	a6
	rts
L1:
	link	a6,#-8
	bra	L2