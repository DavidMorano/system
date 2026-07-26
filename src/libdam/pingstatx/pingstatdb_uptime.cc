/* pingstatdb_uptime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to manipulate a PINGSTATDB file */
/* version %I% last-modified %G% */

#define	CF_DEBUG	1		/* non-switchable debug print-outs */

/* revision history:

	= 1998-04-13, David A-D- Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pingstatdb

  	Description:
	This subroutine maintains a PINGSTATDB file.  These files
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
#include	<new>			/* C++TD */
#include	<functional>		/* C++TD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ugetx.h>		/* LIBU |getustime(3u)| */
#include	<ucmem.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
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

#define	TO_LOCK		30		/* seconds */
#define	TO_MINUPDATE	3		/* minimum time between updates */

#define	LASTUPDATE	"*LAST_UPDATE*"

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#endif

#define	DEBUGPRINTT(str,tval)		\
    	if_constexpr (f_debug) {	\
	    debugprintt(str,tval) ;	\
	}
    

/* imported namespaces */

using std::bind_front ;			/* subroutine */
using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int debugprintt(cchar *,time_t) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

namespace {
    struct uptimer {
	PSD		*op ;	/* caller supplied */
	PSD_UP		*up ;	/* caller supplied */
	cchar		*hn ;	/* caller supplied */
	bfile		*pfp ;
	dater		*cdp ;
	dater		*udp ;
	time_t		dt ;
	uint		ts ;	/* time-stamp */
	bool		f_up ;
	uptimer(PSD *o,cchar *h,PSD_UP *u) noex : op(o), up(u), hn(h) { 
	    pfp = op->pfp ;
	    cdp = nullptr ;
	    udp = nullptr ;
	    dt = getustime ;
	    ts = 0 ;
	    f_up = false ;
	} ; /* end ctor */
	operator int () noex ;
	int readoff	() noex ;
	int lockon	() noex ;
	int lockoff	() noex ;
	int daterbeg	() noex ;
	int daterend	() noex ;
	int sync	() noex ;
	int timestamp	() noex ;
	int decide	() noex ;
	int upold	(PSD_REC *) noex ;
	int upnew	() noex ;
	int enter	(uint) noex ;
	int uplast	() noex ;
	int done	() noex ;
	int seekend	(off_t *) noex ;
    } ; /* end struct (uptimer) */
} /* end namespace */

int pingstatdb_uptime(PSD *op,cchar *hn,PSD_UP *up) noex {
    	int		rs ;
	int		fchanged = false ; /* return-value */
	DEBUGPRINTF("ent hn=%s\n",hn) ;
	if ((rs = pingstatdb_magic(op,hn,up)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (hn[0]) ylikely {
	        rs = SR_BADF ;	/* not open for writing */
		if (op->fl.writable) ylikely {
		    uptimer uo(op,hn,up) ;
	            rs = uo ;
		    fchanged = int(rs > 0) ;
		} /* end if (writable) */
	    } /* end if (valid) */
	} /* end if (pingstatdb_magic) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n", rs,fchanged) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end subroutine (pingstatdb_uptime) */

uptimer::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		fchanged = false ; /* return-value */
	if ((rs = readoff()) >= 0) ylikely {
	    if ((rs = lockon()) >= 0) ylikely {
		if ((rs = pingstatdb_checkcache(op)) >= 0) ylikely {
		    if ((rs = daterbeg()) >= 0) {
			if ((rs = decide()) >= 0) {
			    fchanged = int(rs > 0) ;
			    rs = done() ;
			} /* end if (decide) */
			rs1 = daterend() ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if */
		} /* end if (pingstatdb_checkcache) */
		rs1 = lockoff() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (lockon) */
	} /* end if (readoff) */
	return (rs >= 0) ? fchanged : rs ;
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

int uptimer::lockoff() noex {
    	int		rs = SR_OK ;
	if (op->fl.writelocked) {
	    op->fl.writelocked = false ;
	    rs = bcontrol(op->pfp,BC_UNLOCK,0) ;
	}
	return rs ;
} /* end method (uptimer::lockoff) */

int uptimer::daterbeg() noex {
    	cnothrow	nt{} ;
	timeb		*nowp = op->nowp ;
    	int		rs = SR_NOMEM ;
	cchar		*zn = op->znbuf ;
	if (cdp = new(nt) dater ; cdp) {
	    if (cdp = new(nt) dater ; cdp) {
		if ((rs = cdp->start(nowp,zn)) >= 0) {
		    if ((rs = udp->start(nowp,zn)) >= 0) {
			ts = conv<uint>(cdp->cb.time) ;
		    } /* end if (dater_start) */
		    if (rs < 0) {
			cdp->finish() ;
		    } /* end if (error) */
		} /* end if (dater_start) */
	        if (rs < 0) {
		    delete udp ;
		    udp = nullptr ;
	        } /* end if (error) */
	    } /* end if (new-dater) */
	    if (rs < 0) {
		delete cdp ;
		cdp = nullptr ;
	    } /* end if (error) */
	} /* end if (new-dater) */
	return rs ;
} /* end method (uptimer::daterbeg) */

int uptimer::daterend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (udp) {
	    {
	        rs1 = udp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete udp ;
		udp = nullptr ;
	    }
	} /* end if */
	if (cdp) {
	    {
	        rs1 = cdp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete cdp ;
		cdp = nullptr ;
	    }
	} /* end if */
	return rs ;
} /* end method (uptimer::daterend) */

int uptimer::sync() noex {
    	int		rs = SR_BUGCHECK ;
	if (pfp) {
	    op->mtime = dt ;
	    rs = pfp->control(BC_SYNC,0) ;
	} /* end if (non-null) */
	return rs ;
} /* end method (uptimer::sync) */

int uptimer::timestamp() noex {
    	int		rs = SR_BUGCHECK ;
	if ((up->timestamp == 0) || (up->timestamp > dt)) {
	    ts = conv<uint>(dt) ;
	} else {
	    ts = conv<uint>(up->timestamp) ;
	}
	return rs ;
} /* end method (uptimer::timestamp) */

int uptimer::decide() noex {
    	int		rsn = SR_NOTFOUND ;
    	int		rs = SR_BUGCHECK ;
	int		fchanged = false ;
	if ((rs = timestamp()) >= 0) {
	    if (PSD_REC *rep ; (rs = pingstatdb_recget(op,hn,&rep)) >= 0) {
		rs = upold(rep) ;
		fchanged = int(rs > 0) ;
	    } else if (rs == rsn) {
		rs = upnew() ;
		fchanged = int(rs > 0) ;
	    } /* end if (pingstatdb_recget) */
	} /* end if (timestamp) */
	return (rs >= 0) ? fchanged : rs ;
} /* end method (uptimer::decide) */

int uptimer::upold(PSD_REC *rep) noex {
    	dater		*pdp = rep->pdp ;
    	int		rs ;
	int		fchanged = false ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	f_up = !!rep->f_up ;
	if (time_t ptime{} ; (rs = pdp->gettime(&ptime)) >= 0) {
	    uint pts = conv<uint>(ptime) ;
	    cchar *zn = op->znbuf ;
	    bool f_greater = (ts > pts) ;
	    DEBUGPRINTT("dater_gettime() ptime=",ptime) ;
	    /* the update (ping) time */
	    if (pts > ts) {
	        ts = pts ;
	    }
	    if (time_t t = time_t(ts) ; (rs = udp->settimezn(t,zn,-1)) >= 0) {
		dater *tdp = nullptr ;
		uint ts_chg = 0 ;
		int to_upd = TO_MINUPDATE ;
	        /* the change time */
	        if (fchanged || (up->timechange != 0)) {
	            ts_chg = up->timechange ;
	            if (up->timechange == 0) {
	                ts_chg = conv<uint>(dt) ;
		    }
	            tdp = cdp ;
		    custime time_chg = time_t(ts_chg) ;
	            rs = cdp->settimezn(time_chg,op->znbuf,-1) ;
	        } else {
	            tdp = rep->cdp ;
	        }
	        /* force a change */
	        DEBUGPRINTF("count=%d\n", up->count) ;
	        if ((! fchanged) && (up->count != rep->cnt)) {
	            fchanged = true ;
	        }
	        DEBUGPRINTT("dater_gettime() ptime=",ptime) ;
		bool f = fchanged ;
	        f = f || ((intconv(dt - ptime) > to_upd) && f_greater) ;
		if ((rs >= 0) && f) {
	            coff boff = rep->roff ;
	            if ((rs = bseek(op->pfp,boff,SEEK_SET)) >= 0) {
			cauto rec_wr = bind_front(record_write,rep) ;
	                rs = rec_wr(op->pfp,tdp,udp,up->count,f_up) ;
	                DEBUGPRINTF("record_write() rs=%d\n",rs) ;
		    }
	        } /* end if (did the update) */
	    } /* end if (dater_settimexan) */
	} /* end if (dater_gettime) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n",rs,fchanged) ;
    	return (rs >= 0) ? fchanged : rs ;
} /* end method (uptimer::upold) */

int uptimer::upnew() noex {
    	int		rs ;
	int		fchanged = false ; /* return-value */
	bool		f_rec = false ;
	cchar		*zn = op->znbuf ;
	DEBUGPRINTF("ent\n") ;
	(void) f_rec ;
	if (time_t t = time_t(ts) ; (rs = udp->settimezn(t,op->znbuf)) >= 0) {
	    uint ts_chg = 0 ;
	    if ((ts_chg = up->timechange) == 0) {
	        ts = conv<uint>(dt) ;
	    }
	    t = time_t(ts_chg) ;
	    if ((rs = cdp->settimezn(t,zn)) >= 0) {
	        fchanged = true ;
		if (off_t off ; (rs = seekend(&off)) >= 0) {
		    con uint roff = conv<uint>(off) ;
		    rs = enter(roff) ;
		} /* end if (seekend) */
	    } /* end if (dater_settimezn) */
	} /* end if (dater_gettime) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n",rs,fchanged) ;
    	return (rs >= 0) ? fchanged : rs ;
} /* end method (uptimer::upnew) */

int uptimer::enter(uint roff) noex {
	timeb		*nowp = op->nowp ;
	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	cchar		*zn = op->znbuf ;
	DEBUGPRINTF("ent\n") ;
	if (PSD_REC *rep = new(nt) PSD_REC ; rep) {
	    cauto 	rec_st = bind_front(record_start,rep) ;
	    cauto 	rec_wr = bind_front(record_write,rep) ;
	    cauto 	rec_fi = bind_front(record_finish,rep) ;
	    if ((rs = rec_st(nowp,zn,roff,hn)) >= 0) {
		cint cnt = up->count ;
	        bool f_rec = true ;
	        if ((rs = rec_wr(op->pfp,udp,cdp,cnt,f_up)) >= 0) {
		    vechand *rlp = op->rlp ;
	            DEBUGPRINTF("record_write() rs=%d\n", rs) ;
	            rs = rlp->add(rep) ;
		} /* end if (record_write) */
	        if ((rs < 0) && f_rec) {
	            rec_fi() ;
	        } /* end if (error) */
	    } /* end if (record_start) */
	    if (rs < 0) {
		delete rep ;
	    } /* end if (error) */
	} /* end if (new-record) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end method (uptimer::enter) */

int uptimer::uplast() noex {
    	custime t = time_t(ts) ;
	return pingstatdb_recupd(op,dt,cdp,LASTUPDATE,f_up,t) ;
} /* end method (uptimer::uplast) */

int uptimer::done() noex {
    	int		rs ;
	if ((rs = uplast()) >= 0) {
	    rs = sync() ;
	}
	return rs ;
} /* end method (uptimer::done) */

int uptimer::seekend(off_t *fop) noex {
    	int		rs ;
	if ((rs = pfp->seek(0z,SEEK_END)) >= 0) {
	    rs = pfp->tell(fop) ;
	}
	return rs ;
} /* end method (uptimer::seekend) */
			

/* local subroutines */

local int debugprintt(cchar *str,time_t tval) noex {
    	int		rs = SR_OK ;
	if_constexpr (f_debug) {
    	    cchar	*fmt = "%s %s\n" ;
	    char	tbuf[TIMEBUFLEN + 1] ;
	    timestr_logz(tval,tbuf) ;
	    rs = DEBUGPRINTF(fmt,str,tbuf) ;
	} /* end if_constexpr (f_debug) */
    	return rs ; 
} /* end subroutine (debugprintt) */


