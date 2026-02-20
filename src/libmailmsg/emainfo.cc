/* emainfo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse mail route-address host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-12, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	emainfo

	Description:
	This subroutine will parse email route addresses into
	hostname and localname parts.  The assumption is that only
	route addresses are given to us.  If this is wrong, the
	results are indeterminate.  The hostname part is just the
	first host in the address as if the "focus" (using SENDMAIL
	language) was on the first host.

	Synopsis:
	int emainfo_load(emainfo *op,cchar *sp,int sl) noex

	Arguments:
	op		pointer to 'emainfo' structure
	sp		string buffer containing route emainfo
	sl		length of string buffer

	Returns:
	ematype_local		type - local emainfo
	ematype_uucp		type - UUCP
	ematype_arpa		type - ARPAnet normal
	ematype_arparoute	type - ARPAnet route emainfo
	<0			type - bad emainfo of some kind (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<storebuf.h>
#include	<strn.h>		/* |strnchr(3uc)| */
#include	<strwcpy.h>
#include	<ematypes.h>
#include	<localmisc.h>

#include	"emainfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int emainfo_load(emainfo *op,cchar *sp,int µsl) noex {
    	int		rs = SR_FAULT ;
	int		rtype = 0 ; /* return-value */
	if (op && sp) {
	    rs = SR_INVALID ;
	    if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
		rs = memclear(op) ;
	        cchar	*cp ;
	        cchar	*cp1, *cp2 ;
                /* what kind of address do we have? */
	        if ((cp1 = strnchr(sp,sl,'@')) != nullptr) {
	            if ((cp2 = strnchr(sp,sl,':')) != nullptr) {
		        /* ARPAnet route address */
	                op->type = ematype_arparoute ;
	                if ((cp = strnchr(sp,sl,',')) != nullptr) {
	                    op->hpart = (cp1 + 1) ;
	                    op->hlen = intconv(cp - (cp1 + 1)) ;
	                    op->lpart = (cp + 1) ;
	                    op->llen = intconv((sp + sl) - (cp + 1)) ;
	                } else {
	                    op->hpart = (cp1 + 1) ;
	                    op->hlen = intconv(cp2 - (cp1 + 1)) ;
	                    op->lpart = (cp2 + 1) ;
	                    op->llen = intconv((sp + sl) - (cp2 + 1)) ;
	                } /* end if */
	            } else {
		        /* normal ARPAnet address */
	                op->type = ematype_arpa ;
	                op->hpart = (cp1 + 1) ;
	                op->hlen = intconv((sp + sl) - (cp1 + 1)) ;
	                op->lpart = sp ;
	                op->llen = intconv(cp1 - sp) ;
	            } /* end if */
	        } else if ((cp = strnrchr(sp,sl,'!')) != nullptr) {
	            op->type = ematype_uucp ;
	            op->hpart = sp ;
	            op->hlen = intconv(cp - sp) ;
	            op->lpart = (cp + 1) ;
	            op->llen = intconv((sp + sl) - (cp + 1)) ;
	        } else {
	            /* local */
	            op->type = ematype_local ;
	            op->hpart = nullptr ;
	            op->hlen = 0 ;
	            op->lpart = sp ;
	            op->llen = sl ;
	        } /* end if */
	        rtype = op->type ;
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rtype : rs ;
}
/* end subroutine (emainfo_load) */

int emainfo_mktype(emainfo *op,ematypes type,char *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		idx = 0 ; /* return-value */
	if (op && rbuf) {
	    rs = SR_INVALID ;
	    if (rlen >= 0) {
	        storebuf	sb(rbuf,rlen) ;
	        int		tl ;
	        rs = SR_OK ;
	        if (type >= 0) {
	            switch (type) {
	            case ematype_local:
		        tl = min(op->llen,rlen) ;
	                idx = intconv(strwcpy(rbuf,op->lpart,tl) - rbuf) ;
	                break ;
	            case ematype_uucp:
	                if (op->hpart && (op->hlen >= 0)) {
	                    if ((rs = sb.strw(op->hpart,op->hlen)) >= 0) {
	                        rs = sb.chr('!') ;
		            }
	                } /* end if (had a host part) */
		        if (rs >= 0) {
	                    if ((rs = sb.strw(op->lpart,op->llen)) >= 0) {
	                        idx = sb ;
		            }
		        } /* end if (ok) */
	                break ;
	            case ematype_arpa:
	                if ((rs = sb.strw(op->lpart,op->llen)) >= 0) {
	                    if (op->hpart && (op->hlen >= 0)) {
	                        if ((rs = sb.chr('@')) >= 0) {
	                            rs = sb.strw(op->hpart,op->hlen) ;
			        }
		            }
		            idx = sb ;
	                } /* end if */
	                break ;
	            case ematype_arparoute:
	                if (op->hpart && (op->hlen >= 0)) {
	                    if ((rs = sb.chr('@')) >= 0) {
	                        rs = sb.strw(op->hpart,op->hlen) ;
		            }
		            if (rs >= 0) {
	                        if (strnchr(op->lpart,op->llen,':') != np) {
	                            rs = sb.chr(',') ;
	                        } else {
	                            rs = sb.chr(':') ;
		                }
		            } /* end if (ok) */
	                } /* end if (had a host part) */
		        if (rs >= 0) {
	                    rs = sb.strw(op->lpart,op->llen) ;
	                    idx = sb ;
		        } /* end if (ok) */
	                break ;
		    default:
			rs = SR_NOTSUP ;
			break ;
	            } /* end switch */
	        } else {
	            tl = min(op->llen,rlen) ;
	            idx = intconv(strwcpy(rbuf,op->lpart,tl) - rbuf) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (emainfo_mktype) */


