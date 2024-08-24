/* strlocktype */
/* lang=C20 */

/* return a signal abbreviation string given a signal number */
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
	n		signal number to lookup

	Returns:
	-		character-string representation of signal

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local structures */

struct locktype {
	int		t ;
	cchar		*n ;
} ;


/* local variables */

static const struct locktype	types[] = {
	{ F_UNLOCK, "UNLOCK" },
	{ F_WLOCK, "WLOCK" },
	{ F_RLOCK, "RLOCK" },
	{ F_TWLOCK, "TWLOCK" },
	{ F_TRLOCK, "TRLOCK" },
	{ F_WTEST, "WTEST" },
	{ F_RTEST, "RTEST" },
	{ -1, NULL }
} ;


/* exported subroutines */

cchar *strlocktype(uint t) noex {
	int		i ;
	cchar		*s = "unknown" ;
	for (i = 0 ; types[i].t >= 0 ; i += 1) {
	    if (types[i].t == t) {
		s = types[i].n ;
		break ;
	    }
	} /* end for */
	return s ;
}
/* end subroutine (strlocktype) */


