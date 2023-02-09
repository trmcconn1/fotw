# Solution of Mendelson, 5.2(b)
# A natural number x = n is represented on the tape as a sequence |||...| where
# the number of | characters = n+1. (0 is |). The pair x y is represented
# on the initial tape as x+1 |'s followed by a blank(B) followed by y+1 |'s.
# See Mendelson, p. 235

# The question was: what does this machine do?
# The answer is that it computes (x-1)+ which is known as delta(x)

.alphabet B|

0 | B 1
1 B R 2
2 B | 2
