/* emaentry_getbestaddr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the "best" address out of an EMA-entry address specification */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* switchable debug print-outs */

/* revision history:

	= 1999-02-01, David A­D­ Morano
	This code was part of another subroutine and I pulled it
	out to make a subroutine that can be used in multiple places.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	emaentry_getbestaddr

  	Description:
	This subroutine extracts the "best" address out of an
	EMA-entry address specification (given in raw string form).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ema.h>			/* LICUC */
#include	<localmisc.h>		/* LICU */

#include	"emaentry_getbestaddr.h"

/* local defines */

#ifndef	EMAENT
#define	EMAENT		ema_ent
#endif


/* external subroutines */


/* local subroutines */


/* exported variables */


/* exported subroutines */

int emaentry_getbestaddr(EMAENT *ep,cchar **rpp) noex {
	int		rs = SR_OK ;
	int		cl = 0 ; /* return-value */
	cchar		*cp = nullptr ;
	if (ep && rpp) ylikely {
	    if (ep->rp && (ep->rl > 0)) {
	        cp = ep->rp ;
	        cl = ep->rl ;
	    } else if (ep->ap && (ep->al > 0)) {
	        cp = ep->ap ;
	        cl = ep->al ;
	    } /* end if */
	} /* end if (non-null) */
	*rpp = cp ;
	return (rs >= 0) ? cl : rs ;
} /* end subroutine (emaentry_getbestaddr) */


