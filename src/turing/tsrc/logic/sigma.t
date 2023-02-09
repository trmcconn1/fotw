# sigma: shift machine  
# Tape effect:  B(W1)B(W2)->B  ---->  B(W2)->B...
# where W1 and W2 are any words of non-blank characters
# and -> denotes the initially scanned cell. W1 is wiped out and W2 moved
# into its place.
# See Mendelson, page 240
# 

.alphabet B|

# :Loop  
#
# Search left for first non-blank character (Lambda machine)
0 B B Lambda0 
0 | | Lambda0
.prepend Lambda
.include Lambda.t
.prepend

# take one step left
Lambdahalt | L 1
Lambdahalt B L 1

# exit loop in state 2 if blank
1 B B 2

# else print blank and shift rightward word one step left (T machine)
1 | B T0
.prepend T
.include T.t
.prepend
Thalt B B 0 # goto loop
Thalt | | 0


# out of loop in state 2
# shift rightward word one step left ( 2nd invocation of T machine )
2 B B TF0
2 | | TF0
.prepend TF  # needs to be different to avoid conflict with T0, Thalt
.include T.t
.prepend

# halt 
TFhalt B B halt
TFhalt | | halt

