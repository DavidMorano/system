/* nlsdialassist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* we provide NLS dialing assistance */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	nlsdialassist

	Description:
	We provide NLS dialing assistance.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sbuf.h>
#include	<snwcpy.h>
#include	<strn.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"nlsmsg.h"		/* <- NLS dial-response codes */
#include	"nlsdialassist.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	nlsresponse(char *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mknlsreq(char *nlsbuf,int nlslen,cchar *svcbuf,int svclen) noex {
	int		rs ;
	int		len = 0 ;
	if (sbuf svc ; (rs = svc.start(nlsbuf,nlslen)) >= 0) {
	    {
	        svc.strw(NLPS_REQ2,-1) ;
	        svc.strw(svcbuf,svclen) ;
	        svc.chr(0) ;
	    }
	    len = svc.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if (nlsbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mknlsreq) */

int readnlsresp(int fd,char *tbuf,int tlen,int to) noex {
	int		rs ;
	int		rs1 ;
	if (char *rbuf ; (rs = malloc_mn(&rbuf)) >= 0) {
	    cint	rlen = rs ;
	    if ((rs = uc_readlinetimed(fd,rbuf,rlen,to)) >= 0) {
	        if ((rs = nlsresponse(tbuf,tlen,rbuf,rs)) >= 0) {
	            cint	code = rs ;
	            switch (code) {
	            case NLSSTART:
	                break ;
	            case NLSFORMAT:
	            case NLSUNKNOWN:
	                rs = SR_BADREQUEST ;
	                break ;
	            case NLSDISABLED:
	                rs = SR_NOTAVAIL ;
	                break ;
	            default:
	                rs = SR_PROTO ;
	                break ;
	            } /* end switch */
	        } /* end if (parsing response) */
	    } /* end if (reading response) */
	    rs1 = uc_free(rbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (readnlsresp) */


/* local subroutine */

static int nlsresponse(char *tbuf,int tlen,cchar *rbuf,int rlen) noex {
	int		rs = SR_PROTO ;
	int		sl = rlen ;
	int		pv = 0 ;	/* used-afterwards */
	int		code = 0 ;	/* return-value */
	cchar		*sp = rbuf ;
	if (cchar *tp ; (tp = strnchr(sp,sl,':')) != nullptr) {
	    int		ll = intconv(tp - sp) ;
	    if ((rs = cfdeci(sp,ll,&pv)) >= 0) {
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if ((tp = strnchr(sp,sl,':')) != nullptr) {
		    ll = intconv(tp - sp) ;
	            if ((rs = cfdeci(sp,ll,&code)) >= 0) {
	                sl -= intconv((tp + 1) - sp) ;
	                sp = (tp + 1) ;
	                rs = snwcpy(tbuf,tlen,sp,sl) ;
	            }
	        } else {
	            rs = SR_PROTO ;
		}
	    }
	}
	if ((rs >= 0) && (pv != NLPS_REQVERSION)) {
	    rs = SR_NOPROTOOPT ;
	}
	return (rs >= 0) ? code : rs ;
}
/* end subroutine (nlsresponse) */


