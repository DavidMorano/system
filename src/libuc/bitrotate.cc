/* bitrotate SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform a bit rotation of an unsigned integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	bitrotate

	Description:
	This subroutine rotates an integer by some specified number
	of bits.  A positive specifivation rotates the integer right.
	A negative specification rotates the integer left.

	Synopsis:
	{T} bitrotate({T} v,int n) noex

	Arguments:
	T		type is one of: uint, ulong, ulonglong
	v		integer to rotate
	n		number of bits to rotate by

	Returns:
	-		the rotated integer

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |abs(3c)| */
#include	<bit>			/* |rotr(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"bitrotate.h"


/* local defines */

#ifdef	__cpp_lib_bitops
#define	CF_ROT	1			/* use C++20 bit-rotate */
#else
#define	CF_ROT	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

constexpr cbool		f_rot = CF_ROT ;

template<typename UT> static UT bitrotatex(UT v,int n) noex {
	UT		rv ;
	if_constexpr (f_rot) {
	    rv = std::rotr(v,n) ;
	} else {
	    if (n) {
	        cint	nb = (szof(UT) * CHAR_BIT) ;
	        int	nr, nl ;
	        if (n >= 0) {
	            nr = n ;
	            nl = (nb - n) ;
	        } else {
	            n = (- n) ;
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
/* end subroutine (bitrotate) */


/* local variables */


/* exported variables */


/* exported subroutines */

uint		bitrotateui(uint v,int n) noex {
    	return bitrotatex(v,n) ;
}

ulong		bitrotateul(ulong v,int n) noex {
    	return bitrotatex(v,n) ;
}

ulonglong	bitrotateull(ulonglong v,int n) noex {
    	return bitrotatex(v,n) ;
}


