# doright.t: perform a move right action on the dseg. At entry we are reading
# a mark at the left end of an r/state pair in cseg. I.e. tape
# is ...sd->-rs;...*...-sd-d...  where s are state numbers and d is a datum.
# First, delete the sd in dseg, so that tape becomes 
# -rs; ... * ...->--d
# Then swap -- with d and go back to the hypen in cseg. Tape is then
# ->rs;... d--
# Then insert s before the last hyphen. Then swap last - with datum following. 
# Finish with -sdrs;... * ... ->-sd-

.include global.t

.ifdef TEST1
.tape 01|R-r23;B2BB1;B*||BB-132|-B||B
.undef TEST1
# final tape should be 0-1|r23;B2BB1;B*||BB|R-23B-||B
.endif
.ifdef TEST2
.tape 01|R-r23;B2BB1;B*||BB-132|-
.undef TEST2
# final tape should be 0-1|r23;B2BB1;B*||BB|R-23B-
.endif
.ifdef TEST3
.tape 01|R-r23;B2BB1;B*-132|-B||
.undef TEST3
# final tape should be 0-1|r23;B2BB1;B*|-23B-||
.endif

# Go right to the last -

0 - R 1
.not-(1,R,1)
1 - R 2
.not-(2,R,2)
# swap - with previous datum
2 - L ssd
ssd B - ssrB
ssrB - R sswB
sswB - B sslB
sslB B L dloop

ssd | - ssr|
ssr| - R ssw|
ssw| - | ssl|
ssl| | L dloop

# Backspace until the character is a hyphen
dloop - L dl0
.not-(dl0,R,bs00)
.call(bs,0)
.any(bs0halt,-,dloop)

# now we have -ds;...*...->--d  Swap -- with d
dl0 - R dl1
dl1 - R dl1
dl1 B - dlB
dlB - L dl2B
dl2B - L dl3B
dl3B - B 3

dl1 | - dl|
dl| - L dl2|
dl2| - L dl3|
dl3| - | 3

# Now insert another hyphen: this provides a place to copy a digit in the
# copy loop below

3 B R ins00
3 | R ins00
.call(ins,0)
ins0halt - L ins0halt
.not-(ins0halt,L,gl)

# go back to - in cseg
.not-(gl,L,gl)
# swap r with -
gl - R 4
4 r - rr
rr - L wr
wr - r 5
5 r R cloop

# Now begin big copy loop: step right, and if it is a digit swap with -, then
# Remember the digit and copy to the left of the second - in dseg.
# Then go back left to hyphen
# and re-enter loop reading -. We exit the loop with tape
# ds-->;  

cloop - R cloop
cloop ; ; cleanup
cloop 1 1 c1
cloop 2 2 c2
cloop 3 3 c3
cloop 4 4 c4
cloop 5 5 c5
cloop 6 6 c6
cloop 7 7 c7
cloop 8 8 c8
cloop 9 9 c9


.define c2block ( xx ) \
cxx xx - dxx\
dxx - L exx\
exx - xx fxx\
fxx xx R gxx\
gxx - R hxx\
\.not-(hxx,R,hxx)\
hxx - R jxx\
\.not-(jxx,R,jxx)\
jxx - xx kxx\
kxx xx R ins10

.c2block(1)
.c2block(2)
.c2block(3)
.c2block(4)
.c2block(5)
.c2block(6)
.c2block(7)
.c2block(8)
.c2block(9)

# now insert another hyphen to provide a place to copy next digit
.call(ins,1)
ins1halt - L 6

# Go back to the hyphen in cseg
.not-(6,L,6)
6 - L 8
.not-(8,L,8)
8 - - cloop   # and reenter copy loop


# cleanup: Remove 2nd hyphen in dseg. No longer needed. 
# Swap last - in dseg with datum that follows it; 
# move - in cseg back where it needs to be,
# and finish by repositioning read head reading first - in dseg

# Head for 3rd - in dseg
.not-(cleanup,R,cleanup)
cleanup - R 9
.not-(9,R,9)
9 - R bs10
# delete 2nd hyphen in dseg
.call(bs,1)
bs1halt - R 10

# swap - with following datum
10 B - cb
10 | - c|
10 0 - c0
cb - L db
db - B 11
c| - L d|
d| - | 11
c0 - L d0
d0 - B 11 # Note that moving right at the end of the simulated tape "creates"
	  # a new simulated entry, i.e, a cell filled with a B. 

# Now we must deal with - back in cseg

.not-(11,L,11)
11 - L 12
.not-(12,L,12)
# swap - with the state number 
12 - - swpms00
.call(swpms,0)
# swap - with r character
swpms0halt - L 13
13 r - 14
14 - R 15
15 - r 16
# swap - with datum
16 r L swpmd10
.call(swpmd,1)
.any(swpmd1halt,-,swpms10)
# swap - with state number
.call(swpms,1)
# Go back to first - in dseg
swpms1halt - B 17
17 B R 18
.not-(18,R,18)
18 - - halt 


