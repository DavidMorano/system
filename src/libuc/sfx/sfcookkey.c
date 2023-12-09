/* sfcookkey */
/* lang=C20 */

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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */

#undef	CH_COOK
#define	CH_COOK		'%'


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getkey(cchar *,int,cchar *,cchar **) noex ;


/* local variables */


/* exported subroutines */

int sfcookkey(cchar *sp,int sl,cchar **rpp) noex {
	const int	sch = CH_COOK ;
	int		cl = -1 ;
	cchar	*cp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl >= 1) {
	    cchar	*ss = "{}" ;
	    cchar	*tp ;
	    while ((tp = strnchr(sp,sl,sch)) != nullptr) {
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	        if (sl > 0) {
	            if (sp[0] != sch) {
	                cl = getkey(sp,sl,ss,&cp) ;
	                if (cl >= 0) break ;
	            }
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end if */
	    } /* end while */
	} /* end if */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
}
/* end subroutine (sfcookkey) */


/* local subroutines */

static int getkey(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		cl = -1 ;
	cchar	*cp = nullptr ;
	if (sl > 0) {
	    if (sp[0] == ss[0]) {
	        cchar	*tp ;
	        sp += 1 ;
	        sl -= 1 ;
	        if ((tp = strnchr(sp,sl,ss[1])) != nullptr) {
	            cp = sp ;
	            cl = (tp-sp) ;
	        }
	    } else {
	        cp = sp ;
	        cl = 1 ;
	    }
	} /* end if */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
}
/* end subroutine (getkey) */


