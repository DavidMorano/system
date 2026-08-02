/* fmtutil1 MODEUL (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* options for |fmtstr(3uc)| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtutil

	Description:
	This is the support portion of the FMTUTIL module.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"fmtutil.hh"

module fmtutil ;

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */

constexpr numbases		numbase ;


/* exported subroutines */

bool hasourbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    cint	ch = mkchar(*sp) ;
	    f = isourbad(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
} /* end subroutine (hasourbad) */


