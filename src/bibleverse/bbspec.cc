/* bbspec SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a bible-book-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object parses and loads a given bible-book-specification
	string into itself.  The given day-specification string
	looks like:
		<bookname>[:]<chapter>[:<verse>]
	or
		<booknum>:<chapter>[:<verse>]

	Examples are:
		mat5:1
		mat:5:1
		40:5:1

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<estrings.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"bbspec.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward references */

local int	siourbrk(cchar *,int,int) noex ;


/* local variables */


/* exported subroutines */

int bbspec_load(BBSPEC *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (op == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	memclear(op) ; /* <- potentially dangerous */
	op->v = 1 ;

	if (sl < 0) sl = lenstr(sp) ;

	cchar *cp ;
	if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	    sp = cp ;
	    sl = cl ;
	} /* end if (sfshrink) */

	if (int si, v ; sl > 0) {
	    int ch = mkchar(sp[0]) ;
	    if (isalphalatin(ch)) {
	        if ((si = siourbrk(sp,sl,true)) > 0) {
		    op->namp = sp ;
	  	    op->naml = si ;
		    sp += si ;
		    sl -= si ;
		    if (sl > 0) {
			ch = mkchar(sp[0]) ;
		 	if (ch == ':') {
			    sp += 1 ;
			    sl -= 1 ;
			}
		    } /* end if (non-zero positive) */
	        } /* end if (siourbrk) */
	    } else if (isdigitlatin(ch)) {
	        if ((si = siourbrk(sp,sl,true)) > 0) {
		    if ((rs = cfdeci(sp,si,&v)) >= 0) {
		        op->b = schar(v) ;
		    }
	        } /* end if (siourbrk) */
	    } else {
	        rs = SR_DOM ;
	    }
	    if ((rs >= 0) && (sl > 0)) {
		if (cchar *tp = strnchr(sp,sl,':') ; tp) {
		    cint tl = intconv(tp - sp) ;
		    if ((rs = cfdeci(sp,tl,&v)) >= 0) {
			op->b = schar(v) ;
			sl -= intconv((tp + 1) - sp) ;
			sp = (tp +1 ) ;
			if (sl > 0) {
		    	    rs = cfdeci(sp,sl,&v) ;
		    	    op->v = schar(v) ;
			}
		    } /* end if (cfdeci) */
		} else {
		    rs = cfdeci(sp,sl,&v) ;
		    op->b = schar(v) ;
		} /* end if */
	    } else {
	        rs = SR_DOM ;
	    }
	} else {
	    rs = SR_DOM ;
	}

#if	CF_DEBUGS
	debugprintf("bbspec_parse: ret rs=%d\n",rs) ;
	debugprintf("bbspec_parse: ret b=%d c=%d v=%d\n",op->v,op->c,op->v) ;
#endif

	return rs ;
}
/* end subroutine (bbspec_load) */


/* local subroutines */

local int siourbrk(cchar *sp,int sl,int f_dig) noex {
	int	i = -1 ; /* return-value */
	bool	f = false ;
	for (i = 0 ; i < sl ; i += 1) {
	    cint ch = (sp[i] & UCHAR_MAX) ;
	    if (f_dig) {
		f = isdigitlatin(ch) ;
	    } else {
		f = isalphalatin(ch) ;
	    }
	    f = f || (ch == ':') ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
} /* end subroutine (siourbrk) */


