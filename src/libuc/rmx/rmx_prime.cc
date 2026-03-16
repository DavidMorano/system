/* rmx_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* remove characters from the end of the given c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	rmeol
	rmwht
	rmblk
	rmtrailclass
	rmtrailchr
	rmochr
	rmrchr
	rmext
	rmcomment

	Description:
	Return the length of the given string without any EOL
	(End-Of-Line) characters included. Any EOL characters are
	only considered starting from the end of the counted string.

	Synopsis:
	int rmeol(cchar *sp,int sl) noex
	int rmwht(cchar *sp,int sl) noex
	int rmblk(cchar *sp,int sl) noex
	int rmtailclass(cchar *sp,int sl,cchar *ss) noex
	int rmtrailchr(cchar *sp,int sl,int sch) noex
	int rmochr(cchar *sp,int sl,int sch) noex
	int rmrchr(cchar *sp,int sl,int sch) noex
	int rmext(cchar *sp,int sl) noex
	int rmcomment(cchar *sp,int sl) noex

	Arguments:
	sp		pinter to constant string to consider
	sl		length of supplied string
	ss		c-string of characters representing a class
	sch		character to search for

	Returns:
	-		length of string without removed ending

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
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


/* forward references */

namespace {
    struct rmtrailer {
	iscond_f	ourcond ;
        cchar		*sp ;
        int		sl ;
	rmtrailer(cchar *p,int l) noex : sp(p), sl(l) { } ;
	bool x() noex ;
	virtual bool iscond(int ch) const noex {
	    return ourcond(ch) ;
	} ;
	bool operator () (iscond_f cond) noex {
	    ourcond = cond ;
	    return x() ;
	} ;
    } ; /* end struct (rmtrailer) */
} /* end namespace */

local bool isclass(cchar *ss,int ch) noex {
    	ch &= UCHAR_MAX ;
    	return (strchr(ss,ch) != nullptr) ;
} /* end subroutine (isclass) */


/* local variables */


/* exported variables */


/* exported subroutines */

bool rmtrailer::x()  noex {
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

int rmeol(cchar *sp,int sl) noex {
    	rmtrailer to(sp,sl) ;
	return to(iseol) ;
}
/* end subroutine (rmeol) */

int rmwht(cchar *sp,int sl) noex {
    	rmtrailer to(sp,sl) ;
	return to(iswht) ;
}
/* end subroutine (rmwht) */

int rmblk(cchar *sp,int sl) noex {
    	rmtrailer to(sp,sl) ;
	return to(isblk) ;
}
/* end subroutine (rmblk) */

namespace {
    struct rmtrailer_class : rmtrailer {
	cchar	*ss ;
	rmtrailer_class(cchar *p,int l,cchar *s) noex : rmtrailer(p,l) {
	    ss = s ;
	} ;
	bool iscond(int ch) const noex override final {
	    return isclass(ss,ch) ;
	} ;
    } ; /* end struct (rmtrailer_class) */
    struct rmtrailer_chr : rmtrailer {
	int	chx_s ;
	rmtrailer_chr(cchar *p,int l,int c) noex : rmtrailer(p,l) {
	    chx_s = c ;
	} ;
	bool iscond(int ch) const noex override final {
	    ch &= UCHAR_MAX ;
	    return (ch == chx_s) ;
	} ;
    } ; /* end struct (rmtrail_chr) */
} /* end namespace */

int rmtrailclass(cchar *sp,int sl,cchar *ss) noex {
    	rmtrailer_class to(sp,sl,ss) ;
	return to.x() ;
}
/* end subroutine (rmtrailclass) */

int rmtrailchr(cchar *sp,int sl,int sch) noex {
    	rmtrailer_chr to(sp,sl,sch) ;
	return to.x() ;
}
/* end subroutine (rmtrailchr) */

extern "C" {
    typedef char *(*strnx_f)(cchar *,int,int) noex ;
}

local int rmxchr(strnx_f sx,cchar *sp,int sl,int ch) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (cchar *tp ; (tp = sx(sp,sl,ch)) != nullptr) {
	        sl = intconv(tp - sp) ;
	    }
	} else {
	    sl = -1 ;
	} /* end if */
	return sl ;
}
/* end subroutine (rmxchr) */

int rmochr(cchar *sp,int sl,int ch) noex {
    	return rmxchr(strnochr,sp,sl,ch) ;
}
/* end subroutine (rmochr) */

int rmrchr(cchar *sp,int sl,int ch) noex {
    	return rmxchr(strnrchr,sp,sl,ch) ;
}
/* end subroutine (rmrchr) */

int rmext(cchar *sp,int sl) noex {
    	if (sp) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
    	    if (cint si = siext(sp,sl) ; si >= 0) {
	        sl = si ;
	    }
	} else {
	    sl = -1 ;
	} /* end if (non-null) */
    	return sl ;
}
/* end subroutine (rmext) */

int rmcomment(cchar *lp,int ll) noex {
	int		rl = -1 ;
	if (lp) ylikely {
	    if (ll < 0) ll = lenstr(lp) ;
	    if ((rl = rmochr(lp,ll,'#')) == ll) {
	       rl = rmeol(lp,ll) ;
	    }
	} /* end if (non-null) */
	return rl ;
}
/* end subroutine (rmcomment) */


