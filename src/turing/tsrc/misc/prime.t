# prime.t: check whether unary number N is prime
# N is represented as N consecutive | characters, and at entry we are
# reading the leftmost |
#
# Program halts in state prime if | is prime, otherwise in not_prime
#
# The algorithm marks the first k |s with X so that this initial block
# can be used as a trial divisor. (The main loop of the program is over
# values of k from 2 up to N.) For example, suppose the loop starts with
# XXX||...|, so that the trial divisor is 3. The idea is to repeatedly
# copy the trial block rightward until it either exactly covers N (not prime)
# or spills past the right end (possibly prime.) In the course of this
# copying, various markers must be used to keep track of things. The actual
# course of events is:
#
# XXX||...|  --> XXXBBB|||...| --> XXXYYYMMM||...| --> XXX|||BBB||...|
#   
# and now repeat, moving the B block right, etc. When we reach the end
# in the possibly prime case, we go back restoring all but the initial Xs
# to |, and then mark another X for the next trial divisor

.alphabet 0|BXYM

.ifdef TEST1
# 11
.tape |||||||||||
.undef TEST1
.endif
.ifdef TEST2
# 12
.tape ||||||||||||
.undef TEST2
.endif

# (Entry point of program is further below )
# copy the immediately leftward block of B characters to the right, replacing
# them with M. There results YY..YMMM...M with the Ys marking the position
# of the original block of Bs. When done copying, we position back past the
# right end, changing the Ys back to |s and Ms to Bs in preparation for
# another round.
# If during the copy we try to copy over a 0 (indicating the end of N) then
# enter a reset state which restores all Bs, Ms, and Ys to |s in preparation
# for the next trial divisor.

cb0 | L nxtltr      
nxtltr B Y grow_right      # find the next B to the left and mark it
nxtltr | | copy_done
nxtltr X X copy_done
nxtltr M L nxtltr          # used on return for next letter
nxtltr Y L nxtltr          # used on return for next letter
grow_right Y R grow_right  # pass previously marked Bs
grow_right M R grow_right  # pass previously marked |s
grow_right 0 0 reset
grow_right | M nxtltr      # replace next | with M and go back for next B
copy_done X R copy_done
copy_done | R copy_done    # Move back to right end restoring Ys to |s and
copy_done Y | copy_done    # Ms to Bs as you go
copy_done M B conclude
conclude M B conclude
conclude B R conclude
conclude | | cb_halt
conclude 0 0 cb_halt

# Similar to above, only copy initial block of Xs right as Bs

cx0 | L xnxtltr      
xnxtltr X Y xgrow_right      # find the next B to the left and mark it
xnxtltr 0 0 reposition
xnxtltr B L xnxtltr          # used on return for next letter
xnxtltr Y L xnxtltr          # used on return for next letter
xgrow_right Y R xgrow_right  # pass previously marked Bs
xgrow_right B R xgrow_right  # pass previously marked |s
xgrow_right 0 0 prime        # If this happens k > N/2 and, since we are still
                             # running, the conclusion must be that N is prime
xgrow_right | B xnxtltr      # replace rightmost | with B and go back for next
reposition 0 R reposition         
reposition Y X reposition
reposition X R reposition
reposition B R reposition
reposition | | cx_halt

# Handle trivial case N = 1, and otherwise mark first two 
# | with X, reading the following character

# Entry point of program
0 | X 0
0 X R 1
1 0 0 not_prime   # We do not deem 1 to be prime
1 | X 1
1 X R main_loop

# Main loop: see if the length of the X'd block divides N. Exit loop in
# not_prime if so, otherwise X out the next digit and repeat.   

main_loop 0 0 prime # This should only apply when N is 2
main_loop | | cx0   # do cx block above to copy the x block right as Bs
cx_halt 0 0 sub_loop
cx_halt | | sub_loop

# sub_loop: repeatedly copy the block of Bs right as Ms, cleaning up
# afterward by changing the Ms to Bs and the Ys (produced from the original
# Bs) as |s

sub_loop 0 0 not_prime
sub_loop | | cb0
cb_halt 0 0 sub_loop
cb_halt | | sub_loop

# reset: Entered when we encounter the end of N while copying a block. This
# will eventually happen when the block length does not divide N
# Go left restoring Ms Bs and Ys to |s

reset 0 L reset
reset | L reset
reset M | reset
reset Y | reset
reset B | reset
reset X R restart   # Increment X block for next trial divisor
restart | X restart
restart X R main_loop

