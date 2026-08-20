/* discard_proc SUPPORT */

/* process a hostname */

#define	CF_DEBUGS	0
#define	CF_DEBUG	1

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapted from other programs that
	do similar things.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	discard_process

	Description:
	This subroutine processes one hostname at a time.  It should
	be called repeatedly, once for each host.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<varsub.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<pingstatdb.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
#endif

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

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif

#ifndef	PI
#define	PI	proginfo
#endif
#define	VS	varsub
#define	PSD	pingstatdb
#define	PSD_ENT	pingstatdb_ent


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern int	inetping(cchar *,int) noex ;


/* external variables */


/* local structures */


/* local forward references */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int process(PI *pip,VS *dp,VS *ep,PSD *psp,cc *name,int mpint,int to) noex {
    	cnothrow	nt{} ;
	custime		dt = getustime ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("process: name=\"%s\" to=%d f_update=%d dt=%s\n",
	        name,to,
	        f_update,
	        timestr_log(dt,timebuf)) ;
	    debugprintf("process: mpint=%d\n",mpint) ;
	}
#endif

	rs = SR_NOMEM ;
	if (PSD_ENT *pep = new(nt) PSD_ENT ; pep) {
	    if (char *ebuf ; (rs = mem.hostname(&ebuf)) >= 0) {
		cint elen = rs ;
	rs_match = pingstatdb_match(psp,name,pep,ebuf,elen) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: rs_match=%d\n", rs_match) ;
#endif

	c_up = ((rs_match >= 0) && pep->f_up) ? 0 : -1 ;
	f_ourself = false ;
	if (f_update) {

/* are we trying to PING ourselves ! */

	    rs = vecstr_find(&pip->localnames,name) ;

	    f_ourself = (rs >= 0) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("process: f_ourself=%d\n", f_ourself) ;
#endif

	}

	if (f_update && (rs_match >= 0)) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("process: turn off f_update ? \n") ;
#endif

	    if ((! f_ourself) || (! pep->f_up)) {

	        dater_gettime(&pep->pdate,&lastcheck) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("process: lastcheck=%s\n",
	                timestr_log(lastcheck,timebuf)) ;
#endif

	        if ((dt - lastcheck) < mpint) {
	            f_update = false ;
		}

	    } else {

		c_up = 0 ;
	        f_update = false ;

	    }

	} /* end if (deciding if an update was needed) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: f_update=%d\n",f_update) ;
#endif

	if (! f_update) {
	    rs = rs_match ;
	    if ((rs_match >= 0) && (! pep->f_up)) {
		rs = SR_HOSTDOWN ;
	    }
	}
	rs1 = SR_NOANODE ;
	if (f_update) {
	    int	f_up ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: updating host=%s\n",name) ;
#endif

	    if (pip->debuglevel > 0) {
	        bprintf(pip->efp,"%s: updating host=%s\n",
	            pip->progname,name) ;
	    }

	    rs = SR_OK ;
	    f_up = true ;
	    if (! f_ourself) {
	        if (to < 0) {
	            to = LONGTIME ;
		}
	        rs = inetping(name,to) ;
	        f_up = (rs >= 0) ;
		c_up = (rs >= 0) ? 1 : -1 ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(2))
	            debugprintf("process: inetping rs=%d\n",rs) ;
#endif

	    }

	    if ((rs != SR_HOSTDOWN) && (rs < 0)) {

	        rs1 = SR_NOTSUP ;
	        logfile_printf(&pip->lh,"inetping host=%s rs=%d\n",
	            name,rs) ;

	    } else {

	        int	f_state0, f_state1 ;
	        int	f_present ;


/* prior state if any */

	        f_present = false ;
	        if (rs_match >= 0) {
	            f_present = true ;
	            f_state0 = pep->f_up ;
	        }

/* new state */

	        f_state1 = f_up ;

/* update the DB with the new information */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("process: doing the update \n") ;
#endif

	        rs1 = pingstatdb_update(psp,name,f_state1,dt) ;

/* record any changes */

	        if (f_present && (! LEQUIV(f_state0,f_state1))) {
		    dt = getustime ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("process: host state change\n") ;
#endif

	            logfile_printf(&pip->lh,"%s %s %s\n",
	                timestr_logz(dtaytime,timebuf),
	                ((f_state1) ? "U" : "D"),
	                name) ;

/* should we make an entry in the summary file ? */

	            if (pip->sumfp != nullptr) {
	                bprintf(pip->sumfp,"%s %s %s\n",
	                    timestr_logz(dt,timebuf),
	                    ((f_state1) ? "U" : "D"), name) ;
	            } /* end if (summary file entry) */

	        } /* end if (any changes) */

	    } /* end if (doing the DB update) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("process: pingstatdb_update rs=%d\n",rs1) ;
#endif

	} /* end if (update) */

/* do we want to print a report on this one ? */

	if ((! pip->fl.update) || (pip->verboselevel > 0)) {
	    char	cdate[RF_LOGZLEN + 2], pdate[RF_LOGZLEN + 2] ;
	    if ((rs1 >= 0) && (rs_match < 0)) {
	        rs_match = pingstatdb_match(psp,name,&pep) ;
	    }
	    if (! pip->fl.nooutput) {

	    if (rs_match >= 0) {
	            bprintf(pip->ofp,"%c %s\n",
	                ((pep->f_up) ? 'U' : 'D'),
	                pep->hostname) ;

	        if (pip->verboselevel > 0) {
	            dater_mklogz(&pep->cdate,cdate,RF_LOGZLEN + 1) ;
	            dater_mklogz(&pep->pdate,pdate,RF_LOGZLEN + 1) ;
		    if (pip->verboselevel > 1) {
			time_t	ptime ;
			dater_gettime(&pep->pdate,&ptime) ;
	                bprintf(pip->ofp,"%*d %s %s (%s)\n",
	                    RF_NUMDIGITS,pep->count,
	                    cdate,pdate,
	                    timestr_elapsed((dt - ptime),timebuf)) ;
		    } else {
	                bprintf(pip->ofp,"%*d %s %s\n",
	                    RF_NUMDIGITS,pep->count,
	                    cdate,pdate) ;
		    }
	        } /* end if (verbose-level) */

	    } else {
	        bprintf(pip->ofp,"%c %s\n", '-', name) ;
	        if (pip->verboselevel > 0) bprintf(pip->ofp, "*\n") ;
	    }

	    } /* end if (output enabled) */
	} /* end if (printing report) */

	if ((rs >= 0) && (c_up < 0)) {
		rs = SR_HOSTDOWN ;
	}

		rs = mem.free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    delete pep ;
	} /* end if (new-PDS_ENT) */

#if	CF_DEBUG
	if ((DEBUGLEVEL(2) && (rs < 0)) || DEBUGLEVEL(3))
	    debugprintf("process: exiting rs=%d c_up=%d\n", rs,c_up) ;
#endif

	return (rs >= 0) ? c_up : rs ;
} /* end subroutine (process) */


