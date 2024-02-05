/* cfoct SUPPORT */
/* lang=C++20 */

/* convert a digit c-string to its integer value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A�D� Morano
	This subroutine was written by being adapted from a previous version of
	the same, which itself was adapted from an original asembly-language
	version.

	= 2013-04-30, David A�D� Morano
	I took the plunge and rewrote this using the LIBC subroutines
	'strtoXX(3c)'.  It is no longer stand-alone, like in the old days, but
	we have been mostly on UNIX�i for some time now (decades) and use in
	non-UNIX�i environments is now quite rare.  I hope that this is not a
	problem.  We will see.

	= 2023-10,10, David A-D- Morano
	I took the liberty to rewite these subroutines in terms of
	a subroutine-template. This did nothing to make the code
	faster in any way. After template expansion the code is the
	same as before.  All type-versions of the former subroutines
	are now in a single subroutine-template. I guess that I had
	too much idle time on my hands.

*/

/* Copyright � 1998,2013,2023 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Subroutines to convert digit c-strings to integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>		/* <- for |uc_str{xx}(3uc)| */
#include	<stdintx.h>
#include	<cfxxxx.hh>

#include	"cfoct.h"


/* local defines */

#define	OURBASE		8


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	b = OURBASE ;


/* exported variables */


/* exported subroutines */

int cfocti(cchar *sp,int sl,int *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}

int cfoctl(cchar *sp,int sl,long *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}

int cfoctll(cchar *sp,int sl,longlong *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}

int cfoctui(cchar *sp,int sl,uint *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}

int cfoctul(cchar *sp,int sl,ulong *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}

int cfoctull(cchar *sp,int sl,ulonglong *rp) noex {
	return cfxxxx(sp,sl,b,rp) ;
}


