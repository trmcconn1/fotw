# Computes the sum of two natural numbers x + y
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# See Mendelson, p. 235

.alphabet B|

# Zap the first |, move right to the blank between x and y and convert it
# to a |. Then zap the rightmost |
0 | B 0
0 B R q1
q1 | R q1
q1 B | q2
q2 | R q2
q2 B L q3
q3 | B q3
