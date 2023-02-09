# ins.t: insert. Assume we are reading a mark (-). Insert new mark,
# shifting the current mark and everything to its right one cell right.
# Thus ...x->-z... -->  x->--z...
# We finish reading the copy of the original -. 
# Note that this is only ever used in the dseg so we can assume z is 1-9,
# - or data.
# Assumption: there is no mark to the right of the one we are initially
# reading.

.include global.t

.ifdef TEST
.tape ||BB-12|R-B||B
.undef TEST
.endif

# Go right to the second 0
.not0(0,R,0)
0 0 R loop

# Now loop: take two steps left. If the character there is not -, copy
# it to the cell to the right 

.not-(loop,L,l1)
.any(l1,L,l2)
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

.ifndef cpinstblock
.define cpinstblock ( xx )\
cxx xx R dxx\
\.any(dxx,xx,loop) 
.endif

.cpinstblock(0) 
.cpinstblock(1) 
.cpinstblock(2) 
.cpinstblock(3) 
.cpinstblock(4) 
.cpinstblock(5) 
.cpinstblock(6) 
.cpinstblock(7) 
.cpinstblock(8) 
.cpinstblock(9) 
.cpinstblock(B) 
.cpinstblock(|) 
.cpinstblock(-) 

# Here when we have finished moving everything to the right.
# We are now reading the original -, as required.
loop - L halt
