#include<stdlib.h>
#include<stdio.h>
#include "/NextDeveloper/Headers/bsd/curses.h"

int
main()
{

	initscr();
	erase();
	refresh();
	endwin();
	return 0;
}
