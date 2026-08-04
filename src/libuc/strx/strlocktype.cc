/* strlocktype SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return a c-string abbreviation for a UNIX® file-lock type */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlocktype

	Description:
        We take a signal number and we return a corresponding signal
        abbreviation string.

	Synopsis:
	cchar *strlocktype(uint n) noex 

	Arguments:
	n		file-lock type

	Returns:
	-		character-string representation of signal

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strlocktype.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

struct locktype {
	int		t ;
	cchar		*n ;
} ; /* end struct */


/* forward references */


/* local variables */

constexpr locktype	types[] = {
	{ F_UNLOCK, "UNLOCK" },
	{ F_WLOCK, "WLOCK" },
	{ F_RLOCK, "RLOCK" },
	{ F_TWLOCK, "TWLOCK" },
	{ F_TRLOCK, "TRLOCK" },
	{ F_WTEST, "WTEST" },
	{ F_RTEST, "RTEST" },
	{ -1, nullptr }
} ; /* end array */


/* exported variables */


/* exported subroutines */

cchar *strlocktype(int t) noex {
	cchar		*s = "unknown" ;
	for (int i = 0 ; types[i].t >= 0 ; i += 1) {
	    if (types[i].t == t) {
		s = types[i].n ;
		break ;
	    }
	} /* end for */
	return s ;
} /* end subroutine (strlocktype) */


