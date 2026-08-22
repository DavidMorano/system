/* cfhex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written by being adapted from one of
	my previous versions of the same (from the early 1980s;
	embedded work at AT&T), which itself was adapted from an
	original asembly-language version (in Digital Equipment Corp
	VAX assembly).

	= 2013-04-30, David A­D­ Morano
	See my change notes in the source file |cfsigx(3uc)|.

*/

/* Copyright © 1998,2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	cfhex

	Description:
	Subroutines to convert digit c-strings to integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cfhex.h"

#pragma		GCC dependency		"mod/cfsysx.ccm"

import cfsysx ;				/* |cfsysx(3uc)| */

/* local defines */

#define	OURBASE		16


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported variables */


/* exported subroutines */

int cfhexi(cchar *sp,int sl,int *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfhexl(cchar *sp,int sl,long *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfhexll(cchar *sp,int sl,longlong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfhexui(cchar *sp,int sl,uint *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfhexul(cchar *sp,int sl,ulong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfhexull(cchar *sp,int sl,ulonglong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */


