/* vecstr_addpath SUPPORT */
/* lang=C++20 */

/* add a "path" compnent to the string-list */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

	= 2023-10-27, David A­D­ Morano
	I changed to make the MAXPATHLEN value dynamic.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	------------------------------------------------------------------------
	Name:
	vecstr_addpathclean

	Description:
	This subroutine adds a "path" componment to the vector-string
	list.  It cleans up the path-component first by passing it
	through 'pathclean(3dam)'.

	Synopsis:
	int vecstr_addpathclean(vecstr *lp,cchar *pp,int pl) noex

	Arguments:
	lp		vector-string list object pointer
	pp		path-string pointer
	pl		length of given string

	Returns:
	>=0		OK
	<0		some error


	------------------------------------------------------------------------
	Name:
	vecstr_addcspath

	Description:
	This subroutine will add (uniquely) the Condig-String (CS) 
	PATH value to the vector-list.

	Synopsis:
	int vecstr_addcspath(vecstr *) noex

	Arguments:
	vsp		pointer to VECSTR object

	Returns:
	>=0		number of elements loaded
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<strn.h>
#include	<pathclean.h>
#include	<vecstr.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported subroutines */

int vecstr_addpathclean(vecstr *vlp,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (vlp && lp) {
	    rs = SR_OK ;
	    if (ll < 0) ll = strlen(lp) ;
	    if (ll > 0) {
		if ((rs = maxpathlen) >= 0) {
		    cint	dlen = rs ;
		    char	*dbuf ;
		    if ((rs = uc_libmalloc((dlen+1),&dbuf)) >= 0) {
	                cchar	*tp ;
	                while ((tp = strnpbrk(lp,ll,":;")) != nullptr) {
		            if ((tp-lp) >= 0) {
	    	                if ((rs = pathclean(dbuf,lp,(tp-lp))) >= 0) {
		                    rs = vecstr_adduniq(vlp,dbuf,rs) ;
		                    if (rs < INT_MAX) c += 1 ;
		                }
		            }
		            ll -= ((tp+1)-lp) ;
		            lp = (tp+1) ;
		            if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (ll > 0)) {
	    	            if ((rs = pathclean(dbuf,lp,ll)) >= 0) {
		                rs = vecstr_adduniq(vlp,dbuf,rs) ;
		                if (rs < INT_MAX) c += 1 ;
		            }
	                }
			rs1 = uc_libfree(dbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a) */
		} /* end if (maxpathlen) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_addpathclean) */

int vecstr_addpath(vecstr *vlp,cchar *lp,int ll) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (vlp && lp) {
	    rs = SR_OK ;
	    if (ll < 0) ll = strlen(lp) ;
	    if (ll > 0) {
	        cchar	*tp ;
	        while ((tp = strnpbrk(lp,ll,":;")) != nullptr) {
		    if ((tp-lp) >= 0) {
		        rs = vecstr_adduniq(vlp,lp,(tp-lp)) ;
		        if (rs < INT_MAX) c += 1 ;
		    }
		    ll -= ((tp+1)-lp) ;
		    lp = (tp+1) ;
		    if (rs < 0) break ;
	        } /* end while */
	        if ((rs >= 0) && (ll > 0)) {
		    rs = vecstr_adduniq(vlp,lp,ll) ;
		    if (rs < INT_MAX) c += 1 ;
	        }
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_addpath) */

int vecstr_addcspath(vecstr *vsp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (vsp) {
	    if ((rs = maxpathlen) >= 0) {
		cint	plen = (2*rs) ;
	        char	*pbuf ;
	        if ((rs = uc_libmalloc((plen+1),&pbuf)) >= 0) {
		    cint	req = _CS_PATH ;
	            if ((rs = uc_confstr(pbuf,plen,req)) >= 0) {
	                rs = vecstr_addpath(vsp,pbuf,rs) ;
	                c += rs ;
	            } /* end if */
	            rs1 = uc_libfree(pbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_addcspath) */


