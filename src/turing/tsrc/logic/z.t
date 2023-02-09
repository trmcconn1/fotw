# Computes the zero function Z(x)
# A natural number is n represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |)
# See Mendelson, p. 235

.alphabet B|

0 | B q1 
q1 B R 0
0 B | q2

