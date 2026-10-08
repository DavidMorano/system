/* strlcpy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* buffer-size-conscious string operation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-01, David A­D­ Morano
	This subroutine was written because I want to possibly) use
	the new (experimental?) subroutine |strlcpy(3c)|.  The idea
	is that when that new subroutine does get standardized and
	is actually inside of the system standard C-language library,
	programs using this will just automatically switch to be
	using the real one that is vendor implemented.  The vendor
	implemented version might be optimized for speed, but being
	hand-optimized being written in assembly lanuage.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	strlcpy

	Description:
	This subroutine is a knock off of the nes subroutine
	|strlcpy(3c)| that is supposed to be coming (in standard
	operating system libraries).

	Synopsis:
	size_t strlcpy(char *dst,cchar *src,size_t maxlen) noex

	Arguments:
	dst		desination byte buffer pointer
	src		source c-string pointer
	maxlen		maximum number of bytes to be copied
			(including the terminating NUL byte)

	Returns:
	-		number of bytes copied, including the termating
			NUL byte

	Notes:
	1. The 'size_t' type was first standardized in the C programming
	language standard of 1989 (ANSI 1989).  It was also standardized
	in ISO C90 (1990).
	2. The subroutine |strlcpy(3c)| has not yet been standardized,
	but there rumors that it may be in the future.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strlen(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strlcpy.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

size_t strlcpy(char *dst,cchar *src,size_t maxlen) noex {
	size_t		rsz = 0 ; /* return-value */
	if (dst && src) ylikely {
	    cint	ml = intconv(maxlen) ;
	    int		i{} ; /* used-afterwards */
	    for (i = 0 ; (i < (ml - 1)) && *src ; i += 1) {
	        dst[i] = *src++ ;
	    } /* end for */
	    dst[i] = '\0' ;
	    rsz = size_t((*src == '\0') ? i : (i + intconv(strlen(src)))) ;
	} /* end if (non-null) */
	return rsz ;
} /* end subroutine (strlcpy) */


