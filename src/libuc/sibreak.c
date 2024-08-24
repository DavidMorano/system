/* sibreak SUPPORT */
/* lang=C++20 */

/* subroutine to find a character in a given string */
/* version %I% last-modified %G% */

#define	CF_STRCHR	1		/* use |strchr(3c)| */

/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	sibreak

	Description:
	This subroutine determines if any of the characters from
	the parameter string (argument 'ss') are found in the primary
	string buffer specified ('s').  

	Synopsis:
	int sibreak(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined
	sl	length of string of break character to break on
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
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"six.h"


/* local defines */

#ifndef	CF_STRCHR
#define	CF_STRCHR	1		/* use |strchr(3c)| */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* forward references */


/* local variables */

constexpr bool		f_strchr = CF_STRCHR ;


/* exported variables */


/* exported subroutines */

int sibreak(cchar *sp,int sl,cchar *ss) noex {
	int		i ; /* used afterwards */
	bool		f = false ;
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
	        for (i = 0 ; sl && sp[i] ; i += 1) {
	            for (int j = 0 ; ss[j] ; j += 1) {
	                f = (sp[i] == ss[j]) ;
		        if (f) break ;
	            } /* end for */
	            if (f) break ;
	            sl -= 1 ;
	        } /* end for */
	    } /* end if_constexpr (f_strchr) */
	} else {
	    f = ((i = strcspn(sp,ss)) , (sp[i] != '\0')) ;
	}
	return (f) ? i : -1 ;
}
/* end subroutine (sibreak) */


