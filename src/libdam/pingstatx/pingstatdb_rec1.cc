/* pingstatdb_rec1 MODULE (implementation) */
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
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecitem.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"
#include	"pingstatdb_rec.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module pingstatdb_rec ;

import libutil ;			/* |lenstr(3u)| */
import pingstatdb_com ;

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

template<typename ... Args>
local inline int record_ctor(record *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(ep) ;
	    if (ep->cdp = new(nt) dater ; op->cdp) {
	        if (ep->pdp = new(nt) dater ; op->pdp) {
		    rs = SR_OK ;
	        } /* end if (new-dater) */
	        if (rs < 0) {
		    delete op->cdp ;
		    op->cdp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-dater) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_ctor) */

local int record_dtor(record *op) noex {
	int		rs = SR_OK ;
	if (op->pdp) ylikely {
	    delete op->pdp ;
	    op->pdp = nullptr ;
	}
	if (op->cdp) ylikely {
	    delete op->cdp ;
	    op->cdp = nullptr ;
	}
	return rs ;
} /* end subroutine (record_dtor) */


/* exported subroutines */

int record_start(PSD_RED *ep,timeb *nowp,cc *zn,uint roff,
		cc *hn,dater *dp) noex {
	int		rs = SR_OK ;
	int		hl ;
	DEBUGPRINTF("ent hn=%s\n",hn) ;
	if ((rs = record_ctor(ep,nowp,zn,hn,dp)) >= ) ylikely {
	    dater *cdp = op->cdp ;
	    ep->roff	= roff ;
	    ep->cnt	= 1 ;
	    ep->f_up	= false ;
	    if ((rs = cdp->startcopy(dp)) >= 0) {
	        if ((rs = pdp->startcopy(dp)) >= 0) {
	            hl = lenstr(hn) ;
	            ep->hostlen = hl ;
	            if (char *cp ; (rs = mem.strw(hn,hl,&cp)) >= 0) {
		        ep->hostname = cp ;
		    }
	            if (rs < 0) {
	                pdp->finish() ;
		    } /* end if (error) */
	        } /* end if (dater_startcopy) */
	        if (rs < 0) {
	            cdp->finish() ;
	        } /* end if (error) */
	    } /* end if (dater_startcopy) */
	    if (rs < 0) {
	        record_dtor(ep) ;
	    } /* end if (error) */
	} /* end if (record_ctor) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (record_start) */

int record_finish(PSD_REC *ep) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->hostname) {
	        voidp vp = voidp(ep->hostname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->hostname = nullptr ;
	    } /* end if (memory-release) */
	    if (dater *pdp = ep->pfp ; pdp) {
	        rs1 = pdp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (dater *cdp = ep->cfp ; cdp) {
	        rs1 = cdp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = record_dtor(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_finish) */

int record_startbuf(PSD_REC *ep,time *nowp,cc *zname,uint roff,
		char *buf,int buflen) noex {
	int		rs ;
	int		bl = buflen ;
	cchar	*bp = buf ;

	if (ep == nullptr) return SR_FAULT ;
	if (zname == nullptr) return SR_FAULT ;
	if (buf == nullptr) return SR_FAULT ;

	DEBUGPRINTF("name=%s\n",hostname) ;

	rs = dater_start(ep->cdp,nowp,zname,-1) ;
	if (rs < 0)
	    goto bad0 ;

	rs = dater_start(ep->pdp,nowp,zname,-1) ;
	if (rs < 0)
	    goto bad1 ;

	DEBUGPRINTF("3 name=%s\n",hostname) ;

	ep->roff = roff ;
	ep->len = buflen ;
	rs = cfdeci(bp,RF_NUMDIGITS,&ep->cnt) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_NUMDIGITS + 1) ;
	bl -= (RF_NUMDIGITS + 1) ;
	rs = dater_setlogz(ep->cdp,bp,bl) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_LOGZLEN + 1) ;
	bl -= (RF_LOGZLEN + 1) ;
	rs = dater_setlogz(ep->pdp,bp,bl) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_LOGZLEN + 1) ;
	bl -= (RF_LOGZLEN + 1) ;
	if (bl <= 0) {
	    rs = SR_INVALID ;
	    goto bad2 ;
	}

	DEBUGPRINTF("4 name=%s\n",hostname) ;
	ep->f_up = (toupper(*bp) == 'U') ;

	bp += (RF_UPSTAT + 1) ;
	bl -= (RF_UPSTAT + 1) ;
	if (bl <= 0) {
	    rs = SR_INVALID ;
	    goto bad2 ;
	}

#if	CF_DEBUG
	DEBUGPRINTF("4a name=%s\n",hostname) ;
	DEBUGPRINTF("bl=%u ext_name=%r\n",bl,bp,bl) ;
#endif

	ep->hostlen = bl ;
	rs = uc_mallocstrw(bp,bl,&ep->hostname) ;
	if (rs < 0)
	    goto bad2 ;

	DEBUGPRINTF("5 name=%s\n",hostname) ;
ret0:
	return rs ;

/* bad stuff */
bad2:
	dater_finish(ep->pdp) ;

bad1:
	dater_finish(ep->cdp) ;

bad0:
	goto ret0 ;
} /* end subroutine (record_startbuf) */

int record_update(PSD_REC *ep,bfile *fp,dater *dp,int f_up) noex {
	int		rs = SR_OK ;
	char		cdate[RF_LOGZLEN + 2] ;
	char		pdate[RF_LOGZLEN + 2] ;

	if (ep == nullptr) return SR_FAULT ;

	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("ent, host=%s\n",ep->hostname) ;
	    DEBUGPRINTF("cur_date=%s\n",
	        timestr_log(dp->b.time,timebuf)) ;
	}

	if (! LEQUIV(ep->f_up,f_up)) {

	    DEBUGPRINTF("changed status\n") ;
	    ep->f_up = f_up ;
	    ep->cnt = 1 ;
	    dater_setcopy(ep->cdp,dp) ;

	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("status change cdate=%s\n",
	            timestr_logz(ep->cdate.b.time,timebuf)) ;
	    }

	} else {
	    ep->cnt += 1 ;
	}

	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("cdate=%s\n",
	        timestr_logz(ep->cdate.b.time,timebuf)) ;
	}

	dater_mklogz(ep->cdp,cdate,(RF_LOGZLEN + 1)) ;

	DEBUGPRINTF("cdate mklogz=%s\n",
	    cdate) ;

/* always update the last-update-date for the record */

	dater_setcopy(ep->pdp,dp) ;

	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("pdate=%s\n",
	        timestr_logz(ep->pdate.b.time,timebuf)) ;
	}

	dater_mklogz(ep->pdp,pdate,RF_LOGZLEN + 1) ;
	{
	    DEBUGPRINTF("pdate mklogz=%s\n",
	        pdate) ;
	}
	rs = bprintf(fp,"%*d %-*s %-*s %c %s\n",
	    RF_NUMDIGITS,ep->cnt,
	    RF_LOGZLEN,cdate,
	    RF_LOGZLEN,pdate,
	    ((ep->f_up) ? 'U' : 'D'),
	    ep->hostname) ;

	return rs ;
} /* end subroutine (record_update) */

int record_write(PSD_REC *ep,bfile *fp,dater *cp,dater *dp,
		int count,int f_up) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) ylikely {
	char		cdate[RF_LOGZLEN + 2] ;
	char		pdate[RF_LOGZLEN + 2] ;
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("ent host=%s\n",ep->hostname) ;
	    DEBUGPRINTF("count=%d\n",count) ;
	    DEBUGPRINTF("cur_date=%s\n",
	        timestr_log(dp->b.time,timebuf)) ;
	}
	ep->f_up = f_up ;
	if (count != 0) {
	    ep->cnt = count ;
	} else {
	    ep->cnt += 1 ;
	}
	/* the "change" date */
	dater_setcopy(ep->cdp,cp) ;
	dater_mklogz(ep->cdp,cdate,(RF_LOGZLEN + 1)) ;
	/* always update the last-update-date for the record */
	dater_setcopy(ep->pdp,dp) ;
	dater_mklogz(ep->pdp,pdate,RF_LOGZLEN + 1) ;
	/* pop it */
	{
	    cchar *fmt = "%*d %-*s %-*s %c %s\n" ;
	    rs = bprintf(fp,fmt,
	        RF_NUMDIGITS,ep->cnt,
	        RF_LOGZLEN,cdate,
	        RF_LOGZLEN,pdate,
	        ((ep->f_up) ? 'U' : 'D'),
	        ep->hostname) ;
	} /* end block */
	return rs ;
} /* end subroutine (record_write) */


