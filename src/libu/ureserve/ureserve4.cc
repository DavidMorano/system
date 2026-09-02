/* ureserve5 MODULE (sfx -- module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces - sfx */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2020-05-07, David A-D- Morano
	I converted this (formerly a header-only file) to a module.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Module:
	ureserve

	Description:
	This module provides some subroutines are used in certain
	circumstances where the LIBUC library is not availble (for
	whatever reasons).

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |eol| */

#pragma		GCC dependency		"mod/libutil.ccm"

module ureserve ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using libu::strwcmp ;			/* subroutine */
using libu::siochr ;			/* subroutine */
using libu::sirchr ;			/* subroutine */


/* local typedefs */

/* external subroutines */


/* external variables */


/* local structures */

namespace {
   struct sfnextx {
	cchar		**rpp ;
	cchar		*sp ;
	int		sl ;
	sfnextx(cchar *p,int l,cchar **r) noex : rpp(r), sp(p), sl(l) { } ;
	operator int () noex ;
	virtual bool termx(int ch) const noex {
	    return (ch == 0) ;
	} ; /* end */
	bool isterm(int ch) const noex {
	    bool	f = false ;
	    ch &= UCHAR_MAX ;
	    f = f || (ch == '\n') ;
	    f = f || char_iswhite(ch) ;
	    f = f || termx(ch) ;
	    return f ;
	} ; /* end method (isterm) */
   } ; /* end struct (sfnextx) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int sfshrink(cchar *sp,int sl,cchar **rpp) noex {
    	if (sp) ylikely {
	    if (sl >= 0) {
	        while ((sl > 0) && char_iswht(*sp)) {
	            sp += 1 ;
	            sl -= 1 ;
	        } /* end while */
	        if (sp[0] == '\0') sl = 0 ;
	    } else {
	        while (char_iswht(*sp)) {
	            sp += 1 ;
	        }
	        sl = lenstr(sp) ;
	    } /* end if */
	    while ((sl > 0) && char_iswht(sp[sl - 1])) {
	        sl -= 1 ;
	    }
	} else {
	    sl = -1 ;
	}
	if (rpp) *rpp = sp ;
	return sl ;
} /* end subroutine (sfshrink) */

int sfbasename(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    while ((sl > 1) && (sp[sl - 1] == '/')) {
	        sl -= 1 ;
	    } /* end while */
	    {
	        int	si ; /* used-afterwards */
	        for (si = sl ; si > 0 ; si -= 1) {
	            if (sp[si - 1] == '/') break ;
	        }
	        if ((sl == 1) && (si == 1) && (sp[0] == '/')) {
	            si -= 1 ;
	        }
	        rp = (sp + si) ;
	        rl = (sl - si) ;
	    } /* end block */
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfbasename) */

int sfdirname(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    int		i ; /* used-afterwards */
	    while ((sl > 0) && (sp[sl - 1] == '/'))  {
	        sl -= 1 ;
	    } /* end while */
	    for (i = sl ; i > 0 ; i -= 1) {
	        if (sp[i - 1] == '/') break ;
	    } /* end for */
	    rp = sp ;
	    if (i == 1) {
	        rl = 1 ;
	    } else if (i <= 0) {
	        rl = 0 ;
	    } else {
	        rl = (i - 1) ;
	    }
	} /* end if (getlenstr) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfdirname) */

int sfprogname(cchar *sp,int sl,cchar **rpp) noex {
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ;
	if (sp) ylikely {
	    if ((cl = sfbasename(sp,sl,&cp)) > 0) {
	        if (int si ; (si = sirchr(cp,cl,'.')) >= 0) {
		    cl -= si ;
	        }
	        if ((cl > 0) && (cp[0] == '=')) {
		    cp += 1 ;
		    cl -= 1 ;
	        }
	    } /* end if (sfbasename) */
	} /* end if (non-null) */
	if (rpp) *rpp = cp ;
	return cl ;
} /* end subroutine (sfprogname) */

int sfrootname(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = -1 ;
	if (sp && rpp) {
	    rl = sfbasename(sp,sl,rpp) ;
	}
	return rl ;
} /* end subroutine (sfrootname) */

int sfnext(cchar *sp,int sl,cchar **rpp) noex {
	sfnextx sf(sp,sl,rpp) ;
	return sf ;
} /* end subroutine (sfnext) */

int sfnextchr(cchar *sp,int sl,int sch,cchar **rpp) noex {
    	int		rl = -1 ;
	struct esfx : sfnextx {
	    int		sch ;		/* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return (ch == sch) ;
	    } ;
	} ; /* end struct */
	if (sch) ylikely {
	    esfx sf(sp,sl,rpp) ;
	    sf.sch = sch ;		/* <- separate initialization */
	    rl = sf ;
	} /* end if */
	return rl ;
} /* end subroutine (sfnextchr) */

int sfnextbrk(cchar *sp,int sl,cchar *bstr,cchar **rpp) noex {
	int		rl = -1 ;
	struct esfx : sfnextx {
	    cchar	*bstr ;		/* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return (strchr(bstr,ch) != nullptr) ;
	    } ;
	} ; /* end struct */
	if (bstr) ylikely {
	    esfx	sf(sp,sl,rpp) ;
	    sf.bstr = bstr ;		/* <- separate initialization */
	    rl = sf ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextbrk) */

int sfnextterm(cchar *sp,int sl,cchar *terms,cchar **rpp) noex {
	int		rl = -1 ;
	struct esfx : sfnextx {
	    cchar	*terms ;	/* <- initialized separately */
	    esfx(cchar *p,int l,cchar **r) noex : sfnextx(p,l,r) { } ;
	    bool termx(int ch) const noex override final {
		return batst(terms,ch) ;
	    } ;
	} ; /* end struct */
	if (terms) ylikely {
	    esfx sf(sp,sl,rpp) ;
	    sf.terms = terms ;		/* <- separate initialization */
	    rl = sf ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextterm) */

int sfcontent(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (sp) ylikely {
	    rl = 0 ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sl > 0) ylikely {
	        if (int si ; (si = siochr(sp,sl,'#')) >= 0) {
		    sl = si ;
	        } else {
		    while (sl && iseol(sp[sl - 1])) {
			sl -= 1 ;
		    } /* end while */
	        } /* end if (comment or EOL) */
		if (sl) {
		    rl = sfshrink(sp,sl,&rp) ;
		} else {
		    rp = sp ;
		}
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	if (rpp) *rpp = rp ;
	return rl ;
} /* end subroutine (sfcontent) */

int sfkeyval(cchar *sp,int sl,cchar *key,cchar **rpp) noex {
	int		vl = -1 ; /* return-value */
	cchar		*vp = nullptr ;
	if (sp) ylikely {
	    cchar	*cp{} ;
	    if (int cl ; (cl = sfcontent(sp,sl,&cp)) > 0) {
		cchar	*kp{} ;
		if (int kl ; (kl = sfnext(cp,cl,&kp)) > 0) {
		    if (strwcmp(key,kp,kl) == 0) {
			cint	xl = intconv((cp + cl) - (kp + kl)) ;
			cchar	*xp = (kp + kl) ;
			vl = sfnext(xp,xl,&vp) ;
		    } /* end if (strwcmp) */
		} /* end if (sfnext) */
	    } /* end if (sfcontent) */
	} /* end if (non-null) */
	if (rpp) {
	    *rpp = (vl >= 0) ? vp : nullptr ;
	}
	return vl ;
} /* end subroutine (sfkeyval) */


/* local subroutines */

sfnextx::operator int () noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (sp && rpp) ylikely {
	    while (sl && char_iswhite(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    rp = sp ;
	    while (sl && *sp && (! isterm(*sp))) {
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	    rl = intconv(sp - rp) ;
	    *rpp = rp ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (sfnextx::operator) */


