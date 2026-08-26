/* cgi SUPPORT (CGI creation and output) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* hack to output the HTTP headers */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cgi

	Description:
	This is a hack to create and output some basic CGIL.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<sbuf.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC |timestr_msg(3uc)| */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<shio.h>

#include	"cgi.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	KBUFLEN
#define	KBUFLEN		MAX(TIMEBUFLEN,60)
#endif

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))


/* external subroutines */

extern "C" {
    extern int	bufprintf(char *,int,cchar *,...) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int cgi_ctor(cgi *op,Args ... args) noex {
    	CGI		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cgi_ctor) */

local int cgi_dtor(cgi *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (cgi_dtor) */

template<typename ... Args>
local inline int cgi_magic(cgi *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == CGI_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (cgi_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int cgi_start(cgi *op,shio *ofp) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_ctor(op,ofp)) >= 0) {
	    op->ofp = ofp ;
	    op->magval = CGI_MAGIC ;
	    op->wlen += wlen ;
	    if (rs < 0) {
		cgi_dtor(op) ;
	    } /* end if (error) */
	} /* end if (cgi_ctor) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_start) */

int cgi_finish(cgi *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = cgi_magic(op)) >= 0) {
	    op->ofp = nullptr ;
	    {
		rs1 = cgi_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cgi_finish) */

int cgi_eoh(cgi *op) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op)) >= 0) {
	    rs = shio_putc(op->ofp,CH_NL) ;
	    wlen += rs ;
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_eof) */

int cgi_hdrdate(cgi *op,time_t t) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op)) >= 0) {
	    shio	*ofp = op->ofp ;
	    cint	hlen = KBUFLEN ;
	    cchar	*hdr = "date" ;
	    char	tbuf[TIMEBUFLEN+1] ;
	    char	hbuf[KBUFLEN +1] ;
	    timestr_msg(t,tbuf) ;
	    if ((rs = bufprintf(hbuf,hlen,"%s: %s",hdr,tbuf)) >= 0) {
	        if ((rs = shio_print(ofp,hbuf,rs)) >= 0) {
	            wlen += rs ;
	        } /* end if (shio_write) */
	    } /* end if (bufprintf) */
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_hdrdate) */

int cgi_hdr(cgi *op,cchar *kp,cchar *vp,int vl) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op,kp)) >= 0) {
	    shio	*ofp = op->ofp ;
	    cint	klen = KBUFLEN ;
	    char	kbuf[KBUFLEN+1] ;
	    if ((rs = bufprintf(kbuf,klen,"%s: ",kp)) >= 0) {
	        if ((rs = shio_write(ofp,kbuf,rs)) >= 0) {
	            wlen += rs ;
	            rs = shio_print(ofp,vp,vl) ;
	            wlen += rs ;
	        } /* end if (shio_write) */
	    } /* end if (bufprintf) */
	    op->wlen += wlen ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_hdr) */

int cgi_write(cgi *op,cvoid *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op,lbuf)) >= 0) {
	    rs = shio_write(op->ofp,lbuf,llen) ;
	    wlen += rs ;
	    op->wlen += rs ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_write) */

int cgi_println(cgi *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op,lbuf)) >= 0) {
	    rs = shio_print(op->ofp,lbuf,llen) ;
	    wlen += rs ;
	    op->wlen += rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_println) */

int cgi_printf(cgi *op,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = cgi_magic(op,fmt)) >= 0) {
	    va_begin(ap,fmt) ;
	    rs = cgi_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (cgi_printf) */

int cgi_vprintf(cgi *op,cchar *fmt,va_list ap) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op,fmt,ap)) >= 0) {
	    rs = shio_vprintf(op->ofp,fmt,ap) ;
	    wlen += rs ;
	    op->wlen += rs ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_vprintf) */

int cgi_putc(cgi *op,int ch) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = cgi_magic(op)) >= 0) {
	    rs = shio_putc(op->ofp,ch) ;
	    wlen += rs ;
	    op->wlen += rs ;
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cgi_putc) */


