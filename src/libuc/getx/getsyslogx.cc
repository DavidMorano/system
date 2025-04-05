/* getsyslogx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a sys-log (SYSLOG) facility or priority */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-10-14, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsyslogx

	Description:
	These subroutines allow for lookup of either a SYSLOG facilty
	name or a facility number (returning a name-string).  The
	database is not dynamic, but so far it does not have to be.

	Synopsis:
	int getsyslogfac(cchar *sp,int sl) noex
	int getsyslogpri(cchar *sp,int sl) noex

	Arguments:
	sp		name of the SYSLOG facility name lookup
	sl		length of name

	Returns:
	>=0		resulting SYSLOG facility number
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/syslog.h>		/* for LOG_XXX numbers */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<nleadstr.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"getsyslogx.h"


/* local defines */

#ifndef	FACNAMELEN
#define	FACNAMELEN	12
#endif

#ifndef	PRINAMELEN
#define	PRINAMELEN	12		/* to handle 'information' */
#endif


/* external subroutines */


/* external variables */


/* local structures */

struct logitem {
	cchar		*name ;
	int		val ;
} ;


/* local variables */

constexpr logitem	logfacs[] = {
	{ "kernel",	LOG_KERN },
	{ "user",	LOG_USER },
	{ "mail",	LOG_MAIL },
	{ "daemon",	LOG_DAEMON },
	{ "auth",	LOG_AUTH },
	{ "syslog",	LOG_SYSLOG },
	{ "lpr",	LOG_LPR },
	{ "news",	LOG_NEWS },
	{ "uucp",	LOG_UUCP },
	{ "cron",	LOG_CRON },
	{ "local0",	LOG_LOCAL0 },
	{ "local1",	LOG_LOCAL1 },
	{ "local2",	LOG_LOCAL2 },
	{ "local3",	LOG_LOCAL3 },
	{ "local4",	LOG_LOCAL4 },
	{ "local5",	LOG_LOCAL5 },
	{ "local6",	LOG_LOCAL6 },
	{ "local7",	LOG_LOCAL7 },
	{ nullptr, 0 }
} ;

constexpr logitem	logpris[] = {
	{ "emergency",	LOG_EMERG },
	{ "alert",	LOG_ALERT },
	{ "critical",	LOG_CRIT },
	{ "error",	LOG_ERR },
	{ "warning",	LOG_WARNING },
	{ "notice",	LOG_NOTICE },
	{ "information", LOG_INFO },
	{ "debug",	LOG_DEBUG },
	{ nullptr, 0 }
} ;


/* exported variables */


/* exported subroutines */

int getsyslogfac(cchar *sp,int sl) noex {
	const logitem	*lfs = logfacs ;
	cint		n = 2 ;
	int		rs = SR_OK ;
	int		val = 0 ;
	char		nbuf[FACNAMELEN + 1] ;
	if ((sl < 0) || (sl > FACNAMELEN)) {
	    sl = FACNAMELEN ;
	}
	if (int nlen ; (nlen = intconv(strwcpylc(nbuf,sp,sl) - nbuf)) > 0) {
	    int		i ; /* used-afterwards */
	    int		m ;
	    bool	f = false ;
	    rs = SR_NOTFOUND ;
	    for (i = 0 ; lfs[i].name != nullptr ; i += 1) {
	        cchar	*anp = lfs[i].name ;
	        m = nleadstr(anp,nbuf,nlen) ;
	        f = ((m == nlen) && ((m >= n) || (anp[m] == '\0'))) ;
	        if (f) break ;
	    } /* end for */
	    if (f) {
	        val = lfs[i].val ;
		rs = SR_OK ;
	    }
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? val : rs ;
}
/* end subroutine (getsyslogfac) */

int getsyslogpri(cchar *sp,int sl) noex {
	const logitem	*lfs = logpris ;
	cint		n = 2 ;
	int		rs = SR_OK ;
	int		val = LOG_INFO ;
	char		nbuf[PRINAMELEN + 1] ;
	if ((sl < 0) || (sl > PRINAMELEN)) {
	    sl = PRINAMELEN ;
	}
	if (int nlen ; (nlen = intconv(strwcpylc(nbuf,sp,sl) - nbuf)) > 0) {
	    cint	ch = mkchar(nbuf[0]) ;
	    if (isdigitlatin(ch)) {
	        if ((rs = cfdeci(nbuf,nlen,&val)) >= 0) {
		    if ((val < 0) || (val > 7)) rs = SR_DOM ;
	        }
	    } else if (isalphalatin(ch)) {
	        int		i ; /* used-afterwards */
	        int		m ;
	        bool		f = false ;
	        for (i = 0 ; lfs[i].name != nullptr ; i += 1) {
	            cchar	*anp = lfs[i].name ;
	            m = nleadstr(anp,nbuf,nlen) ;
	            f = ((m == nlen) && ((m >= n) || (anp[m] == '\0'))) ;
	            if (f) break ;
	        } /* end for */
	        val = lfs[i].val ;
	        if (! f) rs = SR_NOTFOUND ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? val : rs ;
}
/* end subroutine (getlogpri) */

const char *strfacname(int fac) noex {
	const logitem	*lfs = logfacs ;
	cchar		*res = nullptr ;
	if (fac >= 0) {
	    int		i ; /* used-afterwards */
	    bool	f = false ;
	    for (i = 0 ; lfs[i].name != nullptr ; i += 1) {
	        f = (lfs[i].val == fac) ;
	        if (f) break ;
	    } /* end for */
	    if (f) res = lfs[i].name ;
	} /* end if (valid) */
	return res ;
}
/* end subroutine (strfacname) */


