# max.t: compute the smaller of x and y
# Computes the minimum of two natural numbers x and y
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# We assume we are reading the left end of x at the beginning.

.alphabet B|

.ifdef TEST
.tape |||||B||||  # for test: max(4,3) = 3
.endif

# A few convenient macros
.define right ( start , end ) start B R end\
start | R end

.define left ( start , end ) start B L end\
start | L end

.define any ( start , end ) start B B end\
start | | end

.define call ( foo , num ) \.prepend foonum\
\.include foo.t\
\.prepend

# Copy y to the right of y, making the tape xByBy->B

.any(0,R00)          
.call(R,0)          # Go to the right end
.any(R0halt,K00)
.call(K,0)          # Copy
.any(K0halt,loop)   #

# Now enter a loop in which we remove a | from the right of y and from the
# left of x. If x runs out first, the tape will be y+1By-x and we finish by
# subtracting, leaving the answer x. If y runs out first
# the tape will be x-y-1By+1 and we finish by cleaning up, leaving y.

#loop:
# step left and remove rightmost | of y
.left(loop,1)
1 | B Lambda00
1 B B no_y # No more y. Exit loop in state no_y
# Go to left end by using Lambda 3 times
.call(Lambda,0)
.any(Lambda0halt,Lambda10)
.call(Lambda,1)
.any(Lambda1halt,Lambda20)
.call(Lambda,2)
# step right and remove the leftmost | of x
.right(Lambda2halt,2)
2 B B no_x   # x all used up. Exit loop in state no_x
2 | B Rho00  # remove leftmost | of x
# Go back to right end by using Rho 3 times and re-enter loop
.call(Rho,0)
.any(Rho0halt,Rho10)
.call(Rho,1)
.any(Rho1halt,Rho20)
.call(Rho,2)
.any(Rho2halt,loop)


# no_x: tape is ->By+1By-x-1. Now we do what amounts to subtraction: loop,
# removing successively one | from the left of y+1 and one | from the right
# of y-x-1, until nothing of the latter remains.  
no_x B R 3
3 | B Rho30
.call(Rho,3)
.any(Rho3halt,Rho40)
.call(Rho,4)
Rho4halt B L 4
4 B B halt
4 | B Lambda30
.call(Lambda,3)
.any(Lambda3halt,Lambda40)
.call(Lambda,4)
.any(Lambda4halt,no_x)

# no_y: tape is x-y-1By+1->B. Call cleanup machine
.any(no_y,C00)
.call(C,0)
.any(C0halt,halt)


