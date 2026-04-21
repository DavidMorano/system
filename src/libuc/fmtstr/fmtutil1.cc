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
#include	<clanguage.h>
#include	<usysbase.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"fmtutil.h"

module fmtutil ;


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
}
/* end subroutine (hasourbad) */


