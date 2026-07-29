/* progstat_host SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a hostname */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 2001-03-01, David A­D­ Morano
	The subroutine was adapted from other programs that do
	similar things.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pingstat_host

	Description:
	This subroutine processes one hostname at a time.  It should
	be called repeatedly, once for each host.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<pingstatdb.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LONGTIME
#define	LONGTIME	(5 * 60)
#endif

#define	RF_NUMDIGITS	8		/* count field width */
#define	RF_LOGZLEN	18		/* time log field width */
#define	RF_UPSTAT	1		/* up status field width */
#define	RF_LEAD0	(RF_NUMDIGITS + 2*RF_LOGZLEN)
#define	RF_LEAD1	(RF_UPSTAT + 4)
#define	RF_LEAD		(RF_LEAD0 + RF_LEAD1)
#define	RF_BUFLEN	(RF_LEAD + MAXHOSTNAMELEN + 3)

#ifndef	PI
#define	PI	proginfo
#endif
#define	PSD	pingstatdb
#define	PSD_ENT	pingstatdb_ent


/* external subroutines */

extern int	inetping(cchar *,int) noex ;



/* external variables */


/* local structures */


/* local forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int proghost(PI *pip,cc *name,int mpint,int to) noex {
	PSD		*psp = &pip->ps ;
	cnothrow	nt{} ;
	time_t	lastcheck ;
	int	rs = SR_OK ;
	int	rs1 ;
	int	rs_match ;
	int	c_up = -1 ;
	int	f_update ;
	int	f_ourself ;
	char	timebuf[TIMEBUFLEN + 1] ;
	if (name == nullptr) return SR_FAULT ;
	if (name[0] == '\0') return SR_INVALID ;

/* OK, continue */

	f_update = pip->fl.update ;
	pip->daytime = time(nullptr) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("proghost: host=%s to=%d f_update=%d daytime=%s\n",
	        name,to,
	        f_update,
	        timestr_log(pip->daytime,timebuf)) ;
	    debugprintf("proghost: mpint=%d\n",mpint) ;
	}
#endif

	rs = SR_NOMEM ;
	if (PSD_ENT *pep = new(nt) PSD_ENT ; pep) {
	    if (char *ebuf ; (rs = mem.hostname(&ebuf)) >= 0) {
		cint elen = rs ;
		rs_match = pingstatdb_match(psp,name,pep,ebuf,elen) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	debugprintf("proghost: host=%s\n",name) ;
	    debugprintf("proghost: rs_match=%d\n", rs_match) ;
	}
#endif

	c_up = ((rs_match >= 0) && pe.f_up) ? 0 : -1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("proghost: c_up=%d f_update=%u\n",c_up,f_update) ;
#endif

	f_ourself = false ;
	if (f_update) {

/* are we trying to PING ourselves! */

	    rs = vecstr_find(&pip->localnames,name) ;
	    f_ourself = (rs >= 0) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("proghost: f_ourself=%u\n", f_ourself) ;
#endif

	} /* end if */

	if (f_update && (rs_match >= 0)) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("proghost: turn off f_update? \n") ;
#endif

	    if ((! f_ourself) || (! pe.f_up)) {
		lastcheck = pe.ti_ping ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("proghost: lastcheck=%s\n",
	                timestr_log(lastcheck,timebuf)) ;
#endif

	        if ((pip->daytime - lastcheck) < mpint)
	            f_update = false ;

	    } else {
	        c_up = 0 ;
	        f_update = false ;
	    } /* end if */

	} /* end if (deciding if an update was needed) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("proghost: f_update=%d\n",f_update) ;
#endif

	if (! f_update) {
	    rs = rs_match ;
	    if ((rs_match >= 0) && (! pe.f_up)) {
	        rs = SR_HOSTDOWN ;
	    }
	} /* end if */

	rs1 = SR_NOANODE ;
	if (f_update) {
	    int	f_up = true ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("proghost: updating host=%s\n",name) ;
#endif

	    pingstatdb_check(&pip->ps,pip->daytime) ;

	    if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: updating host=%s\n",
	            pip->progname,name) ;

	    rs = SR_OK ;
	    if (! f_ourself) {

	        if (to < 0)
	            to = LONGTIME ;

	        rs = inetping(name,to) ;
	        f_up = (rs >= 0) ;
	        c_up = (rs >= 0) ? 1 : -1 ;
		pip->daytime = time(nullptr) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("proghost: inetping rs=%d\n",rs) ;
#endif

	    if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: ping=%d\n",
	            pip->progname,rs) ;

	    } /* end if (not ourself) */

	    if ((rs != SR_HOSTDOWN) && (rs < 0)) {

	        rs1 = SR_NOTSUP ;
		if (pip->open.logprog) {
		    cchar	*fmt = "inetping host=%s (%d)\n" ;
	            logfile_printf(&pip->lh,fmt,name,rs) ;
		}

	    } else {
	        int	f_state0, f_state1 ;
	        int	f_present ;

/* prior state if any */

	        f_present = false ;
	        if (rs_match >= 0) {
	            f_present = true ;
	            f_state0 = pe.f_up ;
	        }

/* new state */

	        f_state1 = f_up ;

/* update the DB with the new information */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("proghost: doing the update \n") ;
#endif

	        rs1 = pingstatdb_update(psp,name,f_state1,pip->daytime) ;

/* record any changes */

	        if (f_present && (! LEQUIV(f_state0,f_state1))) {

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("proghost: host state change\n") ;
#endif

	            pip->daytime = time(nullptr) ;

		    if (pip->open.logprog) {
	                logfile_printf(&pip->lh,"%s %s %s\n",
	                    timestr_logz(pip->daytime,timebuf),
	                    ((f_state1) ? "U" : "D"),
	                    name) ;
		    }

/* should we make an entry in the summary file? */

	            if (pip->sumfp != nullptr) {
	                rs = bprintf(pip->sumfp,"%s %s %s\n",
	                    timestr_logz(pip->daytime,timebuf),
	                    ((f_state1) ? "U" : "D"), name) ;
	            } /* end if (summary file entry) */

	        } /* end if (any changes) */

	    } /* end if (doing the DB update) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("proghost: pingstatdb_update rs=%d\n",rs1) ;
#endif

	} /* end if (update) */

/* do we want to print a report on this one? */

	if ((! pip->fl.update) || (pip->verboselevel > 1)) {

	    if ((rs1 >= 0) && (rs_match < 0)) {
	        rs_match = pingstatdb_match(psp,name,pep,ebuf,elen) ;
	    }

	    if ((! pip->fl.nooutput) && (pip->verboselevel > 0)) {

	        if (rs_match >= 0) {

		    if (pip->verboselevel > 0)
			rs = bprintf(pip->ofp,"%c %s\n",
	                    ((pe.f_up) ? 'U' : 'D'),
	                    pe.hostname) ;

	            if ((rs >= 0) && (pip->verboselevel > 1)) {
			time_t	e = (pip->daytime - pe.ti_ping) ;
			cchar	*fmt ;
			char	pt[TIMEBUFLEN + 1] ;
			char	ct[TIMEBUFLEN + 1] ;
			char	et[TIMEBUFLEN + 1] ;

			fmt = "%*d %s %s\n",
			et[0] = '\0' ;
			if (pip->verboselevel >= 3) {
			    fmt = "%*d %s %s (%s)\n",
	                    timestr_elapsed(e,et) ;
			}

			rs = bprintf(pip->ofp,fmt,
	                        RF_NUMDIGITS,pe.count,
	                        timestr_log(pe.ti_change,ct),
	                        timestr_log(pe.ti_ping,pt),
	                        et) ;

	            } /* end if */
	        } else {
		    if (pip->verboselevel > 0) {
	                rs = bprintf(pip->ofp,"%c %s\n",'-', name) ;
		    }
	            if (pip->verboselevel > 1) {
	                rs = bprintf(pip->ofp, "*\n") ;
		    }
	        } /* end if */

	    } /* end if (output enabled) */

	} /* end if (printing report) */

		rs1 = mem.free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
		delete pep ;
	    } /* end if (new-PSD_ENT) */

	if ((rs >= 0) && (c_up < 0)) {
	    rs = SR_HOSTDOWN ;
	}

#if	CF_DEBUG
	if ((DEBUGLEVEL(2) && (rs < 0)) || DEBUGLEVEL(3)) {
	    debugprintf("proghost: ret rs=%d c_up=%u\n", rs,c_up) ;
	}
#endif

	return (rs >= 0) ? c_up : rs ;
} /* end subroutine (pingstat_host) */


