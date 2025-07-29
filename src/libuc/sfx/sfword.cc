/* sfword SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find an English word within the given string */
/* version %I% last-modified %G% */

#define	CF_ALLOWSMODE	0		/* faster? */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sfword

	Description:
	This subroutine finds an English language word within the
	given string.  Leading white space is removed along with
	leading or trailing single-quote (apostrophe) characters.
	Trailing single quotes are not removed if they appear to
	be making the embodied English word possessive.  Also,
	leading and trailing hypen (minus-sign) characters are also
	removed (and will be removed even when intermixed with
	regular white-space characters).

	Synopsis:
	int sfword(cchar *sp,sl,cchar **rpp) noex

	Arguments:
	sp		source string
	sl		length of supplied source string
	rpp		pointer to pointer to get result (an English word)

	Return:
	-		length of resulting word

	Notes:
	= CF_ALLOWSMORE: I do not know for sure but this was a
	(vain?) attempt at speeding up this subroutine.  Profiles
	show that time is being spent in here (how much compared
	with everything else?).  There is not much to improve upon,
	without removing nice old faithful subroutine calls (like
	|sfshrink(3dam)|).  Instead we play around (elsewhere) with
	removing the string conversion to lower-case before an array
	comparison. You be the judge.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strncmp(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>		/* |CH_{xx}| */
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<char.h>		/* |char_iswhite(3uc)| + ... */
#include	<localmisc.h>

#include	"sfx.h"			/* |sfshrink(3uc)| */


/* local defines */

#ifndef	CF_ALLOWSMODE
#define	CF_ALLOWSMODE	0
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	sfshrinkmore(cchar *,int,cchar **) noex ;
static bool	iswhitemore(int) noex ;


/* local variables */

constexpr cpcchar	allows[] = {
	"'s",
	"'t",
	"s'",
#if	CF_ALLOWSMODE
	"'S",
	"'T",
	"S'",
#endif /* CF_ALLOWSMODE */
	nullptr
} ; /* end array (allows) */

cbool		f_allows = CF_ALLOWSMODE ;


/* exported variables */


/* exported subroutines */

int sfword(cchar *sp,int sl,cchar **rpp) noex {
    	cnullptr	np{} ;
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ;
	if (sp) ylikely {
	    if ((cl = sfshrinkmore(sp,sl,&cp)) > 0) {
	        bool	f = false ;
	        if (cp[0] == CH_SQUOTE) {
		    cp += 1 ;
		    cl -= 1 ;
	        }
	        if (cl > 2) {
	            if (cc *tp ; (tp = strnrchr(cp,cl,CH_SQUOTE)) != np) {
		        f = ((cl - ((tp + 1) - cp)) > 1) ;
		    }
		    if (! f) {
	    	        int	i = (cl - 2) ;
		        if_constexpr (f_allows) {
			    cchar	*cs = (cp + i) ;
	                    for (int j = 0 ; allows[j] ; j += 1) {
			        f = (allows[j][0] == cs[0]) ;
	                        f = f && (strncmp(allows[j],cs,2) == 0) ;
			        if (f) break ;
	                    } /* end for */
		        } else {
			    char	lowbuf[4] ;
			    cchar	*cs = lowbuf ;
			    lowbuf[0] = CHAR_TOLC((cp + i)[0]) ;
			    lowbuf[1] = CHAR_TOLC((cp + i)[1]) ;
	                    for (int j = 0 ; allows[j] ; j += 1) {
			        f = (allows[j][0] == cs[0]) ;
	                        f = f && (strncmp(allows[j],cs,2) == 0) ;
			        if (f) break ;
	                    } /* end for */
		        } /* end if_constexpr (f_allows) */
		    } /* end if */
	        } /* end if (more than one character) */
	        if ((! f) && (cp[cl - 1] == CH_SQUOTE)) {
	            cl -= 1 ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	if (rpp) *rpp = cp ;
	return cl ;
}
/* end subroutine (sfword) */


/* local subroutines */

static int sfshrinkmore(cchar *sp,int sl,cchar **rpp) noex {
	int		cl = 0 ;
	cchar		*cp ;
	if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	    while (cl && iswhitemore(cp[0])) {
		cp += 1 ;
		cl -= 1 ;
	    }
	    *rpp = cp ;
	    while (cl && iswhitemore(cp[cl-1])) {
		cl -= 1 ;
	    }
	} /* end if (sfshrink) */
	return cl ;
}
/* end subroutine (sfshrinkmore) */

static bool iswhitemore(int ch) noex {
	return CHAR_ISWHITE(ch) || (ch == '-') ;
}
/* end subroutine (iswhitemore) */


