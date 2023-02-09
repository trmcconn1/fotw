# mult.t: compute x*y
# Computes the product of two natural numbers x and y
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# We assume we are reading the left end of x at the beginning.
# See Mendelson, p. 242
#
# Remark: This program leaves some garbage on the tape in addition to the
# answer, so it would need to be modified to show that the product is
# Turing computable. We have not bothered to do this, since the program
# is already complicated enough.

.alphabet B|

.ifdef TEST
.tape |||||B||||  # for test: 4*3
.undef TEST
.endif

# A few convenient macros
.define right ( start , end ) start B R end\
start | R end

.define left ( start , end ) start B L end\
start | L end

.define all ( start , end ) start B B end\
start | | end

.define call ( foo , num ) \.prepend foonum\
\.include foo.t\
\.prepend

# Move right. If the next character is a blank, then x is 0, a case we
# handle as special 

.right(0,1)
1 B B case0
1 | | case1

# Create a new zero entry to the right of everything, then call the
# Cleanup machine so that only the zero is left on the tape.

# case 0
.all(case0,Rho00)
.call(Rho,0)
.right(Rho0halt,2)
2 B | 3
.right(3,C00)
.call(C,0)
.all(C0halt,halt)

# case 1
# Remove 1 | from x so that there now are as many |s in it as its value
.left(case1,4)
4 | B copyloop
4 B B copyloop

# copy loop: make x copies of y to the right of the original copy. 
# 

.right(copyloop,6)
.right(6,7)
7 B B break # leave  copy loop

# decrement x
7 | L 8
8 B B R00
8 | B R00
# Move past the right end of the content
.call(R,0)
.all(R0halt,K00)
# Copy y to the right
.call(K,0)
.all(K0halt,L00)
# Move past the left end of the content
.call(L,0)
.all(L0halt,copyloop)        # reenter copy loop

# We come here after breaking out of copy loop.
# We have y y ... y ( x  times ) on the tape, separated by B's. 
# We are reading the blank before the first y.
# Let m denote the number of strokes in y,  then erasing all the B's would give
# mx + x - 1 strokes. Since y + 1 = m, this is yx + 2x - 1 strokes. If we 
# erase 2 strokes for each of the x-1 B's, then we are left with 
# yx + 2x -1 - 2x + 2 = yx + 1 strokes, the correct answer.

# Fill in the blanks loop:
# probe right to see if there is another y
.right(break,9)
9 | L 10
9 B B halt  # done if at end of string of y's
# Fill in a gap between y's. (We do this at the beginning too, but it gets
# wiped out right away.
10 B | Lambda00
10 | | Lambda00
# Go back and rub out two |s at the left end. The need for this is explained
# above. 
.call(Lambda,0)
.right(Lambda0halt,11)
11 B B 12
11 | | 12
.right(12,13)
13 B B Rho10
13 | B Rho10
# Find the next B to fill in (or the right end)
.call(Rho,1)
# Loop back to beginning of fill in blanks loop
.all(Rho1halt,break) 
