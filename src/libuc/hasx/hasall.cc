/* hasall SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasall{x}

	Description:
	These subroutines check if the specified counted c-string
	has all of some characteristic we are looking for.  All of
	the characters in the given c-string must belong to the
	specificed character class.

	Names:
	hasallalpha
	hasallalnum
	hasalldigit
	hasalldigex
	hasalloctal
	hasallwhite
	hasallblank
	hasallprint
	hasalllc
	hasalluc
	hasallhdrkey
	hasallterm
	hasallbase
	hasallchr
	hasallset
	
	Aliases:
	hasalldig
	hasalloct
	hasalldec
	hasallhex
	hasallwht
	hasallblk


	Name:
	hasall{class}

	Description:
	Are all of the characters in the given counted c-string within
	the give character chass?

	Synopsis:
	bool hasall{class}(cchar *sp,int sl) noex

	Arguments:
	{class}		one of: alpha, alnum, digit, digex, octal, 
			white, blank, lc, uc, hdrkey
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string condition fails
	true		c-string condition succeeds


	Name:
	hasallbase
	hasallchr
	hasallset

	Description:
	We test if a counted strin contains all of the proper digits
	consistent with the numeric base supplied.

	Synopsis:
	bool hasallbase(cchar *sp,int sl,int base) noex
	bool hasallchr(cchar *sp,int sl,int ch) noex
	bool hasallset(cchar *sp,int sl,cchar *sstr) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length
	base		base to check against
	ch		character occupying given string
	sset		character set (a c-string) occupying given string

	Returns:
	false		c-string condition fails
	true		c-string condition succeeds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>
#include	<mkchar.h>
#include	<char.h>		/* |CHAR_TOVAL(3uc)| */
#include	<ischarx.h>		/* |ishdrkey(3uc)| + |iswht(3uc)| */
#include	<localmisc.h>		/* |UC(3dam)| */

#include	"hasall.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef bool (*isc_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool hasallx(isc_f isx,cchar *sp,int sl) noex {
	bool		f = true ;
	if (sp) ylikely {
	    while (sl && *sp) {
		cint ch = mkchar(*sp) ;
	        f = isx(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasallx) */


/* local variables */

static cint		maxbase = ulibval.maxbase ;


/* exported variables */


/* exported subroutines */

bool hasallalpha(cchar *sp,int sl) noex {
    	return hasallx(isalphalatin,sp,sl) ;
}
/* end subroutine (hasallalpha) */

bool hasallalnum(cchar *sp,int sl) noex {
    	return hasallx(isalnumlatin,sp,sl) ;
}
/* end subroutine (hasallalnum) */

bool hasalldigit(cchar *sp,int sl) noex {
    	return hasallx(isdigitlatin,sp,sl) ;
}
/* end subroutine (hasalldigit) */

bool hasalldigex(cchar *sp,int sl) noex {
    	return hasallx(isdigexlatin,sp,sl) ;
}
/* end subroutine (hasalldigex) */

bool hasalloctal(cchar *sp,int sl) noex {
    	return hasallx(isoctallatin,sp,sl) ;
}
/* end subroutine (hasalloctal) */

bool hasallwhite(cchar *sp,int sl) noex {
    	return hasallx(iswhitelatin,sp,sl) ;
}
/* end subroutine (hasallwhite) */

bool hasallblank(cchar *sp,int sl) noex {
    	return hasallx(isblanklatin,sp,sl) ;
}
/* end subroutine (hasallwhite) */

bool hasallprint(cchar *sp,int sl) noex {
    	return hasallx(isprintlatin,sp,sl) ;
}
/* end subroutine (hasallprint) */

bool hasalllc(cchar *sp,int sl) noex {
    	return hasallx(islowerlatin,sp,sl) ;
}
/* end subroutine (hasalllc) */

bool hasalluc(cchar *sp,int sl) noex {
    	return hasallx(isupperlatin,sp,sl) ;
}
/* end subroutine (hasalluc) */

bool hasallhdrkey(cchar *sp,int sl) noex {
    	return hasallx(ishdrkey,sp,sl) ;
}
/* end subroutine (hasallhdrkey) */

bool hasallterm(cchar *sp,int sl) noex {
    	return hasallx(istermlatin,sp,sl) ;
}
/* end subroutine (hasallterm) */

bool hasallbase(cchar *sp,int sl,int b) noex {
	bool		f = false ;
	if (sp && (maxbase >= 0)) ylikely {
	    if ((b >= 2) && (b <= maxbase)) {
	        while (sl && *sp) {
	            cint	ch = mkchar(*sp) ;
	            cint	v = CHAR_TOVAL(*sp) ;
	            f = (v < b) ;
	            f = f || ((ch == '-') || iswht(ch) || (ch == CH_NBSP)) ;
	            if (! f) break ;
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	    } /* end if (valid base) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallbase) */

namespace {
    struct haser {
	virtual bool cond(int) const noex = 0 ;
	bool operator () (cchar *,int) const noex ;
    } ; /* end struct (haser) */
    struct haser_chr : haser {
	int	chx_s ;
	haser_chr(int c) noex : chx_s(c) { } ;
	bool cond(int ch) const noex override final {
	    return (ch == chx_s) ;
	} ;
    } ; /* end struct */
    struct haser_set : haser {
	cchar	*sstr ;
	haser_set(cchar *s) noex : sstr(s) { } ;
	bool cond(int ch) const noex override final {
	    return (strchr(sstr,ch) != nullptr) ;
	} ;
    } ; /* end struct */
} /* end namespace */

bool haser::operator () (cchar *sp,int sl) const noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = cond(ch) ;
	        if (! f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
} /* end method (haser::operator) */

bool hasallchr(cchar *sp,int sl,int chx_s) noex {
    	haser_chr ho(chx_s) ;
	return ho(sp,sl) ;
}
/* end subroutine (hasallchr) */

bool hasallset(cchar *sp,int sl,cchar *sstr) noex {
    	bool		f = false ;
	if (sstr) {
    	    haser_set ho(sstr) ;
	    f = ho(sp,sl) ;
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (hasallset) */


/* local subroutines */


