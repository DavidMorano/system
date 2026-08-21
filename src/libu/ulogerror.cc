/* ulogerror SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* log an error in the error buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2023-10-08, David A-D- Morano
	This is a new thing, because, why now?
	
*/

/* Copyright © 2023 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ulogerror

	Description:
	This is a facility to log a small number of error reports.
	These reports can optionally be written out (in formatted
	form) to STDERR on program exit.

	Synopsis:
	int logerror(cchar *id,int err,cchar *msg) noex

	Arguments:
	id		the ID of the reporter
	err		the numerial error from reporter
	msg		optional message being reported

	Returns:
	0		your error report was not recorded
	1		your error report was recorded

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD |getenv(3c)| */
#include	<cstdio>		/* CSTD |snprintf(3c)| */
#include	<cstring>		/* CSTD |strlcpy(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ulogerror.h"		/* <- redundant */


/* local defines */

#define	ULOGERRMGR_IDLEN	16
#define	ULOGERRMGR_MSGLEN	80
#define	ULOGERRMGR_NENTS	10
#define	ULOGERRMGR_VARNAME	"ULOGERROR_OUTPUT"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ulogerrmgr_ent {
	int		err ;
	char		id[ULOGERRMGR_IDLEN+1] ;
	char		msg[ULOGERRMGR_MSGLEN+1] ;
    } ; /* end struct */
    struct ulogerrmgr {
	ulogerrmgr_ent	ents[ULOGERRMGR_NENTS] = {} ;
	int		c = 0 ;
	int record(cchar *,int,cchar * = nullptr) noex ;
	void fini() noex ;
	destruct ulogerrmgr() noex {
	    fini() ;
	} ; /* end destruct */
    private:
	void fini_ents(int) noex ;
    } ; /* end struct ulogerrmgr) */
} /* end namespace */


/* forward references */

static charp strdcpyu(char *dp,int dl,cchar *sp) noex {
	char		*rp = dp ;
	if (dl >= 0) {
	    csize	dsz = (dl + 1) ;
	    if (size_t rsz ; (rsz = strlcpy(dp,sp,dsz)) >= dsz) {
	        rp = charp(dp+dl) ;
	    } else {
	        cint	sl = int(rsz) ;
	        rp = charp(dp+sl) ;
	    }
	} /* end if (non-zero positive) */
	return rp ;
} /* end subroutine (strdcpyu) */


/* local variables */

constexpr int		wlen = (ULOGERRMGR_MSGLEN*2) ;
constexpr char		var[] = ULOGERRMGR_VARNAME ;
constinit ulogerrmgr	ulogerrmgr_data ;


/* exported variables */


/* exported subroutines */

int ulogerror(cchar *id,int err,cchar *msg) noex {
	int		f = false ;
	if (id) {
	    f = ulogerrmgr_data.record(id,err,msg) ;
	}
	return f ;
} /* end subroutine (ulogerror) */


/* local subroutines */

int ulogerrmgr::record(cchar *id,int err,cchar *msg) noex {
	static cint	idlen = ULOGERRMGR_IDLEN ;
	static cint	mglen = ULOGERRMGR_MSGLEN ;
	int		f = false ;
	if (c < ULOGERRMGR_NENTS) {
	    f = true ;
	    ents[c].err = err ;
	    strdcpyu(ents[c].id,idlen,id) ;
	    if (msg) {
	        strdcpyu(ents[c].msg,mglen,msg) ;
	    }
	    c += 1 ;
	} /* end if (recorded) */
	return f ;
} /* end subroutine (ulogerrmgr::record) */

void ulogerrmgr::fini() noex {
	static cint	fd = FD_STDERR ;
	cchar		*announce = "ulogerror: dump\n" ;
	if (cchar *vnp = getenv(var) ; vnp) {
	    if (vnp[0] && (c > 0)) {
		write(fd,announce,strlen(announce)) ;
		fini_ents(fd) ;
	    } /* end if (have valid) */
	} /* end if (have variable) */
} /* end subroutine (ulogerrmgr::fini) */

void ulogerrmgr::fini_ents(int fd) noex {
	cauto	snfmt = snprintf ;
	char	wbuf[wlen+1] ;
	cchar	*fmt = "ulogerror: %s (%3d) %s\n" ;
	for (int i = 0 ; i < c ; i += 1) {
	    csize	wsize = size_t(wlen + 1) ;
	    cint	err = ents[i].err ;
	    cchar	*id = ents[c].id ;
	    cchar	*msg = ents[c].msg ;
	    if (int wl ; (wl = snfmt(wbuf,wsize,fmt,id,err,msg)) > 0) {
		write(fd,wbuf,wl) ;
	    } /* end if (snprintf) */
	} /* end for */
} /* end subroutine (ulogerrmgr::fini_ents) */


