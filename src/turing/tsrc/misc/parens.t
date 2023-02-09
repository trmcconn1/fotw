# parens.t: check the initial contents of the tape to see if it contains
# a well formed (i.e. balanced) expression made only of parentheses
#
# This problem and the algorithm that solves it comes From Marvin L. Minksy,
# Computation: Finite and Infinite Machines, Prentice Hall, Englewood-Cliffs,
# 1967. It is noteworthy that this is one of the simpler problems that
# cannot be solved by a finite state machine.
#
# If the initial tape was well formed, this will leave a tape with only X...X 
# Otherwise, it will leave one or more parentheses on the tape.
#
# The algorithm moves through the expression depth first, attempting to match
# each next ) with the preceeding unmatched (. 
# 

.alphabet B()X

.ifdef TEST1
# This one is balanced
.tape (()(()()(())))
.undef TEST1
.endif
.ifdef TEST2
# This one is not -- the ninth from the left is an unmatched )
.tape (()(()())(())))
.undef TEST2
.endif

# Locate next ) going right
.define find_close fc B B end\
fc X R fc\
fc ( R fc\
fc ) ) fcok\

# Locate next ( going left
.define find_start fs B B halt\
fs X L fs\
fs ) L fs\
fs ( ( fsok 

# The method is very simple: Assume we are looking at a beginning (. Mark
# it out with an X and search right for the next closing ), which need not be
# its match. Mark that with an X and search left for the another opening (
# and repeat. If at any time searching right we meet a blank we had
# better find only one remaining ( (the matching one) going back left.
# and the parentheses match. If searching right we meet a blank we are done.
# The initial tape was well formed if and only if only B and X cells remain.


0 ( X fc
.find_close
fcok ) X fs
.find_start
fsok ( X fc

# We come here when we have reached the right end having found no ). In
# this case we search left one more time to look for straggling (
end B L end
end X L end
end ( X halt
end ) ) halt
