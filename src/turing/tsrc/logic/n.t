# Computes the successor function N(x)
# A natural number n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1 (0 is |).
# See Mendelson, p. 235

.alphabet B|

0 | L q1 
q1 B | q2

