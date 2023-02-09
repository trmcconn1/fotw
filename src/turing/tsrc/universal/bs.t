# bs.t: backspace. Assume we are reading a mark (-). Delete the tape character
# to the left of it, and move the mark and everything to the right of it
# one cell left. Thus ...xy->-z... -->  x->-z...
# This is only ever used in dseg, so we can assume that xyz are 1-9, -, or
# data. (We also assume there is no mark to the right of the one we are
# initially reading.) 

.include global.t

.ifdef TEST
.tape ||BB-12|R-B||B
.undef TEST
.endif

.any(0,L,1)  # Take two steps left
.any(1,L,loop)

# Now loop: take two steps right. If the character there is not zero, copy
# it to the cell to the left

.not0(loop,R,l1)
.any(l1,R,l2)
l2 0 0 c0
l2 1 1 c1
l2 2 2 c2
l2 3 3 c3
l2 4 4 c4
l2 5 5 c5
l2 6 6 c6
l2 7 7 c7
l2 8 8 c8
l2 9 9 c9
l2 | | c|
l2 B B cB
l2 - - c-

.ifndef cppbsblock
.define cppbsblock ( xx )\
cxx xx L dxx\
\.any(dxx,xx,loop) 
.endif
.cppbsblock(0) 
.cppbsblock(1) 
.cppbsblock(2) 
.cppbsblock(3) 
.cppbsblock(4) 
.cppbsblock(5) 
.cppbsblock(6) 
.cppbsblock(7) 
.cppbsblock(8) 
.cppbsblock(9) 
.cppbsblock(B) 
.cppbsblock(|) 
.cppbsblock(-) 

# Here when we have finished moving everything to the left
# Just need to to go back to original mark (now moved one left)

loop 0 0 tomark
.not-(tomark,L,tomark) 
tomark - - halt
