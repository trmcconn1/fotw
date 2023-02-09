# test.t: test addition machine simulation

.include global.t

# The following tape runs addition 2 + 3 = 5
# setup tape
.tape B1|B1;B1Br2;B2|r2;B2B|3;B3|r3;B3Bl4;B4|B4;B*-1|-||B||||

.include universal.t
