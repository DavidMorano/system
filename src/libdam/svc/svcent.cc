/* svcent SUPPORT */
/* lang=C++20 */

/* service file entry (SVCENT) */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-10-13, David A­D­ Morano
	This was split out of the HOMEPAGE program (where it was
	originally local).

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	svcent

	Description:
	We perform some slight management on SVCENT objects.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t(3c++)| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"svcent.h"
#include	"svckv.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int svcent_getval(svcent *sep,cchar *k,cchar **rpp) noex {
	cint		n = sep->nkeys ;
	cchar		*(*kv)[2] = sep->keyvals ;
	return svckv_val(kv,n,k,rpp) ;
}
/* end subroutine (svcent_getval) */

int svcent_getdeval(svcent *sep,cchar *k,cchar **rpp) noex {
	cint		n = sep->nkeys ;
	cchar		*(*kv)[2] = sep->keyvals ;
	return svckv_dequote(kv,n,k,rpp) ;
}
/* end subroutine (svcent_getdeval) */

int svcent_islib(svcent *sep,cchar **rpp) noex {
	cint		n = sep->nkeys ;
	int		vl ;
	cchar		*(*kv)[2] = sep->keyvals ;
	cchar		*k1 = "so" ;
	if ((vl = svcent_getval(sep,k1,rpp)) > 0) {
	    cchar	*dummy ;
	    if (svckv_isprog(kv,n,&dummy) == 0) {
		vl = 0 ;
	    }
	}
	return vl ;
}
/* end subroutine (svcent_islib) */


