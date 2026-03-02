/* zprintf SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* 'Named File' printf subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	printf

	Description:
	This subroutine performs a 'printf' like function but to
	the named file which is passed as the first argument.

	Synopsis:
	int zprintf(cchar *fname,cchar *fmt,...) noex

	Arguments:
	fname		file to print to
	fmt		standard format string
	...		enverything else

	Returns:
	>=0		length of data (in bytes) written to file
	<0		failure (system-return)

	Notes:

	Q. Does this subroutine have to be multi-thread-safe?
	A. In short, of course!

	Q. What do we not hve to place a mutex lock around the
	|write(2)| subroutine?
        A. Because we think that because we open the file a-fresh, getting a
        unique file-pointer, we *think* that the |write(2)| shoule be atomic,
        thus making this subroutine multi-thread-safe.


	+ Note on locking:

	There is no problem using (for example) |uc_lockf(3uc)| for establishing	the lock on the file.  The problem comes in with the associated un-lock
        component. Since the file advances the file-pointer (file-offset) value,
        the assocated un-lock does not unlock the proper file section, but
        rather a section beyong what was written. So we use |uc_lockfile(3uc)|
        instead to just lock and unlock the entire file.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstdio>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |strwcpy(3u)| */
#include	<mkx.h>
#include	<rmx.h>
#include	<format.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"zprintf.h"
#include	"debugmkhex.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	PBUFLEN		512		/* Print-Buffer-Length */

#define	SI		subinfo


/* imported namespaces */


/* local typedefs */

using libu::strwcpy ;			/* subroutine */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*fn ;
	cchar		*id ;
	char		*bp ;
	int		mc ;
	int		bl ;
	int		wl ;
	int		blen ;
	int		ilen ;
} ; /* end struct (subinfo) */


/* forward references */

local int subinfo_start		(SI *,char *,cchar *,cchar *,int) noex ;
local int subinfo_finish	(SI *) noex ;
local int subinfo_wrline	(SI *,cchar *,int) noex ;
local int subinfo_flushover	(SI *,int) noex ;
local int subinfo_write		(SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int zprint(cchar *fn,cchar *sp,int µsl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (fn && sp) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		rs = SR_OK ;
		if (int sl ; (sl = getlenstr(sp,µsl)) > 0) {
	            cint	of = (O_WRONLY | O_APPEND) ;
	            if ((rs = u_open(fn,of,0666)) >= 0) {
	                cint	fd = rs ;
		        cint	cmd = F_LOCK ;
		        if ((rs = u_lockf(fd,cmd,0z)) >= 0) {
	                    if ((rs = u_write(fd,sp,sl)) >= 0) {
		                len = rs ;
			        if ((sl > 0) && (sp[sl-1] != '\n')) {
			            char	nbuf[2] = "\n" ;
			            rs = u_write(fd,nbuf,1) ;
			            len += rs ;
			        }
		            } /* end if (uc_writen) */
		        } /* end if (uc_lockfile) */
	                rs1 = u_close(fd) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (file) */
	        } /* end if (non-zero positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (zprint) */

int zprintf(cchar *fn,cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		fl = 0 ; /* return-value */
	if (fn && fmt) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		rs = SR_OK ;
	        if (fmt[0]) {
	            cint	flen = PBUFLEN ;
	            char	fbuf[PBUFLEN + 1] ;
	            va_begin(ap,fmt) ;
	            if ((fl = vsnprintf(fbuf,flen,fmt,ap)) > 0) {
		        rs = zprint(fn,fbuf,fl) ;
	            }
	            va_end(ap) ;
	        } /* end if (non-nul) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (zprintf) */

int zprinthexblock(cchar *fn,cchar *id,int mc,cvoid *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fn && vp) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        int		sl = vl ;
	        cchar		*sp = (cchar *) vp ;
	        char		b[PBUFLEN + 1] ;
	        if (mc < 0) mc = COLUMNS ;
	        if (sl < 0) sl = lenstr(sp) ;
	        if (SI si ; (rs = subinfo_start(&si,b,fn,id,mc)) >= 0) {
	            while (sl > 0) {
		        rs = subinfo_wrline(&si,sp,sl) ;
		        sp += rs ;
		        sl -= rs ;
		        if (rs < 0) break ;
	            } /* end while */
	            wlen = subinfo_finish(&si) ;
	            if (rs >= 0) rs = wlen ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (zprinthexblock) */


/* local subroutines */

local int subinfo_start(SI *sip,char *bp,cchar *fn,cchar *id,int mc) noex {
    	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->bp = bp ;
	    sip->fn = fn ;
	    sip->mc = mc ;
	    sip->blen = PBUFLEN ;
	    if (id) {
	        sip->id = id ;
	        sip->ilen = lenstr(id) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subfino_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	if (sip->bl > 0) {
	    if ((rs = zprint(sip->fn,sip->bp,sip->bl)) >= 0) {
	        sip->wl += rs ;
		sip->bl = 0 ;
	    }
	}
	return (rs >= 0) ? sip->wl : rs ;
}
/* end subroutine (subfino_finish) */

local int subinfo_wrline(SI *sip,cchar *sp,int sl) noex {
	cint		mlen = MIN((3*sl),(sip->mc-sip->ilen+1)) ;
	int		rs ;
	int		ul = 0 ;

	if ((rs = subinfo_flushover(sip,mlen)) >= 0) {
	    if (sip->id) {
		rs = subinfo_write(sip,sip->id,sip->ilen) ;
	    }
	    if (rs >= 0) {
		cint	alen = (sip->blen - sip->bl) ;
		cint	n = (mlen / 3) ;
	        char	*bp = (sip->bp + sip->bl) ;
	        if ((rs = mkhexstr(bp,alen,sp,n)) >= 0) {
		    sip->bl += rs ;
		    sip->bp[sip->bl++] = '\n' ;
		    ul = n ;
		}
	    }
	} /* end if (subinfo_flushover) */

	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (subinfo_wrline) */

local int subinfo_flushover(SI *sip,int mlen) noex {
	int		rs = SR_OK ;
	if (mlen > (sip->blen-sip->bl)) {
	    char	*bp = (sip->bp + sip->bl) ;
	    if ((rs = zprint(sip->fn,bp,sip->bl)) >= 0) {
	        sip->wl += rs ;
		sip->bl = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (subfino_flushover) */

local int subinfo_write(SI *sip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < (sip->blen - sip->bl)) {
	    char	*bp = (sip->bp + sip->bl) ;
	    rs = intconv(strwcpy(bp,sp,sl) - sip->bp) ;
	    sip->bl += rs ;
	} else {
	    rs = SR_OVERFLOW ;
	}
	return rs ;
}
/* end subroutine (subfino_write) */


