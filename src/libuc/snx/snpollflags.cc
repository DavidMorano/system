/* snpollflags SUPPORT */
/* lang=C++20 */

/* make string version of the poll-event flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snpollflags

	Description:
	Ths subroutine creates in the result string a symbolic
	representation of poll-event flags.

	Synopsis:
	int snpollflags(char *dbuf,int dlen,int flags) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	flags		poll-event-flags

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>
#include	<poll.h>
#include	<cstring>
#include	<usystem.h>
#include	<storebuf.h>

#include	"snflags.h"
#include	"snx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct flagstrs {
	int		f ;
	cchar		*s ;
} ;


/* foward references */


/* local variables */

static const struct flagstrs	fs_poll[] = {
	{ POLLIN, "IN" },
	{ POLLOUT, "OUT" },
	{ POLLERR, "ERR" },
	{ POLLHUP, "HUP" },
	{ POLLNVAL, "NVAL" },
#ifdef	POLLPRI
	{ POLLPRI, "PRI" },
#endif
#ifdef	POLLRDNORM
	{ POLLRDNORM, "RDNORM" },
#endif
#ifdef	POLLWRNORM
	{ POLLWRNORM, "WRNORM" },
#endif
#ifdef	POLLRDBAND
	{ POLLRDBAND, "RDBAND" },
#endif
#ifdef	POLLWRBAND
	{ POLLWRBAND, "WRBAND" },
#endif
#ifdef	POLLREMOVE
	{ POLLREMOVE, "REMOVE" },
#endif
	{ 0, NULL }
} ;


/* exported variables */


/* exported subroutines */

int snpollflags(char *dbuf,int dlen,int flags) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) {
	    snflags	ss ;
	    if ((rs = snflags_start(&ss,dbuf,dlen)) >= 0) {
	        for (int i = 0 ; (rs >= 0) && fs_poll[i].f ; i += 1) {
	            if (flags & fs_poll[i].f) {
	                rs = snflags_addstr(&ss,fs_poll[i].s) ;
		    }
	        } /* end for */
	        rs1 = snflags_finish(&ss) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snpollflags) */


