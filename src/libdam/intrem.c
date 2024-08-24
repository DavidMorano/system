/* intrem SUPPORT */
/* lang=C++20 */

/* integer remainder */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David AÂ­DÂ­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	{x}irem

	Description:
	These subroutine calculates the unsigned remainder of a
	division of the two given numbers.

	Synopsis:
	int {x}rem(int v,int m) noex

	Arguments:
	v	number to find the remainder for
	m	the modulus to use in the calculation

	Returns:
	-	the remainder value

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"intrem.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int irem(int v,int m) noex {
	const int	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (irem) */

long lrem(long v,int m) noex {
	const long	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (lrem) */

longlong llrem(longlong v,int m) noex {
	const longlong	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (llrem) */

uint uirem(uint v,int m) noex {
	const uint	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (uirem) */

ulong ulrem(ulong v,int m) noex {
	const ulong	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (ulrem) */

ulonglong ullrem(ulonglong v,int m) noex {
	const ulonglong	q = (v/m) ;
	return (v-(q*m)) ;
}
/* end subroutine (ullrem) */


