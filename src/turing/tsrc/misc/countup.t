# countup.t: write all numbers on the tape in binary and in order, from left to
#            right, separated by a blank tape character (_). Once a _ has
#            been written, never again move to the left of it. 
#
#            This program runs forever, at least in principle. It 
#            demonstrates that a Turing machine is capable of generating
#            all that ever was, is, and ever will be. (So help me God.)
#
#            The method is to copy the previously generated number to the
#            right, and then add 1 to it. During the copy, we change 1 to B
#            and 0 to A to keep track of what has been copied. We also
#            use an additional marker character (X) to hold the place 
#            (temporarily) between old and new copy.

.alphabet _01ABX

# Macro
# Move to the right over previously copied stuff, and drop letter on first
# blank square

.define copy ( xxx ) \
copyxxx A R copyxxx \
copyxxx 0 R copyxxx \
copyxxx B R copyxxx \
copyxxx 1 R copyxxx \
copyxxx X R copyxxx \
copyxxx _ xxx goleft

# Entry point of program, and of main loop. Move right to blank tape.
0 A R 0
0 B R 0
0 _ X 0
0 X L goleft       # place new mark on blank square.

# Go left to first blank tape past previous number.

goleft A L goleft
goleft B L goleft
goleft 0 L goleft
goleft 1 L goleft
goleft X L goleft
goleft _ R copy    # copy number to the right (in ABAB... form), converting
                   # it back to 0s and 1s as you go.
copy 0 R copy
copy 1 R copy      # skip previously converted and copied digits
copy A 0 copyA
.copy(A)
copy B 1 copyB
.copy(B)

# When copy encounters the mark (X), change it to its permanent form (_)
# and enter states to add 1 (=B) to the copied number.

copy X _ 1
1 _ R add
add A B 0       # 0 + 1 = 1, done adding. Loop back. 
add B A 2       # 1 + 1 = 0, with carry
2 A R add
add _ B 0       # We reached right end while carrying: add extra B on right
                # The new number is written with As and Bs. They will be
                # converted to permanent form in next go around.


