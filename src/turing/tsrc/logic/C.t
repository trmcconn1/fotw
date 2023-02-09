# C: "cleanup" machine  
# Tape effect: ~BBXW->B  --> ~W->B...B 
# where ~ is any character, B is a blank, W denotes a word of nonblank
# characters, and X denotes an arbitrary sequence of blank separated words.
# -> denotes the initially scanned cell. 
# See Mendelson, page 241 (Note that the machine diagram, figure 5.13,
# appears to be wrong. The P machine should be the right analogue of script
# L, i.e, the right-end machine, rather than Mendelson's P which merely
# finds the first blank to the right.)
# 

.alphabet B|

# :Loop  
#
# Probe to see if there is anything non blank to the left of W
# First go left to the first blank (Lambda machine)

0 B B Lambda0 
0 | | Lambda0
.prepend Lambda
.include Lambda.t
.prepend

# take one more step left
Lambdahalt | L 1
Lambdahalt B L 1

# exit loop in state 2 if blank (probe failed)
1 B B 2

# else go back to where you were and run the left shift machine (sigma)
# to move W on top of the last word of X
1 | | P0
.prepend P
.include P.t
.prepend

Phalt | | sigma0
Phalt B B sigma0
.prepend sigma
.include sigma.t
.prepend

# loop back to beginning in state 0
sigmahalt B B 0
sigmahalt | | 0



# out of loop in state 2
# The tape should now be ~->BBW
# We need to shift W left two spaces to eat up the two B's using the
# left translation (T) machine. The only awkward thing is that T leaves
# us back to the right of W

2 B B T0
2 | | T0
.prepend T
.include T.t
.prepend
Thalt B B LambdaF0 
Thalt | | LambdaF0
# Go back to left side of W
.prepend LambdaF  # don't conflict with previous prefix
.include Lambda.t
.prepend
# take one step left and repeat
LambdaFhalt | L 3
LambdaFhalt B L 3
3 B B TT0
3 | | TT0
.prepend TT       # don't conflict with previous prefix
.include T.t
.prepend
TThalt B B halt
TThalt | | halt
