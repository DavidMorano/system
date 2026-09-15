/* usupport_hasx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasext

	Description:
	This subroutine determines if the given counted string has 
	the given extension (given as a c-string).

	Synopsis:
    	bool hasext(cchar *sp,int sl,cchar *ext) noex

	Arguments:
	sp		test string pointer
	sl		test string length
	ext		etension c-string to search for

	Returns:
	false		no	- test string has the extension
	true		yes	- test string has the extention

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |UC(3u)| */

#include	"usupport_siext.hh"
#include	"usupport_hasx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::siext ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    bool hasext(cchar *sp,int sl,cchar *ext) noex {
    	bool		f = false ;
	if (cint si = siext(sp,sl) ; si >= 0) {
	    f = (strcmp((sp + si),ext) == 0) ;
	} /* end if (siext) */
    	return f ;
    } /* end subroutine (hasext) */
} /* end namespace (libu) */


