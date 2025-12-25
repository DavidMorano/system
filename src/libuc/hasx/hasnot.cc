/* hasnot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* has a counted c-string some characteristic? */
/* version %I% last-modified %G% */

#define	CF_HASNOTDOTSWITCH	1	/* switch or not */

/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	hasnot

	Names:
	hasnotdots
	hasnotempty
	
	Aliases:
	hasNotDots
	hasNotEmpty

	Description:
	These subroutines determine if a given counted c-string
	does not have something.

	Synopsis:
	bool hasnotdots(cchar *sp,int sl) noex
	bool hasnotempty(cchar *sp,int sl) noex

	Arguments:
	sp		given counted c-string pointer
	sl		given counted c-string length

	Returns:
	true		determination is true
	false		determination is false

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
#include	<ischarx.h>
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasnot.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#ifndef	CF_HASNOTDOTSWITCH
#define	CF_HASNOTDOTSWITCH	0
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr bool		f_hasnotdotswitch = CF_HASNOTDOTSWITCH ;


/* exported variables */


/* exported subroutines */

bool hasnotdots(cchar *sp,int µsl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    if (sp[0] == '.') {
		if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	            if_constexpr (f_hasnotdotswitch) {
	                switch (sl) {
	                case 1:
	                    f = false ;
	                    break ;
	                case 2:
	                    f = (sp[1] != '.') ;
	                    break ;
	                } /* end switch */
	            } else {
	                if (sl <= 2) {
	                    f = (sl != 1) ;
	                    if ((! f) && (sl == 2)) {
				f = (sp[1] != '.') ;
			    }
	                } /* end if */
	            } /* end if_constexpr (f_hasnotdotswitch) */
		} /* end if (getlenstr) */
	    } /* end if (had a leading dot) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnotdots) */

bool hasnotempty(cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = ((!CHAR_ISWHITE(ch)) && (ch != CH_NL)) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasnotempty) */


/* local subroutines */


