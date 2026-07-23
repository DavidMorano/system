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
#include	<ugetx.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<dater.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<initnow.h>		/* LIBUC */
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
#define	PSD_MAG		PINGSTATDB_MAGIC

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
	    if (bfile *pfp = new(nt) bfile ; pfp) {
		op->pfp = pfp ;
	        if (vechand *rlp = new(nt) vechand ; rlp) {
		    op->rlp = rlp ;
	            if (timeb *nowp = new(nt) timeb ; nowp) {
		        rs = SR_OK ;
		    } /* end if (new-timeb) */
		    if (rs < 0) {
			delete rlp ;
			op->rlp = nullptr ;
		    } /* end if (error) */
	        } /* end if (new-vecelem) */
	        if (rs < 0) {
		    delete pfp ;
		    op->pfp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-bfile) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pingstatdb_ctor) */

local int pingstatdb_dtor(pingstatdb *op) noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (vechand *rlp = resumelife<vechand>(op->rlp) ; rlp) {
	        delete rlp ;
	        op->rlp = nullptr ;
	    }
	    if (bfile *pfp = resumelife<bfile>(op->pfp) ; pfp) {
		delete pfp ;
		op->pfp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (pingstatdb_dtor) */

local int	pingstatdb_opener(pingstatdb *) noex ;

local int	entry_load(PSD_ENT *,PSD_REC *) noex ;

local int	mkbstr(char *,int) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_create	= CF_CREATE ;
cbool		f_unlock	= CF_UNLOCK ;


/* exported variables */


/* exported subroutines */

int pingstatdb_open(PSD *op,cchar *fname,int omode,mode_t fperm) noex {
	int		rs ;
	DEBUGPRINTF("ent omode=%4o fname=%s\n",omode,fname) ;
	if (omode & O_CREAT) {
	    DEBUGPRINTF("creating as needed\n") ;
	}
	if ((rs = pingstatdb_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		if ((rs = pingstatdb_cominit(op)) >= 0) {
	            char	bstr[10] ;
	            if_constexpr (f_create) {
		        omode |= O_CREAT ;
	            }
	            if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) {
		        bfile *pfp = op->pfp ;
	                op->fname = cp ;
	                op->fl.writable = isaccmode.wr(omode) ;
	                mkbstr(bstr,omode) ;
	                if ((rs = pfp->open(op->fname,bstr,fperm)) >= 0) {
	                    cint	cmd = BC_CLOSEONEXEC ;
	                    if ((rs = pfp->control(cmd,true)) >= 0) {
			        vechand *rlp = op->rlp ;
	                        if ((rs = rlp->start(0,0)) >= 0) {
			            rs = pingstatdb_opener(op) ;
	                            if (rs < 0) {
	                                rlp->finish() ;
			            } /* end if (error) */
	                        } /* end if (vechand_start) */
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
	    } /* end if (pingstatdb_cominit) */
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
	    if ((rs = initnow(op->nowp,znbuf,znlen)) >= 0) {
		if (cchar *cp ; (rs = mem.strw(znbuf,rs,&cp)) >= 0) {
		    op->znbuf = cp ;
		    op->magval = PSD_MAG ;
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
	    if (vechand *rlp = op->rlp ; rlp) {
	        rs1 = rlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
		voidp vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->znbuf) {
	        voidp vp = voidp(op->znbuf) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->znbuf = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = pingstatdb_comfini(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = pingstatdb_dtor(op) ;
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
	            if ((rs = vechand_get(op->rlp,i,&vp)) >= 0) {
		        if (PSD_REC *rp = resumelife<PSD_REC>(vp) ; rp) {
	                    curp->i = i ;
	                    if (ep) {
	                        rs = entry_load(ep,rp) ;
	                        hl = rs ;
	                    } else {
	                        hl = lenstr(ep->hostbuf) ;
			    }
	                } /* end if (non-null) */
	            } /* end if (vechand_get) */
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

int pingstatdb_match(PSD *op,cchar *hn,PSD_ENT *ep) noex {
	int		rs ;
	int		hl = 0 ; /* return-value */
	DEBUGPRINTF("ent hn=%s\n",hn) ;
	if ((rs = pingstatdb_magic(op,hn)) >= 0) {
	    rs = SR_INVALID ;
	    if (hn[0]) {
		rs = SR_OK ;
	        if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	            rs = bcontrol(op->pfp,BC_LOCKREAD,TO_LOCK) ;
	            op->fl.readlocked = (rs >= 0) ;
	        }
	        if (rs >= 0) {
	            if ((rs = pingstatdb_checkcache(op)) >= 0) {
			/* return SR_NOTFOUND if we fall off of the end */
			PSD_REC	*rp ;
	    		if ((rs = pingstatdb_recget(op,hn,&rp)) >= 0) {
	        	    if (rp) {
	                        if (ep) {
	                            rs = entry_load(ep,rp) ;
	                            hl = rs ;
	                        } else {
	                            hl = lenstr(ep->hostbuf) ;
	                        }
	                    } /* end if (non-null) */
	                } /* end if (pingstatdb_recget) */
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

local int pingstatsb_updates(PSD *op,cc *hn,int f_up,time_t ts) noex {
	custime		dt = getustime ;
	timeb		*nowp = op->nowp ;
	int		rs ;
	int		rs1 ;
	int		fchanged = false ; /* return-value */
	nowp->time = dt ;
	if (dater d ; (rs = d.start(nowp,op->znbuf,-1)) >= 0) {
	    if ((ts == 0) || (ts > dt)) {
	        ts = dt ;
	    }
	    {
	        DEBUGPRINTF("about to match\n") ;
	        rs = vechand_count(op->rlp) ;
	        DEBUGPRINTF("entries in cache %d\n",rs) ;
	    }
	    if ((rs = pingstatdb_recupd(op,dt,&d,hn,f_up,ts)) >= 0) {
		cchar *luh = LASTUPDATE ;
	        fchanged = (rs > 0) ;
                /* update the LASTUPDATE record */
	        rs = pingstatdb_recupd(op,dt,&d,luh,f_up,ts) ;
                /* udpate our last modification time */
	        op->mtime = dt ;
	        bcontrol(op->pfp,BC_SYNC,0) ;
	    } /* end if (pingstatdb_recupd) */
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
	DEBUGPRINTF("ent hn=%s\n",hn) ;
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

int pingstatdb_check(PSD *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pingstatdb_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
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

local int entry_load(PSD_ENT *ep,PSD_REC *rep) noex {
	int		rs = SR_BUGCHECK ;
	int		hl = 0 ; /* return-value */
	if (ep && rep) {
	    dater *pdp = rep->pdp ;
	    dater *cdp = rep->cdp ;
	    ep->ti_change	= 0 ;
	    ep->ti_ping		= 0 ;
	    ep->cnt		= 0 ;
	    ep->f_up		= 0 ;
	    ep->hostbuf[0]	= '\0' ;
	    if ((rs = dater_gettime(pdp,&ep->ti_ping)) >= 0) {
	        if ((rs = dater_gettime(cdp,&ep->ti_change)) >= 0) {
	            ep->cnt = rep->cnt ;
	            ep->f_up = rep->f_up ;
	            rs = mkpath(ep->hostbuf,rep->hostbuf) ;
	            hl = rs ;
		} /* end if */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? hl : rs ;
} /* end subroutine (entry_load) */

/* make the open mode string for BIO, remember O_RDONLY is the fake */
int mkbstr(char *ostr,int omode) noex {
    	int		rl = -1 ; /* return-value */
	DEBUGPRINTF("omode=%04o\n",omode) ;
	if (ostr) {
	    char *bp = ostr ;
	    if (isaccmode.wr(omode)) {
	        *bp++ = 'w' ;
	    }
	    if (omode & O_APPEND)	*bp++ = 'a' ;
	    if (omode & O_CREAT)	*bp++ = 'c' ;
	    if (omode & O_TRUNC)	*bp++ = 't' ;
	    *bp++ = 'r' ;		/* ALWAYS need for locking */
	    *bp = '\0' ;
	    rl = conv<int>(bp - ostr) ;
	} /* end if (non-null) */
	return rl ;
} /* end subroutine (mkbstr) */


