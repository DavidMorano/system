/* strabbrsig SUPPORT */
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
	strabbrsig

	Description:
        We take a signal number and we return a corresponding signal
        abbreviation string.

	Synopsis:
	cchar *strabbrsig(uint n) noex

	Arguments:
	n		signal number to lookup

	Returns:
	-		character-string representation of signal

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ucodenames.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strabbrsig.h"


/* local defines */

#if	defined(_SIGRTMIN) && defined(_SIGRTMAX)
#define	F_SIGRT		1
#else
#define	F_SIGRT		0
#endif /* SIGRTXXX */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct sigabbr {
	int		n ;
	cchar		*s ;
} ; /* end struct */


/* forward references */


/* local variables */

constexpr sigabbr	cvts[] = {
	{ 0,		"TEST" },
	{ SIGHUP,	"HUP" },
	{ SIGINT,	"INT" },
	{ SIGQUIT,	"QUIT" },
	{ SIGILL,	"ILL" },
	{ SIGTRAP,	"TRAP" }, /* 5 */
	{ SIGABRT,	"ABRT" },
	{ SIGEMT,	"EMT" },
	{ SIGFPE,	"FPE" },
	{ SIGKILL,	"KILL" },
	{ SIGBUS,	"BUS" }, /* 10 */
	{ SIGSEGV,	"SEGV" },
	{ SIGSYS,	"SYS" },
	{ SIGPIPE,	"PIPE" },
	{ SIGALRM,	"ALRM" },
	{ SIGTERM,	"TERM" }, /* 15 */
	{ SIGUSR1,	"USR1" },
	{ SIGUSR2,	"USR2" },
	{ SIGCLD,	"CLD" },
	{ SIGCHLD,	"CHLD" },
	{ SIGPWR,	"PWR" }, /* 20 */
	{ SIGWINCH,	"WINCH" },
	{ SIGURG,	"URG" },
	{ SIGPOLL,	"POLL" },
	{ SIGSTOP,	"STOP" },
	{ SIGTSTP,	"TSTP" }, /* 25 */
	{ SIGCONT,	"CONT" },
	{ SIGTTIN,	"TTIN" },
	{ SIGTTOU,	"TTOU" },
	{ SIGVTALRM,	"VTALRM" },
	{ SIGPROF,	"PROF" }, /* 30 */
	{ SIGXCPU,	"XCPU" },
	{ SIGXFSZ,	"XFSZ" },
	{ SIGWAITING,	"WAITING" },
	{ SIGLWP,	"LWP" },
	{ SIGFREEZE,	"FREEZE" },
	{ SIGTHAW,	"THAW" },
	{ SIGCANCEL,	"CANCEL" },
	{ SIGLOST,	"LOST" },
	{ SIGRTMIN,	"RTMIN" },
	{ SIGRTMAX,	"RTMAX" },
	{ -1, nullptr }
} ; /* end array (cvts) */

cbool		f_sigrt		= F_SIGRT ;


/* exported variables */


/* exported subroutines */

const char *strabbrsig(int n) noex {
	cchar		*s = nullptr ;
	if_constexpr (f_sigrt) {
	    if ((n >= SIGRTMIN) && (n <= SIGRTMAX)) {
	        s = "RTXXX" ;
	    }
	} /* end if_constexpr (f_sigrt) */
	if (s == nullptr) {
	    int rs ;
	    if (cchar *rp ; (rs = ucodename_sig(n,&rp)) >= 0) {
		s = rp ;
	    } else {
	        s = "*UNK*" ;
	    }
	} /* end if */
	return s ;
} /* end subroutine (strabbrsig) */


