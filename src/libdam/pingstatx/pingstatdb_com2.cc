/* pingstatdb_com2 MODULE (implementation) */
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
	pingstatdb_rec

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

module ;

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
#include	<bufsizeget.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"
#include	"pingstatdb_rec.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module pingstatdb_com ;

import libutil ;			/* |lenstr(3u)| */
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

#define	DTR		dater

#define	RLENMULT	2		/* HostNameLen multiplier */

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

#define	DEBUGPRINTT(str,tval)		\
    	if_constexpr (f_debug) {	\
	    debugprintt(str,tval) ;	\
	}


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef time_t		ti_t ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct updater {
	PSD		*op ;
	DTR		*dp ;
	cchar		*hn ;
	time_t		dt ;
	time_t		ta ;
	int		f_up ;
	updater(PSD *o,DTR *d,cc *h,time_t ªdt,time_t ªta,int f) noex {
	    op = o ;
	    dp = d ;
	    hn = h ;
	    dt = ªdt ;
	    ta = ªta ;
	    f_up = f ;
	} ; /* end ctor */
	operator int () noex ;
	int decide	() noex ;
	int updold	(PSD_REC *) noex ;
	int updnew	() noex ;
	int seekend	(off_t *) noex ;
    } ; /* end struct (updater) */
} /* end namespace */


/* forward references */

local int debugprintt(cchar *,time_t) noex ;


/* local variables */

cint			to_upd		= TO_MINUPDATE ;
cbool			f_debug		= CF_DEBUG ;


/* exported variables */

int pingstatdb_recupd(PSD *op,ti_t dt,DTR *dp,cc *hn,int f_up,ti_t ta) noex {
	int		rs = SR_BUGCHECK ;
	int		fchanged = false ;
	DEBUGPRINTF("ent\n") ;
	DEBUGPRINTF("hn=%s\n",hn) ;
	DEBUGPRINTF("f_up=%u\n",f_up) ;
	DEBUGPRINTT("ta=%s",ta) ;
	if (op && hn && dp) ylikely {
	    rs = SR_INVALID ;
	    if(hn[0]) ylikely {
	        updater uo(op,dp,hn,dt,ta,f_up) ;
	        rs = uo ;
		fchanged = int(rs > 0) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n",rs,fchanged) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end subroutine (pingstatdb_recupd) */

updater::operator int () noex {
    	cint		rsn SR_NOTFOUND ;
	int		rs ;
	int		fchanged = false ;
	DEBUGPRINTF("ent\n") ;
	if (PSD_REC *rep ; (rs = pingstatdb_recget(op,hn,&rep)) >= 0) {
	    rs = updold(rep) ;
	} else if (rs == rsn) {
	    rs = updnew() ;
	} /* end if (printstatdb) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n",rs,fchanged) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end method updater::operator) */

int updater::decide() noex {
    	cint		rsn SR_NOTFOUND ;
	int		rs ;
	int		fchanged = false ;
	if (PSD_REC *rep ; (rs = pingstatdb_recget(op,hn,&rep)) >= 0) {
	    rs = updold(rep) ;
	    fchanged = int(rs > 0) ;
	} else if (rs == rsn) {
	    rs = updnew() ;
	    fchanged = true ;
	} /* end if (printstatdb) */
	return (rs >= 0) ? fchanged : rs ;
} /* end method updater::decide) */

int updater::updold(PSD_REC *rep) noex {
    	dater		*pdp = rep->pdp ;
    	int		rs ;
	int        	fchanged = false ; /* return-value */
	if (time_t ptime ; (rs = pdp->gettime(&ptime)) >= 0) ylikely {
	    bool fgreater = (ta > ptime) ;
	    if (ptime > ta) {
		ta = ptime ;
	    }
	    if ((rs =  dp->settimezn(ta,op->znbuf)) >= 0) ylikely {
	        bool f = fchanged ;
                f = f || (((dt - ptime) > to_upd) && fgreater) ;
		if (f) {
		    bfile *pfp = op->pfp ;
                    coff boff = rep->roff ;
                    if ((rs = pfp->seek(boff,SEEK_SET)) >= 0) ylikely {
                        rs = record_update(rep,op->pfp,dp,f_up) ;
                        DEBUGPRINTF("record_recupd() rs=%d\n",rs) ;
		    } /* end if (bseek) */
                } /* end if (did the update) */
	    } /* end if (dater_settimean) */
	} /* end if (dater_gettime) */
	return (rs >= 0) ? fchanged : rs ;
} /* end method (updater::updold) */

int updater::updnew() noex {
    	return SR_OK ;
} /* end method (updater::updold) */

int updater::seekend(off_t *fop) noex {
    	bfile		*pfp = op->pfp ;
    	int		rs ;
	if ((rs = pfp->seek(0z,SEEK_END)) >= 0) {
	    rs = pfp->tell(fop) ;
	return rs ;
} /* end ethod (updater::seekend) */

#ifdef	COMMENT
int pingstatdb_recupd(PSD *op,time_t dt,DTR *dp,
		cchar *hn,int f_up,time_t tt) noex {
	int		rs = SR_BUGCHECK ;
	int		f_changed = false ;
	    DEBUGPRINTF("hn=%s\n",hn) ;
	    DEBUGPRINTF("f_up=%u\n",f_up) ;
	    DEBUGPRINTT("tt=%s",tt) ;
	if (op && hn && dp) ylikely {

	PSD_REC	*rp ;
	if ((rs = pingstatdb_recget(op,hn,&rp)) >= 0) {
	off_t	boff ;
	    int		f_greater = (! LEQUIV(f_up,rp->f_up)) ;

	    DEBUGPRINTF("found match rs=%d\n",rs) ;
	    time_t	ptime ;
	    rs = dater_gettime(&rp->pdate,&ptime) ;

	    f_greater = ((rs >= 0) && (ts > ptime)) ;

	    if (ptime > ts) {
	        ts = ptime ;
	    }

	    dater_settimezn(dp,ts,op->znbuf,-1) ;
	    DEBUGPRINTT("ptime=%s",ptime) ;
	    if ((rs < 0) || f_changed ||
	        (((dt - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(op->pfp,boff,SEEK_SET) ;

	        rs = record_update(rp,op->pfp,dp,f_up) ;
	        DEBUGPRINTF("record_recupd() rs=%d\n",rs) ;
	    } /* end if (did the update) */

	} else if (rs == SR_NOTFOUND) {
	    uint	roff ;
	    cint	sz = szof(PSD_REC) ;
	    int		f_rec = false ;

	    DEBUGPRINTF("no match found rs=%d\n",rs) ;
	    DEBUGPRINTF("zname=%s\n",op->znbuf) ;
	    if ((rs = dater_settimezn(dp,ts,op->znbuf,-1)) >= 0) {
		timeb	*nowp = op->nowp ;
		cchar	*zn = op->znbuf ;

	        f_changed = true ;
	        bseek(op->pfp,0L,SEEK_END) ;

	        btell(op->pfp,&boff) ;
	        roff = boff ;

	    PSD_REC	r ;
	        if ((rs = record_start(&r,nowp,zn,roff,hn,dp)) >= 0) {
	            f_rec = true ;
		}

	        DEBUGPRINTF("record_start() rs=%d hn=%s\n",rs,hn) ;

	    } /* end if */

	    if (rs >= 0) {
	        rs = record_update(&r,op->pfp,dp,f_up) ;
	        DEBUGPRINTF("record_update() rs=%d\n", rs) ;
	    } /* end if */

	    if (rs >= 0) {
	        rs = vechand_add(op->rlp,&r,sz) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&r) ;
	    } /* end if (error) */
	} /* end if (target entry) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d fchanged=%u\n", rs,f_changed) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end subroutine (pingstatdb_recupd) */
#endif /* COMMENT */

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


