# max.t: Compute  [x/2], where [] is the greatest integer function.
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). 
# We assume we are reading the left end of x at the beginning.

.alphabet B|

.ifdef TEST
.tape |||||   
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

# Copy to the right, decrementing the counter(x) by two each time
# loop:
# Start loop by adding | on the answer. Thus, an answer will aways be
# present. (Decrementing before writing leads to an infinite loop on [0].)
#
# Move two words right.
.any(0,Rho00)
.call(Rho,0)
.any(Rho0halt,Rho10)
.call(Rho,1)
Rho1halt B | Lambda00  # write the new |.
# Move two words left.
.call(Lambda,0)
.any(Lambda0halt,Lambda10)
.call(Lambda,1)
# Now we are back at left end of x.
# Blank out next two |s, quit if there are fewer
.right(Lambda1halt,1)
1 B B odd   # x has reached zero (odd x. One too many | in this case )
1 | B 2
.right(2,3)
3 B B halt   # x has reached zero (even x.)
3 | B 0      # renter loop in state 0

# Fixup odd case. I do not see a way to avoid an asymmetry between odd and
# even cases. Here we have one too many |s left on the test.
odd B R odd
odd | B halt
