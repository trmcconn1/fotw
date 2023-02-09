#  Rho: search right for the first blank character

.alphabet B|

# Take one step right, regardless
0 B R 1
0 | R 1

# continue right, halting on the first blank cell
1 | R 1
1 B B halt
