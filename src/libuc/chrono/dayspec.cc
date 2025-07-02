/* dayspec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a day-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Group:
	dayspec_def
	dayspec_load
	dayspec_yday

	Decription:
	This object parses and loads a given day-specification
	string into itself.  The given day-specification string
	looks like:

		[[<year>]<mon>[-]]<day>
	or
		[[<year>-]<mon>[-]]<day>
	or
		[<year>/]<mon>[/]<day>

	Examples are:
		20130214
		2013-0214
		2013-02-14
		2-14
		feb14
		feb-14
		2013/2/14
		2013feb14
		2013feb-14
		2013-feb14
		2/14
		14

	Results (same as |localtime(3c)|):
	y		0-2037
	m		0-11
	d		1-31

	Synopsis:
	int dayspec_def(dayspec *op) noex
	int dayspec_load(dayspec *op,cchar *sp,int sl) noex
	int dayspec_yday(dayspec *op) noex

	Arguments:
	dayspec		OBJECT pointer
	sp		c-string pointer
	sl		c-string length

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getmjd.h>		/* |getyrd(3uc)| */
#include	<calstrs.h>
#include	<estrings.h>
#include	<cfdec.h>
#include	<six.h>
#include	<char.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"dayspec.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	dayspec_parse(dayspec *,cchar *,int) noex ;

static int	siourbrk(cchar *,int,int) noex ;
static int	parsemonth(cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int dayspec_def(dayspec *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->y = -1 ;
	    op->m = -1 ;
	    op->d = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dayspec_def) */

int dayspec_load(dayspec *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    op->y = -1 ;
	    op->m = -1 ;
	    op->d = -1 ;
	    if (sp) {
	        rs = dayspec_parse(op,sp,sl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dayspec_load) */

int dayspec_yday(dayspec *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_DOM ;
	    if ((op->y >= 0) && (op->m >= 0) && (op->d >> 0)) {
		rs = getyrd(op->y,op->m,op->d) ;
	    } /* end if (valid-domain) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dayspec_yday) */


/* private subroutines */

static int dayspec_parse(dayspec *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		yl = 0 ;
	int		ml = 0 ;
	int		dl = 0 ;
	int		v ;
	cchar		*yp = nullptr ;
	cchar		*mp = nullptr ;
	cchar		*dp = nullptr ;
	if (sl < 0) sl = lenstr(sp) ;
	if (hasalldig(sp,sl)) {
	    switch (sl) {
	    case 8:
	    case 7:
	    case 6:
	    case 5:
	        yp = sp ;
	        yl = min(4,(sl - 4)) ;
	        sp += 4 ;
	        sl -= 4 ;
		fallthrough ;
		/* FALLTHROUGH */
	    case 4:
	    case 3:
	        mp = sp ;
	        ml = min(2,(sl - 2)) ;
	        sp += 2 ;
	        sl -= 2 ;
		fallthrough ;
	        /* FALLTHROUGH */
	    case 2:
	    case 1:
	        dp = sp ;
	        dl = min(2,(sl - 0)) ;
		break ;
	    } /* end switch */
	} else {
	    if (int ti ; (ti = siourbrk(sp,sl,0)) >= 0) {
		int	si ;
	        bool	f_dig = false ;
		{
	            yp = sp ;
	            yl = ti ;
		}
		{
	            mp = sp ;
	            ml = ti ;
		}
		{
	            sl -= ti ;
	            sp += ti ;
		}
		if (sl > 0) {
		    if ((si = sialnum(sp,sl)) > 0) {
			sp += si ;
			sl -= si ;
		    }
		}
		if (sl > 0) {
		    cint	ch = mkchar(sp[0]) ;
		    f_dig = isalphalatin(ch) ;
	        }
	        if ((ti = siourbrk(sp,sl,f_dig)) >= 0) {
		    {
	                mp = sp ;
	                ml = ti ;
		    }
		    {
	                dp = (sp + ti) ;
	                dl = (sl - ti) ;
		    }
		    if (sl > 0) {
		        if ((si = sialnum(dp,dl)) > 0) {
			    dp += si ;
			    dl -= si ;
		        }
		    }
	        } else {
		    switch (sl) {
		    case 4:
		    case 3:
	        	mp = sp ;
	        	ml = min(2,(sl - 2)) ;
	        	sp += 2 ;
	        	sl -= 2 ;
			/* FALLTHROUGH */
		    case 2:
		    case 1:
	                dp = sp ;
	        	dl = min(2,(sl - 0)) ;
			break ;
		    default:
			rs = SR_INVALID ;
			break ;
		    } /* end switch */
	        } /* end if */
	    } else {
	        dp = sp ;
	        dl = sl ;
	    } /* end if */
	} /* end if (all-digital) */
	if ((rs >= 0) && (yp != nullptr) && (yl > 0)) {
	    rs = cfdeci(yp,yl,&v) ;
	    op->y = shortconv(v) ;
	}
	if ((rs >= 0) && (mp != nullptr) && (ml > 0)) {
	    rs = parsemonth(mp,ml) ;
	    op->m = schar(rs) ;
	}
	if ((rs >= 0) && (dp != nullptr) && (dl > 0)) {
	    rs = cfdeci(dp,dl,&v) ;
	    op->d = schar(v) ;
	}
	return rs ;
}
/* end subroutine (dayspec_parse) */

static int parsemonth(cchar *mp,int ml) noex {
	int		rs = SR_INVALID ;
	int		mi = -1 ;
	cchar		*cp ;
	if (int cl ; (cl = sfshrink(mp,ml,&cp)) > 0) {
	    cint	ch = mkchar(cp[0]) ;
	    if (isalphalatin(ch)) {
	        mi = matpcasestr(calstrs_months,2,cp,cl) ;
	        rs = (mi >= 0) ? mi : SR_INVALID ;
	    } else {
	        rs = cfdeci(cp,cl,&mi) ;
	        mi -= 1 ;
	    }
	} /* end if (sfshrink) */
	return (rs >= 0) ? mi : rs ;
}
/* end subroutine (parsemonth) */

static int siourbrk(cchar *sp,int sl,int f_dig) noex {
	int		i = -1 ; /* return-value */
	bool		f = false ;
	for (i = 0 ; i < sl ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    if (f_dig) {
		f = isdigitlatin(ch) ;
	    } else {
		f = isalphalatin(ch) ;
	    }
	    f = f || ((ch == '-') || (ch == '/') || (ch == ':')) ;
	    f = f || CHAR_ISWHITE(ch) ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (siourbrk) */

int dayspec::load(cchar *sp,int sl) noex {
	return dayspec_load(this,sp,sl) ;
}

dayspec_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case dayspecmem_def:
	        rs = dayspec_def(op) ;
	        break ;
	    case dayspecmem_yday:
	        rs = dayspec_yday(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (dayspec_co::operator) */


