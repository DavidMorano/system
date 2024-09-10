/* getutmpent SUPPORT */
/* lang=C++20 */

/* get user information from UTMP database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name
	getutmpent

	Description:
	Get the user login name from the UTMP database.

	Synopsis:
	int getutmpent(utmpentx *ep,pid_t sid) noex
	int getutmpname(char *rbuf,int rlen,pid_t sid) noex
	int getutmphost(char *rbuf,int rlen,pid_t sid) noex
	int getutmpline(char *rbuf,int rlen,pid_t sid) noex

	Arguments:
	ep		pointer to UTMPENTX object
	rbuf		buffer to hold result
	rlen		length of user supplied buffer
	sid		session ID to lookup

	Returns:
	>=0		length of user buf
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>		/* |getsid(3c)| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<utmpacc.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"getutmpent.h"


/* local defines */

#define	AEBUFLEN	sizeof(struct utmpx)

#ifndef	VARUTMPLINE
#define	VARUTMPLINE	"UTMPLINE"
#endif
#ifndef	VARLOGLINE
#define	VARLOGLINE	"LOGLINE"
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ufinder ;
    typedef int (ufinder::*ufinder_m)() noex ;
    struct ufinder {
        utmpacc_ent	ae{} ;
        char		*aebuf ;
	pid_t		sid ;
	int		aelen = AEBUFLEN ;
	ufinder(pid_t i) noex : sid(i) { } ;
	int start() noex ;
	int finish() noex ;
	operator int () noex ;
	int trysid() noex ;
	int tryline() noex ;
	int trystat() noex ;
    } ;
}


/* forward references */

static int utmpent_utmpacc(utmpentx *,pid_t) noex ;
static int utmpent_load(utmpentx *,utmpacc_ent *) noex ;


/* local variables */

constexpr ufinder_m	mems[] = {
	&ufinder::trysid,
	&ufinder::tryline,
	&ufinder::trystat
} ;

constexpr cpcchar	utmpenvs[] = {
	VARUTMPLINE,
	VARLOGLINE
} ;


/* exported variables */


/* exported subroutines */

int getutmpent(utmpentx *ep,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    if (sid <= 0) sid = getsid(0) ;
	    ep->id[0] = '\0' ;
	    ep->line[0] = '\0' ;
	    ep->user[0] = '\0' ;
	    ep->host[0] = '\0' ;
	    ep->session = 0 ;
	    ep->date = 0 ;
	    ep->sid = sid ;
	    rs = utmpent_utmpacc(ep,sid) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpent) */

int getutmpname(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpentx	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LUSER ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) > 0) {
	        rs = sncpy(rbuf,rlen,e.user) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpname) */

int getutmphost(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpentx	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LHOST ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) > 0) {
	        rs = sncpy(rbuf,rlen,e.host) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmphost) */

int getutmpline(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpentx	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LLINE ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) > 0) {
	        rs = sncpy(rbuf,rlen,e.line) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpline) */


/* local subroutines */

ufinder::operator int () noex {
	int		rs = SR_OK ;
	for (auto &m : mems) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end method (ufinder::operator) */

int ufinder::start() noex {
	return uc_malloc(aelen,&aebuf) ;
}

int ufinder::finish() noex {
	int	rs = SR_BUGCHECK ;
	if (aebuf) {
	    rs = uc_free(aebuf) ;
	    aebuf = nullptr ;
	}
	return rs ;
}

int ufinder::trysid() noex {
	return utmpacc_entsid(&ae,aebuf,aelen,sid) ;
}

int ufinder::tryline() noex {
	static cint	oursid = getsid(0) ;
	int		rs = SR_OK ;
	if (sid == oursid) {
	    for (auto const &vn : utmpenvs) {
	        if (cchar *line ; (line = getenv(vn)) != nullptr) {
	            rs = utmpacc_entline(&ae,aebuf,aelen,line,-1) ;
	        }
		if (rs != 0) break ;
	    } /* end for */
	} /* end if (referencing ourself) */
	return rs ;
}

int ufinder::trystat() noex {
	return utmpacc_entstat(&ae,aebuf,aelen,sid) ;
}

static int utmpent_utmpacc(utmpentx *ep,pid_t sid) noex {
	ufinder		fo(sid) ;
	int		rs ;
	int		rs1 ;
	int		ffound = false ;
	if ((rs = fo.start()) >= 0) {
	    if ((rs = fo) > 0) {
		ffound = true ;
	        rs = utmpent_load(ep,&fo.ae) ;
	    }
	    rs1 = fo.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ufinder) */
	return (rs >= 0) ? ffound : rs ;
}
/* end subroutine (utmpent_utmpacc) */

static int utmpent_load(utmpentx *ep,utmpacc_ent *aep) noex {
	int		rs = SR_FAULT ;
	if (ep && aep) {
	    rs = SR_OK ;
	    strwcpy(ep->id,aep->id,min(GETUTMPENT_LID,UTMPACC_LID)) ;
	    strwcpy(ep->user,aep->user,min(GETUTMPENT_LUSER,UTMPACC_LUSER)) ;
	    strwcpy(ep->line,aep->line,min(GETUTMPENT_LLINE,UTMPACC_LLINE)) ;
	    strwcpy(ep->host,aep->host,min(GETUTMPENT_LHOST,UTMPACC_LHOST)) ;
	    ep->session = aep->session ;
	    ep->date = aep->ctime ;
	    ep->sid = aep->sid ;
	} /* end if (utmpacc_entsid) */
	return rs ;
}
/* end subroutine (utmpent_load) */


