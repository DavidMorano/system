/* cfdecf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary floating value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written adapted from assembly.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	cfdec{x}

	Description:
	Subroutines to convert decimal strings to binary floating
	values.  The usual floating types are supported.

	Synopsis:
	int cfdec{x}(cchar *sp,int sl,{x} *rp) noex

	Arguments:
	{x}		one of: float, double, longdouble
	sp		source string
	sl		source string length
	rp		pointer to hold result

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucstrto.h>		/* |ucstrto{x}(3u)| */
#include	<sfx.h>			/* |sfdigs(3uc)| */
#include	<strnul.hh>
#include	<localmisc.h>

#include	"cfdecf.h"
#include	"cfutil.hh"


/* local defines */


/* local namespaces */

using cfx::sfdigs ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> local int cfdecfx(cchar *snp,int snl,T *rp) noex {
	int		rs = SR_FAULT ;
	if (snp && rp) ylikely {
	    cchar	*sp{} ;
	    rs = SR_DOM ;
	    if (int sl ; (sl = sfdigs(snp,snl,&sp)) > 0) ylikely {
		strnul	str(sp,sl) ;
	        rs = uc_strto(str,nullptr,rp) ;
	    } /* end if (sfdigs) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cfdecfx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cfdecf(cchar *snp,int snl,float *rp) noex {
    	return cfdecfx(snp,snl,rp) ;
}

int cfdecd(cchar *snp,int snl,double *rp) noex {
    	return cfdecfx(snp,snl,rp) ;
}

int cfdecld(cchar *snp,int snl,longdouble *rp) noex {
    	return cfdecfx(snp,snl,rp) ;
}


