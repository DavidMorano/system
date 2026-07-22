/* pingstatdb_uptime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a PINGSTATDB file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_CREATE	0		/* always create the file? */
#define	CF_UNLOCK	1		/* always unlock after an operation */

/* revision history:

	= 1998-04-13, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pingstatdb

  	Description:
	This subroutine maintains a PINGSTATDB file. These files
	are used to maintain the names and status of a PING event.

	Synopsis:

	Arguments:
	op		ping status_file pointer
	fname		filename

	Returns:
	<0		error
	==0		record written
	==1		record created

	Format of file records:

	- eight digits of a decimal count number field
	- the time of the last status change
	- the time of the last entry update
	- the UP/DOWN status
	- the hostname

	Format example:

       3 000505_1018:55_EDT 000505_2233:28_EDT U rcb
       5 000505_1018:55_EDT 000505_2234:43_EDT U *LAST_UPDATE*
       1 000505_2234:37_EDT 000505_2234:37_EDT U bars
       1 000505_2234:40_EDT 000505_2234:40_EDT U amps
       1 000505_2234:43_EDT 000505_2234:43_EDT U farads
       1 000505_2234:46_EDT 000505_2234:46_EDT U ergs

	Oh, and finally, since file record locking sucks, that is: it is
	very often BROKEN.  On the stupid Sun platforms, we attempt to
	lock to be a nice guy but we have relatively small timeouts
	after which we proceed anyway!  This strategy is applied to
	all record lockes used throughout this subroutine.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<netdb.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecitem.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import pingstatdb_util ;
import pingstatdb_rec ;

/* local defines */

#define	PSD		pingstatdb
#define	PSD_FL		pingstatdb_fl
#define	PSD_ENT		pingstatdb_ent
#define	PSD_CUR		pingstatdb_cur
#define	PSD_UP		pingstatdb_up
#define	PSD_REC		pingstatdb_rec

/* record-format (RF) paramters */
#define	RF_NUMDIGITS	8		/* RF count field width */
#define	RF_LOGZLEN	23		/* RF time log field width */
#define	RF_UPSTAT	1		/* RF up status field width */
#define	RF_LEAD0	(RF_NUMDIGITS + (2*RF_LOGZLEN))
#define	RF_LEAD1	(RF_UPSTAT + 4)
#define	RF_LEAD		(RF_LEAD0 + RF_LEAD1)

#define	BUFLEN		(RF_LEAD + MAXHOSTNAMELEN + 3)

#define	TO_LOCK		30		/* seconds */
#define	TO_MINUPDATE	3		/* minimum time between updates */

#define	LASTUPDATE	"*LAST_UPDATE*"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif
#ifndef	CF_CREATE
#define	CF_CREATE	0		/* always create the file? */
#endif
#ifndef	CF_UNLOCK
#define	CF_UNLOCK	1		/* always unlock after an operation */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	pingstatdb_checkcache(PSD *) noex ;
local int	pingstatdb_readrecords(PSD *) noex ;
local int	pingstatdb_fes(PSD *) noex ;
local int	pingstatdb_getrec(PSD *,cchar *,PSD_REC **) noex ;
local int	pingstatdb_updrec(PSD *,time_t,dater *,cchar *,
			int,time_t) noex ;

local int	record_start(PSD_REC *,TIMEB *,cchar *,
			uint,cchar *,dater *) noex ;
local int	record_startbuf(PSD_REC *,TIMEB *,cchar *,
			uint,cchar *,int) noex ;
local int	record_update(PSD_REC *,bfile *,dater *,int) noex ;
local int	record_write(PSD_REC *,bfile *,
			dater *,dater *,int,int) noex ;
local int	record_finish(PSD_REC *) noex ;

local int	entry_load(PSD_ENT *,PSD_REC *) noex ;

local int	mkbstr(mode_t,char *) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_create	= CF_CREATE ;
cbool		f_unlock	= CF_UNLOCK ;

#if	CF_DEBUG
static cchar	hostname[] = "EMPTY" ;
#endif


/* exported variables */


/* exported subroutines */

int pingstatdb_uptime(PSD *op,cchar *hostname,PSD_UP *up) noex {
	PSD_REC		e, *rp ;
	dater		cd, ud, *cdp ;
	off_t		boff ;
	custime		dt = time(nullptr) ;
	time_t		ptime = 0 ;
	uint		timestamp ;
	uint		ctimeange ;
	uint		roff ;
	int		rs = SR_OK ;
	int		sz ;
	int		f_up = true ;
	int		f_changed = false ;

	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;
	if (up == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if (hostname[0] == '\0') return SR_INVALID ;

	if (! op->fl.writable) {
	    rs = SR_BADF ;	/* not open for writing */
	    goto ret0 ;
	}

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_uptime: lock stuff\n") ;
#endif

	if (op->fl.readlocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    bcontrol(op->pfp,BC_UNLOCK,0) ;
	}

	if (! op->fl.writelocked) {
	    rs = bcontrol(op->pfp,BC_LOCKWRITE,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	} /* end if (we did not already have a lock on the file) */

	if (rs < 0)
	    goto ret0 ;

	rs = pingstatdb_checkcache(op) ;
	if (rs < 0)
	    goto ret1 ;

	op->now.time = dt ;
	rs = dater_start(&ud,&op->now,op->zname,-1) ;
	if (rs < 0)
	    goto ret1 ;

	rs = dater_start(&cd,&op->now,op->zname,-1) ;
	if (rs < 0)
	    goto ret2 ;

	timestamp = up->timestamp ;
	if ((up->timestamp == 0) || (up->timestamp > dt)) {
	    timestamp = dt ;
	}

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN+1] ;
	    DEBUGPRINTF("pingstatedb_uptime: now=%s\n",
	        timestr_logz(op->now.time,timebuf)) ;
	    DEBUGPRINTF("pingstatedb_uptime: dater_start() rs=%d cur_date=%s\n",
	        rs,timestr_logz(ud.b.time,timebuf)) ;
	}
#endif

#if	CF_DEBUG
	{
	    DEBUGPRINTF("pingstatedb_uptime: about to match\n") ;
	    rs = vecitem_count(op->elp) ;
	    DEBUGPRINTF("pingstatedb_uptime: entries in cache %d\n",rs) ;
	}
#endif

	if ((rs = pingstatdb_getrec(op,hostname,&rp)) >= 0) {
	    int	f_greater = (! LEQUIV(f_up,rp->f_up)) ;

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: found match rs=%d\n",rs) ;
#endif

	    rs = dater_gettime(&rp->pdate,&ptime) ;

	    f_greater = ((rs >= 0) && (timestamp > ptime)) ;

/* the update (ping) time */

	    if (ptime > timestamp)
	        timestamp = ptime ;

	    dater_settimezn(&ud,timestamp,op->zname,-1) ;

/* the change time */

	    if (f_changed || (up->ctimeange != 0)) {

	        ctimeange = up->ctimeange ;
	        if (up->ctimeange == 0) {
	            ctimeange = dt ;
		}

	        cdp = &cd ;
	        dater_settimezn(&cd,(time_t) ctimeange,op->zname,-1) ;

	    } else {
	        cdp = &rp->cdate ;
	    }

/* force a change */

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: count=%d\n",
	        up->count) ;
#endif

	    if ((! f_changed) && (up->count != rp->count))
	        f_changed = true ;

#if	CF_DEBUG
	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("pingstatedb_uptime: dater_gettime() ptime=%s\n",
	            timestr_logz(ptime,timebuf)) ;
	    }
#endif

	    if ((rs < 0) || f_changed ||
	        (((dt - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(op->pfp,boff,SEEK_SET) ;

	        rs = record_write(rp,op->pfp,cdp,&ud,up->count,f_up) ;

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatedb_uptime: record_write() rs=%d\n",rs) ;
#endif

	    } /* end if (did the update) */

	} else {
	    TIMEB	*nowp = &op->now ;
	    int		f_rec = false ;
	    cchar	*zn = op->zname ;
	    cchar	*hn = hostname ;

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: no match found rs=%d\n",rs) ;
#endif

	    dater_settimezn(&ud,timestamp,op->zname,-1) ;

	    if ((ctimeange = up->ctimeange) == 0) {
	        ctimeange = dt ;
	    }
	    dater_settimezn(&cd,(time_t) ctimeange,op->zname,-1) ;

	    f_changed = true ;
	    bseek(op->pfp,0L,SEEK_END) ;

	    btell(op->pfp,&boff) ;
	    roff = boff ;

	    if ((rs = record_start(&e,nowp,zn,roff,hn,&ud)) >= 0) {
	        f_rec = true ;
	    }

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: record_start() rs=%d\n",rs) ;
	    DEBUGPRINTF("pingstatedb_uptime: hostname=%s\n", hostname) ;
#endif

	    if (rs >= 0) {
	        rs = record_write(&e,op->pfp,&ud,&cd,up->count,f_up) ;
	    }

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: record_write() rs=%d\n",
	        rs) ;
#endif

	    if (rs >= 0) {
	        sz = szof(PSD_REC) ;
	        rs = vecitem_add(op->elp,&e,sz) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&e) ;
	    } /* end if (error) */
	} /* end if (target entry) */

/* update the LASTUPDATE entry */

	if (rs >= 0) {
	    rs = pingstatdb_updrec(op,dt,&cd,LASTUPDATE,f_up,timestamp) ;
	}

/* udpate our last modification time to keep our cache current */

	op->mtime = dt ;
	bcontrol(op->pfp,BC_SYNC,0) ;

	dater_finish(&cd) ;

ret2:
	dater_finish(&ud) ;

/* unlock it */
ret1:
	op->fl.writelocked = false ;
	bcontrol(op->pfp,BC_UNLOCK,0) ;

ret0:

#if	CF_DEBUG
	DEBUGPRINTF("pingstatedb_uptime: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (pingstatdb_uptime) */


/* local subroutines */

local int pingstatdb_checkcache(PSD *op) noex {
	int		rs ;
	int		f_cached = op->fl.cached ;
	DEBUGPRINTF("ent f_cached=%d\n", f_cached) ;
	if (ustat sb ; (rs = bcontrol(op->pfp,BC_STAT,&sb)) >= 0) {
	    if (f_cached) {
	        if (sb.st_mtime > op->mtime) {
	            f_cached = false ;
	            pingstatdb_fes(op) ;
	        }
	    } /* end if */
	    if (! f_cached) {
	        if ((rs = pingstatdb_readrecords(op)) >= 0) {
	            op->mtime = sb.st_mtime ;
	            op->fl.cached = true ;
	        }
	    } else {
	        rs = vecitem_count(op->elp) ;
	    }
	} /* end if (bcontrol) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingstatdb_checkcache) */

local int pingstatdb_readrecords(PSD *op) noex {
	int		rs ;
	int		c = 0 ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = brewind(op->pfp)) >= 0) {
	    PSD_REC	e ;
	    uint	roff = 0 ;
	    cint	sz = szof(PSD_REC) ;
	    cint	rlen = BUFLEN ;
	    int		line = 1 ;
	    int		bl ;
	    int		f_eol ;
	    int		f_bol = true ;
	    char	rbuf[BUFLEN + 1] ;
	    while ((rs = breadln(op->pfp,rbuf,rlen)) > 0) {
	        int len = rs ;
	        bl = (len - 1) ;
	        f_eol = (rbuf[bl] == '\n') ;
	        rbuf[bl] = '\0' ;
	        if (f_bol && (bl > RF_LEAD)) {
	            timeb	*nowp = &op->now ;
	            cchar	*zn = op->zname ;
	            DEBUGPRINTF("line=%u\n",line) ;
	            if ((rs = record_startbuf(&e,nowp,zn,roff,rbuf,bl)) >= 0) {
	                c += 1 ;
	                rs = vecitem_add(op->elp,&e,sz) ;
	                if (rs < 0) {
	                    record_finish(&e) ;
			} /* end if (error) */
	            } /* end if (record_startbuf) */
	        } /* end if (a live one) */
	        roff += len ;
	        line += 1 ;
	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while (reading file records) */
	} /* end if (brewind) */
	DEBUGPRINTF("ret rs=%d c=%u\n",rs,c) ;
	return (rs >= 0) ? c : rs ;
} /* end subroutine (pingstatdb_readrecords) */

local int pingstatdb_fes(PSD *op) noex {
	vecitem		*elp = op->elp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	/* delete for an uncompacted vector */
	void *vp ;
	for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	    if (PSD_REC *ep = resumelife<PSD_REC>(vp) ; ep) {
	        record_finish(ep) ;
	        vecitem_del(op->elp,i) ;
	    }
	} /* end for */
	/* delete for a compacted vector */
	for (int i = 0 ; (rs1 = elp->get(i,&vp)) >= 0 ; i += 1) {
	    if (PSD_REC *ep = resumelife<PSD_REC>(vp) ; ep) {
	        record_finish(ep) ;
	        vecitem_del(elp,i) ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (pingstatdb_fes) */

local int pingstatdb_updrec(PSB *op,time_t dt,dater *dp,
		cchar *hostname,int f_up,time_t timestamp) noex {
	PSD_REC	*rp ;
	off_t	boff ;
	int		rs ;
	int		f_changed = false ;

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN+1] ;
	    DEBUGPRINTF("pingstatdb_updrec: hostname=%s\n",hostname) ;
	    DEBUGPRINTF("pingstatdb_updrec: f_up=%u\n",f_up) ;
	    DEBUGPRINTF("pingstatdb_updrec: timestamp=%s\n",
	        timestr_logz(timestamp,timebuf)) ;
	}
#endif /* CF_DEBUG */

	if ((rs = pingstatdb_getrec(op,hostname,&rp)) >= 0) {
	    time_t	ptime ;
	    int		f_greater = (! LEQUIV(f_up,rp->f_up)) ;

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatdb_updrec: found match rs=%d\n",rs) ;
#endif

	    rs = dater_gettime(&rp->pdate,&ptime) ;

	    f_greater = ((rs >= 0) && (timestamp > ptime)) ;

	    if (ptime > timestamp)
	        timestamp = ptime ;

	    dater_settimezn(dp,timestamp,op->zname,-1) ;

#if	CF_DEBUG
	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("pingstatdb_updrec: dater_gettime() ptime=%s\n",
	            timestr_logz(ptime,timebuf)) ;
	    }
#endif

	    if ((rs < 0) || f_changed ||
	        (((dt - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(op->pfp,boff,SEEK_SET) ;

	        rs = record_update(rp,op->pfp,dp,f_up) ;

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatdb_updrec: record_updrec() rs=%d\n",rs) ;
#endif

	    } /* end if (did the update) */

	} else if (rs == SR_NOTFOUND) {
	    PSD_REC	r ;
	    uint	roff ;
	    cint	sz = szof(PSD_REC) ;
	    int		f_rec = false ;

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatdb_updrec: no match found rs=%d\n",rs) ;
	    DEBUGPRINTF("pingstatdb_updrec: zname=%s\n",op->zname) ;
#endif

	    if ((rs = dater_settimezn(dp,timestamp,op->zname,-1)) >= 0) {
		TIMEB	*nowp = &op->now ;
		cchar	*zn = op->zname ;
		cchar	*hn = hostname ;

	        f_changed = true ;
	        bseek(op->pfp,0L,SEEK_END) ;

	        btell(op->pfp,&boff) ;
	        roff = boff ;

	        if ((rs = record_start(&r,nowp,zn,roff,hn,dp)) >= 0) {
	            f_rec = true ;
		}

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatdb_updrec: record_start() rs=%d hn=%s\n",
	            rs,hostname) ;
#endif

	    } /* end if */

	    if (rs >= 0) {
	        rs = record_update(&r,op->pfp,dp,f_up) ;

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatdb_updrec: record_update() rs=%d\n",
	            rs) ;
#endif

	    } /* end if */

	    if (rs >= 0) {
	        rs = vecitem_add(op->elp,&r,sz) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&r) ;
	    } /* end if (error) */
	} /* end if (target entry) */

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_updrec: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (pingstatdb_updrec) */

local int pingstatdb_getrec(PSD *op,cchar *hostname,PSD_REC **rpp) noex {
	int		rs ;
	int		i ; /* return-value */
	for (i = 0 ; (rs = vecitem_get(op->elp,i,rpp)) >= 0 ; i += 1) {
	    if (*rpp == nullptr) continue ;
	    if (strcmp(hostname,(*rpp)->hostname) == 0) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (pingstatdb_getrec) */


