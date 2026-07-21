/* pingstatdb SUPPORT */
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
	all record locked used throughout this subroutine.

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
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */
#include	<bfile.h>		/* LIBB */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"pingstatdb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

struct pingstatdb_rec {
	dater		cdate ;		/* last change date */
	dater		pdate ;		/* last ping date */
	cchar		*hostname ;	/* host name */
	uint		roff ;		/* record offset (within file) */
	int		len ;		/* length of file record */
	int		count ;		/* count since last change */
	int		hostlen ;
	int		f_up ;		/* UP-DOWN status */
} ; /* end struct (pingstatdb_rec) */


/* forward references */

template<typename ... Args>
local inline int pingstatdb_ctor(pingstatdb *op,Args ... args) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if (bfile *bfp = new(nt) bfile ; bfp) {
		op->bfp = bfp ;
	        if (vecitem *elp = new(nt) vecitem ; elp) {
		    op->elp = elp ;
		    rs = SR_OK ;
	        } /* end if (new-bfile) */
	        if (rs < 0) {
		    delete bfp ;
		    op->bfp = nullptr ;
		}
	    } /* end if (new-bfile) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pingstatdb_ctor) */

local int pingstatdb_dtor(pingstatdb *op) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (vecitem *elp = resumelife<vecitem>(op->ep) ; elp) {
	        delete elp ;
	        op->clp = nullptr ;
	    }
	    if (bfile *bfp = resumelife<bfile>(op->bfp) ; bfp) {
		delete bfp ;
		op->bfp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pingstatdb_dtor) */

template<typename ... Args>
local inline int pingstatdb_magic(pingstatdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PINGSTATDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (pingstatdb_magic) */

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

#if	CF_DEBUG
static cchar	*hostname = "EMPTY" ;
#endif


/* exported variables */


/* exported subroutines */

int pingstatdb_open(PSD *op,cchar *fname,mode_t omode,int fperm) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	char		bstr[10] ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_open: ent\n") ;
#endif

	if (op == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_open: omode=%4o fname=%s\n",
	    omode,fname) ;
	if (omode & O_CREAT)
	    DEBUGPRINTF("pingstatdb_open: creating as needed\n") ;
#endif

#if	CF_CREATE
	omode |= O_CREAT ;
#endif

	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    op->fl.ame = cp ;
	    op->fl.writable = ((omode & O_WRONLY) || (omode & O_RDWR)) ;
	    mkbstr(omode,bstr) ;
	    if ((rs = bopen(&op->pfile,op->fl.ame,bstr,fperm)) >= 0) {
	        cint	cmd = BC_CLOSEONEXEC ;
	        if ((rs = bcontrol(&op->pfile,cmd,true)) >= 0) {
	            if ((rs = vecitem_start(op->elp,0,0)) >= 0) {
			rs = pingstatdb_opener(op) ;
	                if (rs < 0) {
	                    vecitem_finish(op->elp) ;
			}
	            }
	        } /* end if (bcontrol) */
	        if (rs < 0) {
	            bclose(&op->pfile) ;
		}
	    } /* end if (bopen) */
	    if (rs < 0) {
	        uc_free(op->fl.ame) ;
	        op->fl.ame = nullptr ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (pingstatdb_open) */

local int pingstatdb_opener(pingstatdb *op) noex {
    	int		rs ;
	if (char *znbuf ; (rs = malloc_zn(&znbuf)) >= 0) {
	    cint	znlen = rs ;
	    if ((rs = initnow(&op->now,znbuf,znlen)) >= 0) {
		if (char *a ; (rs = uc_mallocstrw(znbuf,rs,&a)) >= 0) {
		    op->zname = a ;
		    op->magval = PSD_MAGIC ;
		} /* end if (memory-acquire) */
	    }
	    rs = rsfree(rs,znbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (pingstatdb_opener) */

int pingstatdb_close(PSD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;
	{
	rs1 = pingstatdb_fes(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = bclose(&op->pfile) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = vecitem_finish(op->elp) ;
	if (rs >= 0) rs = rs1 ;
	}
	if (op->fl.ame != nullptr) {
	    rs1 = uc_free(op->fl.ame) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fl.ame = nullptr ;
	}
	if (op->zname) {
	    rs1 = uc_free(op->zname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->zname = nullptr ;
	}
	op->magval = 0 ;
	return rs ;
}
/* end subroutine (pingstatdb_close) */


/* initialize a cursor */
int pingstatdb_curbegin(PSD *op,PSD_CUR *curp)
{

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	op->fl.cursor = true ;
	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (pingstatdb_curbegin) */


/* free up a cursor */
int pingstatdb_curend(PSD *op,PSD_CUR *curp)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if (op->fl.readlocked || op->fl.writelocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    rs1 = bcontrol(&op->pfile,BC_UNLOCK,0) ;
	    if (rs >= 0) rs = rs1 ;
	}

	op->fl.cursor = false ;
	curp->i = -1 ;
	return rs ;
}
/* end subroutine (pingstatdb_curend) */


/* enumerate the entries */
int pingstatdb_enum(PSD *op,PSD_CUR *curp,PSD_ENT *ep)
{
	PSD_REC	*rp ;
	int		rs = SR_OK ;
	int		hl = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	    rs = bcontrol(&op->pfile,BC_LOCKREAD,TO_LOCK) ;
	    op->fl.readlocked = (rs >= 0) ;
	}

	if (rs >= 0) {
	    if ((rs = pingstatdb_checkcache(op)) >= 0) {
	        const int	i = (curp->i < 0) ? 0 : (curp->i + 1) ;
	        if ((rs = vecitem_get(op->elp,i,&rp)) >= 0) {
	            curp->i = i ;
	            if (rp != nullptr) {
	                if (ep != nullptr) {
	                    rs = entry_load(ep,rp) ;
	                    hl = rs ;
	                } else {
	                    hl = strlen(ep->hostname) ;
			}
	            } /* end if (non-null) */
	        } /* end if (vecitem_get) */
	        if (rs < 0) {
	            op->fl.readlocked = false ;
	            op->fl.writelocked = false ;
	            bcontrol(&op->pfile,BC_UNLOCK,0) ;
	        }
	    } /* end if (pingstatdb_checkcache) */
	} /* end if (ok) */

	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (pingstatdb_enum) */


/* match on a hostname */
int pingstatdb_match(PSD *op,cchar *hostname,PSD_ENT *ep)
{
	PSD_REC	*rp ;
	int		rs = SR_OK ;
	int		hl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if (hostname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_match: name=%s\n",hostname) ;
#endif

	if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	    rs = bcontrol(&op->pfile,BC_LOCKREAD,TO_LOCK) ;
	    op->fl.readlocked = (rs >= 0) ;
	}

	if (rs >= 0) {
	    if ((rs = pingstatdb_checkcache(op)) >= 0) {

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_match: name=%s\n", hostname) ;
#endif

/* return SR_NOTFOUND if we fall off of the end */

	    if ((rs = pingstatdb_getrec(op,hostname,&rp)) >= 0) {
	        if (rp != nullptr) {
	            if (ep != nullptr) {
	                rs = entry_load(ep,rp) ;
	                hl = rs ;
	            } else {
	                hl = strlen(ep->hostname) ;
	            }
	        } /* end if (non-null) */
	    } /* end if (pingstatdb_getrec) */

#if	CF_UNLOCK
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    bcontrol(&op->pfile,BC_UNLOCK,0) ;
#endif /* CF_UNLOCK */

	    } else {
	        op->fl.readlocked = false ;
	        op->fl.writelocked = false ;
	        bcontrol(&op->pfile,BC_UNLOCK,0) ;
	    } /* end if (pingstatdb_checkcache) */
	} /* end if (ok) */

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_match: ret rs=%d hl=%u\n",rs,hl) ;
#endif

	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (pingstatdb_match) */


/* update an entry */
int pingstatdb_update(PSD *op,cchar *hostname,int f_up,time_t timestamp)
{
	dater		d ;
	const time_t	daytime = time(nullptr) ;
	int		rs = SR_OK ;
	int		f_changed = false ;

	if (op == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if (hostname[0] == '\0') return SR_INVALID ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_update: hostname=%s\n",hostname) ;
#endif

	if (! op->fl.writable) {
	    rs = SR_BADF ;	/* not open for writing */
	    goto ret0 ;
	}

	if (op->fl.readlocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    bcontrol(&op->pfile,BC_UNLOCK,0) ;
	}

	if (! op->fl.writelocked) {
	    rs = bcontrol(&op->pfile,BC_LOCKWRITE,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	} /* end if (we did not already have a lock on the file) */

	if (rs < 0)
	    goto ret0 ;

	rs = pingstatdb_checkcache(op) ;
	if (rs < 0)
	    goto ret1 ;

	op->now.time = daytime ;
	rs = dater_start(&d,&op->now,op->zname,-1) ;
	if (rs < 0)
	    goto ret1 ;

	if ((timestamp == 0) || (timestamp > daytime))
	    timestamp = daytime ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("pingstatdb_update: about to match\n") ;
	    rs = vecitem_count(op->elp) ;
	    DEBUGPRINTF("pingstatdb_update: entries in cache %d\n",rs) ;
	}
#endif

	rs = pingstatdb_updrec(op,daytime,&d,hostname,f_up,timestamp) ;
	f_changed = (rs > 0) ;

/* update the LASTUPDATE record */

	if (rs >= 0)
	    rs = pingstatdb_updrec(op,daytime,&d,LASTUPDATE,f_up,timestamp) ;

/* udpate our last modification time to keep our cache current */

	op->mtime = daytime ;
	bcontrol(&op->pfile,BC_SYNC,0) ;

	dater_finish(&d) ;

/* unlock it */
ret1:
	op->fl.writelocked = false ;
	bcontrol(&op->pfile,BC_UNLOCK,0) ;

ret0:

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_update: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (pingstatdb_update) */


/* write an entry */
int pingstatdb_uptime(PSD *op,cchar *hostname,PSD_UP *up)
{
	PSD_REC	e, *rp ;
	dater		cd, ud, *cdp ;
	off_t	boff ;
	const time_t	daytime = time(nullptr) ;
	time_t		ptime = 0 ;
	uint		timestamp ;
	uint		ctimeange ;
	uint		roff ;
	int		rs = SR_OK ;
	int		size ;
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
	    bcontrol(&op->pfile,BC_UNLOCK,0) ;
	}

	if (! op->fl.writelocked) {
	    rs = bcontrol(&op->pfile,BC_LOCKWRITE,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	} /* end if (we did not already have a lock on the file) */

	if (rs < 0)
	    goto ret0 ;

	rs = pingstatdb_checkcache(op) ;
	if (rs < 0)
	    goto ret1 ;

	op->now.time = daytime ;
	rs = dater_start(&ud,&op->now,op->zname,-1) ;
	if (rs < 0)
	    goto ret1 ;

	rs = dater_start(&cd,&op->now,op->zname,-1) ;
	if (rs < 0)
	    goto ret2 ;

	timestamp = up->timestamp ;
	if ((up->timestamp == 0) || (up->timestamp > daytime))
	    timestamp = daytime ;

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
	        if (up->ctimeange == 0)
	            ctimeange = daytime ;

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
	        (((daytime - ptime) > TO_MINUPDATE) && f_greater)) {

	        boff = rp->roff ;
	        bseek(&op->pfile,boff,SEEK_SET) ;

	        rs = record_write(rp,&op->pfile,cdp,&ud,up->count,f_up) ;

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

	    if ((ctimeange = up->ctimeange) == 0)
	        ctimeange = daytime ;

	    dater_settimezn(&cd,(time_t) ctimeange,op->zname,-1) ;

	    f_changed = true ;
	    bseek(&op->pfile,0L,SEEK_END) ;

	    btell(&op->pfile,&boff) ;
	    roff = boff ;

	    if ((rs = record_start(&e,nowp,zn,roff,hn,&ud)) >= 0) {
	        f_rec = true ;
	    }

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: record_start() rs=%d\n",rs) ;
	    DEBUGPRINTF("pingstatedb_uptime: hostname=%s\n", hostname) ;
#endif

	    if (rs >= 0) {
	        rs = record_write(&e,&op->pfile,&ud,&cd,up->count,f_up) ;
	    }

#if	CF_DEBUG
	    DEBUGPRINTF("pingstatedb_uptime: record_write() rs=%d\n",
	        rs) ;
#endif

	    if (rs >= 0) {
	        size = sizeof(PSD_REC) ;
	        rs = vecitem_add(op->elp,&e,size) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&e) ;
	    }
	} /* end if (target entry) */

/* update the LASTUPDATE entry */

	if (rs >= 0) {
	    rs = pingstatdb_updrec(op,daytime,&cd,LASTUPDATE,f_up,timestamp) ;
	}

/* udpate our last modification time to keep our cache current */

	op->mtime = daytime ;
	bcontrol(&op->pfile,BC_SYNC,0) ;

	dater_finish(&cd) ;

ret2:
	dater_finish(&ud) ;

/* unlock it */
ret1:
	op->fl.writelocked = false ;
	bcontrol(&op->pfile,BC_UNLOCK,0) ;

ret0:

#if	CF_DEBUG
	DEBUGPRINTF("pingstatedb_uptime: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (pingstatdb_uptime) */


int pingstatdb_check(PSD *op,time_t daytime)
{
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magval != PSD_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0)
	    daytime = time(nullptr) ;

	if (op->fl.readlocked) {
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	    bcontrol(&op->pfile,BC_UNLOCK,0) ;
	}

	if (! op->fl.writelocked) {
	    rs = bcontrol(&op->pfile,BC_LOCKWRITE,TO_LOCK) ;
	    op->fl.writelocked = (rs >= 0) ;
	}

	if (rs >= 0) {
	    rs = pingstatdb_checkcache(op) ;
	    op->fl.writelocked = false ;
	    bcontrol(&op->pfile,BC_UNLOCK,0) ;
	}

	return rs ;
}
/* end subroutine (pingstatdb_check) */


/* local subroutines */


/* check on the status of the file entry cache */
local int pingstatdb_checkcache(PSD *op)
{
	ustat	sb ;
	int		rs ;
	int		f_cached = op->fl.cached ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_checkcache: ent f_cached=%d\n",
	    f_cached) ;
#endif

	if ((rs = bcontrol(&op->pfile,BC_STAT,&sb)) >= 0) {
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

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_checkcache: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pingstatdb_checkcache) */


/* read file entries */
local int pingstatdb_readrecords(PSD *op)
{
	int		rs ;
	int		c = 0 ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_readrecords: ent\n") ;
#endif

	if ((rs = brewind(&op->pfile)) >= 0) {
	    PSD_REC	e ;
	    uint	roff = 0 ;
	    const int	size = sizeof(PSD_REC) ;
	    const int	rlen = BUFLEN ;
	    int		line = 1 ;
	    int		len ;
	    int		bl ;
	    int		f_eol ;
	    int		f_bol = true ;
	    char	rbuf[BUFLEN + 1] ;
	    while ((rs = breadln(&op->pfile,rbuf,rlen)) > 0) {
	        len = rs ;

	        bl = (len - 1) ;
	        f_eol = (rbuf[bl] == '\n') ;

	        rbuf[bl] = '\0' ;
	        if (f_bol && (bl > RF_LEAD)) {
	            TIMEB	*nowp = &op->now ;
	            cchar	*zn = op->zname ;

#if	CF_DEBUG
	            DEBUGPRINTF("pingstatdb_readrecords: line=%u\n",line) ;
#endif

	            if ((rs = record_startbuf(&e,nowp,zn,roff,rbuf,bl)) >= 0) {
	                c += 1 ;
	                rs = vecitem_add(op->elp,&e,size) ;
	                if (rs < 0)
	                    record_finish(&e) ;
	            } /* end if (record_startbuf) */

	        } /* end if (a live one) */

	        roff += len ;
	        line += 1 ;
	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while (reading file records) */
	} /* end if (brewind) */

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_readrecords: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (pingstatdb_readrecords) */


/* free up the entries in the cache */
local int pingstatdb_fes(PSD *op)
{
	PSD_REC	*ep ;
	VECITEM		*elp = op->elp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

/* delete for an uncompacted vector */

	for (i = 0 ; vecitem_get(elp,i,&ep) >= 0 ; i += 1) {
	    if (ep != nullptr) {
	        record_finish(ep) ;
	        vecitem_del(op->elp,i) ;
	    }
	} /* end for */

/* delete for a compacted vector */

	i = 0 ;
	while ((rs1 = vecitem_get(elp,i,&ep)) >= 0) {
	    if (ep != nullptr) {
	        record_finish(ep) ;
	        vecitem_del(elp,i) ;
	    } else {
	        i += 1 ;
	    }
	} /* end while */
	if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_fes: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pingstatdb_fes) */


local int pingstatdb_updrec(op,dt,dp,hostname,f_up,timestamp)
PINGSTATDB	*op ;
time_t		dt ;
dater		*dp ;
cchar	hostname[] ;
int		f_up ;
time_t		timestamp ;
{
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
	        bseek(&op->pfile,boff,SEEK_SET) ;

	        rs = record_update(rp,&op->pfile,dp,f_up) ;

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatdb_updrec: record_updrec() rs=%d\n",rs) ;
#endif

	    } /* end if (did the update) */

	} else if (rs == SR_NOTFOUND) {
	    PSD_REC	r ;
	    uint	roff ;
	    const int	size = sizeof(PSD_REC) ;
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
	        bseek(&op->pfile,0L,SEEK_END) ;

	        btell(&op->pfile,&boff) ;
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
	        rs = record_update(&r,&op->pfile,dp,f_up) ;

#if	CF_DEBUG
	        DEBUGPRINTF("pingstatdb_updrec: record_update() rs=%d\n",
	            rs) ;
#endif

	    } /* end if */

	    if (rs >= 0) {
	        rs = vecitem_add(op->elp,&r,size) ;
	    }

	    if ((rs < 0) && f_rec) {
	        record_finish(&r) ;
	    }
	} /* end if (target entry) */

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb_updrec: ret rs=%d f_changed=%u\n",
	    rs,f_changed) ;
#endif

	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (pingstatdb_updrec) */


local int pingstatdb_getrec(PSD *op,cchar *hostname,
		PSD_REC **rpp)
{
	int		rs ;
	int		i ; /* return-value */
	for (i = 0 ; (rs = vecitem_get(op->elp,i,rpp)) >= 0 ; i += 1) {
	    if (*rpp == nullptr) continue ;
	    if (strcmp(hostname,(*rpp)->hostname) == 0) break ;
	} /* end for */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (pingstatdb_getrecord) */


/* initialize a fresh entry */
/* ARGSUSED */
local int record_start(ep,nowp,zname,roff,hostname,dp)
PSD_REC	*ep ;
TIMEB		*nowp ;
cchar	zname[] ;
uint		roff ;
cchar	hostname[] ;
dater		*dp ;
{
	int		rs ;
	int		hl ;

	if (ep == nullptr) return SR_FAULT ;
	if (zname == nullptr) return SR_FAULT ;
	if (hostname == nullptr) return SR_FAULT ;
	if (dp == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	DEBUGPRINTF("record_start: hostname=%s\n",hostname) ;
#endif

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("record_start: given time=%s\n",
	        timestr_logz(dp->b.time,timebuf)) ;
	}
#endif

	memset(ep,0,sizeof(PSD_REC)) ;
	ep->roff = roff ;
	ep->count = 1 ;
	ep->f_up = false ;

	if ((rs = dater_startcopy(&ep->cdate,dp)) >= 0) {
	    if ((rs = dater_startcopy(&ep->pdate,dp)) >= 0) {
	        hl = strlen(hostname) ;
	        ep->hostlen = hl ;
	        rs = uc_mallocstrw(hostname,hl,&ep->hostname) ;
	        if (rs < 0)
	            dater_finish(&ep->pdate) ;
	    } /* end if (dater_startcopy) */
	    if (rs < 0)
	        dater_finish(&ep->cdate) ;
	} /* end if (dater_startcopy) */

	return rs ;
}
/* end subroutine (record_start) */


/* initialize an entry from a buffer (w/ 'logz' string) */
local int record_startbuf(ep,nowp,zname,roff,buf,buflen)
PSD_REC	*ep ;
TIMEB		*nowp ;
cchar	zname[] ;
uint		roff ;
cchar	buf[] ;
int		buflen ;
{
	int		rs ;
	int		bl = buflen ;
	cchar	*bp = buf ;

	if (ep == nullptr) return SR_FAULT ;
	if (zname == nullptr) return SR_FAULT ;
	if (buf == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 1 name=%s\n",hostname) ;
#endif

	memset(ep,0,sizeof(PSD_REC)) ;

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 2 name=%s\n",hostname) ;
#endif

	rs = dater_start(&ep->cdate,nowp,zname,-1) ;
	if (rs < 0)
	    goto bad0 ;

	rs = dater_start(&ep->pdate,nowp,zname,-1) ;
	if (rs < 0)
	    goto bad1 ;

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 3 name=%s\n",hostname) ;
#endif

	ep->roff = roff ;
	ep->len = buflen ;
	rs = cfdeci(bp,RF_NUMDIGITS,&ep->count) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_NUMDIGITS + 1) ;
	bl -= (RF_NUMDIGITS + 1) ;
	rs = dater_setlogz(&ep->cdate,bp,bl) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_LOGZLEN + 1) ;
	bl -= (RF_LOGZLEN + 1) ;
	rs = dater_setlogz(&ep->pdate,bp,bl) ;
	if (rs < 0)
	    goto bad2 ;

	bp += (RF_LOGZLEN + 1) ;
	bl -= (RF_LOGZLEN + 1) ;
	if (bl <= 0) {
	    rs = SR_INVALID ;
	    goto bad2 ;
	}

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 4 name=%s\n",hostname) ;
#endif

	ep->f_up = (toupper(*bp) == 'U') ;

	bp += (RF_UPSTAT + 1) ;
	bl -= (RF_UPSTAT + 1) ;
	if (bl <= 0) {
	    rs = SR_INVALID ;
	    goto bad2 ;
	}

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 4a name=%s\n",hostname) ;
	DEBUGPRINTF("record_startbuf: bl=%u ext_name=%r\n",bl,bp,bl) ;
#endif

	ep->hostlen = bl ;
	rs = uc_mallocstrw(bp,bl,&ep->hostname) ;
	if (rs < 0)
	    goto bad2 ;

#if	CF_DEBUG
	DEBUGPRINTF("record_startbuf: 5 name=%s\n",hostname) ;
#endif

ret0:
	return rs ;

/* bad stuff */
bad2:
	dater_finish(&ep->pdate) ;

bad1:
	dater_finish(&ep->cdate) ;

bad0:
	goto ret0 ;
}
/* end subroutine (record_startbuf) */


/* update this entry to the file */
local int record_update(ep,fp,dp,f_up)
PSD_REC	*ep ;
bfile		*fp ;
dater		*dp ;
int		f_up ;
{
	int		rs = SR_OK ;
	char		cdate[RF_LOGZLEN + 2] ;
	char		pdate[RF_LOGZLEN + 2] ;

	if (ep == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("record_update: ent, host=%s\n",ep->hostname) ;
	    DEBUGPRINTF("record_update: cur_date=%s\n",
	        timestr_log(dp->b.time,timebuf)) ;
	}
#endif

	if (! LEQUIV(ep->f_up,f_up)) {

#if	CF_DEBUG
	    DEBUGPRINTF("record_update: changed status\n") ;
#endif

	    ep->f_up = f_up ;
	    ep->count = 1 ;
	    dater_setcopy(&ep->cdate,dp) ;

#if	CF_DEBUG
	    {
	        char	timebuf[TIMEBUFLEN + 1] ;
	        DEBUGPRINTF("record_update: status change cdate=%s\n",
	            timestr_logz(ep->cdate.b.time,timebuf)) ;
	    }
#endif

	} else {
	    ep->count += 1 ;
	}

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("record_update: cdate=%s\n",
	        timestr_logz(ep->cdate.b.time,timebuf)) ;
	}
#endif

	dater_mklogz(&ep->cdate,cdate,(RF_LOGZLEN + 1)) ;

#if	CF_DEBUG
	DEBUGPRINTF("record_update: cdate mklogz=%s\n",
	    cdate) ;
#endif

/* always update the last-update-date for the record */

	dater_setcopy(&ep->pdate,dp) ;

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("record_update: pdate=%s\n",
	        timestr_logz(ep->pdate.b.time,timebuf)) ;
	}
#endif

	dater_mklogz(&ep->pdate,pdate,RF_LOGZLEN + 1) ;

#if	CF_DEBUG
	{
	    DEBUGPRINTF("record_update: pdate mklogz=%s\n",
	        pdate) ;
	}
#endif

	rs = bprintf(fp,"%*d %-*s %-*s %c %s\n",
	    RF_NUMDIGITS,ep->count,
	    RF_LOGZLEN,cdate,
	    RF_LOGZLEN,pdate,
	    ((ep->f_up) ? 'U' : 'D'),
	    ep->hostname) ;

	return rs ;
} /* end subroutine (record_update) */


/* write out this entry to the file */
local int record_write(ep,fp,cp,dp,count,f_up)
PSD_REC	*ep ;
bfile		*fp ;
dater		*cp ;
dater		*dp ;
int		count ;
int		f_up ;
{
	int		rs = SR_OK ;
	char		cdate[RF_LOGZLEN + 2] ;
	char		pdate[RF_LOGZLEN + 2] ;

	if (ep == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	{
	    char	timebuf[TIMEBUFLEN + 1] ;
	    DEBUGPRINTF("record_write: ent host=%s\n",ep->hostname) ;
	    DEBUGPRINTF("record_write: count=%d\n",count) ;
	    DEBUGPRINTF("record_write: cur_date=%s\n",
	        timestr_log(dp->b.time,timebuf)) ;
	}
#endif /* CF_DEBUG */

	ep->f_up = f_up ;
	if (count != 0) {
	    ep->count = count ;
	} else {
	    ep->count += 1 ;
	}

/* the "change" date */

	dater_setcopy(&ep->cdate,cp) ;

	dater_mklogz(&ep->cdate,cdate,(RF_LOGZLEN + 1)) ;

/* always update the last-update-date for the record */

	dater_setcopy(&ep->pdate,dp) ;

	dater_mklogz(&ep->pdate,pdate,RF_LOGZLEN + 1) ;

/* pop it */

	rs = bprintf(fp,"%*d %-*s %-*s %c %s\n",
	    RF_NUMDIGITS,ep->count,
	    RF_LOGZLEN,cdate,
	    RF_LOGZLEN,pdate,
	    ((ep->f_up) ? 'U' : 'D'),
	    ep->hostname) ;

	return rs ;
}
/* end subroutine (record_write) */


/* free up an entry */
local int record_finish(PSD_REC *ep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep == nullptr) return SR_FAULT ;

	if (ep->hostname != nullptr) {
	    rs1 = uc_free(ep->hostname) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->hostname = nullptr ;
	} /* end if */

	rs1 = dater_finish(&ep->cdate) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = dater_finish(&ep->pdate) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (record_finish) */

local int entry_load(PSD_ENT *ep,PSD_REC *rp)
{
	int		rs = SR_OK ;
	int		hl = 0 ;

	if (ep == nullptr) return SR_FAULT ;
	if (rp == nullptr) return SR_FAULT ;

	ep->ti_change = 0 ;
	ep->ti_ping  = 0 ;
	ep->count = 0 ;
	ep->f_up = 0 ;
	ep->hostname[0] = '\0' ;

	if (rs >= 0)
	    rs = dater_gettime(&rp->pdate,&ep->ti_ping) ;

	if (rs >= 0)
	    rs = dater_gettime(&rp->cdate,&ep->ti_change) ;

	if (rs >= 0) {
	    ep->count = rp->count ;
	    ep->f_up = rp->f_up ;
	    rs = mkpath1(ep->hostname,rp->hostname) ;
	    hl = rs ;
	}

	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (entry_load) */


/* make the open mode string for BIO, remember O_RDONLY is the fake */
int mkbstr(mode_t omode,char *ostr)
{
	char		*bp = ostr ;

#if	CF_DEBUG
	int		f_read = true ;
#endif

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb/mkbstr: omode=%04o\n",omode) ;
#endif

	if ((omode & O_RDWR) || (omode & O_WRONLY)) {

	    *bp++ = 'w' ;
#if	CF_DEBUG
	    if ((omode & O_WRONLY) && (! (omode & O_RDWR)))
	        f_read = false ;
#endif

	}

#if	CF_DEBUG
	DEBUGPRINTF("pingstatdb/mkbstr: f_read=%d\n",f_read) ;
#endif

	if (omode & O_APPEND)
	    *bp++ = 'a' ;

	if (omode & O_CREAT)
	    *bp++ = 'c' ;

	if (omode & O_TRUNC)
	    *bp++ = 't' ;

#ifdef	COMMENT
	if (f_read)
	    *bp++ = 'r' ;
#else
	*bp++ = 'r' ;		/* ALWAYS need for locking */
#endif /* COMMENT */

	*bp = '\0' ;
	return (bp - ostr) ;
}
/* end subroutine (mkbstr) */


