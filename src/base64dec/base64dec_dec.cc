/* base64dec_dec SUPPORT base64) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* decode a file (encoded in BASE64) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was newly written.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:

  	Description:
	This module does the work of decoding the BASE64 input.

	Synopsis:
	int base64dec_dec(PI *pip,FILE *ofp,cchar *name) noex

	Arguments:
	pip		program information pointer
	ofp		FILE output file pointer
	name		filename

	Returns:
	>=		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ascii.h>
#include	<rmx.h>			/* |rmoeol(3uc)| */
#include	<strn.h>
#include	<b64decoder.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* |LIBDEBUG| */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif

#define	STATE	state


/* external subroutines */


/* external variables */


/* local structures */

struct state {
	char		*obuf ;
	int		olen ;
	int		cr ;
} ; /* end struct (state) */


/* forward references */

local int	procln(PI *,FILE *,b64decoder *,STATE *,cchar *,int) noex ;

local int	bwritetext(FILE *,int *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int base64dec_dec(PI *pip,FILE *ofp,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (pip && ofp && name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	size += (llen+1) ;
	size += (olen+1) ;
	if ((rs = uc_malloc(size,&abuf)) >= 0) {
	    char	*lbuf = (abuf + 0) ;
	    char	*obuf = (abuf + (llen+1)) ;
	    if (b64decoder d ; (rs = b64decoder_start(&d)) >= 0) {
	        if ((rs = bopen(ifp,name,"r",0666)) >= 0) {
	    	    STATE	cb ;
	    	    cb.obuf = obuf ;
	    	    cb.olen = olen ;
	    	    cb.cr = 0 ;
	            while ((rs = breadln(ifp,lbuf,llen)) > 0) {
	                if (cint len = rmeol(lbuf,rs) ; len > 0) {
			    rs = procln(pip,ofp,&d,&cb,lbuf,len) ;
			    wlen += rs ;
			} /* end if (positive) */
	                if (rs < 0) break ;
	            } /* end while (reading) */
	            rs1 = bclose(ifp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (ifile) */
	        rs1 = b64decoder_finish(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (b64decoder) */
	    rs1 = uc_free(abuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdecode: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progdecode) */


/* local subroutines */

local int procln(PI *pip,FILE *ofp,b64decoder *dp,STATE *statep,
		cchar *lp,int ll) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = b64decoder_load(dp,lp,ll)) > 0) {
	    cint	olen = statep->olen ;
	    int		*crp = &statep->cr ;
	    char	*obuf = statep->obuf ;
	    while ((rs = b64decoder_read(dp,obuf,olen)) > 0) {
		cint	ol = rs ;
		if (pip->fl.text) {
		    rs = bwritetext(ofp,crp,obuf,ol) ;
	    	    wlen += rs ;
		} else {
		    rs = bwrite(ofp,obuf,ol) ;
		    wlen += rs ;
		}
		if (rs < 0) break ;
	    } /* end while (reading result) */
	} /* end if (b64decoder_load) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procln) */

/* write out adjusted text */
local int bwritetext(FILE *ofp,int crp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*cp ;

	while ((cp = strnbrk(sp,sl,"\r\n")) != nullptr) {

	    if (*crp && (*sp != '\n')) {
	        *crp = false ;
	        rs = bputc(ofp,'\r') ;
	        wlen += rs ;
	    } /* end if */

	    if ((rs >= 0) && ((cp - sp) > 0)) {
	        rs = bwrite(ofp,sp,(cp - sp)) ;
	        wlen += rs ;
	    } /* end if */

	    if ((rs >= 0) && (*cp == '\n')) {
	        *crp = false ;
	        rs = bputc(ofp,'\n') ;
	        wlen += rs ;
	    } else if (*cp == '\r') {
	        *crp = TRUE ;
	    }

	    sl -= ((cp + 1) - sp) ;
	    sp = (cp + 1) ;

	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (sl > 0)) {

	    if (*crp) {
	        *crp = false ;
	        rs = bputc(ofp,'\r') ;
	        wlen += rs ;
	    } /* end if */

	    if (rs >= 0) {
	        rs = bwrite(ofp,sp,sl) ;
	        wlen += rs ;
	    }

	} /* end if (end processing) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (bwritetext) */

local int coneol(cchar *lp,int ll) noex {
	int		el = 0 ;
	if ((ll > 0) && (lp[ll -1] == CH_NL)) {
	    ll -= 1 ;
	    el += 1 ;
	}
	if ((ll > 0) && (lp[ll -1] == CH_CR)) {
	    ll -= 1 ;
	    el += 1 ;
	}
	return el ;
}
/* end subroutine (coneol) */


