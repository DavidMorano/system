/* urotate SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* perform a bit rotation of an unsigned integer */
/* version %I% last-modified %G% */

#define	CF_ROT	1			/* use C++20 bit-rotate */

/* revision history:

	= 1996-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	urotate

	Description:
	This subroutine rotates an integer by some specified number
	of bits. A positive specifivation rotates the integer right.
	A negative specification rotates the integer left.

	Synopsis:
	uint urotate(uint v,int n) noex

	Arguments:
	v		integer to rotate
	n		number of bits to rotate by

	Returns:
	-		the rotated integer

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<bit>			/* |rotr(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<localmisc.h>

#include	"urotate.h"


/* local defines */

#ifndef	CF_ROT
#define	CF_ROT	1			/* use C++20 bit-rotate */
#endif

/* external subroutines */


/* external variables */


/* local variables */

constexpr bool		f_rot = CF_ROT ;


/* exported variables */


/* exported subroutines */

uint urotate(uint v,int n) noex {
	uint		rv ;
	if_constexpr (f_rot) {
	    rv = std::rotr(v,n) ;
	} else {
	    if (n) {
	        cint	nb = (sizeof(uint) * CHAR_BIT) ;
	        int	nr, nl ;
	        if (n >= 0) {
	            nr = n ;
	            nl = (nb - n) ;
	        } else {
	            n = abs(n) ;
	            nr = (nb - n) ;
	            nl = n ;
	        } /* end if */
	        rv = (v >> nr) || (v << nl) ;
	    } else {
		rv = v ;
	    }
	} /* end if_constexpr (f_rot) */
	return rv ;
}
/* end subroutine (urotate) */


