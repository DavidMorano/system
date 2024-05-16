/* nextpowtwo SUPPORT */
/* lang=C++20 */

/* calculate the next higher power of two for a given number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	nextpowtwo

	Description:
	This subroutine calculates the next higher power of two for
	the value given as the argument.  This is very useful in
	many memory mappings and other storage allocation schemes
	where a power of two is preferred for management simplicity.

	Synopsis:
	int nextpowtwo(int v) noex

	Arguments:
	v		supplied integer value

	Returns:
	-               the next higher power of two for the given
			value; it will be the same if the given
			value is already at a power of two

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* <- |CHAR_BIT| */
#include	<bit>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>

#include	"powtwo.h"


/* local defines */


/* imported namespaces */

using std::countl_zero ;
using std::countl_one ;


/* local typedefs */


/* external subroutines */


/* local subroutines */


/* forward references */

extern "C" {
   extern int nextpowtwo(int) noex ;
}


/* local subroutine-template */

static inline int flbsi(int v) noex {
	static constexpr int	nbit = (sizeof(int) * CHAR_BIT) ;
	uint			uv = uint(v) ;
	int			bn = -1 ;
	if (v) {
	    cint	nc = countl_zero(uv) ;
	    bn = (nbit - nc - 1) ;
	}
	return bn ;
}
/* end subroutine-template (flbsi) */


/* exported variables */


/* exported subroutines */

int nextpowtwo(int v) noex {
	int		lb ;
	int		nn = 0 ;
	if ((lb = flbsi(v)) >= 0) {
	    uint	mask = ((1 << lb) - 1) ;
	    if ((v & mask) && (lb < 31)) lb += 1 ;
	    nn = (1 << lb) ;
	}
	return nn ;
}
/* end subroutine (nextpowtwo) */


