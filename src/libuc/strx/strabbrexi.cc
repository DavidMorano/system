/* strabbrexi SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return an abbreviation c-string given a process exit number */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strabbrexi

	Description:
	This subroutine takes as a given arugment a process exit
	code and it returns an abbreviation c-string corresponding
	to it.

	Synopsis:
	cchar *strabbrexi(uint n) noex

	Arguments:
	n		process exit code

	Returns:
	-		character-string representation

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<ucodenames.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"strabbrexi.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

cchar *strabbrexi(int n) noex {
	cchar		*s = nullptr ;
	if (s == nullptr) {
	    int rs ;
	    if (cchar *rp ; (rs = ucodename_exit(n,&rp)) >= 0) {
		s = rp ;
	    } else {
	        s = "*UNK*" ;
	    } /* end */
	} /* end if (needed) */
	return s ;
} /* end subroutine (strabbrexi) */


