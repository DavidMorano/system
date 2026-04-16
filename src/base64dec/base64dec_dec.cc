/* base64dec_dec SUPPORT (base64dec) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* decode a file (encoded in BASE64) */
/* version %I% last-modified %G% */

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
	int base64dec_dec(PI *pip,cchar *name) noex

	Arguments:
	pip		program information pointer
	name		filename

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>
#include	<cstdlib>
#include	<cstdio>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<umem.hh>
#include	<ascii.h>
#include	<rmx.h>			/* |rmoeol(3uc)| */
#include	<strn.h>		/* |strnbrk(3uc)| */
#include	<b64decoder.h>
#include	<ccfile.hh>
#include	<localmisc.h>
#include	<libf.h>		/* LIBF */
#include	<libdebug.h>		/* LIBDEBUG */
#include	<dprintf.hh>		/* debugging */

#include	"base64dec.h"		/* program-private */

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ulibvals.ccm"

import libutil ;			/* |lenstr(3u)| */
import ulibvals ;			/* |ulibval(3u)| */

/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* run-time debugging */
#endif


/* imported namespaces */

using libu::umem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct decoder {
	PI		*pip ;
	cchar		*name ;
	char		*a ;
	char		*lbuf ;
	char		*obuf ;
	int		llen ;
	int		olen ;
	decoder(PI *p,cchar *n) noex : pip(p), name(n) { 
	    a = nullptr ;
	} ;
	int start() noex ;
	int finish() noex ;
	int decobj() noex ;
	int decfile(b64decoder *) noex ;
	int procln(b64decoder *,cchar *,int) noex ;
	int proctext(cchar *,int) noex ;
	int proctextln(cchar *,int) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct decoder() {
	    if (a) dtor() ;
	} ;
    } ; /* end struct (decoder) */
    void decoder::dtor() noex {
	finish() ;
    } ;
} /* end namespace */


/* forward references */

local char *strnend(cchar *sp,int sl) noex {
    	return strnsub(sp,sl,"\r\n") ;
} /* end method (strnend) */


/* local variables */

static int	maxlinelen = ulibval.maxline ;

cbool		f_debug = CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int base64dec_dec(PI *pip,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent name=%s\n",name) ;
	if (pip && name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
		decoder od(pip,name) ;
		rs = od ;
		wlen = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (base64dec_dec) */


/* local subroutines */

decoder::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if ((rs = start()) >= 0) {
	    int		sz = 0 ;
	    int		ai = 2 ;
	    sz += (ai * (maxlinelen + 1)) ;
	    llen = maxlinelen ;
	    olen = maxlinelen ;
	    if ((rs = umem.mall(sz,&a)) >= 0) {
	        lbuf = (a + (--ai * (maxlinelen + 1))) ;
	        obuf = (a + (--ai * (maxlinelen + 1))) ;
	        {
	            rs = decobj() ;
	            wlen = rs ;
	        }
	        rs1 = umem.free(a) ;
	        if (rs >= 0) rs = rs1 ;
	        a = nullptr ;
	    } /* end if (m-a-f) */
	    rs1 = finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::operator) */

int decoder::start() noex {
    	return SR_OK ;
} /* end method (decoder::start) */

int decoder::finish() noex {
    	return SR_OK ;
} /* end method (decoder::finish) */

int decoder::decobj() noex {
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (b64decoder d ; (rs = d.start) >= 0) {
	    {
	        rs = decfile(&d) ;
		wlen += rs ;
	    }
	    rs1 = d.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (b64decoder) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::decobj) */

int decoder::decfile(b64decoder *dop) noex {
    	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if (ccfile inf ; (rs = inf.open(name,"r")) >= 0) {
	    while ((rs = inf.readln(lbuf,llen)) > 0) {
		if (cint len = rmeol(lbuf,rs) ; len > 0) {
		    rs = procln(dop,lbuf,len) ;
		    wlen += rs ;
		} /* end if (positive) */
		if (rs < 0) break ;
	    } /* end while (reading) */
	    rs1 = inf.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ccfile) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::decfile) */

int decoder::procln(b64decoder *dop,cchar *lp,int ll) noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent\n") ;
	if ((rs = dop->load(lp,ll)) > 0) {
	    while ((rs = dop->read(obuf,olen)) > 0) {
		cint	ol = rs ;
		if (pip->fl.text) {
		    rs = proctext(obuf,ol) ;
	    	    wlen += rs ;
		} else {
		    rs = fwriter(pip->ofp,obuf,ol) ;
		    wlen += rs ;
		}
		if (rs < 0) break ;
	    } /* end while (reading result) */
	} /* end if (b64decoder_load) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::procln) */

int decoder::proctext(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	DPRINTF("ent\n") ;
	for (char *tp ; (tp = strnend(sp,sl)) != nullptr ; ) {
	    cint tl = intconv(tp - sp) ;
	    {
	        rs = proctextln(sp,tl) ;
	        wlen += rs ;
	    }
	    sl -= intconv((tp + 2) - sp) ;
	    sp = (tp + 2) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && (sl > 0)) {
	    rs = fwriter(pip->ofp,sp,sl) ;
	    wlen += rs ;
	} /* end if (remaining) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::proctext) */

int decoder::proctextln(cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	DPRINTF("ent\n") ;
	if (sl > 0) {
	    if ((rs = fwriter(pip->ofp,sp,sl)) >= 0) {
		wlen += rs ;
		rs = fputch(pip->ofp,CH_NL) ;
		wlen += rs ;
	    }
	} /* end if (non-zero positive) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end method (decoder::proctextln) */


