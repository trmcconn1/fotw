# max.t: compute the larger of x and y
# Computes the maximum of two natural numbers x and y
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# We assume we are reading the left end of x at the beginning.

.alphabet B|

.ifdef TEST
.tape |||B|||||  # for test: max(2,4) = 4
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
# left of x. If x runs out first, the tape will be yBy-x-1 and we finish by
# cleaning y-x-1 from the tape, leaving the answer y. If y runs out first
# the tape will be x-y-1By and we finish by adding.

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


# no_x: tape is ->ByBy-x-1
.any(no_x,Rho30)
.call(Rho,3)
# clean any |s that remain to the right
.right(Rho3halt,3)
3 | B 4
3 B B halt
4 B R 3

# no_y: tape is x-y-1By->B
.any(no_y,Lambda30)
.call(Lambda,3)
Lambda3halt B | Lambda40  # rub out space before y to fake an addition
.call(Lambda,4)
.right(Lambda4halt,5)
5 | B halt


