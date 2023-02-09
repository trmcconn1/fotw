# K:  The word copier 
# Tape effect: BW->B --> BWBW->B  
# where B is a blank, W denotes a nonempty word of nonblank
# characters, and -> denotes the initially scanned cell. 
# See Mendelson, page 241 
#
# A generalization of this the n-shift copier which does the following:
# BWnB...BW1->B --> BWnB...BW1BWn->B...
# I.e., it copies the nth argument to the left instead of the first.
# So this file gives the 1-shift copier. We make comments below about what
# what modifications are needed to get the n-shift copier.

.alphabet B|

# Macro for calling a routine named xxx for the yyyth time
.define CALL (xxx,yyy) \.prepend xxxyyy\
\.include xxx.t\
\.prepend

# Go to the blank before the beginning of W

0 B B Lambda00 
0 | | Lambda00
.CALL(Lambda,0)

# Do Lambda above n times for n-shift copier

# :Loop  We are pointing at the character just before the next one to
#        be copied.
# Take one step right. This puts us at the next character of W to be copied.
Lambda0halt | R 1
Lambda0halt B R 1

# exit loop in state 2 if blank (We have reached the end of W)
1 B B 2

# otherwise mark with a blank and go right to blank past W and its
# partially built copy
1 | B Rho00
.CALL(Rho,0)
Rho0halt B B Rho10
Rho0halt | | Rho10
.CALL(Rho,1)

# Copy current character of W
Rho1halt B | Lambda10
Rho1halt | | Lambda10

# Invoke Rho above n+1 times for n-shift copier

#Go back to the marked cell of W and unmark it
.CALL(Lambda,1)
Lambda1halt B B Lambda20
Lambda1halt | | Lambda20
.CALL(Lambda,2)

# Invoke Lambda n+1 times for n-shift copier

# Unmark cell and reenter loop in state Lambda0halt
Lambda2halt B | Lambda0halt
Lambda2halt | | Lambda0halt

# out of loop in state 2
# we need only go to the B past the copy of W

2 B B Rho20
2 | | Rho20
.CALL(Rho,2)

# Invoke Rho n times for n-shift copier

# Exit in halt state to conform with conventions.
Rho2halt B B halt
Rho2halt | | halt
