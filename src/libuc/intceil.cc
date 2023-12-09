/* intceil */
/* lang=C++20 */

/* Integer-Ceiling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	{xx}ceil

	Description:
	This subroutine calculates the unsigned ceiling of a number
	given a specified modulus.

	Synopsis:
	int {xx}ceil({xx} v,int m) noex

	Arguments:
	v	number to find the ceiling for
	m	the modulus to use in the calculation

	Returns:
	-	the ceiling value

	Note:
	Note that a "sequence point" needs to be present in this
	code so that the total expression doesn't get optimized
	out!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdint.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"intceil.h"


/* local defines */


/* local subroutine-templates */

template<typename T>
T xceil(T v,int m) noex {
	const T		n = (v + (m - 1)) / m ;
	return (n * m) ;
}
/* end subroutine-template (xceil) */


/* exported subroutines */

int iceil(int v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (iceil) */

long lceil(long v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (lceil) */

longlong llceil(longlong v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (llceil) */

off_t ofceil(off_t v,int m) noex {
	return xceil(v,m) ;
}

intptr_t ipceil(intptr_t v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (ipceil) */


uint uceil(uint v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (uceil) */

ulong ulceil(ulong v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (ulceil) */

ulonglong ullceil(ulonglong v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (ullceil) */

size_t szceil(size_t v,int m) noex {
	return xceil(v,m) ;
}

uintptr_t uipceil(uintptr_t v,int m) noex {
	return xceil(v,m) ;
}
/* end subroutine (uipceil) */


