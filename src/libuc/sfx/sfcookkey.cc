/* sfcookkey SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* string-find a cookie key */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfcookkey

	Description:
	This subroutine finds a cookie key in a string.  

	Synopsis:
	int sfcookkey(cchar *sp,int sl,cchar **rpp) noex

	Arguments:
	sp	supplied string to test
	sl	length of supplied string to test
	rpp	pointer to store result "thing" pointer

	Returns:
	>=0	length of resulting key-name
	<0	no key found

	Notes:
	A zero-length key-name can be returned.  This is an error
	but it should be processed so that the consequences of the
	error can be made manifest.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC |strnchr(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"sfx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#undef	CHX_COOK
#define	CHX_COOK	'%'


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	getkey(cchar *,int,cchar *,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfcookkey(cchar *sp,int sl,cchar **rpp) noex {
    	cnullptr	np{} ;
	cint		sch = CHX_COOK ;
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl >= 1) ylikely {
	        cchar	ss[] = "{}" ;
	        for (cc *tp ; (tp = strnchr(sp,sl,sch)) != np ; ) {
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (sl > 0) {
	                if (sp[0] != sch) {
	                    cl = getkey(sp,sl,ss,&cp) ;
	                    if (cl >= 0) break ;
	                }
	                sp += 1 ;
	                sl -= 1 ;
	            } /* end if */
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
} /* end subroutine (sfcookkey) */


/* local subroutines */

local int getkey(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ;
	if (sp && ss) ylikely {
	    if (sl > 0) ylikely {
	        if (sp[0] == ss[0]) {
	            sp += 1 ;
	            sl -= 1 ;
	            if (cc *tp = strnchr(sp,sl,ss[1]) ; tp) {
	                cp = sp ;
	                cl = intconv(tp - sp) ;
	            }
	        } else {
	            cp = sp ;
	            cl = 1 ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
} /* end subroutine (getkey) */


