/* vecstrx_addpath SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<libmallocxx.h>		/* <- currently unused */
#include	<strn.h>
#include	<pathclean.h>
#include	<localmisc.h>

#include	"vecstrx.hh"


/* local defines */

#define	NPATH	4			/* buffer multiply-factor */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int vecstrx_addone(vecstrx *op,char *pbuf,cchar *sp,int sl) noex {
    	int		rs ;
	int		c = 0 ;
	if ((rs = pathclean(pbuf,sp,sl)) >= 0) {
	    rs = op->adduniq(pbuf,rs) ;
	    if (rs < INT_MAX) c += 1 ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addone) */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int vecstrx::addpathclean(cchar *lp,int ll) noex {
    	auto		mall = uc_libmalloc ;
    	auto		free = uc_libfree ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (lp) {
	    rs = SR_OK ;
	    if (ll < 0) ll = strlen(lp) ;
	    if (ll > 0) {
		if ((rs = maxpathlen) >= 0) {
		    cint	plen = rs ;
		    if (char *pbuf{} ; (rs = mall((plen+1),&pbuf)) >= 0) {
	                cchar	*tp ;
	                while ((tp = strnpbrk(lp,ll,":;")) != nullptr) {
		            if ((tp-lp) >= 0) {
				cint	pl = (tp - lp) ;
				rs = vecstrx_addone(this,pbuf,lp,pl) ;
				c += rs ;
		            }
		            ll -= ((tp+1)-lp) ;
		            lp = (tp+1) ;
		            if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (ll > 0)) {
			    rs = vecstrx_addone(this,pbuf,lp,ll) ;
			    c += rs ;
	                }
			rs1 = free(pbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addpathclean) */

int vecstrx::addpath(cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (lp) {
	    rs = SR_OK ;
	    if (ll < 0) ll = strlen(lp) ;
	    if (ll > 0) {
	        cchar	*tp ;
	        while ((tp = strnpbrk(lp,ll,":;")) != nullptr) {
		    if ((tp-lp) >= 0) {
		        rs = adduniq(lp,(tp-lp)) ;
		        if (rs < INT_MAX) c += 1 ;
		    }
		    ll -= ((tp+1)-lp) ;
		    lp = (tp+1) ;
		    if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && (ll > 0)) {
		    rs = adduniq(lp,ll) ;
		    if (rs < INT_MAX) c += 1 ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_addpath) */

int vecstrx::addcspath() noex {
    	auto		mall = uc_libmalloc ;
    	auto		free = uc_libfree ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if ((rs = maxpathlen) >= 0) {
            cint    clen = (NPATH * rs) ;
            if (char *cbuf{} ; (rs = mall((clen+1),&cbuf)) >= 0) {
                cint        req = _CS_PATH ;
                if ((rs = uc_sysconfstr(cbuf,clen,req)) >= 0) {
                    rs = addpath(cbuf,rs) ;
                    c += rs ;
                } /* end if */
                rs1 = free(cbuf) ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (m-a-f) */
        } /* end if (maxpathlen) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx::addcspath) */


