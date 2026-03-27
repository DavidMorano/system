/* nextpowtwo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* calculate the next higher power of two for a given number */
/* version %I% last-modified %G% */

#define	CF_POWCEIL	1		/* used ceiling-type bound */

/* revision history:

	= 1998-03-21, David A­D­ Morano
	This code was originally written.

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

	Notes:
	Yes, yes.  The subroutine |bit_ceil(3c++)| does not exactly
	give the "next power of two" like the present subroutine
	claims, but it is close enough (for government work).

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<concepts>
#include	<bit>			/* |countl_zero(3c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<localmisc.h>

#include	"nextpowtwo.h"

#pragma		GCC dependency		"mod/flbs.ccm"

import flbs ;				/* |flbsi(3u)| */

/* local defines */

#ifndef	CF_POWCEIL
#define	CF_POWCEIL	1		/* used ceiling-type bound */
#endif


/* imported namespaces */

using std::integral ;			/* concept */
using std::countl_zero ;		/* subroutine-template */
using std::countl_one ;			/* subroutine-template */
using std::bit_ceil ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int nextpowtwo(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int		nbits = (szof(int) * CHAR_BIT) ;
constexpr bool		f_powceil = CF_POWCEIL ;


/* exported variables */


/* exported subroutines */

int nextpowtwo(int v) noex {
	int		nn = 0 ;
	if_constexpr (f_powceil) {
	    uint	uv = uint(v) ;
	    nn = bit_ceil(uv) ;
	} else {
	    if (int lbit ; (lbit = flbsi(v)) >= 0) {
	        uint	mask = ((1 << lbit) - 1) ;
	        if ((v & mask) && (lbit < (nbits - 1))) lbit += 1 ;
	        nn = (1 << lbit) ;
	    } /* end if */
	} /* end if_constexpr (f_powceil) */
	return nn ;
}
/* end subroutine (nextpowtwo) */


