/* sfthing SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* string-find a thing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfthing

	Description:
	This subroutine finds a "thing" in a string.  I know that
	this does not explain a lot, so please read the code just
	a bit to see what is going on.  Thanks for your patience.

	Synopsis:
	int sfthing(cchar *sp,int sl,cchar *ss,cchar *rpp) noex

	Arguments:
	sp	supplied string to test
	sl	length of supplied string to test
	ss	"thing" characters
	rpp	pointer to store result "thing" pointer

	Returns:
	>=0	length of result "thing" 
	<0	error (will be '-1')

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"sfx.h"


/* local defines */

#define	DEBLEN(slen,m)	(((slen) >= 0) ? MIN((slen),(m)) : (m))

#undef	CH_THING
#define	CH_THING	'$'


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getthing(cchar *,int,cchar *,cchar **) noex ;
static bool	hasgood(cchar *,int) noex ;
static bool	isour(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int sfthing(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		cl = -1 ;
	cchar		*cp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl >= 4) {
	    cchar	*tp ;
	    while ((tp = strnchr(sp,sl,CH_THING)) != nullptr) {
	        sl -= (tp - sp) ;
	        sp = tp ;
	        cl = getthing(sp,sl,ss,&cp) ;
	        if (cl >= 0) break ;
	        sl -= 1 ;
	        sp += 1 ;
	    } /* end while */
	} /* end if */
	if (rpp) {
	    *rpp = (cl >= 0) ? cp : nullptr ;
	}
	return cl ;
}
/* end subroutine (sfthing) */


/* local subroutines */

static int getthing(cchar *sp,int sl,cchar *ss,cchar **rpp) noex {
	int		cl = 0 ;
	bool		f = false ;
	cchar		*cp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl > 0) {
	    char	buf[3] ;
	    buf[0] = '$' ;
	    buf[1] = ss[0] ;
	    buf[2] = '\0' ;
	    if (cchar *tp ; (tp = strnsub(sp,sl,buf)) != nullptr) {
	        cl = sl - ((tp + 2) - sp) ;
	        cp = (tp + 2) ;
	        if ((tp = strnchr(cp,cl,ss[1])) != nullptr) {
	            cl = (tp - cp) ;
	            f = hasgood(cp,cl) ;
	        }
	    }
	} /* end if (positive) */
	if (rpp) {
	    *rpp = (f) ? cp : nullptr ;
	}
	return (f) ? cl : -1 ;
}
/* end subroutine (getthing) */

static bool hasgood(cchar *sp,int sl) noex {
	bool		f = true ;
	for (int i = 0 ; i < sl ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    if (ch == '=') break ;
	    f = isour(ch) ;
	    if (! f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (hasgood) */

static bool isour(int ch) noex {
	bool		f = isalnumlatin(ch) ;
	f = f || (ch == '_') || (ch == '-') || (ch == '=') || (ch == '/') ;
	return f ;
}
/* end subroutine (isour) */


