/* stremacmp SUPPORT */
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
	stremacmp

	Description:
	We compare two EMA strings.

	Synopsis:
	int stremacmp(cchar *e1p,cchar *e2p) noex

	Arguments:
	e1p		first string
	e2p		second string

	Returns:
	>0		the first is greater than the second
	0		the strings are equal
	<0		the first is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<strings.h>		/* |strcasecmp(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strnxcmp.h>		/* |strnncmp(3uc)| */
#include	<localmisc.h>
#include	<cmporders.h>

#include	"stremacmp.h"


/* local defines */

#ifndef	CHX_AT
#define	CHX_AT	'@'
#endif


/* imported namespaces */


/* local typedefs */

typedef cmporders	co ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int stremacmp(cchar *e1p,cchar *e2p) noex {
	int		rc = 0 ;
	if (e1p && e2p) {
	    cchar *t1p = strchr(e1p,CHX_AT) ;
	    cchar *t2p = strchr(e2p,CHX_AT) ;
	    if (t1p && t2p) {
	        if ((rc = strcasecmp((t1p+1),(t2p+1))) == 0) {
	            cint t1l = intconv(t1p - e1p) ;
	            cint t2l = intconv(t2p - e2p) ;
	            rc = strnncmp(e1p,t1l,e2p,t2l) ;
	        } /* end if (strcasecmp) */
	    } else {
	        rc = strcmp(e1p,e2p) ;
	    }
	} /* end if (non-null) */
	return rc ;
}
/* end subroutine (stremacmp) */

int strxemacmp(cchar *s1,cchar *s2) noex {
    	int		rc = 0 ;
	if (s1 || s2) {
	    rc = +1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    rc = stremacmp(s1,s2) ;
		}
	    }
	}
	return rc ;
} /* end subroutine (strxemacmp) */


