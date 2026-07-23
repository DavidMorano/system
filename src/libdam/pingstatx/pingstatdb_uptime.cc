/* pingstatdb_uptime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a PINGSTATDB file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */

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
#include	<ugetx.h>		/* LIBU |getustime(3u)| */
#include	<ucmem.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import pingstatdb_com ;
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


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;

#if	CF_DEBUG
static cchar	hostname[] = "EMPTY" ;
#endif


/* exported variables */


/* exported subroutines */



#ifdef	COMMENT

namespace {
    struct uptimer {
	PSD		*op ;	/* caller supplied */
	PSD_UP		*up ;	/* caller supplied */
	cchar		*hn ;	/* caller supplied */
	bfile		*pfp ;
	uptimer(PSD *o,cchar *h,PSD_UP *u) noex : op(o), up(u), hn(h) { 
	    pfp = op->pfp ;
	} ; /* end ctor */
	operator int () noex ;
	int readoff	() noex ;
	int lockon	() noex ;
    } ; /* end struct (uptimer) */
} /* end namespace */

int pingstatdb_uptime(PSD *op,cchar *hn,PSD_UP *up) noex {
    	int		rs ;
	int		fchanged = false ;
	DEBUGPRINTF("ent hn=%s\n",hn) ;
	if ((rs = pingstatdb_magic(op,hn,up)) >= 0) {
	    rs = SR_INVALID ;
	    if (hn[0]) {
	        rs = SR_BADF ;	/* not open for writing */
		if (op->fl.writable) {
		    uptimer uo(op,hn,up) ;
	            rs = uo ;
		    fchanged = int(rs > 0) ;
		} /* end if (writable) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n", rs,f_changed) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end subroutine (pingstatdb_uptime) */

uptimer::operator int () noex {
    	int		rs ;
	if ((rs = readoff()) >= 0) {
	    if ((rs = lockon()) >= 0) {
		if ((rs = pingstatdb_checkcache(op)) >= 0) {
		    rs = SR_OK ;


		} /* end if (pingstatdb_checkcache) */
	    } /* end if (lockon) */
	} /* end if (readoff) */
	return rs ;
} /* end method (uptimer::operator) */

int uptimer::readoff() noex {
    	int		rs = SR_OK ;
	if (op->fl.readlocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    rs = pfp->control(BC_UNLOCK,0) ;
	} /* end if */
	return rs ;
} /* end method (uptimer::readoff) */

int uptimer::lockon() noex {
    	int		rs = SR_OK ;
	if (! op->fl.writelocked) {
	    cint bcmd = BC_LOCKWRITE ;
	    rs = pfp->control(bcmd,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	} /* end if (we did not already have a lock on the file) */
	return rs ;
} /* end method (uptimer::lockon) */


#endif /* COMMENT */

#ifdef	COMMENT
int pingstatdb_uptime(PSD *op,cchar *hostname,PSD_UP *up) noex {
	PSD_REC		e, *rp ;
	dater		cd, ud, *cdp ;
	off_t		boff ;
	custime		dt = getustime ;
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

	DEBUGPRINTF("lock stuff\n") ;

	if (op->fl.readlocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    bcontrol(op->pfp,BC_UNLOCK,0) ;
	}

	if (! op->fl.writelocked) {
	    cint bcmd = BC_LOCKWRITE ;
	    rs = bcontrol(op->pfp,bcmd,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	} /* end if (we did not already have a lock on the file) */

	if (rs < 0)
	    goto ret0 ;

	rs = pingstatdb_checkcache(op) ;
	if (rs < 0)
	    goto ret1 ;

	op->now.time = dt ;
	rs = dater_start(&ud,op->nowp,op->zname,-1) ;
	if (rs < 0)
	    goto ret1 ;

	rs = dater_start(&cd,op->nowp,op->zname,-1) ;
	if (rs < 0)
	    goto ret2 ;

	timestamp = up->timestamp ;
	if ((up->timestamp == 0) || (up->timestamp > dt)) {
	    timestamp = dt ;
	}

	if_constexpr (f_debug) {
	    char	timebuf[TIMEBUFLEN+1] ;
	    DEBUGPRINTF("now=%s\n",
	        timestr_logz(op->now.time,timebuf)) ;
	    DEBUGPRINTF("dater_start() rs=%d cur_date=%s\n",
	        rs,timestr_logz(ud.b.time,timebuf)) ;
	}

	if_constexpr (f_debug) {
	    DEBUGPRINTF("about to match\n") ;
	    rs = vechand_count(op->rlp) ;
	    DEBUGPRINTF("entries in cache %d\n",rs) ;
	}

	if ((rs = pingstatdb_recget(op,hostname,&rp)) >= 0) {
	    int	f_greater = (! LEQUIV(f_up,rp->f_up)) ;

	    DEBUGPRINTF("found match rs=%d\n",rs) ;

	    rs = dater_gettime(&rp->pdate,&ptime) ;
	    f_greater = ((rs >= 0) && (timestamp > ptime)) ;
	    /* the update (ping) time */
	    if (ptime > timestamp) {
	        timestamp = ptime ;
	    }

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

	    DEBUGPRINTF("count=%d\n",
	        up->count) ;

	    if ((! f_changed) && (up->count != rp->count))
	        f_changed = true ;

	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("dater_gettime() ptime=%s\n",
	            timestr_logz(ptime,timebuf)) ;
	    }

	    if ((rs < 0) || f_changed ||
	        (((dt - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(op->pfp,boff,SEEK_SET) ;

	        rs = record_write(rp,op->pfp,cdp,&ud,up->count,f_up) ;

	        DEBUGPRINTF("record_write() rs=%d\n",rs) ;

	    } /* end if (did the update) */

	} else {
	    TIMEB	*nowp = op->nowp ;
	    bool	f_rec = false ;
	    cchar	*zn = op->zname ;
	    cchar	*hn = hostname ;

	    DEBUGPRINTF("no match found rs=%d\n",rs) ;

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

	    DEBUGPRINTF("record_start() rs=%d\n",rs) ;
	    DEBUGPRINTF("hostname=%s\n", hostname) ;

	    if (rs >= 0) {
	        rs = record_write(&e,op->pfp,&ud,&cd,up->count,f_up) ;
	    }

	    DEBUGPRINTF("record_write() rs=%d\n", rs) ;

	    if (rs >= 0) {
	        sz = szof(PSD_REC) ;
	        rs = vechand_add(op->rlp,&e,sz) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&e) ;
	    } /* end if (error) */
	} /* end if (target entry) */

/* update the LASTUPDATE entry */

	if (rs >= 0) {
	    rs = pingstatdb_recupd(op,dt,&cd,LASTUPDATE,f_up,timestamp) ;
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

	DEBUGPRINTF("ret rs=%d f_changed=%u\n", rs,f_changed) ;
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (pingstatdb_uptime) */
#else /* COMMENT */
int pingstatdb_uptime(PSD *op,cchar *hn,PSD_UP *up) noex {
    	int		rs = SR_FAULT ;
	if (op && hn && up) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pingstatdb_uptime) */
#endif /* COMMENT */


/* local subroutines */


