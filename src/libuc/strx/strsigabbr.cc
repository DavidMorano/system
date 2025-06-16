/* strsigabbr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return a signal abbreviation string given a signal number */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strsigabbr

	Description:
        We take a signal number and we return a corresponding signal
        abbreviation string.

	Synopsis:
	cchar *strsigabbr(uint n) noex

	Arguments:
	n		signal number to lookup

	Returns:
	-		character-string representation of signal

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>

#include	"strsigabbr.h"


/* local defines */

#define	SIGABBR		struct sigabbr


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct sigabbr {
	int		n ;
	cchar		*s ;
} ;


/* forward references */


/* local variables */

static const SIGABBR	cvts[] = {
	{ 0, "TEST" },
	{ SIGHUP, "HUP" },
	{ SIGINT, "INT" },
	{ SIGQUIT, "QUIT" },
	{ SIGILL, "ILL" },
	{ SIGTRAP, "TRAP" }, /* 5 */
	{ SIGABRT, "ABRT" },
	{ SIGEMT, "EMT" },
	{ SIGFPE, "FPE" },
	{ SIGKILL, "KILL" },
	{ SIGBUS, "BUS" }, /* 10 */
	{ SIGSEGV, "SEGV" },
	{ SIGSYS, "SYS" },
	{ SIGPIPE, "PIPE" },
	{ SIGALRM, "ALRM" },
	{ SIGTERM, "TERM" }, /* 15 */
	{ SIGUSR1, "USR1" },
	{ SIGUSR2, "USR2" },
	{ SIGCLD, "CLD" },
	{ SIGCHLD, "CHLD" },
	{ SIGPWR, "PWR" }, /* 20 */
	{ SIGWINCH, "WINCH" },
	{ SIGURG, "URG" },
	{ SIGPOLL, "POLL" },
	{ SIGSTOP, "STOP" },
	{ SIGTSTP, "TSTP" }, /* 25 */
	{ SIGCONT, "CONT" },
	{ SIGTTIN, "TTIN" },
	{ SIGTTOU, "TTOU" },
	{ SIGVTALRM, "VTALRM" },
	{ SIGPROF, "PROF" }, /* 30 */
	{ SIGXCPU, "XCPU" },
	{ SIGXFSZ, "XFSZ" },
#ifdef	SIGWAITING
	{ SIGWAITING, "WAITING" },
#endif
#ifdef	SIGLWP
	{ SIGLWP, "LWP" },
#endif
#ifdef	SIGFREEZE
	{ SIGFREEZE, "FREEZE" },
#endif
#ifdef	SIGTHAW
	{ SIGTHAW, "THAW" },
#endif
	{ SIGCANCEL, "CANCEL" },
	{ SIGLOST, "LOST" },
#if	defined(_SIGRTMIN)
	{ _SIGRTMIN, "RTMIN" },
#endif
#if	defined(_SIGRTMAX)
	{ _SIGRTMAX, "RTMAX" },
#endif
	{ -1, nullptr }
} ; /* end struct (SIGABBR) */


/* exported variables */


/* exported subroutines */

const char *strsigabbr(int n) noex {
	cchar		*s = nullptr ;

#if	defined(_SIGRTMIN) && defined(_SIGRTMAX)
	if ((n > _SIGRTMIN) && (n < _SIGRTMAX)) {
	    s = "RTXXX" ;
	}
#endif /* SIGRTXXX */

	if (s == nullptr) {
	    for (int i = 0 ; cvts[i].n >= 0 ; i += 1) {
	        if (cvts[i].n == n) {
		    s = cvts[i].s ;
		    break ;
	        }
	    } /* end for */
	}

	return s ;
}
/* end subroutine (strsigabbr) */


