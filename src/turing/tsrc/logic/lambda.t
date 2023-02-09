# lambda: search left for the first non-blank character

.alphabet B|

# Take one step left, regardless
0 B L 1
0 | L 1

# continue left, halting on the first non-blank cell
1 B L 1
1 | | halt
