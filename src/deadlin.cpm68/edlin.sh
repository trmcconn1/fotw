#!/bin/sh
#
#
#  edlin: Shell script to launch deadlin.
#  By Terry R. McConnell
#  Example shell script to start deadlin using a custom key map
#  This one works for linux
#

# Change to the appropriate path if anything gets moved:
DEADLIN=deadlin
LINUXMAP=keymaps/linux.map    # keymap for linux console
XRESOURCES=keymaps/Xresources # key mappings for xterm

case $TERM in
	linux) loadkeys $LINUXMAP  2>/dev/null
               $DEADLIN $*
               loadkeys /usr/lib/kbd/keytables/custom.map  2>/dev/null
		;;

	xterm)  xrdb -merge $XRESOURCES
		# Sorry about these colors. I like 'em.
     		xterm -cr yellow -bg maroon -bd Plum1 -title \
			"Deadlin: an edlin clone" -e $DEADLIN $*
		;;
	*) $DEADLIN $*      # Punt
		;;
esac
exit
