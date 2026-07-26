/* pingstatdb_rec1 MODULE (implementation) */
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
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vecitem.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<timestr.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<ismisc.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"
#include	"pingstatdb_rec.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module pingstatdb_rec ;

import libutil ;			/* |lenstr(3u)| */
import sif ;

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

#define	DEBUGPRINTT(str,tval)		\
    	if_constexpr (f_debug) {	\
	    debugprintt(str,tval) ;	\
	}


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef pingstatdb_rec	record ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int record_ctor(record *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = op->init() ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_ctor) */

local int record_dtor(record *op) noex {
	int		rs = SR_OK ;
	if (op) ylikely {
	    rs = op->fini() ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_dtor) */

local int debugprintt(cchar *,time_t) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int record_start(PSD_REC *ep,timeb *nowp,cc *zn,uint roff,cc *hn) noex {
	int		rs = SR_OK ;
	DEBUGPRINTF("ent hn=%s\n",hn) ;
	if ((rs = record_ctor(ep,nowp,zn,hn)) >= 0) ylikely {
	    dater *cdp = ep->cdp ;
	    ep->roff	= roff ;
	    ep->cnt	= 1 ;
	    ep->f_up	= false ;
	    if ((rs = cdp->start(nowp,zn)) >= 0) ylikely {
		dater *pdp = ep->pdp ;
	        if ((rs = pdp->start(nowp,zn)) >= 0) ylikely {
	            if (cchar *cp ; (rs = mem.strw(hn,-1,&cp)) >= 0) ylikely {
		        ep->hostbuf = cp ;
			ep->hostlen = rs ;
		    } /* end if (memory-acquire) */
	            if (rs < 0) {
	                pdp->finish() ;
		    } /* end if (error) */
	        } /* end if (dater_start) */
	        if (rs < 0) {
	            cdp->finish() ;
	        } /* end if (error) */
	    } /* end if (dater_start) */
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
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->hostbuf) {
	        voidp vp = voidp(ep->hostbuf) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->hostbuf = nullptr ;
	    } /* end if (memory-release) */
	    if (dater *pdp = ep->pdp ; pdp) {
	        rs1 = pdp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (dater *cdp = ep->cdp ; cdp) {
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

namespace {
    struct loader {
	PSD_REC		*rep ;		/* caller-supplied */
	ccharp		srcp ;		/* caller-supplied */
	int		srcl ;		/* caller-supplied */
	loader(PSD_REC *o,ccp p,int l) noex : rep(o) {
	    srcp = p ;
	    srcl = l ;
	} ; /* end ctor */
	operator int () noex ;
	int loadcnt	(cchar *,int) noex ;
	int loadcdate	(cchar *,int) noex ;
	int loadpdate	(cchar *,int) noex ;
	int loadstat	(cchar *,int) noex ;
	int loadhn	(cchar *,int) noex ;
    } ; /* end struct (loader) */
} /* end namespace */

int record_load(PSD_REC *rep,uint roff,cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("ent\n") ;
	if (rep && sp) ylikely {
	    if (loader lo(rep,sp,sl) ; (rs = lo) >= 0) {
		rep->roff = roff ;
		rep->len = sl ;
	    } /* end if (loader) */
	} /* end if (non-null) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (record_load) */

loader::operator int () noex {
    	sif		so(srcp,srcl) ;
    	int		rs = SR_OK ;
	cchar *cp ;
	for (int cl, i = 0 ; (cl = so(&cp)) > 0 ; i += 1) {
	    switch (i) {
	    case 0:
		rs = loadcnt(cp,cl) ;
		break ;
	    case 1:
		rs = loadcdate(cp,cl) ;
		break ;
	    case 2:
		rs = loadpdate(cp,cl) ;
		break ;
	    case 3:
		rs = loadstat(cp,cl) ;
		break ;
	    case 4:
		rs = loadhn(cp,cl) ;
		break ;
	    } /* end switch */
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end method (loader::operator) */

int loader::loadcnt(cchar *cp,int cl) noex {
    	int		rs ;
	if (int v ; (rs = cfdeci(cp,cl,&v)) >= 0) ylikely {
	    rep->cnt = v ;
	}
	return rs ;
} /* end method (loader::loadcnt) */

int loader::loadcdate(cchar *cp,int cl) noex {
    	return dater_setlogz(rep->cdp,cp,cl) ;
} /* end method (loader::loadcdate) */

int loader::loadpdate(cchar *cp,int cl) noex {
    	return dater_setlogz(rep->pdp,cp,cl) ;
} /* end method (loader::loadpdate) */

int loader::loadstat(cchar *cp,int cl) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; i < cl ; i += 1) {
	    switch (int ch = mkchar(cp[i]) ; ch) {
	    case 'U':
		rep->f_up = true ;
		break ;
	    case 'D':
		rep->f_up = false ;
		break ;
	    } /* end switch */
	} /* end for */
    	return rs ;
} /* end method (loader::loadstat) */

int loader::loadhn(cchar *cp,int cl) noex {
    	int		rs = SR_OK ;
	cchar *hp ;
	if (int hl = sfshrink(cp,cl,&hp) ; hl > 0) ylikely {
	    if (cchar *ap ; (rs = mem.strw(hp,hl,&ap)) >= 0) ylikely {
		rep->hostbuf = ap ;
		rep->hostlen = rs ;
	    } /* end if (memory-acquire) */
	} /* end if */
    	return rs ;
} /* end method (loader::loadhn) */

int record_update(PSD_REC *ep,bfile *fp,dater *dp,int f_up) noex {
	int		rs = SR_BUGCHECK ;
	DEBUGPRINTF("ent\n") ;
	if (ep && fp && dp) ylikely {
	    dater	*cdp = ep->cdp ;
	    dater	*pdp = ep->pdp ;
	    char	cdbuf[RF_LOGZLEN + 2] ;
	    char	pdbuf[RF_LOGZLEN + 2] ;
	    rs = SR_OK ;
	    if_constexpr (f_debug) {
	        DEBUGPRINTF("host=%s\n",ep->hostbuf) ;
	        DEBUGPRINTT("cur_date=",dp->b.time) ;
	    }
	    if (logdiffer(ep->f_up,f_up)) {
	        DEBUGPRINTF("changed status\n") ;
	        ep->f_up = f_up ;
	        ep->cnt = 1 ;
	        dater_setcopy(ep->cdp,dp) ;
	        if_constexpr (f_debug) {
	            DEBUGPRINTT("status change cdbuf=",cdp->b.time) ;
	        }
	    } else {
	        ep->cnt += 1 ;
	    }
	    if_constexpr (f_debug) {
	        DEBUGPRINTT("cddate=",cdp->b.time) ;
	    }
	    dater_mklogz(ep->cdp,cdbuf,(RF_LOGZLEN + 1)) ;
	    DEBUGPRINTF("cdbuf mklogz=%s\n",cdbuf) ;
	    /* always update the last-update-date for the record */
	    dater_setcopy(ep->pdp,dp) ;
	    if_constexpr (f_debug) {
	        DEBUGPRINTT("pdate=",pdp->b.time) ;
	    }
	    dater_mklogz(ep->pdp,pdbuf,RF_LOGZLEN + 1) ;
	    DEBUGPRINTF("pdbuf mklogz=%s\n", pdbuf) ;
	    {
		cchar *fmt = "%*d %-*s %-*s %c %s\n" ;
		rs = bprintf(fp,fmt,
	    	    RF_NUMDIGITS,ep->cnt,
	    	    RF_LOGZLEN,cdbuf,
	    	    RF_LOGZLEN,pdbuf,
	    	    ((ep->f_up) ? 'U' : 'D'),
	    	    ep->hostbuf) ;
	    } /* end block */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_update) */

int record_write(PSD_REC *ep,bfile *fp,dater *cp,dater *dp,
		int cnt,int f_up) noex {
	int		rs = SR_BUGCHECK ;
	if (ep && fp && dp) ylikely {
	    char	cdbuf[RF_LOGZLEN + 2] ;
	    char	pdbuf[RF_LOGZLEN + 2] ;
	    rs = SR_OK ;
	    {
	        DEBUGPRINTF("ent host=%s\n",ep->hostbuf) ;
	        DEBUGPRINTF("count=%d\n",cnt) ;
	        DEBUGPRINTT("cur_date=",dp->b.time) ;
	    }
	    ep->f_up = f_up ;
	    if (cnt != 0) {
	        ep->cnt = cnt ;
	    } else {
	        ep->cnt += 1 ;
	    }
	    /* the "change" date */
	    dater_setcopy(ep->cdp,cp) ;
	    dater_mklogz(ep->cdp,cdbuf,(RF_LOGZLEN + 1)) ;
	    /* always update the last-update-date for the record */
	    dater_setcopy(ep->pdp,dp) ;
	    dater_mklogz(ep->pdp,pdbuf,RF_LOGZLEN + 1) ;
	    /* pop it */
	    {
	        cchar *fmt = "%*d %-*s %-*s %c %s\n" ;
	        rs = bprintf(fp,fmt,
	            RF_NUMDIGITS,ep->cnt,
	            RF_LOGZLEN,cdbuf,
	            RF_LOGZLEN,pdbuf,
	            ((ep->f_up) ? 'U' : 'D'),
	            ep->hostbuf) ;
	    } /* end block */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (record_write) */


/* private subroutines */

pingstatdb_rec::pingstatdb_rec() noex {
	hostbuf		= nullptr ;
	cdp		= nullptr ;
	pdp		= nullptr ;
	roff		= 0 ;
	len		= 0 ;
	cnt		= 0 ;
	hostlen		= 0 ;
} /* end ctor (record) */

void pingstatdb_rec::dtor() noex {
    	(void) record_finish(this) ;
} /* end method */

int record::init() noex {
    	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	if (cdp = new(nt) dater ; cdp) ylikely {
	    if (pdp = new(nt) dater ; pdp) ylikely {
		rs = SR_OK ;
	    } /* end if (new-dater) */
	    if (rs < 0) {
		delete cdp ;
		cdp = nullptr ;
	    } /* end if (error) */
	} /* end if (new-dater) */
	return rs ;
} /* end method (record::init) */

int record::fini() noex {
    	int		rs = SR_OK ;
	if (pdp) {
	    delete pdp ;
	    pdp = nullptr ;
	}
	if (cdp) {
	    delete cdp ;
	    cdp = nullptr ;
	}
    	return rs ;
} /* end method (record::fini) */

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


