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


/* local typedefs */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int emainfo_load(EMAINFO *eip,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		type = 0 ;
	if (eip && sp) {
	    cchar	*cp ;
	    cchar	*cp1, *cp2 ;
	    rs = memclear(eip) ;
	    if (sl < 0) sl = strlen(sp) ;
/* what kind of address do we have? */
	    if ((cp1 = strnchr(sp,sl,'@')) != nullptr) {
	        if ((cp2 = strnchr(sp,sl,':')) != nullptr) {
/* ARPAnet route address */
	            eip->type = EMAINFO_TARPAROUTE ;
	            if ((cp = strnchr(sp,sl,',')) != nullptr) {
	                eip->host = (cp1 + 1) ;
	                eip->hlen = (cp - (cp1 + 1)) ;
	                eip->local = (cp + 1) ;
	                eip->llen = (sp + sl) - (cp + 1) ;
	            } else {
	                eip->host = (cp1 + 1) ;
	                eip->hlen = (cp2 - (cp1 + 1)) ;
	                eip->local = (cp2 + 1) ;
	                eip->llen = (sp + sl) - (cp2 + 1) ;
	            } /* end if */
	        } else {
/* normal ARPAnet address */
	            eip->type = EMAINFO_TARPA ;
	            eip->host = (cp1 + 1) ;
	            eip->hlen = (sp + sl) - (cp1 + 1) ;
	            eip->local = sp ;
	            eip->llen = (cp1 - sp) ;
	        } /* end if */
	    } else if ((cp = strnrchr(sp,sl,'!')) != nullptr) {
	        eip->type = EMAINFO_TUUCP ;
	        eip->host = sp ;
	        eip->hlen = (cp - sp) ;
	        eip->local = (cp + 1) ;
	        eip->llen = (sp + sl) - (cp + 1) ;
	        eip->hlen = (cp - sp) ;
	    } else {
/* local */
	        eip->type = EMAINFO_TLOCAL ;
	        eip->host = nullptr ;
	        eip->hlen = 0 ;
	        eip->local = sp ;
	        eip->llen = sl ;
	    } /* end if */
	   type = eip->type ;
	} /* end if (non-null) */
	return (rs >= 0) ? type : rs ;
}
/* end subroutine (emainfo) */

int emainfo_mktype(EMAINFO *eip,int type,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (eip && rbuf) {
	    rs = SR_OK ;
	    if (type >= 0) {
		cnullptr	np{} ;
	        switch (type) {
	        case EMAINFO_TLOCAL:
		    {
		        cint	ml = min(eip->llen,rlen) ;
	                rs = strwcpy(rbuf,eip->local,ml) - rbuf ;
	                i += rs ;
		    }
	            break ;
	        case EMAINFO_TUUCP:
	            if (eip->host && (eip->hlen >= 0)) {
		        cint	hl = eip->hlen ;
		        cchar	*hp = eip->host ;
	                rs = storebuf_strw(rbuf,rlen,i,hp,hl) ;
	                i += rs ;
		        if (rs >= 0) {
	                    rs = storebuf_chr(rbuf,rlen,i,'!') ;
	                    i += rs ;
		        }
	            } /* end if (had a host part) */
		    if (rs >= 0) {
	                rs = storebuf_strw(rbuf,rlen,i,eip->local,eip->llen) ;
	                i += rs ;
		    }
	            break ;
	        case EMAINFO_TARPA:
	            rs = storebuf_strw(rbuf,rlen,i,eip->local,eip->llen) ;
	            i += rs ;
	            if ((rs >= 0) && eip->host && (eip->hlen >= 0)) {
	                rs = storebuf_chr(rbuf,rlen,i,'@') ;
	                i += rs ;
	                if (rs >= 0) {
		            cint	hl = eip->hlen ;
			    cchar	*hp = eip->host ;
	                    rs = storebuf_strw(rbuf,rlen,i,hp,hl) ;
	                    i += rs ;
		        }
	            } /* end if */
	            break ;
	        case EMAINFO_TARPAROUTE:
	            if (eip->host && (eip->hlen >= 0)) {
	                rs = storebuf_chr(rbuf,rlen,i,'@') ;
	                i += rs ;
		        if (rs >= 0) {
		            cint	hl = eip->hlen ;
			    cchar	*hp = eip->host ;
	                    rs = storebuf_strw(rbuf,rlen,i,hp,hl) ;
	                    i += rs ;
		        }
		        if (rs >= 0) {
	                    if (strnchr(eip->local,eip->llen,':') != np) {
	                        rs = storebuf_chr(rbuf,rlen,i,',') ;
			        i += rs ;
	                    } else {
	                        rs = storebuf_chr(rbuf,rlen,i,':') ;
	                        i += rs ;
		            }
		        }
	            } /* end if (had a host part) */
		    if (rs >= 0) {
		        cint	ll = eip->llen ;
		        cchar	*lp = eip->local ;
	                rs = storebuf_strw(rbuf,rlen,i,lp,ll) ;
	                i += rs ;
		    }
	            break ;
	        } /* end switch */
	    } else {
		cint	ml = min(eip->llen,rlen) ;
	        i = strwcpy(rbuf,eip->local,ml) - rbuf ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (emainfo_mktype) */


