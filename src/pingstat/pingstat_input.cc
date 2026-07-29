/* pingstat_input SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process messages on the input stream */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_DEBUGN	0		/* special */

/* revision history:

	= 2001-03-01, David A­D­ Morano
	The subroutine was adapted from other programs that do
	similar things.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pingstat_input

	Description:
	This subroutine processes messages that are present on the
	input stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<csignal>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<msgbuf.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<pingstatdb.h>
#include	<pingstatmsg.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	MSGBUFLEN	2048
#define	INBUFLEN	(2 * 1024)

#ifndef	LONGTIME
#define	LONGTIME	(5 * 60)
#endif

#ifndef	TO_READ
#define	TO_READ		1
#endif

#define	DEBFNAME	"/tmp/pingstat.deb"

#ifndef	PI
#define	PI	proginfo
#endif

#define	PSD		pingstatdb
#define	PSD_ENT		pingstatdb_ent
#define	PSD_CUR		pingstatdb_cur
#define	PSD_UP		pingstatdb_up


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	procstream(PI *,int) noex ;
local int	procdatagram(PI *,int) noex ;
local int	procupdate(PI *,pingstatmsg_update *) noex ;
local int	procuptime(PI *,pingstatmsg_uptime *) noex ;
local int	procentry(PI *,cchar *,PSD_UP *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proginput(PI *pip,int fd) noex {
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    DEBUGPRINTF("proginput: type update=%u\n",
	        pingstatmsgtype_update) ;
	    DEBUGPRINTF("proginput: type uptime=%u\n",
	        pingstatmsgtype_uptime) ;
	}
#endif /* CF_DEBUG */

	if (pip->fl.dgram) {
	    rs = procdatagram(pip,fd) ;
	} else {
	    rs = procstream(pip,fd) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("proginput: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (proginput) */


/* local subroutines */

int procstream(PI *pip,int fd) noex {
	pingstatmsg_update	m0 ;
	pingstatmsg_uptime	m1 ;
	pingstatmsg_unknown	mu ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		to = TO_READ ;
	int		ti_start = pip->daytime ;
	int		msgtype ;
	int		sz = 0 ;
	int		s ;
	int		n = 0 ;
	int		mlen ;
	int		loopcount = 0 ;

	if (pip->intmininput > 0) to = pip->intmininput ;

	if (pip->open.logprog && pip->fl.logextra) {
	    logfile_printf(&pip->lh,"read to=%u",to) ;
	}

/* find maximum message size (or machine pagesize) */

	s = pip->pagesize ;
	if (s > sz) sz = s ;
	s = sizeof(struct pingstatmsg_uptime) ;
	if (s > sz) sz = s ;
	s = sizeof(struct pingstatmsg_update) ;
	if (s > sz) sz = s ;

/* initialize */

	if (msgbuf mb ; (rs = msgbuf_start(&mb,fd,sz,to)) >= 0) {
	    int		ml = 0 ;
	    cchar	*mp ;

	    while (rs >= 0) {
		char	*bp ;
	        rs = msgbuf_read(&mb,&mp) ;
	        ml = rs ;

	        if (pip->open.logprog && pip->fl.logextra) {
	            logfile_printf(&pip->lh,"read %d",rs) ;
		}

	        if (rs <= 0) break ;

		bp = (char *) mp ;
	        msgtype = MKCHAR(mp[0]) ;
	        pip->daytime = time(nullptr) ;

	        switch (msgtype) {
	        case pingstatmsgtype_update:
	            mlen = pingstatmsg_update(&m0,1,bp,ml) ;
	            if (mlen > 0) {
	                n += 1 ;
	                rs = procupdate(pip,&m0) ;
	            }
	            break ;
	        case pingstatmsgtype_uptime:
	            mlen = pingstatmsg_uptime(&m1,1,bp,ml) ;
	            if (mlen > 0) {
	                n += 1 ;
	                rs = procuptime(pip,&m1) ;
	            }
	            break ;

	        default:
	            mlen = pingstatmsg_unknown(&mu,1,bp,ml) ;
	            if (mlen > 0)
	                mlen = mu.msglen ;
	            break ;

	        } /* end switch */

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            DEBUGPRINTF("proginput/procstream: "
	                "switch-after rs=%d mlen=%d\n",
	                rs,mlen) ;
#endif

	        if ((rs >= 0) && (pip->intrun >= 0) && (ml == mlen)) {
	            if ((pip->daytime - ti_start) >= pip->intrun) {
	                if (pip->open.logprog) {
	                    logfile_printf(&pip->lh,"runint to") ;
			}
	                break ;
	            }
	        }

	        if ((rs >= 0) && (mlen > 0)) {
	            rs = pingstatdb_check(&pip->ps,pip->daytime) ;
		}

	        if (pip->open.logprog && (mlen >= 0)) {
	            logfile_flush(&pip->lh) ;
		}

	        if (rs >= 0) {
	            rs = msgbuf_adv(&mb,mlen) ;
	            if (pip->open.logprog && pip->fl.logextra) {
	                logfile_printf(&pip->lh,"adv %d\n",rs) ;
		    }
	        }

	        loopcount += 1 ;
	        if (rs < 0) break ;
	    } /* end while (processing messages) */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        DEBUGPRINTF("proginput/procstream: while-after\n") ;
#endif

	    rs1 = msgbuf_finish(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgbuf) */

	if (pip->open.logprog && pip->fl.logextra) {
	    logfile_printf(&pip->lh,"while-out %d",rs) ;
	}

	if (rs == SR_TIMEDOUT)
	    rs = SR_OK ;

	if (rs >= 0) {
	    if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: updates=%u\n",pip->progname,n) ;
	    if (pip->open.logprog)
	        logfile_printf(&pip->lh,"updates=%u",n) ;
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("proginput/procstream: ret rs=%d n=%d\n",rs,n) ;
#endif

	return (rs >= 0) ? n : rs ;
} /* end subroutine (procstream) */

int procdatagram(PI *pip,int fd) noex {
	struct pingstatmsg_update	m0 ;
	struct pingstatmsg_uptime	m1 ;
	struct pingstatmsg_unknown	mu ;
	int		ti_start = pip->daytime ;
	int		ti_read = pip->daytime ;
	cint		to = TO_READ ;
	int		rs = SR_OK ;
	int		sz = 0 ;
	int		msgtype ;
	int		bl = 0 ;
	int		mflags = 0 ;
	int		mopts = 0 ;
	int		loopcount = 0 ;
	int		s ;
	int		mlen ;
	int		n = 0 ;
	char		msgbuf[MSGBUFLEN + 1] ;
	char		*bp ;

#ifdef	COMMENT
	if (pip->intmininput > 0)
	    to = pip->intmininput ;
#endif

	if (pip->open.logprog && pip->fl.logextra)
	    logfile_printf(&pip->lh,"read to=%u",to) ;

/* find maximum message size (or machine pagesize) */

	s = pip->pagesize ;
	if (s > sz) sz = s ;
	s = sizeof(struct pingstatmsg_uptime) ;
	if (s > sz) sz = s ;
	s = sizeof(struct pingstatmsg_update) ;
	if (s > sz) sz = s ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("proginput/procdatagram: while-before\n") ;
#endif

	while (rs >= 0) {

	    while (rs >= 0) {

	        bp = msgbuf ;
	        rs = uc_recve(fd,msgbuf,MSGBUFLEN,mflags,to,mopts) ;
	        bl = rs ;

	        pip->daytime = time(nullptr) ;

	        if (pip->open.logprog && pip->fl.logextra) {
	            logfile_printf(&pip->lh,"recve %d",rs) ;
		}

#if	CF_DEBUGN && defined(DEBFNAME)
	        {
	            pid_t	pid = getpid() ;
	            time_t	daytime = time(nullptr) ;
	            char	timebuf[TIMEBUFLEN + 1] ;
	            nprintf(DEBFNAME,"%s pid=%u recve rs=%d\n",
	                timestr_log(daytime,timebuf),pid,rs) ;
	        }
#endif /* CF_DEBUGN */

	        if (rs <= 0) break ;

	        ti_read = pip->daytime ;
	        msgtype = (bp[0] & 0xff) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            DEBUGPRINTF("proginput/procdatagram: "
	                "uc_recve() bl=%u mt=%u\n",
	                bl,msgtype) ;
#endif

	        while (bl > 0) {

	            switch (msgtype) {

	            case pingstatmsgtype_update:
	                mlen = pingstatmsg_update(&m0,1,bp,bl) ;
	                if (mlen > 0) {
	                    n += 1 ;
	                    rs = procupdate(pip,&m0) ;
	                }
	                break ;

	            case pingstatmsgtype_uptime:
	                mlen = pingstatmsg_uptime(&m1,1,bp,bl) ;
	                if (mlen > 0) {
	                    n += 1 ;
	                    rs = procuptime(pip,&m1) ;
	                }
	                break ;

	            default:
	                mlen = pingstatmsg_unknown(&mu,1,bp,bl) ;
	                if (mlen > 0)
	                    mlen = mu.msglen ;
	                break ;

	            } /* end switch */

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                DEBUGPRINTF("proginput/procdatagram: "
	                    "switch-after rs=%d mlen=%d\n",
	                    rs,mlen) ;
#endif

	            if ((rs < 0) || (mlen < 0)) break ;

	            bp += mlen ;
	            bl -= mlen ;

	        } /* end while */

	        if ((rs >= 0) && (pip->intrun >= 0)) {
	            if ((pip->daytime - ti_start) >= pip->intrun) {
	                if (pip->open.logprog && pip->fl.logextra) {
	                    logfile_printf(&pip->lh,"runint to") ;
			}
	                break ;
	            }
	        }

	        if (pip->open.logprog) {
	            logfile_flush(&pip->lh) ;
		}

	        loopcount += 1 ;
	        if (rs < 0) break ;
	    } /* end while (processing messages) */

	    if (rs >= 0) {
	        rs = pingstatdb_check(&pip->ps,pip->daytime) ;
	    }

#if	CF_DEBUGN && defined(DEBFNAME)
	    nprintf(DEBFNAME,"proginput/procdategram: "
	        "pingstadb_check() rs=%d\n",rs) ;
#endif

	    if ((pip->daytime - ti_read) >= pip->intmininput)
	        break ;

	    if ((rs >= 0) && (pip->intrun >= 0)) {
	        if ((pip->daytime - ti_start) >= pip->intrun) break ;
	    }

	} /* end while */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("proginput/procdatagram: while-after\n") ;
#endif

	if (pip->open.logprog && pip->fl.logextra) {
	    logfile_printf(&pip->lh,"while-out %d",rs) ;
	}

	if (rs == SR_TIMEDOUT)
	    rs = SR_OK ;

	if (rs >= 0) {
	    if (pip->debuglevel > 0) {
	        bprintf(pip->efp,"%s: updates=%u\n",pip->progname,n) ;
	    }
	    if (pip->open.logprog) {
	        logfile_printf(&pip->lh,"updates=%u",n) ;
	    }
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    DEBUGPRINTF("proginput/procdatagram: ret rs=%d n=%d\n",rs,n) ;
#endif

	return (rs >= 0) ? n : rs ;
} /* end subroutine (procdatagram) */

local int procupdate(PI *pip,pingstatmsg_update *mp) noex {
	int		rs = SR_OK ;

	if (mp->hostname[0] != '\0') {
	    PSD_UP	u{} ;
	    u.count = UINT_MAX ;
	    u.timestamp = mp->timestamp ;
	    u.timechange = pip->daytime ;
	    rs = procentry(pip,mp->hostname,&u) ;
	} else {
	    rs = SR_BADFMT ;
	}

	return rs ;
} /* end subroutine (procupdate) */

local int procuptime(PI *pip,pingstatmsg_uptime *mp) noex {
	int		rs = SR_OK ;

	if (mp->hostname[0] != '\0') {
	    PSD_UP	u{} ;
	    u.count = mp->count ;
	    u.timestamp = mp->timestamp ;
	    u.timechange = mp->timechange ;
	    rs = procentry(pip,mp->hostname,&u) ;
	} else {
	    rs = SR_BADFMT ;
	}

	return rs ;
} /* end subroutine (procuptime) */

local int procentry(PI *pip,cchar *hostname,PSD_UP *up) noex {
	PSD		*psp = &pip->ps ;
	time_t		timestamp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs_match ;
	int		f_update = true ;
	int		f_up = false ;
	char		timebuf[TIMEBUFLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    DEBUGPRINTF("proginput/procentry: hostname=%s\n",hostname) ;
#endif

	if (hostname == nullptr) return SR_FAULT ;
	if (hostname[0] == '\0') return SR_INVALID ;

/* get the time stamp that we want to use */

	timestamp = (time_t) up->timestamp ;

	if ((timestamp == 0) || (timestamp > pip->daytime)) {
	    timestamp = pip->daytime ;
	}

/* see if this host is already in the database */

	rs = SR_NOMEM ;
	if (PSD_ENT *pep = new(nt) PSD_ENT ; pep) {
	    if (char *ebuf ; (rs = mem.hostname(&ebuf)) >= 0) {
	        if ((rs = pingstatdb_match(psp,hostname,pep,ebuf,elen)) >= 0) {
		    rs_match = rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    DEBUGPRINTF("proginput/procentry: rs_match=%d\n",rs_match) ;
#endif

#if	CF_DEBUGN && defined(DEBFNAME)
	nprintf(DEBFNAME,"proginput/procentry: "
	    "pingstadb_match() hn=>%s< rs=%d\n",
	    hostname,rs_match) ;
#endif

	if ((rs_match >= 0) && pe.f_up) {
	    if ((pip->daytime - pe.ti_ping) < pip->intminupdate) {
	        f_update = false ;
	    }
	} /* end if (deciding if an update was needed) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    DEBUGPRINTF("proginput/procentry: f_update=%d\n",f_update) ;
#endif

	if (f_update) {
	    int		f_state0, f_state1 ;
	    int		f_present ;
	    cchar	*s ;

	    if (pip->debuglevel > 0) {
	        bprintf(pip->efp,"%s: updating host=%s\n",
	            pip->progname,hostname) ;
	    }

	    f_state0 = false ;
	    f_up = true ;		/* always UP (for now) */

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
	        DEBUGPRINTF("proginput/procentry: doing the update \n") ;
#endif

	    if (up->count != UINT_MAX) {

	        s = "uptime" ;
	        up->timestamp = timestamp ;
	        rs = pingstatdb_uptime(psp,hostname,up) ;

#if	CF_DEBUGN && defined(DEBFNAME)
	        nprintf(DEBFNAME,"proginput/procentry: "
	            "pingstadb_uptime() rs=%d\n",rs) ;
#endif

	    } else {

	        s = "update" ;
	        rs = pingstatdb_update(psp,hostname,f_state1,timestamp) ;

#if	CF_DEBUGN && defined(DEBFNAME)
	        nprintf(DEBFNAME,"proginput/procentry: "
	            "pingstadb_update() rs=%d\n",rs) ;
#endif

	    } /* end if */

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        DEBUGPRINTF("proginput/procentry: pingstatdb_%s() rs=%d\n",
	            s,rs) ;
#endif

/* record any changes */

	    if ((rs >= 0) && (! LEQUIV(f_state0,f_state1))) {
	        cchar	*fmt ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            DEBUGPRINTF("proginput/procentry: host state change\n") ;
#endif

	        if (pip->daytime == 0)
	            pip->daytime = time(nullptr) ;

	        fmt = (f_present) ? "%s %s %s\n" : "%s %s %s (new)\n" ;
	        logfile_printf(&pip->lh,fmt,
	            timestr_logz(timestamp,timebuf),
	            ((f_state1) ? "U" : "D"),
	            hostname) ;

/* should we make an entry in the summary file? */

	        if (pip->sumfp != nullptr) {

	            bprintf(pip->sumfp,fmt,
	                timestr_logz(timestamp,timebuf),
	                ((f_state1) ? "U" : "D"),
	                hostname) ;

	        } /* end if (summary file entry) */

	    } /* end if (any changes) */

	    if (pip->debuglevel > 0) {
		cchar	*pn = pip->progname ;
		bprintf(pip->efp,"%s: mode=%s\n",pn,s) ;
	    }

	} /* end if (update) */

		} /* end if (pingstatdb_match) */
		rs1 = mem.free(ebuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    delete pep ;
	} /* end if (new-PSD_ENT) */
	return rs ;
} /* end subroutine (procentry) */


