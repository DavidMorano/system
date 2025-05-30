/* logfile_userinfo SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* log user information */
/* version %I% last-modified %G% */


/* revision history:

	= 2005-01-20, David A­D­ Morano
	This was originally written to collect some common PCS code
	into one subroutine.  Making and refactoring PCS programs
	accounts for a good bit of these subroutine libraries.

*/

/* Copyright © 2005 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	logfile_userinfo

	Description:
	This subroutine is generally used to make the first log-file
	entry at the start of a program involcation.  The subroutine
	requires that both a LOGFILE object have already been opened
	and that the caller has retrieved the user information using
	the userinfo facility (and passed a pointer to that down
	to this subroutine).

	Synopsis:
	int logfile_userinfo(logfile *op,userinfo *uip,time_t dt,
		cc *pn,cc *vn) noex

	Arguments:
	op		pointer to LOGFILE object
	uip		pointer to userinfo object
	dt		current UNIX® time of day
	pn		program name
	vn		program-version name

	Returns:
	>=0	number of bytes written to log
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<userinfo.h>
#include	<getostype.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"logfile.h"


import uconstants ;

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mkuibang(char *,int,userinfo *) noex ;
    extern int	mkuiname(char *,int,userinfo *) noex ;
}

extern "C" {
    extern char	*timestr_logz(time_t,char *) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct loguser ;
    typedef int (loguser::*loguser_m)() noex ;
    struct loguser {
	logfile		*op ;
	userinfo	*uip ;
	cchar		*pn ;
	cchar		*vn ;
	time_t		dt ;
	loguser(logfile *lp,userinfo *up,time_t t,cc *p,cc *v) noex : dt(t) {
	    op = lp ;
	    uip = up ;
	    dt = (t) ? t : time(nullptr) ;
	    pn = (p) ? p : "" ;
	    vn = (v) ? v : "" ;
	} ;
	operator int () noex ;
	int first() noex ;
	int second() noex ;
	int third() noex ;
    } ; /* end struct (loguser) */
}

struct vars {
	cchar		*a ;
	int		nodenamelen ;
	int		usernamelen ;
	int		bangnamelen ;
} ;


/* forward references */

static int	mkvars() noex ;


/* local variables */

static constexpr loguser_m	mems[] = {
	&loguser::first,
	&loguser::second,
	&loguser::third
} ;

static vars			var ;


/* exported variables */


/* exported subroutines */

int logfile_userinfo(logfile *op,userinfo *uip,time_t dt,cc *pn,cc *vn) noex {
	int		rs ;
	int		wlen = 0 ;
	if ((rs = logfile_magic(op,uip)) >= 0) {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
		loguser		lo(op,uip,dt,pn,vn) ;
		rs = lo ;
		wlen += rs ;
	    } /* end if (mkvars) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (logfile_userinfo) */

loguser::operator int () noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	for (auto m : mems) {
	    rs = (this->*m)() ;
	    wlen += rs ;
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (loguser::operator) */

int loguser::first() noex {
	int		rs ;
	int		wlen = 0 ;
	char		tbuf[TIMEBUFLEN + 1] ;
	{
	    cchar	*ts = timestr_logz(dt,tbuf) ;
	    cchar	*st = "-" ;
	    cchar	*fmt ;
	    if (uip->ostype < ostype_overlast) {
		st = ostypenames[uip->ostype] ;
	    }
	    if ((pn[0] != '\0') || (vn[0] != '\0')) {
	        fmt = "%s %s %s/%s" ;
	    } else {
	        fmt = "%s starting" ;
	    }
	    rs = logfile_printf(op,fmt,ts,pn,vn,st) ;
	    wlen += rs ;
	} /* end block */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (loguser::first) */

int loguser::second() noex {
	int		rs = SR_OK ;
	cchar		*a = var.a ;
	cchar		*mn = uip->machine ;
	cchar		*sn = uip->sysname ;
	cchar		*rn = uip->release ;
	cchar		*dn = uip->domainname ;
	int		wlen = 0 ;
	if (a) {
	    rs = logfile_printf(op,"a=%s os=%s(%s) d=%s",a,sn,rn,dn) ;
	    wlen += rs ;
	} else {
	    rs = logfile_printf(op,"m=%s os=%s(%s) d=%s",mn,sn,rn,dn) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end method (loguser::second) */

int loguser::third() noex {
	cint		nlen = var.bangnamelen ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*nbuf{} ;
	if ((rs = uc_malloc((nlen+1),&nbuf)) >= 0) {
	    if ((rs = mkuibang(nbuf,nlen,uip)) >= 0) {
	        rs = logfile_print(op,nbuf,rs) ;
	        wlen += rs ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (loguser::third) */

static int mkvars() noex {
	cchar		*vn = varname.architecture ;
	int		rs ;
	if ((rs = getbufsize(getbufsize_nn)) >= 0) {
	    var.nodenamelen = rs ;
	    if ((rs = getbufsize(getbufsize_un)) >= 0) {
		var.usernamelen = rs ;
		var.bangnamelen = (var.nodenamelen + var.usernamelen + 1) ;
		var.a = getenv(vn) ;
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */


