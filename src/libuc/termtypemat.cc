/* termtypemat SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match on a terminal-type */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-07-19, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

	= 2017-12-16, David A­D­ Morano
	Updated.

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	termtypemat

	Description:
	We try to match a set of paramters to a terminal-type.

	Synopsis:
	int termtypemat(const termtype *types, cshort *pvp,cshort *svp) noex

	Arguments:
	types		pointer
	pvp		pointer
	svp		pointer

	Returns:
	==0		match
	<0		error *system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |SHORT_MIN| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<matparam.h>
#include	<localmisc.h>

#include	"termtypemat.h"
#include	"termcmd.h"


/* local defines */

#define	U	SHORT_MIN


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	isMatch(cshort *,cshort *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int termtypemat(const termtype *types, cshort *pvp,cshort *svp) noex {
	int		i ; /* used-afterwards */
	int		f = false ;
	for (i = 0 ; types[i].name != nullptr ; i += 1) {
	    if ((f = isMatch(types[i].pv,pvp)) >= 0) {
	        f = isMatch(types[i].sv,svp) ;
	    }
	    if (f >= 0) break ;
	} /* end for */
	return (f >= 0) ? i : -1 ;
}
/* end subroutine (termtypemat) */


/* local subroutines */

static int isMatch(cshort *vp,cshort *pp) noex {
	cint		n = TERMCMD_NP ;
	int		i ; /* used afterwards */
	bool		f = true ;
	for (i = 0 ; (i < 4) && (vp[i] != SHORT_MIN) ; i += 1) {
	    cint	v = vp[i] ;
	    if (v >= 0) {
		f = matparam(pp,n,v) ;
	    } else {
		f = (! matparam(pp,n,-v)) ;
	    }
	    if (! f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (isMatch) */


