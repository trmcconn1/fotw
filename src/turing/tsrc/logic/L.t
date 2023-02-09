# L, the "left end" machine. Initial tape should be blank separated list of
# words. It moves to the first blank to the left of the first word.
# The tape contents are not affected.

.alphabet B| 

# loop:
# Find the first blank to the left ( i.e., do the Lambda machine )
0 | L 1
0 B L 1
1 | | 0

# The above "halted" on a blank square in state 1
# Now take one step to the left

1 B L 2

# If the next cell is a non-blank, we loop back to state 0. Otherwise, clean
# up and exit

2 | | 0
2 B R halt


