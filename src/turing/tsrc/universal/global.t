# Prevent more than one include of this file 

.ifndef GLOBALT
.define GLOBALT

.alphabet 0123456789B|lr*-;

.define call ( xxx, yyy )  \.prepend xxxyyy\
\.include xxx.t\
\.prepend

.define any ( x, y, z ) x 1 y z \
x 2 y z \
x 0 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z \
x ; y z 

# assemble commands with all letters but the mark (-)
.define not- ( x, y, z ) x 1 y z\
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x B y z \
x ; y z \
x | y z \
x r y z \
x l y z \
x * y z 

.define not1 ( x, y, z ) x 2 y z \
x 0 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x B y z \
x ; y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not2 ( x, y, z ) x 1 y z \
x 0 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not3 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not4 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x B y z \
x ; y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not5 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not6 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not7 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not8 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not9 ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define notB ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x | y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define not| ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x r y z \
x l y z \
x * y z \
x - y z 

.define notr ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x l y z \
x * y z \
x - y z 

.define notl ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x * y z \
x - y z 

.define not* ( x, y, z ) x 1 y z \
x 0 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x - y z 

.define not0 ( x, y, z ) x 1 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x ; y z \
x B y z \
x | y z \
x r y z \
x l y z \
x - y z \
x * y z

.define not; ( x, y, z ) x 1 y z \
x 2 y z \
x 3 y z \
x 4 y z \
x 5 y z \
x 6 y z \
x 7 y z \
x 8 y z \
x 9 y z \
x 0 y z \
x B y z \
x | y z \
x r y z \
x l y z \
x - y z \
x * y z
.endif
