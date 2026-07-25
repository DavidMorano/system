/* vecstrx_addpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* add a "path" compnent to the string-list */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

	= 2023-10-27, David A­D­ Morano
	I changed to make the MAXPATHLEN value dynamic.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	------------------------------------------------------------------------
	Name:
	vecstrx_addpathclean

	Description:
	This subroutine adds a "path" componment to the vector-string
	list.  It cleans up the path-component first by passing it
	through |pathclean(3uc)|.

	Synopsis:
	int vecstrx_addpathclean(vecstrx *lp,cchar *pp,int pl) noex

	Arguments:
	lp		vector-string list object pointer
	pp		path-string pointer
	pl		length of given string

	Returns:
	>=0		OK
	<0		error (system-return)


	------------------------------------------------------------------------
	Name:
	vecstrx_addcspath

	Description:
	This subroutine will add (uniquely) the Condig-String (CS) 
	PATH value to the vector-list.

	Synopsis:
	int vecstrx_addcspath(vecstrx *) noex

	Arguments:
	vsp		pointer to VECSTR object

	Returns:
	>=0		number of elements loaded
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU */
#include	<usysconf.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnbrk(3uc)| */
#include	<pathclean.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"vecstrx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */
import ulibvals ;

/* local defines */

#define	NPATH	4			/* buffer multiply-factor */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int vecstrx_addone(vecstrx *op,char *pbuf,cchar *sp,int sl) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = pathclean(pbuf,sp,sl)) >= 0) ylikely {
	    rs = op->adduniq(pbuf,rs) ;
	    if (rs < INT_MAX) c += 1 ;
	} /* end if (pathclean) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx_addone) */


/* local variables */

local int		maxpathlen = ulibval.maxpathlen ;


/* exported variables */


/* exported subroutines */

int vecstrx::addpathclean(cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (lp) ylikely {
	    rs = SR_OK ;
	    if (ll < 0) ll = lenstr(lp) ;
	    if (ll > 0) {
		if ((rs = maxpathlen) >= 0) ylikely {
		    cint	plen = rs ;
		    if (char *pbuf ; (rs = libmem.mall((plen+1),&pbuf)) >= 0) {
	                for (cc *tp ; (tp = strnbrk(lp,ll,":;")) != np ; ) {
			    if (cint tl = intconv(tp - lp) ; tl >= 0) {
				rs = vecstrx_addone(this,pbuf,lp,tl) ;
				c += rs ;
		            }
		            ll -= intconv((tp + 1) - lp) ;
		            lp = (tp + 1) ;
		            if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (ll > 0)) {
			    rs = vecstrx_addone(this,pbuf,lp,ll) ;
			    c += rs ;
	                }
			rs1 = libmem.free(pbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx_addpathclean) */

int vecstrx::addpath(cchar *lp,int ll) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (lp) ylikely {
	    rs = SR_OK ;
	    if (ll < 0) ll = lenstr(lp) ;
	    if (ll > 0) {
	        for (cc *tp ; (tp = strnbrk(lp,ll,":;")) != np ; ) {
		    if (cint tl = intconv(tp - lp) ; tl >= 0) {
		        rs = adduniq(lp,tl) ;
		        if (rs < INT_MAX) c += 1 ;
		    }
		    ll -= intconv((tp + 1) - lp) ;
		    lp = (tp + 1) ;
		    if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && (ll > 0)) {
		    rs = adduniq(lp,ll) ;
		    if (rs < INT_MAX) c += 1 ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx_addpath) */

int vecstrx::addcspath() noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if ((rs = maxpathlen) >= 0) ylikely {
            cint    clen = (NPATH * rs) ;
            if (char *cbuf ; (rs = libmem.mall((clen+1),&cbuf)) >= 0) ylikely {
                cint        req = _CS_PATH ;
                if ((rs = u_sysconfstr(req,cbuf,clen)) >= 0) {
                    rs = addpath(cbuf,rs) ;
                    c += rs ;
                } /* end if */
                rs1 = libmem.free(cbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
        } /* end if (maxpathlen) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (vecstrx::addcspath) */


