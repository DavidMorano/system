/* getprogexec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the program execution name */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-03-01, David A­D­ Morano
	This code was originally written as a KSH built-in command.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprogexec

	Synopsis:
	int getprogexec(char *ebuf,int elen) noex

	Arguments:
	ebuf		result buffer pointer
	elen		result buffer length

	Returns:
	>=0		length if result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<getexecname.h>		/* LIBUC */
#include	<sncpy.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getprogexec.h"


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

int getprogexec(char *ebuf,int elen) noex {
	int		rs = SR_FAULT ;
	if (ebuf) ylikely {
	    static cchar *en = getexecname() ;
	    rs = SR_NOSYS ;
	    ebuf[0] = '\0' ;
	    if (en) {
	        rs = sncpy(ebuf,elen,en) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getprogexec) */


