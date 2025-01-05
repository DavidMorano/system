/* cfdecf SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written adapted from assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cfdecf

	Description:
	Subroutines to convert decimal strings to binary floating
	values.

	Synopsis:
	int cfdecf(cchar *sp,int sl,double *rp) noex

	Arguments:
	sp		source string
	sl		source string length
	rp		pointer to hold result

	Returns:
	>=0		OK
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>		/* <- for |uc_str{xx}(3uc)| */
#include	<sfx.h>
#include	<strnul.hh>
#include	<localmisc.h>		/* <- for |DIGBUFLEN| below */

#include	"cfdecf.h"
#include	"cfutil.h"


/* local defines */


/* local namespaces */

using cfx::sfdigs ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int cfdecf(cchar *snp,int snl,double *rp) noex {
	int		rs = SR_FAULT ;
	if (snp && rp) {
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if (int sl ; (sl = sfdigs(snp,snl,&sp)) > 0) {
		strnul	str(sp,sl) ;
	        rs = uc_strtod(str,nullptr,rp) ;
	    } /* end if (sfdigs) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (cfdecf) */


