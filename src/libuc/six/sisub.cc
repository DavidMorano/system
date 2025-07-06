/* sisub SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find the string-index of a sub-stringin the given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	si{x}sub

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not in the buffer specified by the first two
	arguments.  This subroutine either returns (-1) or it returns
	the character position in the buffer of where the string
	starts.

	Synopsis:
	int si{x}sub(cchar *sp,int sl,cchar *ss) noex

	Arguments:
	sp	string to be examined
	sl	length of string to be examined
	ss	null terminated substring to search for

	Returns:
	>=0	index of found substring
	<0	substring not found

	Notes:
	1. Rationale for using templates rather than passing a 
	function-pointer:
	The thinking is that I want the template instantiation to
	"see" any inlined subroutine and optimize it all out rather
	than being forced to actually call a subroutine (if only a
	pointer was supplied).  Anyway, that was the thinking, even
	if misguided.  Also note that this goes against one of the
	major coding standard rules which is: to optimize for space
	rather than time (without the compiler doing that operation
	itself).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<nleadstr.h>
#include	<toxc.h>
#include	<localmisc.h>

#include	"six.h"

import libutil ;

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)(int) noex ;
    typedef int (*nleadxstr_f)(cchar *,cchar *,int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<toxc_f toxc,nleadxstr_f nleadxstr>
int sixsub(cchar *sp,int sl,cchar *ss) noex {
	cint		sslen = lenstr(ss) ;
	int		i{} ; /* used-afterwards */
	bool		f = false ;
	if (sl < 0) sl = lenstr(sp) ;
	if (sslen <= sl) {
	    cint	sslead = toxc(ss[0]) ;
	    int		m ;
	    for (i = 0 ; i <= (sl - sslen) ; i += 1) {
		f = ((sslen == 0) || (toxc(sp[i]) == sslead)) ;
		if (f) {
	            m = nleadxstr((sp + i),ss,sslen) ;
	            f = (m == sslen) ;
		}
	        if (f) break ;
	    } /* end for */
	} /* end if (possible) */
	return (f) ? i : -1 ;
}
/* end subroutine-template (sixsub) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sibasesub(cchar *sp,int sl,cchar *ss) noex {
	return sixsub<tobc,nleadbasestr>(sp,sl,ss) ;
}
/* end subroutine (sibasesub) */

int sicasesub(cchar *sp,int sl,cchar *ss) noex {
	return sixsub<touc,nleadcasestr>(sp,sl,ss) ;
}
/* end subroutine (sicasesub) */

int sifoldsub(cchar *sp,int sl,cchar *ss) noex {
	return sixsub<tofc,nleadfoldstr>(sp,sl,ss) ;
}
/* end subroutine (sifoldsub) */


