/* ucdescdeb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* extended read */

#define	CF_DEBUG	0		/* non-switchable debug printo-outs */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_recvmsge(int fd,MSGHDRD *msgp,int flags,
		int timeout,int opts) noex

	Arguments:
	fd		file descriptor
	msgp		pointer to MSG structure
	flags		option flags for the reception of MSG
	timeout		time in seconds to wait
	opts		options

	Returns:
	>=0		amount of data returned
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/socket.h>		/* POSIX® */
#include	<sys/uio.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<poll.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<usysutility.hh>	/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<deb.hh>		/* LIBU (DEBPRINTF(3u)| */

#include	"ucdescdeb.hh"

import deb ;				/* debugging (LIBU) */

/* local defines */

#define	POLLEVENTS	(POLLIN | POLLPRI)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug printo-outs */
#endif


/* imported namespaces */

using libu::snprintf ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace ucdesc {
    int ucdesc_revents(char *rbuf,int rlen,cshort re) noex {
    	int		rs = SR_BUGCHECK ;
	int		rl = 0 ; /* return-value */
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) ylikely {
	        rs = snprintf(rbuf,rlen,"%s %s %s %s %s %s %s %s %s",
	            (re & POLLIN)	? "I " : "  ",
	            (re & POLLRDNORM)	? "IN" : "  ",
	            (re & POLLRDBAND)	? "IB" : "  ",
	            (re & POLLPRI)	? "PR" : "  ",
	            (re & POLLWRNORM)	? "WN" : "  ",
	            (re & POLLWRBAND)	? "WB" : "  ",
	            (re & POLLERR)	? "ER" : "  ",
	            (re & POLLHUP)	? "HU" : "  ",
	            (re & POLLNVAL)	? "NV" : "  ") ;
	        rl = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
    } /* end subroutine (ucdesc_revents) */
} /* end namespace (ucdesc) */


