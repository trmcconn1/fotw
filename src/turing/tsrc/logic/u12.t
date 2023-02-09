# Computes the projection U(x,y) = x
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# See Mendelson, p. 235

.alphabet B|

# Move right to the first blank 
#
0 | R 0
0 B R q1

# We should now be reading the first | of y

# Loop while moving right and replacing | with blank  
#
q1 | B q2
q2 B R q1

# Enter the halt state
#
q1 B B q3

