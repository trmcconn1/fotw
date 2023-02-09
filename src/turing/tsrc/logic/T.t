# T: Left translation machine  
# Tape effect:  ->*BWB  ---->  *W->BB
# where * stands for any character, W for any word of non-blank (B) characters
# and -> denotes the initially scanned cell
# See Mendelson, page 240
# 

.alphabet B|

# :Loop  (moves contents of rightward cell one cell to the left)
#
# Take two steps to the right
0 B R 1
0 | R 1
1 B R 2
1 | R 2

2 B B 4 # exit loop in state 4 if blank
2 | L 3 # else copy the non-blank character
3 B | 0 # left and
3 | | 0 # re-enter loop


# Out of loop
4 B L 5 # step left
4 | L 5

# Write trailing blank
5 | B halt
5 B B halt
