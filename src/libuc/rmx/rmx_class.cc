/* rmx_class SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove characters from the end of the given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	rmeol
	rmwht
	rmblk
	rmtrailclass
	rmtrailchr

	Description:
	Search the given counted c-string, from its end, for a
	character (or set of characters) that meet the given criteria
	and return the remaining length of the up until that point.

	Synopsis:
	int rmeol(cchar *sp,int sl) noex
	int rmwht(cchar *sp,int sl) noex
	int rmblk(cchar *sp,int sl) noex
	int rmtailclass(cchar *sp,int sl,cchar *ss) noex
	int rmtrailchr(cchar *sp,int sl,int sch) noex

	Arguments:
	sp		counted c-string pinter
	sl		counted c-string length
	ss		c-string of characters representing a class
	sch		character to search for

	Returns:
	>=0		index of found criteria
	<0		critera was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<strn.h>		/* |strn{x}chr(3uc)| */
#include	<six.h>			/* |siext(3uc)| */
#include	<ischarx.h>		/* |iseol(3uc)| */
#include	<localmisc.h>

#include	"rmx.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*iscond_f)(int) noex ;
}

/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct rmtrailer {
	iscond_f	ourcond = nullptr ;
	rmtrailer() noex = default ;
	rmtrailer(iscond_f fun) noex : ourcond(fun) { } ;
	virtual bool iscond(int ch) const noex {
	    return ourcond(ch) ;
	} ;
	int operator () (cchar *,int) noex ;
    } ; /* end struct (rmtrailer) */
    struct rmtrailer_class : rmtrailer {
	cchar	*ss ;
	rmtrailer_class(cchar *s) noex {
	    ss = s ;
	} ;
	bool iscond(int ch) const noex override final {
	    ch &= UCHAR_MAX ;
    	    return (strchr(ss,ch) != nullptr) ;
	} ;
    } ; /* end struct (rmtrailer_class) */
    struct rmtrailer_chr : rmtrailer {
	int	chx_s ;
	rmtrailer_chr(int c) noex {
	    chx_s = c ;
	} ;
	bool iscond(int ch) const noex override final {
	    ch &= UCHAR_MAX ;
	    return (ch == chx_s) ;
	} ;
    } ; /* end struct (rmtrail_chr) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int rmeol(cchar *sp,int sl) noex {
    	rmtrailer to(iseol) ;
	return to(sp,sl) ;
} /* end subroutine (rmeol) */

int rmwht(cchar *sp,int sl) noex {
    	rmtrailer to(iswht) ;
	return to(sp,sl) ;
} /* end subroutine (rmwht) */

int rmblk(cchar *sp,int sl) noex {
    	rmtrailer to(isblk) ;
	return to(sp,sl) ;
} /* end subroutine (rmblk) */

int rmtrailclass(cchar *sp,int sl,cchar *ss) noex {
    	rmtrailer_class to(ss) ;
	return to(sp,sl) ;
} /* end subroutine (rmtrailclass) */

int rmtrailchr(cchar *sp,int sl,int sch) noex {
    	rmtrailer_chr to(sch) ;
	return to(sp,sl) ;
} /* end subroutine (rmtrailchr) */


/* local subroutines */

int rmtrailer::operator () (cchar *sp,int sl) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl && iscond(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	}
	return sl ;
} /* end method (rmtrailer::operator) */


