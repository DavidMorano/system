/* snpollflags SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of the poll-event flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

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

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

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


/* forward references */


/* local variables */

const flagstrs		fs_poll[] = {
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
} ; /* end array (fs_poll) */


/* exported variables */


/* exported subroutines */

int snpollflags(char *dbuf,int dlen,int flags) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflags ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely{
	        for (int i = 0 ; (rs >= 0) && fs_poll[i].f ; i += 1) {
	            if (flags & fs_poll[i].f) {
	                rs = ss.addstr(fs_poll[i].s) ;
		    }
	        } /* end for */
	        rs1 = ss.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snpollflags) */


