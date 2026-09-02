/* cmptimeval SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string field comparisons */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmptimeval

	Description:
	Compare ITIMER types (using either TIMEVAL or TIMESPEC).


	Synopses:
	int cmpitermerval	(CITIMERVAL *e1p,CITIMERVAL *e2p) noex
	int cmpitermerspec	(CITIMERSPEC *e1p,CITIMERSPEC *e2p) noex

	Arguments:
	e1p		pninter to entry-1
	e2p		pninter to entry-2

	Returns:
	>=0		greater-than
	==0		equal-to
	<0		less-than

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cmptimeval.h"		/* for |TIMEVAL| */
#include	"cmptimespec.h"		/* for |TIMESPEC| */
#include	"cmpitimer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpitimerval(CITIMERVAL *e1p,CITIMERVAL *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
		if (e2p) {
	    	    rc = cmptimeval(&e1p->it_value,&e2p->it_value) ;
		} /* end if */
	    } /* end if */
	} /* end if */
	return rc ;
} /* end subroutine (cmpitimerval) */


