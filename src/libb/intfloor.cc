/* intfloor SUPPORT */
/* lang=C++20 */

/* Integer-Flooring */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	{xx}floor

	Description:
	This subroutine calculates the unsigned flooring of a number
	given a specified modulus.

	Synopsis:
	int {xx}floor({xx} v,int m) noex

	Arguments:
	v	number to find the flooring for
	m	the modulus to use in the calculation

	Returns:
	-	the flooring value

	Note:
	Note that a "sequence point" needs to be present in this
	code so that the total expression doesn't get optimized
	out!


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdint>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"intfloor.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* local subroutine-templates */

template<typename T>
T xfloor(T v,int m) noex {
	const T		n = v / m ;
	return (n * m) ;
}
/* end subroutine-template (xfloor) */


/* exported subroutines */

int ifloor(int v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (ifloor) */

long lfloor(long v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (lfloor) */

longlong llfloor(longlong v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (llfloor) */

intptr_t ipfloor(intptr_t v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (ipfloor) */


uint ufloor(uint v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (ufloor) */

ulong ulfloor(ulong v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (ulfloor) */

ulonglong ullfloor(ulonglong v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (ullfloor) */

uintptr_t uipfloor(uintptr_t v,int m) noex {
	return xfloor(v,m) ;
}
/* end subroutine (uipfloor) */


