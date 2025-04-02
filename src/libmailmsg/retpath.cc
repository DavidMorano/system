/* retpath SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process the "retpath" NEWS object */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	retpath

	Description:
	The so called "retpath" object is the thing that provides
	a route back to the sender of a message.  We use a VECSTR
	object to hold the retpath components. Note carefully that
	we store the retpath components in the order that they
	appear (left to right) in the retpath header value.  This
	represents the order that would be needed to get back to
	the originator and NOT the order from the originator to us!
	This makes this similar to a return-retpath in email circles.

**************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<estrings.h>
#include	<sbuf.h>
#include	<strn.h>
#include	<sfx.h>
#include	<strwcmp.h>
#include	<localmisc.h>

#include	"retpath.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	retpath_iadd(retpath *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int retpath_start(retpath *plp) noex {
    	int		rs = SR_FAULT ;
	if (plp) {
    	    cint	vn = 10 ;
	    cint	vo = VECSTR_OORDERED ;
	    rs = vecstr_start(plp,vn,vo) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (retpath_start) */

int retpath_finish(retpath *plp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (plp) {
	    rs = SR_OK ;
	    {
	        rs1 = vecstr_finish(plp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	return rs ;
}
/* end subroutine (retpath_finish) */

int retpath_add(retpath *plp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (plp && sp) {
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (int cl ; (cl = sfshrink(sp,sl,&cp)) > 0) {
	        rs = retpath_iadd(plp,cp,cl) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (retpath_add) */

/* add ret-path entries by parsing a string of them */
int retpath_parse(retpath *plp,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		n = 0 ;
	if (plp && sp) {
	    int		cl ;
	    cchar	*cp ;
	    if (sl < 0) sl = cstrlen(sp) ;
	    rs = SR_OK ;
	    for (cchar *tp ; (tp = strnpbrk(sp,sl,"!@,%:")) != np ; ) {
	        if ((cl = sfshrink(sp,(tp-sp),&cp)) > 0) {
	            n += 1 ;
	            rs = retpath_iadd(plp,cp,cl) ;
	        }
	        sl -= ((tp+1)-sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (sl > 0)) {
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            n += 1 ;
	            rs = retpath_iadd(plp,cp,cl) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (retpath_parse) */

int retpath_count(retpath *plp) noex {
	return vecstr_count(plp) ;
}
/* end subroutine (retpath_count) */

int retpath_get(retpath *plp,int i,cchar **rpp) noex {
	return vecstr_get(plp,i,rpp) ;
}
/* end subroutine (retpath_get) */

int retpath_search(retpath *plp,cchar *name,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* used-afterwards */
	if (plp) {
	    for (i = 0 ; (rs = plp->get(i,rpp)) >= 0 ; i += 1) {
	        if ((*rpp) != nullptr) {
	            if (strcmp(name,(*rpp)) == 0) break ;
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (retpath_search) */

int retpath_mk(retpath *plp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (plp && rbuf) {
	    if (rlen < 0) rlen = INT_MAX ;
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
	        cchar	*sp ;
	        for (int i = 0 ; plp->get(i,&sp) >= 0 ; i += 1) {
		    if (i > 0) b.chr('!') ;
		    rs = b.strw(sp,-1) ;
		    if (rs < 0) break ;
	        } /* end for */
	        rl = b.finish ;
	        if (rs >= 0) rs = rl ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (retpath_mk) */


/* private subroutines */

static int retpath_iadd(retpath *plp,cchar *sp,int sl) noex {
	int		rs ;
	if (cchar *lp ; (rs = plp->getlast(&lp)) >= 0) {
	    if (strwcmp(lp,sp,sl) != 0) {
		rs = SR_NOTFOUND ;
	    }
	}
	if (rs == SR_NOTFOUND) {
	    rs = plp->add(sp,sl) ;
	} else {
	    rs = INT_MAX ;
	}
	return rs ;
}
/* end subroutine (retpath_iadd) */


