/* sixsub SUPPORT */
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
	2. Note that the standard |strcasestr(3c)| subroutine cannot
	be used since it fails to properly handle Latin(-1) characters.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usys.h>		/* LIBU |strsub(3u)| + |strsubn(3u)| */
#include	<ucmpx.h>		/* LIBU */
#include	<toxc.h>		/* LIBUC |toxc(3u)| */
#include	<localmisc.h>		/* LIBU */

#include	"sixsub.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using usys::strsub ;			/* subroutine */
using usys::strsubn ;			/* subroutine */


/* local typedefs */

extern "C" {
    typedef int (*toxc_f)	(int) noex ;
    typedef int (*cmpn_f)	(cchar *,cchar *,int) noex ;
} /* end extern (C) */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<toxc_f toxc,cmpn_f cmpn>
local int sixsub(cchar *sp,int µsl,cchar *ss) noex {
	int		i{} ; /* used-afterwards */
	bool		f = false ;
	if (int sl ; ss && ((sl = getlenstr(sp,µsl)) > 0)) {
	    cint	sslen = lenstr(ss) ;
	    if (sslen <= sl) ylikely {
	        cint	chx_ss = toxc(ss[0]) ;
	        for (i = 0 ; i < (sl - sslen) ; i += 1) {
		    if (toxc(sp[i]) == chx_ss) {
	                f = (cmpn((sp + i),ss,sslen) == 0) ;
		    }
	            if (f) break ;
	        } /* end for */
	    } /* end if (possible) */
	} /* end if (getlenstr) */
	return (f) ? i : -1 ;
} /* end subroutine-template (sixsub) */


/* local variables */


/* exported variables */


/* exported subroutines */

int sibasesub(cchar *sp,int sl,cchar *ss) noex {
    	int		si = -1 ;
	if (sp && ss) ylikely {
	    if (sl >= 0) {
	        if (syshas.strnstr) {
	            if (cc *tp = strsubn(sp,ss,sl)) {
		        si = intconv(tp - sp) ;
	            }
	        } else {
	            si = sixsub<tobc,cmpstrnbc>(sp,sl,ss) ;
	        } /* end if_constexpr (f_strnstr) */
	    } else {
	        if (cc *tp = strsub(sp,ss)) {
		    si = intconv(tp - sp) ;
	        }
	    } /* end if (length given or not) */
	} /* end if (non-null) */
	return si ;
} /* end subroutine (sibasesub) */

int sicasesub(cchar *sp,int sl,cchar *ss) noex {
    	int		si = -1 ;
	if (sp && ss) ylikely {
	    si = sixsub<touc,cmpstrnic>(sp,sl,ss) ;
	} /* end if (non-null) */
	return si ;
} /* end subroutine (sicasesub) */

int sifoldsub(cchar *sp,int sl,cchar *ss) noex {
    	int		si = -1 ;
	if (sp && ss) ylikely {
	    si = sixsub<tofc,cmpstrnfc>(sp,sl,ss) ;
	} /* end if (non-null) */
	return si ;
} /* end subroutine (sifoldsub) */


