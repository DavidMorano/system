/* field_svcargs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* parse a field object into service envelope items */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A�D� Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	field_svcargs

	Description:
	This subroutine will take a 'field' object and creates a
	string-list of service arguments from it.

	Synopsis:
	int field_svcargs(field *fbp,vecstr *sap) noex

	Arguments:
	fbp		pointer to field object
	sap		pointer to VECSTR of arguments

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	I note that "service" arguments (at least as we see it at the
	present) are the same as "shell" arguments.  So we call
	|field_sharg()| to get the next "service" argument.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<vecstr.h>
#include	<localmisc.h>

#include	"field.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int field_svcargs(field *fbp,vecstr *sap) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (fbp && sap) {
	    cnullptr	np{} ;
	    if ((rs = field_remaining(fbp,np)) >= 0) {
	        cint	alen = rs ;
	        if (char *abuf{} ; (rs = uc_malloc((alen+1),&abuf)) >= 0) {
		    cint	rsn = SR_NOTFOUND ;
	            while ((rs1 = field_sharg(fbp,np,abuf,alen)) >= 0) {
			cint	al = rs1 ;
	                c += 1 ;
	                rs = vecstr_add(sap,abuf,al) ;
	                if (rs < 0) break ;
	            } /* end while */
		    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	            rs1 = uc_free(abuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a) */
	    } /* end if (field_remaining) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (field_svcargs) */


