/* pingstatdb_prime SUPPORT */
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
		} /* end if (error) */
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

local int	pingstatdb_checkcache(PSD *) noex ;
local int	pingstatdb_readrecords(PSD *) noex ;
local int	pingstatdb_fes(PSD *) noex ;
local int	pingstatdb_getrec(PSD *,cchar *,PSD_REC **) noex ;
local int	pingstatdb_updrec(PSD *,time_t,dater *,cchar *,
			int,time_t) noex ;

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

int pingstatdb_open(PSD *op,cchar *fname,mode_t omode,int fperm) noex {
	int		rs ;
	DEBUGPRINTF("ent omode=%4o fname=%s\n",omode,fname) ;
	if (omode & O_CREAT) {
	    DEBUGPRINTF("creating as needed\n") ;
	}
	if ((rs = pingstatdb_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        char	bstr[10] ;
	        if_constexpr (f_create) {
		    omode |= O_CREAT ;
	        }
	        if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) {
		    bfile *pfp = op->pfp ;
	            op->fname = cp ;
	            op->fl.writable = isaccmode.wr(omode) ;
	            mkbstr(omode,bstr) ;
	            if ((rs = pfp->open(op->fname,bstr,fperm)) >= 0) {
	                cint	cmd = BC_CLOSEONEXEC ;
	                if ((rs = bfp->control(cmd,true)) >= 0) {
			    vecitem *elp = op->elp ;
	                    if ((rs = elp->start(0,0)) >= 0) {
			        rs = pingstatdb_opener(op) ;
	                        if (rs < 0) {
	                            elp->finish() ;
			        } /* end if (error) */
	                    } /* end if (vecitem_start) */
	                } /* end if (bcontrol) */
	                if (rs < 0) {
	                    pfp->close() ;
		        } /* end if (error) */
	            } /* end if (bopen) */
	            if (rs < 0) {
			voidp vp = voidp(op->fname) ;
	                mem.free(vp) ;
	                op->fname = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pingstatdb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (pingstatdb_ctor) */
	return rs ;
} /* end subroutine (pingstatdb_open) */

local int pingstatdb_opener(pingstatdb *op) noex {
    	int		rs ;
	int		rs1 ;
	if (char *znbuf ; (rs = mem.zn(&znbuf)) >= 0) {
	    cint	znlen = rs ;
	    if ((rs = initnow(&op->now,znbuf,znlen)) >= 0) {
		if (char *a ; (rs = mem.strw(znbuf,rs,&a)) >= 0) {
		    op->zname = a ;
		    op->magval = PSD_MAGIC ;
		} /* end if (memory-acquire) */
	    } /* end if (initnow) */
	    rs1 = mem.free(znbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (pingstatdb_opener) */

int pingstatdb_close(PSD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pingstatdb_magic(op)) >= 0) {
	    {
	        rs1 = pingstatdb_fes(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (bfile *pfp = op->pfp ; pfp) {
	        rs1 = pfp->close ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (vecitem *elp = op->elp ; elp) {
	        rs1 = elp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
		voidp vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->zname) {
	        rs1 = mem.free(op->zname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->zname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = pingstatb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (pingstatdb_magic) */
	return rs ;
} /* end subroutine (pingstatdb_close) */

int pingstatdb_curbegin(PSD *op,PSD_CUR *curp) noex {
    	int		rs ;
	if ((rs = pingstatdb_magic(op,curp)) >= 0) {
	    op->fl.cursor = true ;
	    curp->i = -1 ;
	} /* end if (pingstatdb_magic) */
	return rs ;
} /* end subroutine (pingstatdb_curbegin) */

int pingstatdb_curend(PSD *op,PSD_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pingstatdb_magic(op,curp)) >= 0) {
	    if (op->fl.readlocked || op->fl.writelocked) {
	        op->fl.readlocked = false ;
	        op->fl.writelocked = false ;
	        rs1 = bcontrol(op->pfp,BC_UNLOCK,0) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    op->fl.cursor = false ;
	    curp->i = -1 ;
	} /* end if (pingstatdb_magic) */
	return rs ;
} /* end subroutine (pingstatdb_curend) */

int pingstatdb_curenum(PSD *op,PSD_CUR *curp,PSD_ENT *ep) noex {
	int		rs ;
	int		hl = 0 ; /* return-value */
	if ((rs = pingstatdb_magic(op,curp)) >= 0) {
	    if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	        rs = bcontrol(op->pfp,BC_LOCKREAD,TO_LOCK) ;
	        op->fl.readlocked = (rs >= 0) ;
	    }
	    if (rs >= 0) {
	        if ((rs = pingstatdb_checkcache(op)) >= 0) {
	            cint	i = (curp->i < 0) ? 0 : (curp->i + 1) ;
		    void *vp ;
	            if ((rs = vecitem_get(op->elp,i,&vp)) >= 0) {
		        if (PSD_REC *rp = resumelife<PSD_REC>(vp) ; rp) {
	                    curp->i = i ;
	                    if (ep) {
	                        rs = entry_load(ep,rp) ;
	                        hl = rs ;
	                    } else {
	                        hl = lenstr(ep->hostname) ;
			    }
	                } /* end if (non-null) */
	            } /* end if (vecitem_get) */
	            if (rs < 0) {
	                op->fl.readlocked = false ;
	                op->fl.writelocked = false ;
	                bcontrol(op->pfp,BC_UNLOCK,0) ;
	            } /* end if (error) */
	        } /* end if (pingstatdb_checkcache) */
	    } /* end if (ok) */
	} /* end if (pingstatdb_magic) */
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (pingstatdb_curenum) */

int pingstatdb_match(PSD *op,cchar *hostname,PSD_ENT *ep) noex {
	int		rs ;
	int		hl = 0 ; /* return-value */
	DEBUGPRINTF("ent name=%s\n",hostname) ;
	if ((rs = pingstatdb_magic(op,hostname)) >= 0) {
	    rs = SR_INVALID ;
	    if (hostname[0]) {
		rs = SR_OK ;
	        if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	            rs = bcontrol(op->pfp,BC_LOCKREAD,TO_LOCK) ;
	            op->fl.readlocked = (rs >= 0) ;
	        }
	        if (rs >= 0) {
	            if ((rs = pingstatdb_checkcache(op)) >= 0) {
			DEBUGPRINTF("name=%s\n", hostname) ;
			/* return SR_NOTFOUND if we fall off of the end */
			PSD_REC	*rp ;
	    		if ((rs = pingstatdb_getrec(op,hostname,&rp)) >= 0) {
	        	    if (rp) {
	                        if (ep) {
	                            rs = entry_load(ep,rp) ;
	                            hl = rs ;
	                        } else {
	                            hl = lenstr(ep->hostname) ;
	                        }
	                    } /* end if (non-null) */
	                } /* end if (pingstatdb_getrec) */
	                if_constexpr (f_unlock) {
	    		    op->fl.readlocked = false ;
	    		    op->fl.writelocked = false ;
	    		    bcontrol(op->pfp,BC_UNLOCK,0) ;
	                } /* end if_constexxpr (f_unlock) */
	           } else {
	               op->fl.readlocked = false ;
	               op->fl.writelocked = false ;
	               bcontrol(op->pfp,BC_UNLOCK,0) ;
	           } /* end if (pingstatdb_checkcache) */
	       } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (pingstatdb_magic) */
	DEBUGPRINTF("ret rs=%d hl=%u\n",rs,hl) ;
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (pingstatdb_match) */

local int pingstatsb_updates(PSD *op,cc *hn,int f_up,cchar *hn,time_t ts) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	int		rs1 ;
	int		fchanged = false ; /* return-value */
	op->now.time = dt ;
	if ((dater d ; rs = d.start(&op->now,op->zname,-1)) >= 0) {
	    if ((ts == 0) || (ts > dt)) {
	        ts = dt ;
	    }
	    {
	        DEBUGPRINTF("about to match\n") ;
	        rs = vecitem_count(op->elp) ;
	        DEBUGPRINTF("entries in cache %d\n",rs) ;
	    }
	    if ((rs = pingstatdb_updrec(op,dt,&d,hn,f_up,ts)) >= 0) {
		cchar *luh = LASTUPDATE ;
	        f_changed = (rs > 0) ;
                /* update the LASTUPDATE record */
	        rs = pingstatdb_updrec(op,dt,&d,luh,f_up,ts) ;
                /* udpate our last modification time */
	        op->mtime = dt ;
	        bcontrol(op->pfp,BC_SYNC,0) ;
	    } /* end if (pingstatdb_updrec) */
	    rs1 = dater_finish(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dater) */
	op->fl.writelocked = false ;
	bcontrol(op->pfp,BC_UNLOCK,0) ;
	return (rs >= 0) ? fchanged : rs ;
} /* end subroutine (pingstatdb_updates) */

int pingstatdb_update(PSD *op,cchar *hn,int f_up,time_t ts) noex {
	int		rs ;
	int		f_changed = false ; /* return-value */
	DEBUGPRINTF("ent hostname=%s\n",hn) ;
	if ((rs = pingstatdb_magic(op,hn)) >= 0) {
	    rs = SR_INVALID ;
	    if (hn[0]) {
		rs = SR_BADF ;
	        if (op->fl.writable) {
		    rs = SR_OK ;
	            if (op->fl.readlocked) {
	                op->fl.readlocked = false ;
	                op->fl.writelocked = false ;
	                bcontrol(op->pfp,BC_UNLOCK,0) ;
	            }
		    if (! op->fl.writelocked) {
	               rs = bcontrol(op->pfp,BC_LOCKWRITE,TO_LOCK) ;
	    	       op->fl.writelocked = (rs >= 0) ;
		    } /* end if (we did not already have a lock on the file) */
		    if (rs >= 0) {
			if ((rs = pingstatdb_checkcache(op)) >= 0) {
			    rs = pingstatsb_updates(op,hn,f_up,ts) ;
			    f_changed = rs ;
			}
		    } /* end if (ok) */
		} /* end if (writeable) */
	    } /* end if (valid) */
	} /* end if (pingstatdb_magic) */
	DEBUGPRINTF("ret rs=%d f_changed=%u\n", rs,f_changed) ;
	return (rs >= 0) ? f_changed : rs ;
} /* end subroutine (pingstatdb_update) */

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

int pingstatdb_check(PSD *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pingstatdb_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if (op->fl.readlocked) {
	        op->fl.readlocked = false ;
	        op->fl.writelocked = false ;
	        bcontrol(op->pfp,BC_UNLOCK,0) ;
	    }
	    if (! op->fl.writelocked) {
	        rs = bcontrol(op->pfp,BC_LOCKWRITE,TO_LOCK) ;
	        op->fl.writelocked = (rs >= 0) ;
	    }
	    if (rs >= 0) {
	        {
	            rs1 = pingstatdb_checkcache(op) ;
	            if (rs >= 0) rs = rs1 ;
	            op->fl.writelocked = false ;
	        }
	        {
	            rs1 = bcontrol(op->pfp,BC_UNLOCK,0) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (ok) */
	} /* end if (pingstatdb_magic) */
	return rs ;
} /* end subroutine (pingstatdb_check) */


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

local int entry_load(PSD_ENT *ep,PSD_REC *rp) noex {
	int		rs = SR_BUGCHECK ;
	int		hl = 0 ; /* return-value */
	if (ep && rp) {
	    dater *pdp = &rp->pdate ;
	    dater *cdp = &rp->cdate ;
	    ep->ti_change = 0 ;
	    ep->ti_ping  = 0 ;
	    ep->count = 0 ;
	    ep->f_up = 0 ;
	    ep->hostname[0] = '\0' ;
	    if ((rs dater_gettime(pdp,&ep->ti_ping)) >= 0) {
	        if ((rs = dater_gettime(cdp,&ep->ti_change)) >= 0) {
	            ep->count = rp->count ;
	            ep->f_up = rp->f_up ;
	            rs = mkpath1(ep->hostname,rp->hostname) ;
	            hl = rs ;
		} /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (entry_load) */

/* make the open mode string for BIO, remember O_RDONLY is the fake */
int mkbstr(mode_t omode,char *ostr) noex {
	char		*bp = ostr ;
	DEBUGPRINTF("omode=%04o\n",omode) ;
	if (isaccmode.wr(omode)) {
	    *bp++ = 'w' ;
	}
	if (omode & O_APPEND)	*bp++ = 'a' ;
	if (omode & O_CREAT)	*bp++ = 'c' ;
	if (omode & O_TRUNC)	*bp++ = 't' ;
	*bp++ = 'r' ;		/* ALWAYS need for locking */
	*bp = '\0' ;
	return (bp - ostr) ;
} /* end subroutine (mkbstr) */


