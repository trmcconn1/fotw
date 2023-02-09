# K2:  The 2 word shift copier 
# Tape effect: BW1BW2->B --> BW1BW2BW1->B  
# where B is a blank, Wi denote nonempty words of nonblank
# characters, and -> denotes the initially scanned cell. 
# See Mendelson, page 241 
#
# See K.t for more discussion

.alphabet B|

# Macro for calling a routine named xxx for the yyyth time
.define CALL (xxx,yyy) \.prepend xxxyyy\
\.include xxx.t\
\.prepend

# Go to the blank before the beginning of W1

0 B B Lambda00 
0 | | Lambda00
.CALL(Lambda,0)
Lambda0halt B B Lambda10
.CALL(Lambda,1)


# :Loop  We are pointing at the character just before the next one to
#        be copied.
# Take one step right. This puts us at the next character of W to be copied.
Lambda1halt | R 1
Lambda1halt B R 1

# exit loop in state 2 if blank (We have reached the end of W)
1 B B 2

# otherwise mark with a blank and go right to blank past W and its
# partially built copy
1 | B Rho00
.CALL(Rho,0)
Rho0halt B B Rho10
Rho0halt | | Rho10
.CALL(Rho,1)
Rho1halt B B Rho20
Rho1halt | | Rho20
.CALL(Rho,2)
# copy current | of W1
Rho2halt B | Lambda20
Rho2halt | | Lambda20

#Go back to the marked cell of W1 and unmark it
.CALL(Lambda,2)
Lambda2halt B B Lambda30
Lambda2halt | | Lambda30
.CALL(Lambda,3)
Lambda3halt B B Lambda40
Lambda3halt | | Lambda40
.CALL(Lambda,4)


# Unmark cell and reenter loop in state Lambda1halt
Lambda4halt B | Lambda1halt
Lambda4halt | | Lambda1halt

# out of loop in state 2
# we need only go to the B past the copy of W

2 B B Rho30
2 | | Rho30
.CALL(Rho,3)
Rho3halt B B Rho40
Rho3halt | | Rho40
.CALL(Rho,4)

# Exit in halt state to conform with conventions.
Rho4halt B B halt
Rho4halt | | halt
