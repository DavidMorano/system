/* sixbrk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to find a character in a given string */
/* version %I% last-modified %G% */

#define	CF_STRCHR	0		/* use |strchr(3c)| */

/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	siobrk
	sirbrk

	Description:
	This subroutine determines if any of the characters from
	the parameter string (argument 'ss') are found in the primary
	string buffer specified ('s').  

	Synopsis:
	int siobrk(cchar *sp,int sl,cchar *ss) noex
	int sirbrk(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined pointer
	sl	string to be examined length
	ss	NUL-terminated substring to search for

	Returns:
	>=0	position (in 's') of a character from 'ss'
	<0	no characters from string 'ss' were present in 's'

	Notes:
	This subroutine is almost identical to the |strcspn(3c)|
	subroutine except that this subroutine returns (-1) if there
	is a failure to match a charater, while |strcspn(3c)| returns
	the index of the NUL character of the source string for a
	failure to match.  The real problem with |strcspn(3c)| is
	that it does not take a counted source c-string as an
	argument, which is what we want here.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"sixbrk.h"

import libutil ;			/* |lenstr(3u)| + |xstrcspn(3u)| */
import chrset ;

/* local defines */

#ifndef	CF_STRCHR
#define	CF_STRCHR	1		/* use |strchr(3c)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_strchr = CF_STRCHR ;


/* exported variables */


/* exported subroutines */

int siobrk(cchar *sp,int sl,const chrset &sset) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = 0 ; sl && sp[i] ; i += 1) {
		cint	ch = mkchar(sp[i]) ;
		if ((f = sset.tst(ch))) break ;
	        sl -= 1 ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (siobrk) */

int siobrk(cchar *sp,int sl,cchar *ss) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp && ss) ylikely {
	    if (sl >= 0) {
	        if_constexpr (f_strchr) {
	            cnullptr	np{} ;
	            for (i = 0 ; sl && sp[i] ; i += 1) {
		        cint	ch = mkchar(sp[i]) ;
		        f = (strchr(ss,ch) != np) ;
	                if (f) break ;
	                sl -= 1 ;
	            } /* end for */
	        } else {
		    chrset	sset(ss) ;
	            for (i = 0 ; sl && sp[i] ; i += 1) {
		        cint	ch = mkchar(sp[i]) ;
		    	if ((f = sset.tst(ch))) break ;
	                sl -= 1 ;
	            } /* end for */
	        } /* end if_constexpr (f_strchr) */
	    } else {
	        f = ((i = xstrcspn(sp,ss)) , (sp[i] != '\0')) ;
	    }
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (siobrk) */

int sirbrk(cchar *sp,int sl,const chrset &sset) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = (sl - 1) ; i >= 0 ; i -= 1) {
		cint	ch = mkchar(sp[i]) ;
		if ((f = sset.tst(ch))) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (sirbrk) */

int sirbrk(cchar *sp,int sl,cchar *ss) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	if (sp && ss) ylikely {
	    chrset	sset(ss) ;
	    if (sl < 0) sl = lenstr(sp) ;
	    for (i = (sl - 1) ; i >= 0 ; i -= 1) {
		cint	ch = mkchar(sp[i]) ;
		if ((f = sset.tst(ch))) break ;
	    } /* end for */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
} /* end subroutine (sirbrk) */


