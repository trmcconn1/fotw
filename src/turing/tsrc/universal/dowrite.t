# dowrite.t: perform a writing action on the dseg. At entry we are reading
# a mark at the left end of a datum/state quadruple in cseg. I.e. tape
# is ...sd->-ds;...*...-sd-...  where s are state numbers and d is a datum.
# First, delete the sd, so that tape becomes 
# ->-ds; ... * ...--
# Then insert s before the last hyphen. Then insert d before the last hyphen.
# Finish with -sdds;... * ... ->-sd-

.include global.t

.ifdef TEST1
.tape 01|R-B23;B2BB1;B*||BB-132|-B||B
.undef TEST1
# final tape should be 0-1|B23;B2BB1;B*||BBR-23B-B||B
.endif
.ifdef TEST2
.tape 01|R-B1;B2BB1;B*-1|-B||B
.undef TEST2
# final tape should be 0-1|B23;B2BB1;B*-1B-B||B
.endif
.ifdef TEST3
.tape 01|R-B23;B2BB1;B*||BB-132|-
.undef TEST3
# final tape should be 0-1|B23;B2BB1;B*||BB-23B-
.endif


# Go left to the last -

0 - R 1
.not-(1,R,1)
1 - R 2
.not-(2,R,2)
2 - - dloop

# Backspace until the character is a hyphen
dloop - L dl0
.not-(dl0,R,bs00)
.call(bs,0)
.any(bs0halt,-,dloop)

# now we have -ds;...*...->--
# Insert another - to be ready for copy block below
dl0 - - ins00
.call(ins,0)

# go back to - in cseg
ins0halt - L ins0halt
.not-(ins0halt,L,3)
.not-(3,L,3)
# swap d with -
3 - R 4
4 | - r|
4 B - rB
r| - L w|
rB - L wB
w| - | 5
wB - B 5
5 | R cloop
5 B R cloop

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

.define c3block ( xx ) \
cxx xx - dxx\
dxx - L exx\
exx - xx fxx\
fxx xx R gxx\
gxx - R hxx\
\.not-(hxx,R,hxx)\
hxx - R jxx\
\.not-(jxx,R,jxx)\
jxx - xx kxx\
kxx xx R 6

.c3block(1)
.c3block(2)
.c3block(3)
.c3block(4)
.c3block(5)
.c3block(6)
.c3block(7)
.c3block(8)
.c3block(9)

# now insert another hyphen to provide a place to copy next digit
6 - - ins10
.call(ins,1)
ins1halt - L 7

# Go back to the hyphen in cseg
.not-(7,L,7)
7 - L 8
.not-(8,L,8)
8 - - cloop   # and reenter copy loop


# cleanup: move - in cseg back where it needs to be, copy datum to the
# leftover - in dseg, and finish by repositioning read head

# Go back to see what the datum was
cleanup ; L cleanup
cleanup - L cleanup
cleanup 1 L cleanup
cleanup 2 L cleanup
cleanup 3 L cleanup
cleanup 4 L cleanup
cleanup 5 L cleanup
cleanup 6 L cleanup
cleanup 7 L cleanup
cleanup 8 L cleanup
cleanup 9 L cleanup
cleanup B B cb
cleanup | | c|

# Copy the remembered datum over the second - in dseg - it remains for this
# purpose.

.not-(cb,R,cb)
cb - R db
.not-(db,R,db)
db - R eb
.not-(eb,R,eb)
eb - B 10


.not-(c|,R,c|)
c| - R d|
.not-(d|,R,d|)
d| - R e|
.not-(e|,R,e|)
e| - | 10

# Now we must deal with - back in cseg

.not-(10,L,10)
10 - L 11
.not-(11,L,11)
# swap it with the state number
11 - - swpms00
.call(swpms,0)
.any(swpms0halt,-,swpmd00)
# swap it with datum
.call(swpmd,0)
.any(swpmd0halt,-,swpmd10)
# swap it with datum
.call(swpmd,1)
.any(swpmd1halt,-,swpms10)
# swap it state number: now back at beginning of current quadruple
.call(swpms,1)
# Go back to first - in dseg
swpms1halt - B 12
12 B L 13
.not-(13,R,13)
13 - - halt 


