# un2bin.t: Convert unary to binary representation. The representation of
# the natural number n in unary is a block of n | characters delimited by
# B characters. We assume we are pointing at leftmost | on entry. As the
# program runs, certain of the | are replaced by X characters and the binary
# number with the usual 1 and 0 digits is written to the left of the
# original number. The final tape contains the binary representation of n
# with MSB to the left, followed by n X characters, with the read head
# scanning the B past the rightmost X.

# We use 5 states. Minsky problem 6.1-1 asks to do the same computation using
# only two states. 
# 

# Test n = 10. Answer should be 1010XXXXXXXXXX->B
.ifdef TEST
.tape 1111111111    
.undef TEST
.endif

.alphabet B|01X

#:loop   # Each iteration divides the number of remaining |s by two by marking
         # half the # |s with an X. If there is a remainder, a 1 is written 
         # as the next binary digit. Otherwise, a 0 is written.
0 | X 1  # X out the first | found
0 X R 0
1 X R 1
1 | R 0  # skip every other |

0 B L even
1 B L odd

even | L even  # Move left past Xs, |s, and the partially written binary
even X L even
even 0 L even
even 1 L even
even B 0 reset # Write a 0 as next binary digit


odd | L odd    # Move left past Xs, |s, and the partially written binary
odd X L odd
odd 0 L odd
odd 1 L odd
odd B 1 reset   # Write a 1 as next binary digit

# Move right past the partially written binary. If a B is ecountered before
# finding any |s then halt
reset 0 R reset
reset 1 R reset
reset X R reset
reset | | 0        # renter loop

