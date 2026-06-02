/* haspath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does the given counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	haspath{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

	Synopsis:
	bool haspath{x}(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it

	Example-paths:
	~<user>[/something/more]
	¬<var>[/something/more]
	%<???>[/something/more]

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"haspath.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"
#pragma		GCC dependency		"mod/chrset.ccm"
#pragma		GCC dependency		"mod/sif.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */
import ureserve ;
import chrset ;
import sif ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

cint	chx_user	= mkchar('~') ;
cint	chx_var		= mkchar('¬') ;
cint	chx_multi	= mkchar('%') ;

namespace {
    struct prefixer {
    	chrset	chrs ;
	consteval void mkchrs() noex {
	    chrs.set(chx_user) ;
	    chrs.set(chx_var) ;
	} ; /* end method */
	consteval prefixer() noex {
	    mkchrs() ;
	} ; /* end ctor */
	bool operator [] (int ch) const noex {
	    ch &= UCHAR_MAX ;
	    return bool(chrs[ch]) ;
	} ; /* end method */
    } ; /* end struct (prefixer) */
} /* end namespace */


/* forward references */

local bool haspathx(int chx,cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    if (int ch ; sl && ((ch = mkchar(sp[0])))) {
	        f = (ch == chx) ;
	    }
	} /* end if (non-null) */
	return f ;
} /* end subroutine (haspathx) */

local inline bool hasmulti(cchar *,int) noex ;


/* local variables */

constexpr prefixer	prefix_data ;


/* exported variables */


/* exported subroutines */

bool haspathuser(cchar *sp,int sl) noex {
    	return haspathx(chx_user,sp,sl) ;
} /* end subroutine (haspathuser) */

bool haspathvar(cchar *sp,int sl) noex {
    	return haspathx(chx_var,sp,sl) ;
} /* end subroutine (haspathvar) */

#ifdef	COMMENT
bool haspathmulti(cchar *sp,int sl) noex {
    	return haspathx(chx_multi,sp,sl) ;
} /* end subroutine (haspathvar) */
#endif /* COMMENT */

bool haspathprefix(cchar *sp,int sl) noex {
	bool f = false ;
	if (sp) ylikely {
	    if (sl && sp[0]) {
		f = prefix_data[sp[0]] ;
	    }
	} /* end if (non-null) */
	return f ;
} /* end subroutine (haspathprefix) */

bool haspathmulti(cchar *sp,int sl) noex {
    	bool f = false ;
	if (cchar *cp ; sp) ylikely {
	    sif so(sp,sl,chx_multi) ; 
	    for (int cl ; (cl = so.chr(&cp)) >= 0 ; ) {
		if ((f = hasmulti(cp,cl))) break ;
	    } /* end for */
	} /* end if (non-null) */
    	return f ;
} /* end subroutine (haspathmulti) */


/* local subroutines */

local bool hasmulti(cchar *cp,int cl) noex {
    	bool f = false ;
	if (cl > 1) {
	    f = (cp[0] == chx_multi) && isalphalatin(cp[1]) ;
	}
    	return f ;
} /* end subroutine (hasmulti) */


