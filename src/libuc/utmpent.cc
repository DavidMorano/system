/* utmpent SUPPORT */
/* lang=C++20 */

/* get user information from UTMP database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name
	getutmpent

	Description:
	Get the user login name from the UTMP database.

	Synopsis:
	int getutmpent(utmpent *ep,pid_t sid) noex
	int getutmpname(char *rbuf,int rlen,pid_t sid) noex
	int getutmphost(char *rbuf,int rlen,pid_t sid) noex
	int getutmpline(char *rbuf,int rlen,pid_t sid) noex

	Arguments:
	ep		pointer to UTMPENT object
	rbuf		buffer to hold result
	rlen		length of user supplied buffer
	sid		session ID to lookup

	Returns:
	>=0		length of user buf
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* <- for |min(3c++)| */
#include	<usystem.h>
#include	<utmpacc.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"utmpent.h"


/* local defines */

#define	AEBUFLEN	sizeof(struct utmpx)
#define	VARUTMPLINE	"UTMPLINE"


/* imported namespaces */

using std::min ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int utmpent_utmpacc(utmpent *,pid_t) noex ;
static int utmpent_load(utmpent *,utmpacc_ent *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getutmpent(utmpent *ep,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    ep->id[0] = '\0' ;
	    ep->line[0] = '\0' ;
	    ep->user[0] = '\0' ;
	    ep->host[0] = '\0' ;
	    ep->session = 0 ;
	    ep->date = 0 ;
	    if (sid <= 0) sid = getsid(0) ;
	    ep->sid = sid ;
	    rs = utmpent_utmpacc(ep,sid) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpent) */

int getutmpname(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpent	e{} ;
	    if (rlen < 0) rlen = UTMPENT_LUSER ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) >= 0) {
	        rs = sncpy1(rbuf,rlen,e.user) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpname) */

int getutmphost(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpent	e{} ;
	    if (rlen < 0) rlen = UTMPENT_LHOST ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) >= 0) {
	        rs = sncpy1(rbuf,rlen,e.host) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmphost) */

int getutmpline(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    utmpent	e{} ;
	    if (rlen < 0) rlen = UTMPENT_LLINE ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) >= 0) {
	        rs = sncpy1(rbuf,rlen,e.line) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpline) */


/* local subroutines */

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
    } ;
}

constexpr ufinder_m	mems[] = {
	&ufinder::trysid,
	&ufinder::tryline
} ;

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
	static int	oursid = getsid(0) ;
	int		rs = SR_OK ;
	if (sid == oursid) {
	    static cchar	*line = getenv(VARUTMPLINE) ;
	    if (line) {
	        rs = utmpacc_entline(&ae,aebuf,aelen,line,-1) ;
	    }
	} /* end if (refering outseld) */
	return rs ;
}

static int utmpent_utmpacc(utmpent *ep,pid_t sid) noex {
	ufinder		fo(sid) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = fo.start()) >= 0) {
	    if ((rs = fo) > 0) {
	        rs = utmpent_load(ep,&fo.ae) ;
	        len = rs ;
	    }
	    rs1 = fo.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ufinder) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (utmpent_utmpacc) */

static int utmpent_load(utmpent *ep,utmpacc_ent *aep) noex {
	int		rs = SR_FAULT ;
	if (ep && aep) {
	    strwcpy(ep->id,aep->id,min(UTMPENT_LID,UTMPACC_LID)) ;
	    strwcpy(ep->user,aep->user,min(UTMPENT_LUSER,UTMPACC_LUSER)) ;
	    strwcpy(ep->line,aep->line,min(UTMPENT_LLINE,UTMPACC_LLINE)) ;
	    strwcpy(ep->host,aep->host,min(UTMPENT_LHOST,UTMPACC_LHOST)) ;
	    ep->session = aep->session ;
	    ep->date = aep->ctime ;
	    ep->sid = aep->sid ;
	} /* end if (utmpacc_entsid) */
	return rs ;
}
/* end subroutine (utmpent_load) */


