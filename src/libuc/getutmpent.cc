/* getutmpent SUPPORT */
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
	int getutmpent(GETUTMPENT *ep,pid_t sid) noex

	Description:
	Get the user login name from the UTMP database.

	Synopsis:
	int getutmpent(GETUTMPENT *ep,pid_t sid) noex
	int getutmpname(char *rbuf,int rlen,pid_t sid) noex
	int getutmphost(char *rbuf,int rlen,pid_t sid) noex
	int getutmpline(char *rbuf,int rlen,pid_t sid) noex

	Arguments:
	ep		pointer to GETUTMPENT object
	rbuf		buffer to hold result
	rlen		length of user supplied buffer
	sid		session ID to lookup

	Returns:
	>=0		length of user buf
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* <- for |min(3c++)| */
#include	<usystem.h>
#include	<utmpacc.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"getutmpent.h"


/* local defines */

#define	AEBUFLEN	sizeof(struct utmpx)


/* local namespaces */

using std::min ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int getutmpent_utmpacc(GETUTMPENT *,pid_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getutmpent(GETUTMPENT *ep,pid_t sid) noex {
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
	    rs = getutmpent_utmpacc(ep,sid) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpent) */

int getutmpname(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) {
	    GETUTMPENT	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LUSER ;
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
	    GETUTMPENT	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LHOST ;
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
	    GETUTMPENT	e{} ;
	    if (rlen < 0) rlen = GETUTMPENT_LLINE ;
	    rbuf[0] = '\0' ;
	    if ((rs = getutmpent(&e,sid)) >= 0) {
	        rs = sncpy1(rbuf,rlen,e.line) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getutmpline) */


/* local subroutines */

static int getutmpent_utmpacc(GETUTMPENT *ep,pid_t sid) noex {
	UTMPACC_ENT	ae{} ;
	cint		aelen = AEBUFLEN ;
	int		rs ;
	char		aebuf[AEBUFLEN+1] ;
	if ((rs = utmpacc_entsid(&ae,aebuf,aelen,sid)) >= 0) {
	    strwcpy(ep->id,ae.id,min(GETUTMPENT_LID,UTMPACC_LID)) ;
	    strwcpy(ep->user,ae.user,min(GETUTMPENT_LUSER,UTMPACC_LUSER)) ;
	    strwcpy(ep->line,ae.line,min(GETUTMPENT_LLINE,UTMPACC_LLINE)) ;
	    strwcpy(ep->host,ae.host,min(GETUTMPENT_LHOST,UTMPACC_LHOST)) ;
	    ep->session = ae.session ;
	    ep->date = ae.ctime ;
	    ep->sid = ae.sid ;
	} /* end if (utmpacc_entsid) */
	return rs ;
}
/* end subroutine (getutmpent_utmpacc) */


