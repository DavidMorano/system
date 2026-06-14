/* biblecite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process a possible bible citation */
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
	This subroutine determines if a string is a bible citation.

	Synopsis:
	int biblecite_ver(biblecite *qp,cchar *lp,int ll,int *sip) noex

	Arguments:
	qp		object pointer
	lp		source string pointer
	ll		source string length
	sip		??

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strn.h>		/* LIBUC */
#include	<six.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DPRINTF(3debug)| */

#include	"biblecite.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

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

int biblecite_ver(biblecite *qp,cchar *lp,int ll,int *sip) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		si = 0 ; /* return-value */
	bool		f = false ;
	if (qp) {
	    int		ch ;
	    int		sl = ll ;
	    cchar	*sp = lp ;
	    rs = memclear(qp) ;
	    if ((si = siskipwhite(lp,ll)) > 0) {
	        sp += si ;
	        sl -= si ;
	    } /* end if */
	    ch = MKCHAR(sp[0]) ;
	    if ((sl >= 5) && isdigitlatin(ch)) {
	        int i{} ;
	        for (i = 0 ; i < 3 ; i += 1) {
	            cchar	*cp = sp ;
	    	    uint	v ;
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
	            rs1 = cfdecui(cp,cl,&v) ;
	            DEBUGPRINTF("cfdecui() rs=%d\n",rs1) ;
	            if (rs1 < 0) break ;
	            switch (i) {
	            case 0:
	                qp->b = v ;
	                break ;
	            case 1:
	                qp->c = v ;
	                break ;
	            case 2:
	                qp->v = v ;
	                break ;
	            } /* end switch */
	        } /* end for */
	        f = (i == 3) ;
	        if (f) {
	            si += siskipwhite(sp,sl) ;
	        }
	    } /* end if (ok) */
	    if (sip) {
	        *sip = (f) ? si : 0 ;
	    }
	    if (!f) si = 0 ;
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d f=%u si=%u\n",rs,f,si) ;
	return (rs >= 0) ? si : 0 ;
} /* end subroutine (biblecite_ver) */


