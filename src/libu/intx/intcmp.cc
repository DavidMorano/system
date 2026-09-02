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
#include	<numeric>		/* C++STD (saturating arithmetic) */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<intsat.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"intcmp.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> local int intcmpsx(T v1,T v2) noex {
    	return intsat(v1 - v2) ;
} /* end subroutine-template (intcmpsx) */

template<typename T> local int intcmpux(T v1,T v2) noex {
    	int rc = 0 ;
	if (v1 != v2) {
	    rc = (v1 > v2) ? +1 : -1 ;
	}
	return rc ;
} /* end subroutine-template (intcmpux) */


/* local variables */


/* exported variables */


/* exported subroutines */

int intcmpsc(char v1,char v2)			noex {
	return intcmpsx(v1,v2) ;
} /* end */

int intcmpss(short v1,short v2)			noex {
	return intcmpsx(v1,v2) ;
} /* end */

int intcmpsi(int v1,int v2)			noex {
	return intcmpsx(v1,v2) ;
} /* end */

int intcmpsl(long v1,long v2)			noex {
	return intcmpsx(v1,v2) ;
} /* end */

int intcmpsll(longlong v1,longlong v2)		noex {
	return intcmpsx(v1,v2) ;
} /* end */

int intcmpuc(uchar v1,uchar v2)			noex {
	return intcmpux(v1,v2) ;
} /* end */

int intcmpus(ushort v1,ushort v2)		noex {
	return intcmpux(v1,v2) ;
} /* end */

int intcmpui(uint v1,uint v2)			noex {
	return intcmpux(v1,v2) ;
} /* end */

int intcmpul(ulong v1,ulong v2)			noex {
	return intcmpux(v1,v2) ;
} /* end */

int intcmpull(ulonglong v1,ulonglong v2)		noex {
	return intcmpux(v1,v2) ;
} /* end */


