/* hashindex SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create an index from a hash value */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
  	Name:
	hasindex

	Description:
	This subroutine creates an index (like for use in an
	index-table) from a given index value (an unsigned integer).

	Synopsis:
	int hashindex(uint i,int n) noex

	Arguments:
	i	given index value
	n	modulus

	Returns:
	-	new index

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<bit>			/* C++STD |has_single_bit(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |MODP2| */

#include	"hashindex.h"


/* local defines */


/* imported namespaces */

using std::has_single_bit ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#ifdef	COMMENT
uint hashindex(uint v,int n) noex {
	uint		rv ;
	if (has_single_bit(v)) {
	    rv = MODP2(v,n) ;
	} else {
	    rv = (v % n) ;
	}
	if (rv == 0) rv = 1 ;
	return rv ;
} /* end subroutine (hashindex) */
#endif /* COMMENT */


