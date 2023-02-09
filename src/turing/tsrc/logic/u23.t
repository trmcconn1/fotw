# Solution of Mendelson, 5.2(a)
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# See Mendelson, p. 235

# The question was: what does this machine do?
# The answer is that it computes U(x1,x2,x3) = x2.

.alphabet B|

# Zap all initial | s rightwards to the first B 
#
0 | | q1
0 B R q1
q1 | B 0

# Found blank between vars? Move right and begin new behaviour 
q1 B R q2

# We should now be reading the first | of x2 
# Skip over x2 and on to beginning of x3 and begin new behaviour
q2 | R q2
q2 B R q3


# Zap x3
q3 | B q4
q4 B R q3

# we halt on blank while in state q3, i.e. at end of x3

