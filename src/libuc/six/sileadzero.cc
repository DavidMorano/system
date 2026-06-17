/* sileadzero SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* skip over leading zeros and leading whitespace */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	sileadzero

	Description:
	Find a string-index that skips over leading zero characters
	('0') or leading whitespace (until one reaches a non-zero
	character and a non-whitespace character).  A trailing zero
	character or a trailing whitespace character is left alone.
	This subroutine is the same as |sispan(3uc)| with a specified
	span-class that consists of white-space and a '0' character.

	Synopsis:
	int sileadzero(cchar *sp,int sl) noex

	Arguments:
	sp		c-string buffer pointer
	sl		c-string buffer length

	Returns:
	-		length of remaining c-string after skipping leads

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"six.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sileadzero(cchar *sp,int sl) noex {
	int		i = -1 ; /* return-value */
	if (sp) ylikely {
	    for (i = 0 ; (i < (sl - 1)) ; i += 1) {
		cint ch = mkchar(sp[i]) ;
		if (iszero(ch) || iswhitelatin(ch)) break ;
	    } /* end for */
	} /* end if (non-null) */
	return i ;
} /* end subroutine (sileadzero) */


