/* intrem SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"intrem.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename T> T remx(T v,int m) noex {
	const T	q = (v/m) ;
	return (v-(q*m)) ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int irem(int v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (irem) */

long lrem(long v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (lrem) */

longlong llrem(longlong v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (llrem) */

uint urem(uint v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (uirem) */

ulong ulrem(ulong v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (ulrem) */

ulonglong ullrem(ulonglong v,int m) noex {
	return remx(v,m) ;
}
/* end subroutine (ullrem) */


