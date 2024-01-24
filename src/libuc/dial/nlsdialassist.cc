/* nlsdialassist SUPPORT */
/* lang=C++20 */

/* we provide NLS dialing assistance */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-17, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We provide NLS dialing assistance.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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


/* local namespaces */


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
	sbuf		svc ;
	int		rs ;
	int		len = 0 ;
	if ((rs = sbuf_start(&svc,nlsbuf,nlslen)) >= 0) {
	    sbuf_strw(&svc,NLPS_REQ2,-1) ;
	    sbuf_strw(&svc,svcbuf,svclen) ;
	    sbuf_char(&svc,0) ;
	    len = sbuf_finish(&svc) ;
	    if (rs >= 0) rs = len ;
	} /* end if (nlsbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mknlsreq) */

int readnlsresp(int fd,char *tbuf,int tlen,int to) noex {
	int		rs ;
	int		rs1 ;
	char		*rbuf{} ;
	if ((rs = malloc_mn(&rbuf)) >= 0) {
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
	int		pv = 0 ;
	int		code = 0 ;
	cchar		*tp ;
	cchar		*sp = rbuf ;
	if ((tp = strnchr(sp,sl,':')) != nullptr) {
	    if ((rs = cfdeci(sp,(tp-sp),&pv)) >= 0) {
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	        if ((tp = strnchr(sp,sl,':')) != nullptr) {
	            if ((rs = cfdeci(sp,(tp-sp),&code)) >= 0) {
	                sl -= ((tp+1)-sp) ;
	                sp = (tp+1) ;
	                rs = snwcpy(tbuf,tlen,sp,sl) ;
	            }
	        } else {
	            rs = SR_PROTO ;
		}
	    }
	}
	if ((rs >= 0) && (pv != NLPS_REQVERSION)) rs = SR_NOPROTOOPT ;
	return (rs >= 0) ? code : rs ;
}
/* end subroutine (nlsresponse) */


