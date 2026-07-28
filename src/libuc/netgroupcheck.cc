/* netgroupcheck SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check names against the system netgroups */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-07-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	netgroupcheck

	Description:
	This subrotuine checks some names against the UNIX® system
	set of netgroups (if there are any).

	Synopsis:
	int netgroupcheck(cchar *dname,vecstr *glp,vecstr *nlp) noex

	Arguments:
	dname		domain-name
	glp		list of system netgroups
	nlp		list of machine names

	Returns:
	>0		match (allowed)
	0		no match (not-allowed)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<netdb.h>		/* POSIXO |innetgr(3nsl)| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"netgroupcheck.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int netgroupcheck(cchar *dname,vecstr *glp,vecstr *nlp) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (dname && glp && nlp) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
	        cchar	*ngp ;
		rs = SR_OK ;
	        for (int i = 0 ; glp->get(i,&ngp) >= 0 ; i += 1) {
	            if (ngp) {
		        cchar	*mnp ;
	                for (int j = 0 ; nlp->get(j,&mnp) >= 0 ; j += 1) {
	                    if (mnp) {
			        cint	ch = mkchar(mnp[0]) ;
	                        if (! isdigitlatin(ch)) {
	                            f = innetgr(ngp,mnp,nullptr,dname) ;
	 	                    if (f) break ;
			        }
		            }
	                } /* end for (machine names) */
		    } /* end if */
		    if (f || (rs < 0)) break ;
	        } /* end for (netgroups) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (netgroupcheck) */


