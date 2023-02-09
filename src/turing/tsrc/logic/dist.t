# dist.t: Computes |x-y|, the distance between x and y.
# For the discussions below, assume x and y appear on the tape in that order. 
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). A pair of numbers is represented
# as ||...|B|....| where B stands for blank 
# The idea behind the algorithm is to alternately remove the leftmost | from
# x and the rightmost | from y until one of them becomes empty. 


.alphabet B|
# Macro for calling routine in xxx.t for the yyyth time 
.define CALL (xxx,yyy) \.prepend xxxyyy\
\.include xxx.t\
\.prepend 

# :loop 
# We are at the beginning of what is left of x. Wipe out the leftmost | and
# step right
0 | B q1
0 B B q1
q1 B R q2
q1 | R q2

# If x is used up (i.e., we are now reading a blank,) we are done since
# the answer is the remains of y. Thus the following instruction only
# matches when there is reason to continue
q2 | | Rho00

# Search right for second of two blanks. It will be the one to the 
# right of the remains of y
.CALL(Rho,0)
Rho0halt B B Rho10
Rho0halt | B Rho10
.CALL(Rho,1)

# Step back left and rub out the right-most | of y
Rho1halt B L q3
Rho1halt | L q3
q3 | B q4
q3 B B q4

# Step left again to see if anything remains of y. If not, exit loop in
# state Lambda20. Else go back left to the second of two spaces, which puts us
# back at the beginning of x (after 1 step right.) Reenter loop.

q4 | L q5
q4 B L q5
q5 B B Lambda20   # exit loop
q5 | | Lambda00
.CALL(Lambda,0)
Lambda0halt | | Lambda10
Lambda0halt B B Lambda10
.CALL(Lambda,1)
Lambda1halt B R 0
Lambda1halt | R 0


###########
# Out of loop. We must finish by adding one | on the beginning
# of x (Think about the case ||B|. The answer should be ||.)
.CALL(Lambda,2)
Lambda2halt B | halt
Lambda2halt | | halt
