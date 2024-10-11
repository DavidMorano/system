/* filer_mailsup SUPPORT */
/* lang=C++20 */

/* mail support from the FILER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	filer_writehdr
	filer_writehdrkey
	filer_printlncont

	Description:
	Here is provided some support routines for the FILER
	object that supports writing mail-message files.

	Synopses:
	int filer_writehdr(filer *,cchar *,int) noex
	int filer_writehdrkey(filer *,cchar *) noex
	int filer_writehdrkey(filer *,cchar *) noex

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mailmsghdrfold.h>
#include	<strn.h>
#include	<char.h>
#include	<localmisc.h>

#include	"filer.h"
#include	"filer_mailsup.h"


/* local defines */

#ifndef	MAILMSGCOLS
#define	MAILMSGCOLS	76
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

static int filer_writehdrval(filer *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int filer_writehdr(filer *fbp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (fbp && sp) {
	    int		kl ;
	    int		vl = -1 ;
	    cchar	*tp ;
	    cchar	*vp = nullptr ;
	    if (sl < 0) sl = strlen(sp) ;
	    kl = sl ;
	    if ((tp = strnchr(sp,sl,'=')) != nullptr) {
	        kl = (tp - sp) ;
	        vp = (tp+1) ;
	        vl = ((sp+sl)-vp) ;
	    }
	    if (kl > 0) {
	        if ((rs = filer_write(fbp,sp,kl)) >= 0) {
	            wlen += rs ;
	            if ((rs = filer_write(fbp,": ",2)) >= 0) {
	                wlen += rs ;
	                if (vl > 0) {
	                    rs = filer_writehdrval(fbp,vp,vl) ;
	                    wlen += rs ;
	                } else {
	                    rs = filer_println(fbp,sp,0) ;
	                    wlen += rs ;
	                }
	            } /* end if (write) */
	        } /* end if (filer_write) */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writehdr) */

int filer_writehdrkey(filer *fbp,cchar *kn) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (fbp && kn) {
	    rs = SR_INVALID ;
	    if (kn[0]) {
	        if ((rs = filer_write(fbp,kn,-1)) >= 0) {
	            wlen += rs ;
	            rs = filer_write(fbp,": ",2) ;
	            wlen += rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_writehdrkey) */

int filer_printlncont(filer *fbp,int leader,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	if (fbp && sp) {
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    if (sl > 0) {
	        char	buf[2] ;
	        if ((rs >= 0) && (leader > 0)) {
	            buf[0] = leader ;
	            buf[1] = '\0' ;
	            rs = filer_write(fbp,buf,1) ;
	            wlen += rs ;
	        }
	        if (rs >= 0) {
	            rs = filer_write(fbp,sp,sl) ;
	            wlen += rs ;
	        }
	        if (rs >= 0) {
	            buf[0] = '\n' ;
	            buf[1] = '\0' ;
	            rs = filer_write(fbp,buf,1) ;
	            wlen += rs ;
	        }
	    } /* end if (non-empty value) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (filer_printlncont) */


/* private subroutines */

static int filer_writehdrval(filer *fbp,cchar *vp,int vl) noex {
	static cint	ln = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	if (vp) {
	    MAILMSGHDRFOLD	folder, *fp = &folder ;
	    static cint		mcols = MAILMSGCOLS ;
	    if (vl < 0) vl = strlen(vp) ;
	    while (vl && CHAR_ISWHITE(*vp)) {
	        vp += 1 ;
	        vl -= 1 ;
	    }
	    if ((rs = mailmsghdrfold_start(fp,mcols,ln,vp,vl)) >= 0) {
	        int	i = 0 ;
	        int	indent = wlen ;
	        while (rs >= 0) {
	    	    int		ll ;
	    	    cchar	*lp = nullptr ;
	            rs = mailmsghdrfold_get(fp,indent,&lp) ;
	            if (rs <= 0) break ;
	            ll = rs ;
	            if ((rs >= 0) && (i > 0)) {
	                rs = filer_write(fbp," ",1) ;
	                wlen += rs ;
	            }
	            if (rs >= 0) {
	                rs = filer_println(fbp,lp,ll) ;
	                wlen += rs ;
	            }
	            indent = 1 ;
	            i += 1 ;
	        } /* end while */
	        rs1 = mailmsghdrfold_finish(fp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mailmsghdrfold) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailbox_writehdrval) */


