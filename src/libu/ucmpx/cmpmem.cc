/* cmpmem SUPPORT */
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
	cmpmem

	Description:
	Compare two memory byte sequences of a specifiied length.

	Synopses:

	Arguments:
	v1p		pninter to entry-1
	c2p		pninter to entry-2
	n		number of bytes to compare

	Returns:
	>0		GT
	==0		EQ
	<0		LT

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |memcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cmpmem.h"


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

int cmpmem(cvoid *v1p,cvoid *v2p,int n) noex {
	int		rc = 0 ;
	if ((n >= 0) && (v1p || v2p)) ylikely {
	    rc = +1 ;
	    if (v1p) {
		rc = -1 ;
		if (v2p) {
		    csize nsz = size_t(n) ;
		    rc = memcmp(v1p,v2p,nsz) ;
		} /* end if */
	    } /* end if */
	} /* end if */
	return rc ;
} /* end subroutine (cmpmem) */


