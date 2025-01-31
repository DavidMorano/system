/* zprintf SUPPORT */
/* encoding=ISO8859-1 */
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
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mkx.h>
#include	<rmx.h>
#include	<strwcpy.h>
#include	<format.h>
#include	<localmisc.h>		/* |COLUMNS| */

#include	"zprintf.h"


/* local defines */

#define	PBUFLEN		512

#define	SI		struct subinfo


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
} ;


/* forward references */

static int subinfo_start(SI *,char *,cchar *,cchar *,int) noex ;
static int subinfo_finish(SI *) noex ;
static int subinfo_wrline(SI *,cchar *,int) noex ;
static int subinfo_flushover(SI *,int) noex ;
static int subinfo_write(SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int zprint(cchar *fn,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (fn == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;

	if (fn[0] == '\0') return SR_INVALID ;

	if (sl < 0) sl = strlen(sp) ;

	if (sl > 0) {
	    cint	of = (O_WRONLY | O_APPEND) ;
	    if ((rs = u_open(fn,of,0666)) >= 0) {
	        cint	fd = rs ;
		cint	cmd = F_LOCK ;
		if ((rs = uc_lockfile(fd,cmd,0L,0L,-1)) >= 0) {
	            if ((rs = uc_writen(fd,sp,sl)) >= 0) {
		        len = rs ;
			if ((sl > 0) && (sp[sl-1] != '\n')) {
			    char	nbuf[2] = "\n" ;
			    rs = uc_writen(fd,nbuf,1) ;
			    len += rs ;
			}
		    } /* end if (uc_writen) */
		} /* end if (uc_lockfile) */
	        u_close(fd) ;
	    } /* end if (file) */
	} /* end if (positive) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (zprint) */

int zprintf(cchar *fn,cchar *fmt,...) noex {
	int		rs = SR_OK ;
	int		fl = 0 ;

	if (fn == NULL) return SR_FAULT ;
	if (fmt == NULL) return SR_FAULT ;

	if (fn[0] == '\0') return SR_INVALID ;

	if (fmt[0] != '\0') {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    cint	flen = PBUFLEN ;
	    char	fbuf[PBUFLEN + 1] ;
	    if ((fl = vsnprintf(fbuf,flen,fmt,ap)) > 0) {
		rs = zprint(fn,fbuf,fl) ;
	    }
	    va_end(ap) ;
	} /* end if (non-nul) */

	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (zprintf) */

int zprinthexblock(cchar *fn,cchar *id,int mc,cvoid *vp,int vl) noex {
	SI		si ;
	int		rs ;
	int		sl = vl ;
	int		wlen = 0 ;
	cchar		*sp = (cchar *) vp ;
	char		b[PBUFLEN + 1] ;

	if (fn == NULL) return SR_FAULT ;
	if (vp == NULL) return SR_FAULT ;

	if (fn[0] == '\0') return SR_INVALID ;

	if (mc < 0) mc = COLUMNS ;
	if (sl < 0) sl = strlen(sp) ;

	if ((rs = subinfo_start(&si,b,fn,id,mc)) >= 0) {
	    while (sl > 0) {
		rs = subinfo_wrline(&si,sp,sl) ;
		sp += rs ;
		sl -= rs ;
		if (rs < 0) break ;
	    } /* end while */
	    wlen = subinfo_finish(&si) ;
	    if (rs >= 0) rs = wlen ;
	} /* end if (subinfo) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (zprinthexblock) */


/* local subroutines */

static int subinfo_start(SI *sip,char *bp,cchar *fn,cchar *id,int mc) noex {
    	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->bp = bp ;
	    sip->fn = fn ;
	    sip->mc = mc ;
	    sip->blen = PBUFLEN ;
	    if (id) {
	        sip->id = id ;
	        sip->ilen = strlen(id) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subfino_start) */

static int subinfo_finish(SI *sip) noex {
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

static int subinfo_wrline(SI *sip,cchar *sp,int sl) noex {
	cint		mlen = MIN((3*sl),(sip->mc-sip->ilen+1)) ;
	int		rs ;
	int		ul = 0 ;

	if ((rs = subinfo_flushover(sip,mlen)) >= 0) {
	    if (sip->id != NULL) {
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

static int subinfo_flushover(SI *sip,int mlen) noex {
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

static int subinfo_write(SI *sip,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < (sip->blen - sip->bl)) {
	    char	*bp = (sip->bp + sip->bl) ;
	    rs = (strwcpy(bp,sp,sl) - sip->bp) ;
	    sip->bl += rs ;
	} else {
	    rs = SR_OVERFLOW ;
	}
	return rs ;
}
/* end subroutine (subfino_write) */


