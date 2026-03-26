/* arrswap SUPPORT */
/* charset=ISO8859-1 */
/* lang=C99 */

/* array swap */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was originally written.

	= 2017-09-15, David A­D­ Morano
	I changed name of this subroutine (never really used much).

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	arrswap{x}

	Descrption:
	We swap two elements in an array of integers.

	Synopsis:
	void arrswap{x}(int *a,int i1,int i2) noex

	Arguments:
	{x}	one of: 'i' 'l' 'll'
	a	array of integers
	i1	element to swap
	i2	element to swap

	Returns:
	-

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<stdintx.h>
#include	<localmisc.h>

#include	"arrswap.h"


/* local defines */


/* external subroutines */


/* forward references */

template<typename T>
void arrswapx(T *a,int i1,int i2) noex {
	T	t = a[i1] ;
	a[i1] = a[i2] ;
	a[i2] = t ;
}
/* end subroutine-template (arrswapx) */


/* local variables */


/* exported variables */


/* exported subroutines */

void arrswapi(int *a,int i1,int i2) noex {
	return arrswapx(a,i1,i2) ;
}
/* end subroutine (arrswapi) */

void arrswapl(long *a,int i1,int i2) noex {
	return arrswapx(a,i1,i2) ;
}
/* end subroutine (arrswapl) */

void arrswapll(longlong *a,int i1,int i2) noex {
	return arrswapx(a,i1,i2) ;
}
/* end subroutine (arrswapll) */


