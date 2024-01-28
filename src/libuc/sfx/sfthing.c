/* sfthing SUPPORT */
/* lang=C20 */

/* string-find a thing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was adapted from others programs that did
	similar types of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

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

#include	<envstandards.h>
#include	<string.h>		/* <- for |strlen(3c)| */
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<strn.h>
#include	<ischarx.h>

#include	"sfx.h"


/* local defines */

#define	DEBLEN(slen,max)	(((slen) >= 0) ? MIN((slen),(max)) : (max))

#undef	CH_THING
#define	CH_THING	'$'


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getthing(cchar *,int,cchar *,cchar **) noex ;
static int	hasgood(cchar *,int) noex ;
static int	isour(int) noex ;


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
	int		f = false ;
	cchar		*cp = nullptr ;
	if (sl < 0) sl = strlen(sp) ;
	if (sl > 0) {
	    cchar	*tp ;
	    char	buf[3] ;
	    buf[0] = '$' ;
	    buf[1] = ss[0] ;
	    buf[2] = '\0' ;
	    if ((tp = strnsub(sp,sl,buf)) != nullptr) {
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

static int hasgood(cchar *sp,int sl) noex {
	int		ch ;
	int		f = true ;
	for (int i = 0 ; i < sl ; i += 1) {
	    ch = (sp[i] & 0xff) ;
	    if (ch == '=') break ;
	    f = isour(ch) ;
	    if (! f) break ;
	} /* end for */
	return f ;
}
/* end subroutine (hasgood) */

static int isour(int ch) noex {
	int		f = isalnumlatin(ch) ;
	f = f || (ch == '_') || (ch == '-') || (ch == '=') || (ch == '/') ;
	return f ;
}
/* end subroutine (isour) */


