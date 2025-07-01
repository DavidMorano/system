/* cfchars SUPPORT */
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
	I took the plunge and rewrote this set of subroutine using
	the LIBC subroutines |strtoXX(3c)|, being much simplified
	now.  It is no longer stand-alone, like in the old days,
	but we have been mostly on UNIX®i for some time now (decades)
	and use in non-UNIX®i environments is now quite rare.  I
	hope that this is not a problem.  We will see.

	= 2023-10,10, David A-D- Morano
	I took the liberty to rewite these subroutines in terms of
	a subroutine-template.  This did nothing to make the code
	faster in any way.  After template expansion the code is the
	same as before.  All type-versions of the former subroutines
	are now in a single subroutine-template.  I guess that I had
	too much idle time on my hands.

*/

/* Copyright © 1998,2013,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	cfchars

	Description:
	Subroutines to convert digit strings to binary integers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<charconv>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"cfchars.h"

import cfcharsx ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* local subroutines */


/* template-subroutines */


/* exported subroutines */

int cfcharsi(cchar *sp,int sl,int b,int *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}

int cfcharsl(cchar *sp,int sl,int b,long *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}

int cfcharsll(cchar *sp,int sl,int b,longlong *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}

int cfcharsui(cchar *sp,int sl,int b,uint *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}

int cfcharsul(cchar *sp,int sl,int b,ulong *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}

int cfcharsull(cchar *sp,int sl,int b,ulonglong *rp) noex {
	return cfcharsx(sp,sl,b,rp) ;
}


