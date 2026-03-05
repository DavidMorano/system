/* hasinet SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasinet{x}

	Description:
	These subroutines check if a specified c-string has the
	characteristic that we are looking for.

	Synopsis:
	bool hasnet4addrstr(cchar *sp,int sl) noex
	bool hasnet6addrstr(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	true		address type confirmed
	false		address type falsified

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |UC(3dam)| */
#include	<strnul.hh>
#include	<dprintf.h>		/* |DPRINTF(3dam)| */

#include	"hasinet.h"
#include	"hasall.h"		/* |hasalldig(3uc)| */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool	hasINET4num	(cchar *,int) noex ;
local bool	hasINET6num	(cchar *,int) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

bool hasinet4addrstr(cchar *sp,int µsl) noex {
	bool		f = false ;
	{
	    strnul ds(sp,µsl) ;
	    DPRINTF("ent s=>%s<\n",ccp(ds)) ;
	}
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    int		c = 0 ;
	    f = true ;
	    for (cc *tp ; (tp = strnchr(sp,sl,'.')) != nullptr ; ) {
	        cint tl = intconv(tp - sp) ;
	        f = hasINET4num(sp,tl) ;
	        if (! f) break ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        c += 1 ;
	    } /* end while */
	    if (f) {
		f = false ;
		if (sl > 0) {
	            c += 1 ;
	            f = hasINET4num(sp,sl) ;
		}
	    } /* end if */
	    f = f && (c > 0) ;
	} /* end if (getlenstr) */
	DPRINTF("ret f=%u\n",f) ;
	return f ;
}
/* end subroutine (hasnet4addrstr) */

bool hasinet6addrstr(cchar *sp,int µsl) noex {
	bool		f = false ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) ylikely {
	    int c = 0 ;
	    int cz = 0 ;
	    for (cc *tp ; (tp = strnchr(sp,sl,':')) != nullptr ; ) {
		c += 1 ;
		if (cint tl = intconv(tp - sp) ; tl > 0) {
		    f = hasINET6num(sp,tl) ;
		    if (! f) break ;
		} else {
		    cz += 1 ;
		} /* end if */
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	    } /* end for */
	    if (f && (sl > 0)) {
		    f = hasINET6num(sp,sl) ;
	    } /* end if (remainder) */
	    f = f && (c > 0) && (cz <= 1) ;;
	} /* end if (getlenstr) */
	return f ;
} /* end subroutine (hasnet6addrstr) */


/* local subroutines */

local bool hasINET4num(cchar *sp,int sl) noex {
	return ((sl > 0) && hasalldig(sp,sl)) ;
} /* end subroutine (hasINET4num) */

local bool hasINET6num(cchar *sp,int sl) noex {
    	bool f = true ;
    	while (sl-- && sp[0]) {
	    f = ishexlatin(sp[0]) ;
	    if (! f) break ;
	} /* end while */
	return f ;
} /* end subroutine (hasINET6num) */


