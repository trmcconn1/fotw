# Solution of Mendelson, 5.4
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# See Mendelson, p. 235

# The question was: what does this machine do?
# The answer is that it computes U(x,y) = y + 2x + 1

.alphabet B|

# In the main loop, we blank out the next place in x, move right to the end
# of y and add a one there; move back to the created blank; fill, and repeat.

# skip over first 1
0 | R q1

# blank the next
q1 | B q2

# jump over the rest of x

q2 B R q3
q3 | R q3

# jump over y
q3 B R q4
q4 | R q4

# add one to the right end of y
q4 B | q5

# return to the blank created above, fill, and repeat
q5 | L q5
q5 B L q6
q6 | L q6
q6 B | 0

# We come here when we begin the loop in state 0 at the end of x
# Now we erase the blank between x and y (now y+x) 
q1 B | q7

# And return left to the front of x
q7 | L q7
q7 B R q8

# Subtract 1 from x to compensate for the join above, and quit
q8 | B q8

