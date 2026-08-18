/* calcite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calendar citation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	calcite

	Description:
	This small object just holds a citation for a calendar entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intcmp.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"calcite.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int calcite_load(calcite *ep,int y,int m, int d) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->y = ushort(y) ;
	    ep->m = uchar(m) ;
	    ep->d = uchar(d) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (calcite_load) */

int cmpcalcite(con calcite *e1p,con calcite *e2p) noex {
    	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
		if (e2p) {
		    if ((rc = intcmp(e1p->y,e2p->y)) == 0) {
		        if ((rc = intcmp(e1p->m,e2p->m)) == 0) {
		            rc = intcmp(e1p->d,e2p->d) ;
		        }
		    }
		}
	    }
	} /* end if */
    	return rc ;
} /* end subroutine (cmpcalcite) */


