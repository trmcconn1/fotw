/* stty.c - fake sgtty interface using Minix 2.0 termios interface 
 *
 *   Why would anyone want such a thing? This is a hack to get C-Kermit
 *   working on minix386 for version 2.0 of the kernel. This version is
 *   Posix compliant, and one should be able to use termios, but this
 *   would require extensive rewriting of the MINIX part of ckutio.c. 
 *   Thus, the main purpose of this file is to fake an implementation of
 *   stty and gtty. It is based upon the "inverse function" written by
 *   Magnus Doell and Bruce Evans.
 *
 *           By Terry McConnell (trmcconn@syr.edu)
 *           (Actually, I did almost nothing -- this basically just amounts
 *            to swapping the code in Doell & Evans' set and get routines.)
 *
 */

#include <termios.h>

#ifdef _MINIX


/* Undefine everything that clashes with sgtty.h. */
#undef B0
#undef B50
#undef B75
#undef B110
#undef B134
#undef B150
#undef B200
#undef B300
#undef B600
#undef B1200
#undef B1800
#undef B2400
#undef B4800
#undef B9600
#undef B19200
#undef B28800
#undef B38400
#undef B57600
#undef B115200
/* Do not #undef CRMOD. We want a warning when they differ! */
#undef ECHO
/* Do not #undef XTABS. We want a warning when they differ! */

/* Redefine some of the termios.h names just undefined with 'T_' prefixed
 * to the name.  Don't bother with the low speeds - Minix does not support
 * them.  Add support for higher speeds (speeds are now easy and don't need
 * defines because they are not encoded).
 */
#define T_ECHO		000001

#include <errno.h>
#include <sgtty.h>

static _PROTOTYPE( int tc_to_sg_speed, (speed_t speed) );
static _PROTOTYPE( speed_t sg_to_tc_speed, (int speed) );
#define B19200   192


static speed_t sg_to_tc_speed(speed)
int speed;
{
    /* The speed encodings in sgtty.h and termios.h are different.  Both are
     * inflexible.  Minix doesn't really support B0 but we map it through
     * anyway.  It doesn't support B50, B75 or B134.
     */
    switch (speed) {
	case B0: return 0;
	case B110: return 110;
	case B200: return 200;
	case B300: return 300;
	case B600: return 600;
	case B1200: return 1200;
	case B1800: return 1800;
	case B2400: return 2400;
	case B4800: return 4800;
	case B9600: return 9600;
	case B19200: return 19200;
#ifdef B28800
	case B28800: return 28800;
#endif
#ifdef B38400
	case B38400: return 38400;
#endif
#ifdef B57600
	case B57600: return 57600;
#endif
#ifdef B115200
	case B115200: return 115200;
#endif
	default: return (speed_t)-1;
    }
}

static int tc_to_sg_speed(speed)
speed_t speed;
{
    /* Don't use a switch here in case the compiler is 16-bit and doesn't
     * properly support longs (speed_t's) in switches.  It turns out the
     * switch is larger and slower for most compilers anyway!
     */
    if (speed == 0) return 0;
    if (speed == 110) return B110;
    if (speed == 200) return B200;
    if (speed == 300) return B300;
    if (speed == 600) return B600;
    if (speed == 1200) return B1200;
    if (speed == 1800) return B1800;
    if (speed == 2400) return B2400;
    if (speed == 4800) return B4800;
    if (speed == 9600) return B9600;
    if (speed == 19200) return B19200;
#ifdef B28800
    if (speed == 28800) return B28800;
#endif
#ifdef B38400
    if (speed == 38400) return B38400;
#endif
#ifdef B57600
    if (speed == 57600) return B57600;
#endif
#ifdef B115200
    if (speed == 115200) return B115200;
#endif
    return -1;
}

int gtty(int filedes, struct sgttyb *sgbuf)
{
    struct termios termiosb;

    if (tcgetattr(filedes, &termiosb) < 0)
    {
	return -1;
    }

    /* Minix sgtty flags:

	CRMOD
	TANDEM
	XTABS
	RAW
	CBREAK
	ODDP
	EVENP
	BITS5
	BITS6
	BITS7
	BITS8
		*/

    /* Minix termios input flags:
     *   BRKINT:  forced off (break is not recognized)
     *   IGNBRK:  forced on (break is not recognized)
     *   ICRNL:   set if CRMOD is set and not RAW (CRMOD also controls output)
     *   IGNCR:   forced off (ignoring cr's is not supported)
     *   INLCR:   forced off (mapping nl's to cr's is not supported)
     *   ISTRIP:  forced off (should be off for consoles, on for rs232 no RAW)
     *   IXOFF:   forced off (rs232 uses CTS instead of XON/XOFF)
     *   IXON:    forced on if not RAW
     *   PARMRK:  forced off (no '\377', '\0', X sequence on errors)
     * ? IGNPAR:  forced off (input with parity/framing errors is kept)
     * ? INPCK:   forced off (input parity checking is not supported)
     */

    sgbuf->sg_flags = 0;
    if(termiosb.c_oflag & OPOST) 
		if(termiosb.c_oflag & XTABS)
			sgbuf->sg_flags |= XTABS;

    if( !(termiosb.c_lflag & ICANON) ) {        
	if(  termiosb.c_lflag & ISIG)
		sg_buf->sg_flags |= CBREAK;
        else sg_buf->sg_flags |= RAW;
    }

    if( termiosb.c_iflag & INCRNL )
	sgbuf->sg_flags |= CRMOD;

    if( termiosb.c_cflag &  PARODD)
	sg_buf->sg_flags |= ODDP;
    else
	if( termiosb.c_cflag & PARENB )
		sg_buf->sg_flags |= EVENP;

    if( termiosb.c_lflag & T_ECHO )
		sg_buf->sg_flags |= ECHO;


    /* Minix output flags:
     *   OPOST:   set if CRMOD or XTABS is set
     *   XTABS:   copied from sg_flags
     *   CRMOD:	  copied from sg_flags
     */

    /* Minix local flags:
     *   ECHO:    set if ECHO is set
     *   ECHOE:   set if ECHO is set (ERASE echoed as error-corecting backspace)
     *   ECHOK:   set if ECHO is set ('\n' echoed after KILL char)
     *   ECHONL:  forced off ('\n' not echoed when ECHO isn't set)
     *   ICANON:  set if neither CBREAK nor RAW
     *   IEXTEN:  forced off
     *   ISIG:    set if not RAW
     *   NOFLSH:  forced off (input/output queues are always flushed)
     *   TOSTOP:  forced off (no job control)
     */

    /* Minix control flags:
     *   CLOCAL:  forced on (ignore modem status lines - not quite right)
     *   CREAD:   forced on (receiver is always enabled)
     *   CSIZE:   CS5-CS8 correspond directly to BITS5-BITS8
     *   CSTOPB:  set for B110 (driver will generate 2 stop-bits than)
     *   HUPCL:   forced off
     *   PARENB:  set if EVENP or ODDP is set
     *   PARODD:  set if ODDP is set
     */

    switch (termiosb.c_cflag & CSIZE)
    {
	case CS5:   sgbuf->sg_flags |= BITS5; break;
	case CS6:   sgbuf->sg_flags |= BITS6; break;
	case CS7:   sgbuf->sg_flags |= BITS7; break;
	case CS8:   sgbuf->sg_flags |= BITS8; break;
    }

    /* Minix may give back different input and output baudrates,
     * but only the input baudrate is valid for both.
     */

    sgbuf->sg_ispeed =
		tc_to_sg_speed((unsigned char) termiosb.c_ispeed);
    sgbuf->sg_ospeed =
		tc_to_sg_speed((unsigned char) termiosb.c_ospeed);

    /* Minix control characters correspond directly except VSUSP and the
     * important VMIN and VTIME are not really supported.
     */

    sgbuf->sg_erase = termios_p.c_cc[VERASE];
    sgbuf->sg_kill = termios_p.c_cc[VKILL]; 

    return 0;
}

int stty(filedes,  struct sgttyb *sgbuf)
{
    struct termios termiosb;
    int tcspeed;

    if ((sgbuf->sg_ispeed != 0 && sgbuf->sg_ispeed != sgbuf->sg_ospeed)
	|| (tcspeed = sg_to_tc_speed(sgbuf->sg_ospeed)) < 0)
    {
	errno = EINVAL;
	return -1;
    }

/* Fill termios with what is there right now. Hopefully much of it will be
   reasonable. */

    tcgetattr(filedes, &termiosb);

    termiosb.c_ispeed = termiosb.c_ospeed = tcspeed;

    if(!(sgbuf->sg_flags & RAW))
	{
	 	termiosb->c_iflag |= IXON;
		if(sgbuf->sg_flags & CRMOD)
			termiosb->c_iflag |= INCRNL;
	}

     termiosb.c_oflag = sgbuf->sg_flags & (CRMOD | XTABS);
     if( termiosb.c_oflag)
		termiosb.c_oflag |= OPOST;

     termiosb.c_lflag = 0;
     if(sgbuf->sg_flags & ECHO)
		termiosb.c_lflag |= T_ECHO | ECHOE | ECHOK;
     if(!(sgbuf->sg_flags & RAW)){
		termiosb.c_lflag |= ISIG;
		if(!(sgbuf->sg_flags & CBREAK))
			termiosb.c_lflag |= ICANON;
     }

     termiosb.c_cflag = CLOCAL | CREAD;
     switch(sgbuf->sg_flags & BITS8){
	case BITS5: termiosb.c_cflag |= CS5; break;
	case BITS6: termiosb.c_cflag |= CS6; break;
	case BITS7: termiosb.c_cflag |= CS7; break;
	case BITS8: termiosb.c_cflag |= CS8; break;
     }
     if(sgbuf->sg_flags & ODDP)
	termiosb.c_cflag |= PARENB | PARODD;

     if(sgbuf->sg_flags & EVENP)
	termiosb.c_cflag |= PARENB;

     if(sgbuf->sg_ispeed == B110)
	termiosb.c_cflag |= CSTOPB;


     termiosb.c_cc[VERASE] = sgbuf->sg_erase;
     termiosb.c_cc[VKILL] = sgbuf->sg_kill;

     /* Make the change */

     if(tcsetattr(filedes, TCSADRAIN, &termiosb) < 0)
	return -1;
     return 0;
}

