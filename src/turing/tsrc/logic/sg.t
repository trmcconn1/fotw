# Computes sg(x) := 0 if x = 0, 1 if x > 0
# Solution of Mendelson, 5.5
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). 
# See Mendelson, p. 235


.alphabet B|

# Move to rightmost |
0 | R 0
0 B L q1

# Move left. If it is a blank, halt in state q2 (leaving 0 on the tape)
q1 | L q2
q2 | L q3

# Continue only if x > 0: blank all | to the left 
q3 | B q4
q4 B L q3

# Halt on a blank cell in state q3 

