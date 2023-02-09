# S: search for a nonblank character on the tape and stop there, if found.
# The tape is not altered
# See problem 5.9 in Mendelson.
# 
# If the tape is blank, the search will last forever

.alphabet B|

# Move one step right
0 B R begin
0 | R begin

# Quit if we found |, since no match. Else enter main loop.
begin B B loop

# Main loop: assume we are at the left end of a run of blanks which we
# regard as currently searched. We maintain our own marks at both ends. The
# mechanism is: go to an end and erase the mark there. Check the next
# "outward" cell. If non blank, exit via a cleanup routine. Else mark
# and go the other end of the block. Repeat. The searched block increases until
# we bump into an unmarked non-blank cell.

loop B L 1
loop | L 1

# When the new letter is |, bail out of the loop
1 | | loop_exit1

# otherwise, mark the new left end of our block of blanks
# and search right for the end of the blank block. 
1 B | rho00
.prepend rho0
.include rho.t
.prepend
# clean up previous mark at right end and consider next letter outwards
rho0halt | B rho0halt
rho0halt B R 2

# When the new letter is |, bail out of loop
2 | | loop_exit2

# Otherwise mark the new right end, and search back left, beginning the
# Loop anew after cleaning up our previous mark

2 B | lambda00
.prepend lambda0
.include lambda.t
.prepend
lambda0halt | | loop

# We come here when we found a nonblank at the left end of the blank block
# We need to go back to the right of the block and erase the mark
# Then return to the previously found cell and exit
loop_exit1 | R rho10
.prepend rho1
.include rho.t
.prepend
rho1halt | B lambda10
.prepend lambda1
.include lambda.t
.prepend

lambda1halt B B halt
lambda1halt | | halt

# We come here when we found a nonblank at the right end of the blank block
# We need to go back to the left end of the block and erase the mark, then
# return here to point to the found cell
loop_exit2 | | lambda20
.prepend lambda2
.include lambda.t
.prepend

lambda2halt | B rho20
.prepend rho2
.include rho.t
.prepend
rho2halt B B halt
rho2halt | | halt

