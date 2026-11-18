/* cmpema SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cmpema

	Description:
	We compare two EMA strings.

	Synopsis:
	int cmpema(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		first string
	e2p		second string

	Returns:
	>0		the first is greater than the second
	0		the strings are equal
	<0		the first is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ucmpx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cmpstrnn.h"
#include	"cmpema.h"


/* local defines */

#ifndef	CHX_AT
#define	CHX_AT		'@'
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cmpema(cchar *e1p,cchar *e2p) noex {
	int		rc = 0 ;
	if (e1p && e2p) ylikely {
	    cchar *t1p = strochr(e1p,CHX_AT) ;
	    cchar *t2p = strochr(e2p,CHX_AT) ;
	    if (t1p && t2p) {
	        if ((rc = cmpstrbc((t1p+1),(t2p+1))) == 0) {
	            cint t1l = intconv(t1p - e1p) ;
	            cint t2l = intconv(t2p - e2p) ;
	            rc = cmpstrnn(e1p,t1l,e2p,t2l) ;
	        } /* end if (cmpstrbc) */
	    } else {
	        rc = cmpstr(e1p,e2p) ;
	    } /* end */
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (cmpema) */

int cmpemaø(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 || s2) ylikely {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    rc = cmpema(s1,s2) ;
		} /* end */
	    }
	} /* end if */
	return rc ;
} /* end subroutine (cmpemaø) */


