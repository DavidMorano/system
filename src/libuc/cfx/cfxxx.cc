/* cfxxx SUPPORT */
/* lang=C++20 */

/* convert a decimal digit string to its binary integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This subroutine was written by being adapted from one of
	my previous versions of the same (from the early 1980s;
	embedded work at AT&T), which itself was adapted from an
	original asembly-language version (in Digital Euipment Corp
	VAX assembly).

	= 2013-04-30, David A­D­ Morano
	I took the plunge and rewrote this set of subroutine using
	the LIBC subroutines |strtoXX(3c)|, being much simplified
	now.  It is no longer stand-alone, like in the old days,
	but we have been mostly on UNIX®i for some time now (decades)
	and use in non-UNIX®i environments is now quite rare.  I
	hope that this is not a problem.  We will see.

	= 2023-10,10, David A-D- Morano
	I took the liberty to rewite these subroutines in terms of
	a subroutine-template. This did nothing to make the code
	faster in any way. After template expansion the code is the
	same as before.  All type-versions of the former subroutines
	are now in a single subroutine-template. I guess that I had
	too much idle time on my hands.

*/

/* Copyright © 1998,2013,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Subroutines to convert digit strings to binary integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>		/* <- for |uc_str{xx}(3uc)| */
#include	<stdintx.h>
#include	<cfxxxx.hh>

#include	"cfxxx.h"


/* local defines */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* local subroutines */


/* template-subroutines */


/* exported variables */


/* exported subroutines */

int cfxxxi(cchar *sp,int sl,int b,int *rp) noex {
	return cfxxxx(uc_strtoi,sp,sl,b,rp) ;
}

int cfxxxl(cchar *sp,int sl,int b,long *rp) noex {
	return cfxxxx(uc_strtol,sp,sl,b,rp) ;
}

int cfxxxll(cchar *sp,int sl,int b,longlong *rp) noex {
	return cfxxxx(uc_strtoll,sp,sl,b,rp) ;
}

int cfxxxui(cchar *sp,int sl,int b,uint *rp) noex {
	return cfxxxx(uc_strtoui,sp,sl,b,rp) ;
}

int cfxxxul(cchar *sp,int sl,int b,ulong *rp) noex {
	return cfxxxx(uc_strtoul,sp,sl,b,rp) ;
}

int cfxxxull(cchar *sp,int sl,int b,ulonglong *rp) noex {
	return cfxxxx(uc_strtoull,sp,sl,b,rp) ;
}


