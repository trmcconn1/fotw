# rho: search right for the first nonzero character

.alphabet B|

# Take one step right, regardless
0 B R 1
0 | R 1

# continue right, halting on the first non-blank cell
1 B R 1
1 | | halt
