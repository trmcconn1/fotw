# bck.t: block check. Used by find.t to match a state/read pair in the
#        data segment with a pair in the code segment. 
#
# Assumes the tape is
#
# ...[0;]-xxxxx....[d*]->-yy...yd-...
#
# Where x and y blocks are blocks of digits [1-9] designating state numbers.
# The y block must be preceeded by data or *. The x block must be preceeded
# by 0 or ;
# The y block must be followed by datum. It does not matter how the x 
# block ends.
#
# If all this is not true about the tape, you are toast.
#
# Returns in state bck_ok or bck_no depending on whether the block following
# the first - matches the block delimited by the 2nd and 3rd -. In either
# case, the tape and read head position are restored to original state.
#

.include global.t

.ifdef TEST
.tape 05|l6B32Br4B-32||6*||BBR-32B-|
.undef TEST
.endif

# gotonext: macro to search for next occurence of the symbol sym in the
# given direction. Begin in state start, end in state end. Assumes we
# are now reading sym
.define gotonext ( dir, sym, start, end ) start sym dir startgt \
\.notsym(startgt,dir,startgt)\
startgt sym sym end

# Enter chk_ok loop: Reading 2nd -. Swap - with character to its right. 
# Go to first - and see whether character to its right matches. If
# not exit the loop reading bad character in state bad_clean. If so, swap 
# that - with the character to its right, reenter loop reading 2nd -.  
# Repeat until we read 3rd hyphen or exit loop in bad_clean.
# If we do read 3rd hyphen, exit loop in ok_clean reading that -.

# loop:
0 - R chk_ok

chk_ok - - ok_clean  # done. Exit with xxx..x- ... yy...y->-

# We need to record the character (first y) we are reading, and do most
# of steps while remembering that character in the state. So we make
# a big macro to do the parallel things for the given state number.

.define chk_ok_blk ( xx ) \
chk_ok xx xx cxx\
\.any(cxx,-,dxx)\
dxx - L exx\
exx - xx fxx\
\.not-(fxx,L,fxx)\
fxx - R gxx\
\.notxx(gxx,L,swpms20)\
gxx xx - hxx\
hxx - L jxx\
jxx - xx kxx\
kxx xx R kxx\
kxx - R backright


# Now we expand the block for each possible character except -

.chk_ok_blk(1)
.chk_ok_blk(2)
.chk_ok_blk(3)
.chk_ok_blk(4)
.chk_ok_blk(5)
.chk_ok_blk(6)
.chk_ok_blk(7)
.chk_ok_blk(8)
.chk_ok_blk(9)
.chk_ok_blk(B)
.chk_ok_blk(|)
.chk_ok_blk(*)
.chk_ok_blk(r)
.chk_ok_blk(l)
.chk_ok_blk(;)

# Move back right to 2nd -
.not-(backright,R,backright)
backright - - 0  # reenter loop reading 2nd -

# ok_clean: tape is now xxxxx- ... yyyyyd-->-. Restore tape
# and exit in bck_ok. Here we use that the yyyyd must be preceeded by a
# datum or by *, and that xxxxx must be preceeded by a B or by 0

ok_clean - L swpmd00     # back to previous -
.call(swpmd,0)           # swap - with datum d
swpmd0halt - - swpms00   # swap - with yyyy 
.call(swpms,0)
.gotonext(L,-,swpms0halt,swpmd10)  # Go to - in code seg
.call(swpmd,1) # and swap with datum 
swpmd1halt - - swpms10 # swap - with xxxxx
.call(swpms,1)
.gotonext(R,-,swpms1halt,bck_ok)  # Move read head to first - in data seg
                                  # And exit signaling successful match


# bad_clean: xx..x-->-z .... yyyy[yd]-: restore tape and postion, exit
# in bck_no.
.call(swpms,2)             # swap - with state number xxxx
.gotonext(R,-,swpms2halt,swpmd20) # move to 2nd -
.call(swpmd,2)                    # Try to swap data. Remember that swpmd does
                                  # nothing on a digit
swpmd2halt - - swpms30
.call(swpms,3)                    # Swap - with (initial part of) y
swpms3halt - - bck_no             # And exit signaling failure to match
