# P, the "right end" machine. Initial tape should be blank separated list of
# words. It moves to the first blank to the right of the last word.
# The tape contents are not affected.

.alphabet B| 

# loop:
# Find the first blank to the left ( i.e., do the Rho machine )
0 | R 1
0 B R 1
1 | | 0

# The above "halted" on a blank square in state 1
# Now take one step to the right

1 B R 2

# If the next cell is a non-blank, we loop back to state 0. Otherwise, clean
# up and exit

2 | | 0
2 B L halt


