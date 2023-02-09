# A universal turing machine. 
#
# This file implements the main loop of the
# machine, which searches for the next instruction and carries it out.
# The ugly details of searching and simulating the action of an instruction
# by rewriting the tape are in various included files.
#
# The tape contains instructions and data for the machine to be simulated.
# The simulated machine uses B| as its alphabet. (d below stands for data
# of the form B or |)
#
# The tape for the simulated machine always has the form
#
# ..... instructions ...*ddd...-xxx...xd-ddddd...d0....
#
# The instruction area is always separated from the data area by a *.
# We shall refer to the part before * as the code segment (cseg) and
# the part following the * as the data segment (dseg.)
# The cseg contains a list of quadruples of the form sdcs,
# where s is a state number (sequence of 1-9 digits), d is | or B, c is action 
# | B l or r. Each instruction is separated from the next by a the two
# character sequence ;B. (The first instruction may be preceeded only by a B.) 
# The semicolon functions as a command separator. The B is, strictly speaking,
# a luxury. It gives a place to put markers while searching for the next
# command without having to insert the marker. (Insertion is a very expensive
# operation, and we have tried to avoid it at all costs!)
# The block marked x in the dseg gives the current state of the simulated
# machine.  Its position precedes the current position of the read head for
# the simulated machine. The current state number and datum in the
# data area are always bracketed with the - character. This is, strictly
# speaking, not necessary, as one could locate the bracketed sequence by
# searching for digits in the dseg, but it makes the programming of
# the universal machine simpler. A hyphen also replaces the B preceeding the 
# current instruction in the cseg, except at startup. 
#

.include global.t  # alphabet and macros

# "Boot" code: We assume the initial tape has the form
# ... instructions ... *-1d-d.... 
# As mentioned above, the hyphens are used as markers by various routines.
# Assuming their presence at the beginning means we don't have to bother
# inserting them ourself. The universal machine's read head is at the left
# end of the instructions, so to get the ball rolling all we have to do
# is search right for the * and then position the read head at the following
# hyphen. Note the initial state for the simulated machine is 1, not 0, as
# 0 is used to mark blank tape for the universal machine.
.not*(0,R,0)
0 * R find0

# loop:

.prepend find
.include find.t  # find next command in instruction area
.prepend

# We are reading state number of the next quadruple to
# be executed. We need to see what action (left, right, or write) that
# is called for. In so doing, we swap the hyphen right until it lies one
# cell to the left of the action letter (the action routines expect this.)
# Begin by swapping the hyphen with successive digits of the state number

findhalt 1 - r1  # r1 for remember 1
findhalt 2 - r2
findhalt 3 - r3
findhalt 4 - r4
findhalt 5 - r5
findhalt 6 - r6
findhalt 7 - r7
findhalt 8 - r8
findhalt 9 - r9
r1 - L c1        # c1 for copy 1
r2 - L c2
r3 - L c3
r4 - L c4
r5 - L c5
r6 - L c6
r7 - L c7
r8 - L c8
r9 - L c9
c1 B 1 1        # Write remembered digit in place of mark (or B first time)
c2 B 2 1
c3 B 3 1
c4 B 4 1
c5 B 5 1
c6 B 6 1
c7 B 7 1
c8 B 8 1
c9 B 9 1
c1 - 1 1        # Write remembered digit in place of mark
c2 - 2 1
c3 - 3 1
c4 - 4 1
c5 - 5 1
c6 - 6 1
c7 - 7 1
c8 - 8 1
c9 - 9 1
# step right to the hyphen and reenter loop
.any(1,R,2)           
2 - R findhalt

findhalt B - rB # when we hit data, we have finished the swap. Now swap
                # - with data
findhalt | - r|
rB - L wB
r| - L w|
wB - B 3
w| - | 3
3 B R 4
3 | R 4
# Now peek right to see what the action is and do it!!!
4 - R 5
5 B L dowrite0
5 | L dowrite0
5 r L doright0
5 l L doleft0

# All of these finish in their halt state pointing at the - . To close
# the loop we merely move to state find 0
.prepend dowrite
.include dowrite.t
.prepend
.prepend doright
.include doright.t
.prepend
.prepend doleft
.include doleft.t
.prepend

dowritehalt - - find0
dorighthalt - - find0
dolefthalt  - - find0

