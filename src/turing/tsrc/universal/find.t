# find.t: find matching state/data pair in cseg
#
# At entry: Reading leading - in - delimited block in dseg. Find matching 
# block in cseg after the mark (-) there. The search moves left, and if
# it encounters blank tape, we bring everything to a halt


.include global.t


# Do first:
0 - L f1        # left to final B in cseg  
.not*(f1,L,f1)
f1 * L f2        # Mark final B in cseg         
f2 B - f2 
f2 - R f3
.not-(f3,R,f3)   # Go back to - in dseg and enter loop
f3 - - loop

# Loop: move back to next previous command in cseg and check whether it 
# matches. If not continue.

loop - L 1        
.not-(1,L,1)
1 - B 2
2 B L 3
3 1 L 3
3 2 L 3
3 3 L 3
3 4 L 3
3 5 L 3
3 6 L 3
3 7 L 3
3 8 L 3
3 9 L 3
3 | L 3
3 r L 3
3 l L 3
3 ; L 3

	          # An annoyance is that a B can also occur as the action or
	          # or cell contents in a command.
                  # a command. If so, it is preceeded by data or digits, as 
                  # opposed to a semicolon.
3 B L chk1
chk1 | L 3           # Nope, it is data.
chk1 B L 3
chk1 1 L 3
chk1 2 L 3
chk1 3 L 3
chk1 4 L 3
chk1 5 L 3
chk1 6 L 3
chk1 7 L 3
chk1 8 L 3
chk1 9 L 3
chk1 ; R chk2
chk1 0 R chk2

chk2 B - 4           # Put mark in place of command separator B
4 - R 5
.not-(5,R,5)  # Now back to hyphen in dseg and run block check (bck)
5 - - bck00
.call(bck,0)
bck0bck_no - - loop  # Reenter loop 

# We come here only when a match has been found. Cleanup by 
# moving back and changing the previous - (marking the
# the found command) to B, and pointing the read-head at the beginning
# of the found command

bck0bck_ok - L 6
.not*(6,L,6)
6 * L 7
.not-(7,L,7)
7 - B 8
8 B R halt       # Now pointing at next command

