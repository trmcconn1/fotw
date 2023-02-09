# Demo Turing Program 
# Write Terry on the tape, then copy it to the left of itself, erasing the
# original in the process.


.alphabet 0tery*@
.define NAME demo.t  # to illustrate defining a macro

# First write "terry" on the tape 
0 0 t 1 
1 t R 1 
1 0 e 2 
2 e R 3 
3 0 r 3 
3 r R 4 
4 0 r 4 
4 r R 5 

# Now move 1 left of bottom non-blank letter on tape

5 0 y tb0          
.prepend tb
.include tb.t     # library routine to move past left end of text
.prepend 
# mark position
tbhalt 0 @ tt0

# go back to text top
.prepend tt
.include tt.t
.prepend 

# Now copy terry to the left. Enter switch mode to select char to carry
# c_t is state for carrying a t left, etc.

tthalt 0 L sw 
sw * 0 sw   # wipe previous mark, if there
sw 0 L sw   # move down
sw t * c_t  # mark, enter state to carry t left
sw r * c_r  # etc for others
sw e * c_e
sw y * c_y  
sw @ 0 halt # we're back to start mark. Erase it and quit

c_t * L c_t
c_t t L c_t
c_t e L c_t
c_t r L c_t
c_t y L c_t
c_t @ L c_t
c_t 0 t tt0   # drop load, back to top

c_r * L c_r
c_r t L c_r
c_r e L c_r
c_r r L c_r
c_r y L c_r
c_r @ L c_r
c_r 0 r tt0    # drop load, back to top

c_e * L c_e
c_e t L c_e
c_e e L c_e
c_e r L c_e
c_e y L c_e
c_e @ L c_e
c_e 0 e tt0    # drop load, back to top

c_y * L c_y
c_y t L c_y
c_y e L c_y
c_y r L c_y
c_y y L c_y
c_y @ L c_y
c_y 0 y tt0    # drop load, back to top

# Just to illustrate use of a macro
.echo End of Source file .NAME

# end of file
