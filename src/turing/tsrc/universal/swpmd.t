# swpmd.t: swap mark (m) (left) with datum (d)
#
# At entry reading - in d-, where x stands for a datum [B or |]
# At exit reading - in -d.
#
# If d is not a datum, do nothing

.include global.t
 
0 - L 0    # Step left

# Record datum in state
0 B R recordB
0 | R record|

# write recorded datum over -. Enter state for left step
recordB - B mdl  
record| - | mdl

# Step left. Enter marking state
mdl B L mdm
mdl | L mdm

# Write - over d
mdm B - halt
mdm | - halt

# Do nothing if d is not a datum
0 0 R halt
0 1 R halt
0 2 R halt
0 3 R halt
0 4 R halt
0 5 R halt
0 6 R halt
0 7 R halt
0 8 R halt
0 9 R halt
0 r R halt
0 l R halt
0 * R halt
0 ; R halt
