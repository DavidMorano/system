/* ureserve5 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* reserved interfaces */
/* version %I% last-modified %G% */

#define	CF_STRCHR	1		/* use |strchr(3c)| */

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
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |eol| */

#pragma		GCC dependency		"mod/libutil.ccm"

module ureserve ;

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_STRCHR
#define	CF_STRCHR	1		/* use |strchr(3c)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int sirchr(cchar *,int,int) noex ;


/* local variables */

cbool		f_strchr = CF_STRCHR ;


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
}
/* end subroutine (sfshrink) */

int sfbasename(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ; /* return-value */
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    while ((sl > 1) && (sp[sl - 1] == '/')) {
	        sl -= 1 ;
	    }
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
}
/* end subroutine (sfbasename) */

int sfdirname(cchar *sp,int µsl,cchar **rpp) noex {
	int		rl = -1 ;
	cchar		*rp = nullptr ;
	if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	    int		i ; /* used-afterwards */
	    while ((sl > 0) && (sp[sl - 1] == '/'))  {
	        sl -= 1 ;
	    }
	    for (i = sl ; i > 0 ; i -= 1) {
	        if (sp[i - 1] == '/') break ;
	    }
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
}
/* end subroutine (sfdirname) */

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
}
/* end subroutine (sfprogname) */

int sfrootname(cchar *sp,int sl,cchar **rpp) noex {
	int		rl = -1 ;
	if (sp && rpp) {
	    rl = sfbasename(sp,sl,rpp) ;
	}
	return rl ;
}
/* end subroutine (sfrootname) */


/* local subroutines */

#ifdef	COMMENR
local int siochr(cchar *sp,int sl,int sch) noex {
    	cnullptr	np{} ;
	int		i = 0 ; /* return-value */
	bool		f = false ;
	sch &= UCHAR_MAX ;
	if (sp) ylikely {
	    if_constexpr (f_strchr) {
	        if (sl > 0) {
	            for (i = 0 ; sl-- && sp[i] ; i += 1) {
	                cint	ch = mkchar(sp[i]) ;
	                if ((f = (ch == sch))) break ;
	            } /* end for */
	        } else {
		    if (cchar *tp ; (tp = strchr(sp,sch)) != np) {
			f = true ;
			i = intconv(tp - sp) ;
		    }
	        } /* end if */
	    } else {
	        for (i = 0 ; sl-- && sp[i] ; i += 1) {
	            cint	ch = mkchar(sp[i]) ;
	            if ((f = (ch == sch))) break ;
	        } /* end for */
	    } /* end if_constexpr (f_strchr) */
	} /* end if (non-null) */
	return (f) ? i : -1 ;
}
/* end subroutine (siochr) */
#endif /* COMMENT */

local int sirchr(cchar *sp,int µsl,int sch) noex {
	int		i = 0 ; /* return-value */
	bool		f = false ;
	sch &= UCHAR_MAX ;
	if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
	    for (i = (sl - 1) ; i >= 0 ; i -= 1) {
	        cint	ch = mkchar(sp[i]) ;
	        if ((f = (ch == sch))) break ;
	    } /* end for */
	} /* end if (getlenstr) */
	return (f) ? i : -1 ;
}
/* end subroutine (sirchr) */


