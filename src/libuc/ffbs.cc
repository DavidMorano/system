/* ffbs SUPPORT */
/* lang=C++20 */

/* find the first bit set in various integer types */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	These subroutines find the first bit set in various integer
	types.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- for |CHAR_BIT| */
#include	<bit>			/* <- for |countr_zero(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<stdintx.h>

#include	"ffbs.h"


/* local defines */


/* imported namespaces */

using std::countr_zero ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T>
int ffbsx(T uv) noex {
	int		n = -1 ;
	if (uv) {
	    n = countr_zero(uv) ;
	}
	return n ;
}
/* end subroutine-template (ffbsx) */


/* local variables */


/* exported variables */


/* exported subroutines */

int ffbsi(int v) noex {
	const uint	uv = uint(v) ;
	return ffbsx(uv) ;
}

int ffbsl(long v) noex {
	const ulong	uv = ulong(v) ;
	return ffbsx(uv) ;
}

int ffbsll(longlong v) noex {
	const ulonglong	uv = ulonglong(v) ;
	return ffbsx(uv) ;
}

int ffbsui(uint uv) noex {
	return ffbsx(uv) ;
}

int ffbsul(ulong uv) noex {
	return ffbsx(uv) ;
}

int ffbsull(ulonglong uv) noex {
	return ffbsx(uv) ;
}


