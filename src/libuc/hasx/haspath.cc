/* haspath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	haspath{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

	Synopsis:
	bool haspath{x}(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<char.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"haspath.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */

#define	ISWHT(ch)	CHAR_ISWHITE(ch)


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

bool haspathvar(cchar *sp,int sl) noex {
    	constexpr int	chx_ec = mkchar('¬') ;
	bool		f = false ;
	if (sp) ylikely {
	    if (sl && sp[0]) {
	        cint ch = mkchar(sp[0]) ;
	        f = (ch == chx_ec) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (haspathvar) */

bool haspathuser(cchar *sp,int sl) noex {
    	constexpr int	chx_ec = mkchar('~') ;
	bool		f = false ;
	if (sp) ylikely {
	    if (sl && sp[0]) {
	        cint ch = mkchar(sp[0]) ;
	        f = (ch == chx_ec) ;
	    }
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (haspathuser) */


/* local subroutines */


