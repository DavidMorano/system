/* cvtdig SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* convert an integer value into a c-string, given a value and base */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written having been adapated (from
	memory) from something I wrote back in the early 1980s (for
	embedded work).  I had to write every ... last  ... thing
	myself back in the old days.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cvtdig

	Description:
	This subroutine converts a value (and a base) into its
	string representation. It only works for bases that are a
	power-of-two, and also only for bases up to sixteen (16).

	Synopsis:
	int cvtdig(char *buf,ulonglong &val,int n,int b) noex

	Arguments:
	buf		result buffer
	val		value to convert
	n		number of bytes in given value to convert, generally:
				1, 2, 4, or 8
	b		the base to use, generally:
				2=binary
				4
				8=octal
				16=hexadecimal

	Returns:
	-		length of result characters

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bit>
#include	<usystem.h>
#include	<stdintx.h>

#include	"cvtdig.hh"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* exported subroutines */


