# Computes x-y := 0 if x <= y, x-y if x>y
# Solution of Mendelson, 5.6
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). A pair of numbers is represented
# as ||...|B|....| where B stands for blank 
# See Mendelson, p. 235
# There may well be a better way to do this.


.alphabet B|

# :loop (over |s in x)

# When x is zero, wipe out any y and halt in q2
0 | R  q1
q1 B R q2
q2 | B q3
q3 B R q2
q2 B B halt

# Otherwise, move right to end of x
q1 | R q4
q4 | R q4
q4 B R q5

# We should now be reading the first | of y. If y is 0, rub it out
# and halt with the remains of x left on the tape

q5 | R q6
q5 B B halt
q6 B L q7
q7 | B halt

# Otherwise, move to the right end of y
q6 | R q9
q9 | R q9

# Move back left, rubbing out the rightmost | of y
q9 B L q10
q10 | B q10

# From here, move to the leftmost | of x (all the way past y). Rub out
# leftmost |, move right, and reenter main loop at 0

q10 B L q11
q11 | L q11
q11 B L q12
q12 | L q12
q12 B R q13
q13 | B q13
q13 B R 0

