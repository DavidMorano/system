/* biblecite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find if present and process a possible bible citation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	biblecite

  	Description:
	This subroutine determines if a string contains a bible
	citation.

	Synopsis:
	int biblecite_ver(biblecite *qp,cchar *lp,int ll) noex

	Arguments:
	qp		object pointer
	lp		source string pointer
	ll		source string length

	Returns:
	>0		citation was found and the number of bytes past it
	==0		no citation found
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"biblecite.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	ISWHT(ch)	iswhitelatin(ch)
#define	ISDIG(ch)	isdigitlatin(ch)

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool			f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int biblecite_ver(biblecite *qp,cchar *lp,int ll) noex {
    	int		rs = SR_FAULT ;
	int		si = 0 ; /* return-value */
	if (qp && lp) {
	    int		sl = ll ;
	    cchar	*sp = lp ;
	    rs = memclear(qp) ;
	    if ((si = siskipwhite(lp,ll)) > 0) {
	        sp += si ;
	        sl -= si ;
	    } /* end if */
	    if (int ch ; (sl >= 5) && ((ch = MKCHAR(sp[0]))) && ISDIG(ch)) {
		bool f = false ;
	        for (int i = 0 ; i < 3 ; i += 1) {
	            cchar	*cp = sp ;
	            int		cl = sl ;
	            if (cchar *tp = strnbrk(sp,sl,": \t\n") ; tp) {
	                cl = intconv(tp - sp) ;
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	            } else {
	                cl = sl ;
	                sp += sl ;
	                sl = 0 ;
	            } /* end if */
	            if (cl == 0) break ;
	            si = intconv((cp + cl) - lp) ;
	            if (uint v ; (rs = cfdec(cp,cl,&v)) >= 0) {
	                switch (i) {
	                case 0:
	                    qp->b = uchar(v) ;
	                    break ;
	                case 1:
	                    qp->c = uchar(v) ;
	                    break ;
	                case 2:
			    f = true ;
	                    qp->v = uchar(v) ;
	                    break ;
	                } /* end switch */
		    } else {
	                DEBUGPRINTF("cfdec() rs=%d\n",rs) ;
		    } /* end if (cfdec) */
		    if (rs < 0) break ;
	        } /* end for */
	        if (f) {
	            si += siskipwhite(sp,sl) ;
		} else {
		    si = 0 ;
	        }
	    } /* end if (ok) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d si=%u\n",rs,si) ;
	return (rs >= 0) ? si : rs ;
} /* end subroutine (biblecite_ver) */


