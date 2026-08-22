/* cfdec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a decimal digit string to its binary integer value */
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
	cfdec

	Description:
	Subroutines to convert digit strings to binary integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"cfdec.h"

#pragma		GCC dependency		"mod/cfsysx.ccm"

import cfsysx ;				/* |cfsysx(3uc)| */

/* local defines */

#define	OURBASE		10


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported variables */


/* exported subroutines */

int cfdeci(cchar *sp,int sl,int *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfdecl(cchar *sp,int sl,long *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfdecll(cchar *sp,int sl,longlong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfdecui(cchar *sp,int sl,uint *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfdecul(cchar *sp,int sl,ulong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */

int cfdecull(cchar *sp,int sl,ulonglong *rp) noex {
	return cfsysx(sp,sl,b,rp) ;
} /* end */


