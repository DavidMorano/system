/* ucgetauid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-15, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_getauid

	Description:
	This subroutine retrieves from the system the Audit-ID of
	the current process.

	Synopsis:
	int uc_getauid() noex

	Arguments:
	*none*

	Returns:
	>=0		Audit-ID of process (as an integer)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |au_id_t| */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_getauid() noex {
	int		rs ;
	if (au_id_t auid{} ; getauid(&auid) >= 0) {
	    rs = intsat(auid) ;
	} else {
	    rs = (neg errno) ;
	}
	return rs ;
} /* end subroutine (uc_getauid) */


