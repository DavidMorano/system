/* intcmp SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* integer remainder */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	{x}rem

	Description:
	These subroutine calculates the unsigned remainder of a
	division of the two given numbers.

	Synopsis:
	int {x}rem(int v,int m) noex

	Arguments:
	{x}	i, l, ll, u, ul, ull
	v	number to find the remainder for
	m	the modulus to use in the calculation

	Returns:
	-	the remainder value

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"intcmp.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> local int intcmpx(T v1,T v2) noex {
    	int rc = 0 ;
	if (v1 != v2) {
	    rc = (v1 > v2) ? +1 : -1 ;
	}
	return rc ;
} /* end subroutine */


/* local variables */


/* exported variables */


/* exported subroutines */

int intcmpc(uchar v1,uchar v2)			noex {
	return intcmpx(v1,v2) ;
} /* end */

int intcmps(ushort v1,ushort v2)		noex {
	return intcmpx(v1,v2) ;
} /* end */

int intcmpi(uint v1,uint v2)			noex {
	return intcmpx(v1,v2) ;
} /* end */

int intcmpl(ulong v1,ulong v2)			noex {
	return intcmpx(v1,v2) ;
} /* end */

int intcmpll(ulonglong v1,ulonglong v2)		noex {
	return intcmpx(v1,v2) ;
} /* end */


