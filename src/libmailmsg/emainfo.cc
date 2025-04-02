/* encoding=ISO8859-1 */
/* emainfo SUPPORT */
/* lang=C++20 */

/* parse mail route-address host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	emainfo

	Descption:
	This subroutine will parse email route addresses into
	hostname and localname parts.  The assumption is that only
	route addresses are given to us.  If this is wrong, the
	results are indeterminate.  The hostname part is just the
	first host in the address as if the "focus" (using SENDMAIL
	language) was on the first host.

	Synopsis:
	int emainfo_load(emainfo *eip,cchar *sp,int sl) noex

	Arguments:
	eip		pointer to 'emainfo' structure
	sp		string buffer containing route emainfo
	sl		length of string buffer

	Returns:
	type
	0		local emainfo
	1		UUCP
	2		ARPAnet normal
	3		ARPAnet route emainfo
	<0		bad emainfo of some kind

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<storebuf.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"emainfo.h"


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


/* local variables */


/* exported variables */


/* exported subroutines */

int emainfo_load(emainfo *eip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		type = 0 ;
	if (eip && sp) {
	    cchar	*cp ;
	    cchar	*cp1, *cp2 ;
	    rs = memclear(eip) ;
	    if (sl < 0) sl = cstrlen(sp) ;
	    /* what kind of address do we have? */
	    if ((cp1 = strnchr(sp,sl,'@')) != nullptr) {
	        if ((cp2 = strnchr(sp,sl,':')) != nullptr) {
		    /* ARPAnet route address */
	            eip->type = EMAINFO_TARPAROUTE ;
	            if ((cp = strnchr(sp,sl,',')) != nullptr) {
	                eip->hpart = (cp1 + 1) ;
	                eip->hlen = (cp - (cp1 + 1)) ;
	                eip->lpart = (cp + 1) ;
	                eip->llen = (sp + sl) - (cp + 1) ;
	            } else {
	                eip->hpart = (cp1 + 1) ;
	                eip->hlen = (cp2 - (cp1 + 1)) ;
	                eip->lpart = (cp2 + 1) ;
	                eip->llen = (sp + sl) - (cp2 + 1) ;
	            } /* end if */
	        } else {
		    /* normal ARPAnet address */
	            eip->type = EMAINFO_TARPA ;
	            eip->hpart = (cp1 + 1) ;
	            eip->hlen = (sp + sl) - (cp1 + 1) ;
	            eip->lpart = sp ;
	            eip->llen = (cp1 - sp) ;
	        } /* end if */
	    } else if ((cp = strnrchr(sp,sl,'!')) != nullptr) {
	        eip->type = EMAINFO_TUUCP ;
	        eip->hpart = sp ;
	        eip->hlen = (cp - sp) ;
	        eip->lpart = (cp + 1) ;
	        eip->llen = (sp + sl) - (cp + 1) ;
	        eip->hlen = (cp - sp) ;
	    } else {
		/* local */
	        eip->type = EMAINFO_TLOCAL ;
	        eip->hpart = nullptr ;
	        eip->hlen = 0 ;
	        eip->lpart = sp ;
	        eip->llen = sl ;
	    } /* end if */
	   type = eip->type ;
	} /* end if (non-null) */
	return (rs >= 0) ? type : rs ;
}
/* end subroutine (emainfo) */

int emainfo_mktype(emainfo *eip,int type,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (eip && rbuf) {
	    rs = SR_OK ;
	    if (type >= 0) {
		cnullptr	np{} ;
		storebuf	b(rbuf,rlen) ;
	        switch (type) {
	        case EMAINFO_TLOCAL:
		    {
		        cint	ml = min(eip->llen,rlen) ;
			rs = b.strw(eip->lpart,ml) ;
		    }
	            break ;
	        case EMAINFO_TUUCP:
	            if (eip->hpart && (eip->hlen >= 0)) {
		        cint	hl = eip->hlen ;
		        cchar	*hp = eip->hpart ;
	                if ((rs = b.strw(hp,hl)) >= 0) {
	                    rs = b.chr('!') ;
		        }
	            } /* end if (had a host part) */
		    if (rs >= 0) {
	                rs = b.strw(eip->lpart,eip->llen) ;
		    }
	            break ;
	        case EMAINFO_TARPA:
	            if ((rs = b.strw(eip->lpart,eip->llen)) >= 0) {
	                if (eip->hpart && (eip->hlen >= 0)) {
	                    if ((rs = b.chr('@')) >= 0) {
		                cint	hl = eip->hlen ;
			        cchar	*hp = eip->hpart ;
	                        rs = b.strw(hp,hl) ;
		            }
	                } /* end if */
		    } /* end if */
	            break ;
	        case EMAINFO_TARPAROUTE:
	            if (eip->hpart && (eip->hlen >= 0)) {
	                if ((rs = b.chr('@')) >= 0) {
		            cint	hl = eip->hlen ;
			    cchar	*hp = eip->hpart ;
	                    if ((rs = b.strw(hp,hl)) >= 0) {
	                        if (strnchr(eip->lpart,eip->llen,':') != np) {
	                            rs = b.chr(',') ;
	                        } else {
	                            rs = b.chr(':') ;
		                }
			    }
		        }
	            } /* end if (had a host part) */
		    if (rs >= 0) {
		        cint	ll = eip->llen ;
		        cchar	*lp = eip->lpart ;
	                rs = b.strw(lp,ll) ;
		    }
	            break ;
	        } /* end switch */
		rl = b.idx ;
	    } else {
		cint	ml = min(eip->llen,rlen) ;
	        rl = strwcpy(rbuf,eip->lpart,ml) - rbuf ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (emainfo_mktype) */


