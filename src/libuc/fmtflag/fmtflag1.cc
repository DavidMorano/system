/* fmtflag1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to mange formatting flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2020-02-15, David A­D­ Morano
	I modularized (w/ C++20 modules) this code.

*/

/* Copyright © 1998,2020 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fmtflag

	Description:
	This object supports the FMTSUB object by manging formatting
	flags (in certain cases).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cwchar>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>

module fmtflag ;

template<typename T> bool bitn(T v,int n) noex {
    	return bool((v >> n) & 1) ;
} /* end subroutine-template (bitn) */

void fmtflag_co::operator = (bool a) noex {
	int	m = (1 << w) ;
	if (a) {
	    op->fg |= m ;
	} else {
	    op->fg &= (compl m) ;
	}
} /* end method (fmtflag_co::operator) */

fmtflag_co::operator bool () noex {
    	bool	f = false ;
	if (op) {
	    if ((w >= 0) && (w < fmtflag_overlast)) {
		f = bitn(op->fg,w) ;
	    } /* end switch */
	} /* end if (non-null) */
	return f ;
} /* end method (fmtflag_co::operator) */


