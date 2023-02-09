# swpms.t: swap mark (m) (left) with s, state digit(s) 
#
# At entry reading - in dxx...x-, where x stands for a digit [1-9]
# (If this assumption is wrong, you are toast.)
# At exit reading - in d-xx...x.
# d is data, action (l,r), empty tape (0) or *
#
# If xx...x does not end in a digit, do nothing

.include global.t
 
# Create quadruples by varying the start state over the digits
.ifndef alldigits
.define alldigits ( state, act, end ) state 1 act end\
state 2 act end\
state 3 act end\
state 4 act end\
state 5 act end\
state 6 act end\
state 7 act end\
state 8 act end\
state 9 act end
.endif

0 - L loop    # Step left, enter swap loop
# swap loop: swap - with last x (exit loop below when no digit to match)
loop - L loop # move 1 step left
# Record digit there in state
loop 1 R record1
loop 2 R record2
loop 3 R record3
loop 4 R record4
loop 5 R record5
loop 6 R record6
loop 7 R record7
loop 8 R record8
loop 9 R record9

# write recorded digit over -. Enter state for left step
record1 - 1 msl  
record2 - 2 msl
record3 - 3 msl
record4 - 4 msl
record5 - 5 msl
record6 - 6 msl
record7 - 7 msl
record8 - 8 msl
record9 - 9 msl

# The next two lines complete the swap of the - and final x
# Step left. Enter marking state
.alldigits(msl,L,msm)
# Mark with - and renter loop reading -
.alldigits(msm,-,loop)

# Exit if we are reading the d before xxxx (applicable if xxxx is empty, too)
loop B R halt
loop ; R halt
loop | R halt
loop l R halt
loop r R halt
loop * R halt
loop 0 R halt
