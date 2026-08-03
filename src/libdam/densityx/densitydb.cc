/* densitydb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage reading or writing of a density file */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */

/* revision history:

	= 2004-02-17, David A­D­ Morano
	This code module was inspired from the mail-message-id
	database (which is used to eliminate repeated mail messages).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	densitydb

	Description:
	This module manages the reading and writing of entries in
	a density file.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<inttypes.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<netdb.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD */
#include	<bit>			/* C++STD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<serialbuf.h>		/* LIBUC */
#include	<mkfnamesuf.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<lockfile.h>		/* LIBUC */
#include	<isfiledesc.h>		/* LIBUC |isfsremote(3uc)| */
#include	<ismisc.h>		/* LIBUC |isaccmode(3uc)| */
#include	<isnot.h>		/* LIBUC */
#include	<iserror.h>		/* LIBUC */
#include	<msgsub.hh>		/* LIBDAM */
#include	<filemagic.hh>		/* LIBDAM */
#include	<localmisc.h>		/* LIBU |MIN| */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"densitydbe.h"
#include	"densitydb.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */
import filemagic ;
import densityhdr ;

/* local defines */

#define	DENSITYDB_FLID		filemagic_sz
#define	DENSITYDB_FLHEAD	dbhdr::bufsz
#define	DENSITYDB_FLTOP		(DENSITYDB_FLID + DENSITYDB_FLHEAD)

#define	DENSITYDB_FOID		0
#define	DENSITYDB_FOHEAD	(DENSITYDB_FOID + DENSITYDB_FLID)
#define	DENSITYDB_FOTAB		(DENSITYDB_FOHEAD + DENSITYDB_FLHEAD)

#define	DENSITYDB_EDSZ		((var.entsz + 3) & (~ 3))
#define	DENSITYDB_BUFSZ		(64 * 1024)
#define	DENSITYDB_READSZ	(16 * 1024)

#define	DENSITYDB_FBUFLEN	(DENSITYDB_FLTOP + 9)

#define	DD		densitydb
#define	DD_ENT		densitydb_ent
#define	DD_CUR		densitydb_cur
#define	FM		filemagic
#define	FM_SZ		filemagic_sz
#define	FH		dbhdr

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		10		/* seconds */

#ifndef	CF_CREAT
#define	CF_CREAT	0		/* always create the file? */
#endif


/* imported namespaces */

using namespace densityx ;
using libuc::mem ;			/* variable */


/* local typedefs */

typedef const msgsub	cmsgsub ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		entsz ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct denbuf {
	char		*bup ;			/* fixed buffer pointer */
	int		bsz ;			/* fixed buffer size */
	int		off ;			/* file offset of valid area */
	int		len ;			/* length of valid area */
    } ; /* end struct (denbuf) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int densitydb_ctor(densitydb *op,Args ... args) noex {
    	DENSITYDB	*hop = op ;
	cnullptr	np{} ;
	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if (denbuf *dbp = new(nt) denbuf ; dbp) ylikely {
		op->dbp = dbp ;
	        if (dbhdr *fhp = new(nt) dbhdr ; fhp) ylikely {
		    op->fhp = fhp ;
		    rs = SR_OK ;
		} /* end if (new-dbhdr) */
		if (rs < 0) {
		    delete dbp ;
		    op->dbp = np ;
		} /* end if (error) */
	    } /* end if (new-denbuf) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (densitydb_ctor) */

local int densitydb_dtor(densitydb *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->fhp) ylikely {
		dbhdr *fhp = resumelife<dbhdr>(op->fhp) ;
		delete fhp ;
		op->fhp = nullptr ;
	    } /* end if (memory-release) */
	    if (op->dbp) ylikely {
		denbuf *dbp = resumelife<denbuf>(op->dbp) ;
		delete dbp ;
		op->dbp = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (densitydb_dtor) */

template<typename ... Args>
local inline int densitydb_magic(densitydb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == DENSITYDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (densitydb_magic) */

local inline int densitydb_locked(densitydb *op) noex {
    	return (op->fl.lockedrd || op->fl.lockedwr) ;
} /* end subroutine (densitydb_locked) */

local int densitydb_openbegin	(DD *,cchar *) noex ;
local int densitydb_opener	(DD *,char *,cchar *,int) noex ;
local int densitydb_openend	(DD *) noex ;

local int densitydb_fileopen	(DD *,time_t) noex ;
local int densitydb_fileclose	(DD *) noex ;
local int densitydb_fileinit	(DD *,time_t) noex ;
local int densitydb_filechanged	(DD *) noex ;
local int densitydb_filecheck	(DD *,time_t,cmsgsub) noex ;
local int densitydb_lockacq	(DD *,time_t,cmsgsub) noex ;
local int densitydb_lockrel	(DD *) noex ;
local int densitydb_bufget	(DD *,int,int,char **) noex ;
local int densitydb_bufbegin	(DD *) noex ;
local int densitydb_bufend	(DD *) noex ;
[[maybe_unused]] local int densitydb_writehead	(DD *) noex ;
[[maybe_unused]] local int densitydb_bufupdate	(DD *,int,int,cchar *) noex ;


/* local variables */

constexpr cpcchar	localfs[] = {
	"ufs",
	"tmpfs",
	"lofs",
	"pcfs",
	"hfs",
	"vxfs",
	nullptr
} ; /* end array */

constexpr cpcchar	suffixes[] = {
	DENSITYDB_FS,
	DENSITYDB_FSA,
	DENSITYDB_FSB
} ; /* end array */

static vars		var ;
cint			hlen		= DENSITYDB_FLTOP ;
cint			magsiz		= DENSITYDB_MAGSIZ ;
cchar			magstr[]	= DENSITYDB_MAGSTR ;
cbool			f_creat 	= CF_CREAT ;


/* exported variables */


/* exported subroutines */

local int densitydb_opens(DD *,cc *) noex ;

int densitydb_open(DD *op,cc *fname,int oflags,mode_t om,int maxent) noex {
	int		rs ;
	int		fcreate = false ; /* return-value */
	if ((rs = densitydb_ctor(op,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        if_constexpr (f_creat) {
	            oflags |= O_CREAT ;
	        }
	        op->oflags = (oflags &= (~ O_TRUNC)) ;
		if ((rs = ucpagesize) >= 0) ylikely {
	            op->pagesz = rs ;
		    if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
	        	op->om = om ;
	        	op->maxent = maxent ;
	        	op->entsz = uceil(var.entsz,szof(int)) ;
		        rs = densitydb_opens(op,fname) ;
			fcreate = (rs > 0) ;
		    } /* end if (vars) */
	        } /* end if (ucpagesize) */
	    } /* end if (valid) */
	    if (rs < 0) {
		densitydb_dtor(op) ;
	    } /* end if (error) */
	} /* end if (densitydb_ctor) */
	return (rs >= 0) ? fcreate : rs ;
} /* end subroutine (densitydb_open) */

local int densitydb_opens(DD *op,cc *fname) noex {
    	int		rs ;
	int		fcreate = false ;
        if ((rs = densitydb_bufbegin(op)) >= 0) ylikely {
            if ((rs = densitydb_openbegin(op,fname)) >= 0) ylikely {
		custime		dt = getustime ;
		fcreate = (rs > 0) ;
                if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) ylikely {
                    cint	am = (op->oflags & O_ACCMODE) ;
                    op->fname = charp(cp) ;
                    op->fl.writable = isaccmode.wr(am) ;
                    op->tiopen = dt ;
                    op->tiopen = dt ;
                    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) ylikely {
			csize fsize = size_t(sb.st_size) ;
                        op->timod = uint(sb.st_mtime) ;
                        op->filesz = intsat(fsize) ;
                        if ((rs = isfsremote(op->fd)) >= 0) {
                            op->fl.remote = (rs > 0) ;
                            if ((rs = densitydb_fileinit(op,dt)) >= 0) {
                                op->magval = DENSITYDB_MAGIC ;
                            }
                        } /* end if (isfsremote) */
                    } /* end if (stat) */
                    if (rs < 0) {
                        voidp vp = voidp(op->fname) ;
                        mem.free(vp) ;
                        op->fname = nullptr ;
                    } /* end if (error) */
                } /* end if (memory-release) */
                if (rs < 0) {
		    densitydb_openend(op) ;
                } /* end if (error) */
            } /* end if (densitydb_openbegin) */
            if (rs < 0) { /* end if (error) */
                densitydb_bufend(op) ;
            } /* end if (error) */
        } /* end if (densitydb_bufbegin) */
	return (rs >= 0) ? fcreate : rs ;
} /* end subroutine (densitydb_opens) */ /* end if (error) */

int densitydb_close(DD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = densitydb_magic(op)) >= 0) ylikely {
	    {
	        rs1 = densitydb_openend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = densitydb_bufend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = densitydb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (densitydb_close) */

int densitydb_count(DD *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = densitydb_magic(op)) >= 0) ylikely {
	    c = (op->filesz - DENSITYDB_FOTAB) / var.entsz ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (densitydb_count) */

int densitydb_curbegin(DD *op,DD_CUR *curp) noex {
    	int		rs ;
	if ((rs = densitydb_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
} /* end subroutine (densitydb_curbegin) */

int densitydb_curend(DD *op,DD_CUR *curp) noex {
    	int		rs ;
	if ((rs = densitydb_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
} /* end subroutine (densitydb_curend) */

local int densitydb_curenums(DD *,DD_CUR *,DD_ENT *) noex ;

int densitydb_curenum(DD *op,DD_CUR *curp,DD_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ; /* return-value */
	if ((rs = densitydb_magic(op,curp,ep)) >= 0) ylikely {
	    rs = SR_NOTFOUND ;
	    if (op->fl.fileinited) ylikely {
		rs = SR_LOCKFAIL ;
		if (! op->fl.cursorlockbroken) {
		    rs = densitydb_curenums(op,curp,ep) ;
		    ei = rs ;
		} /* end if (cursor-lock not broken) */
	    } /* end if (file-initialized) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (densitydb_curenum) */

local int densitydb_curenums(DD *op,DD_CUR *curp,DD_ENT *ep) noex {
	cmsgsub		fc = msgsub::rd ;
	custime		dt = getustime ;
	int		rs ;
	int		ei = 0 ; /* return-value */
	if ((rs = densitydb_filecheck(op,dt,fc)) >= 0) ylikely {
	    cint	fotab = DENSITYDB_FOTAB ;
	    ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	    rs = SR_EOF ;
	    if (op->fl.fileinited) ylikely {
		cint	eoff = uint(fotab + (ei * op->entsz)) ;
		rs = SR_NOTFOUND ;
		if ((eoff + op->entsz) <= op->filesz) {
		    char	*bp ;
		    if ((rs = densitydb_bufget(op,eoff,op->entsz,&bp)) >= 0) {
			if (rs < op->entsz) {
			    rs = SR_EOF ;
			}
		        if ((rs >= 0) && ep) {
	    	            rs = densitydbe_wr(ep,bp,var.entsz) ;
		        } /* end if */
		        if (rs >= 0) {
		            curp->i = ei ;
		            op->fl.cursoracc = true ;
		        } /* end if (ok) */
		    } /* end if (densitydb_bufget) */
		} /* end if (valid) */
	    } /* end if (file-initialized) */
	} /* end if (densirydb_filecheck) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (densitydb_curenums) */

local int densitydb_updates(DD *,time_t,int,DD_ENT *) noex ;

int densitydb_update(DD *op,time_t dt,int idx,DD_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ; /* return-value */
	if ((rs = densitydb_magic(op,ep)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (idx >= 0) ylikely {
		rs = SR_NOTFOUND ;
		if (op->fl.fileinited) {
		    cmsgsub	fc = msgsub::rd ;
		    if (dt == 0) dt = getustime ;
		    if ((rs = densitydb_filecheck(op,dt,fc)) >= 0) {
			rs = densitydb_updates(op,dt,idx,ep) ;
			ei = rs ;
		    }
		    /* update access time as appropriate */
		    if (dt == 0) dt = getustime ;
		    op->tiopen = dt ;
	        } /* end if (initialized) */
	    } /* end if (valid) */
        } /* end if (densitydb_magic) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (densitydb_update) */

local int densitydb_updates(DD *op,time_t dt,int idx,DD_ENT *ep) noex {
    	dbhdr		*fhp = resumelife<dbhdr>(op->fhp) ;
	densitydbe	m0{} ;
    	int		rs = SR_OK ;
	int		ei = idx ; /* return-value */
	cint		entsz = ep->entsz ;
	char		ebuf[entsz + 4] ;
	bool		f_addition = false ;
	bool		f_bufupdate = false ;
	/* continue with the search */
	if (int nents = int(fhp->nents) ; ei > (nents - 1)) {
	    ei = (nents - 1) ;
	}
	if (ep) {
	    m0 = *ep ;
	}
	m0.count += 1 ;
	m0.utime = uint(dt) ;
	if ((rs = m0.rd(ebuf,entsz)) >= 0) ylikely {
	    cint eoff = DENSITYDB_FOTAB + (ei * op->entsz) ;
	    cint wlen = rs ;
	    /* update the in-core file buffer */
	    if (f_bufupdate && op->fl.writable) {
	        rs = densitydb_bufupdate(op,eoff,wlen,ebuf) ;
	    }
	    if ((rs >= 0) && op->fl.writable) {
	        coff uoff = off_t(eoff) ;
	        /* write back this entry */
	        if ((rs = u_writep(op->fd,ebuf,wlen,uoff)) >= wlen) {
	            if (dt == 0) dt = getustime ;
	            fhp->wcount += 1 ;
	            fhp->wtime = uint(dt) ;
	            if (f_addition) {
	                fhp->nents += 1 ;
	                op->filesz += wlen ;
	            }
	            if ((rs = densitydb_writehead(op)) >= 0) {
	                if (op->fl.remote) {
			    u_fsync(op->fd) ;
		        } /* end if (ok) */
		    } /* end if (density_writehead) */
	        } /* end if (data write was successful) */
	    } /* end if (writing updated entry to file) */
	} /* end if (densitydbe_rd) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (densitydb_updates) */

int densitydb_check(DD *op,time_t dt) noex {
	int		rs ;
	if ((rs = densitydb_magic(op)) >= 0) ylikely {
	   if (op->fd >= 0) {
	       if (dt == 0) dt = getustime ;
	       if (! (op->fl.lockedrd || op->fl.lockedwr)) {
		   bool	f = false ;
	           f = f || ((dt - op->tiopen) > TO_ACCESS) ;
		   f = f || ((dt - op->tiopen) > TO_OPEN) ;
		   if (f) {
		       rs = densitydb_fileclose(op) ;
		   }
	       } /* end if (not locked) */
	   } /* end if (was open) */
        } /* end if (densitydb_magic) */
	return rs ;
} /* end subroutine (densitydb_check) */


/* private subroutines */

local int densitydb_openbegin(DD *op,cchar *fname) noex {
	mut int		of = (op->oflags & (~ O_CREAT)) ;
	int		rs ;
	int		rs1 ;
	int		fcreate = false ; /* return-value */
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) ylikely {
	    if ((rs = densitydb_opener(op,tbuf,fname,of)) >= 0) {
		rs = 0 ;
	    } else if (isNotPresent(rs)) {
	        of = op->oflags ;
	        if (of & O_CREAT) {
		    fcreate = true ;
	            rs = densitydb_opener(op,tbuf,fname,of) ;
	        }
	    } /* end if */
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fcreate : rs ;
} /* end subroutine (densitydb_openbegin) */

local int densitydb_opener(DD *op,char *tbuf,cchar *fname,int of) noex {
	int		rs ;
	cchar		*suf = DENSITYDB_FS ;
	cmode		om = op->om ;
	if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) ylikely {
	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	        op->fd = rs ;
	    } else if (isNotPresent(rs)) {
		suf = DENSITYDB_FSA ;
	        if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) {
	    	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	                op->fd = rs ;
	    	    } else if (isNotPresent(rs)) {
		        suf = DENSITYDB_FSB ;
	                if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) {
	    	    	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	              		op->fd = rs ;
	    	    	    }
			}
		    } /* end if */
		} /* end if (mkfnamesuf) */
	    }
	} /* end if (mkfnamesuf) */
	return rs ;
} /* end subroutine (densitydb_opener) */

local int densitydb_openend(DD *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	} /* end if (was open) */
	return rs ;
} /* end subroutine (densitydb_openend) */

local int densitydb_filecheck(DD *op,time_t dt,cmsgsub fc) noex {
	int		rs ;
	int		fch = false ;
	if (dt == 0) dt = getustime ;
	if ((rs = densitydb_fileopen(op,dt)) >= 0) ylikely {
	    if ((! op->fl.lockedrd) && (! op->fl.lockedwr)) {
	        if ((rs = densitydb_lockacq(op,dt,fc)) >= 0) {
	    	    rs = densitydb_filechanged(op) ;
	    	    fch = (rs > 0) ;
		    if (rs < 0) {
			densitydb_lockrel(op) ;
		    } /* end if (error) */
		} /* end if (densitydb_lockacq) */
	    } /* end if (capture lock) */
	} /* end if (densitydb_fileopen) */
	return (rs >= 0) ? fch : rs ;
} /* end subroutine (densitydb_filecheck) */

local int densitydb_fileinitz(DD *,time_t) noex ;
local int densitydb_fileinitn(DD *,time_t) noex ;

local int densitydb_fileinit(DD *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (op->filesz == 0) {
	    rs = densitydb_fileinitz(op,dt) ;
	    rv = rs ;
	} else if (op->filesz >= DENSITYDB_FOTAB) {
	    rs = densitydb_fileinitn(op,dt) ;
	    rv = rs ;
	} /* end if */
	{
	    if ((rs1 = densitydb_locked(op)) > 0) {
	        rs1 = densitydb_lockrel(op) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (was locked) */
	    if (rs >= 0) rs = rs1 ;
	} /* end block */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (densitydb_fileinit) */

local int densitydb_fileinitzw(DD *,time_t) noex ;

local int densitydb_fileinitz(DD *op,time_t dt) noex {
	int		rs ;
	int		rv = 0 ; /* return-value */
	bool		f_locked = false ;
	if ((rs = u_seek(op->fd,0z,SEEK_SET)) >= 0) ylikely {
	    op->fl.fileinited = false ;
	    if (op->fl.writable) {
	        if (! op->fl.lockedwr) {
		    cmsgsub	fwr = msgsub::wr ;
	            if ((rs = densitydb_lockacq(op,dt,fwr)) >= 0) {
	                op->fl.lockedwr = true ;
		    }
	        } /* end if (locking required) */
		/* write the file magic and header stuff */
		if (rs >= 0) {
		    rs = densitydb_fileinitzw(op,dt) ;
		    rv = rs ;
		} /* end if (ok) */
	    } /* end if (writing) */
	} /* end if (u_seek) */
	(void) f_locked ;
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (densitydb_fileinitz) */

local int densitydb_fileinitzw(DD *op,time_t dt) noex {
	dbhdr		*fhp = resumelife<dbhdr>(op->fhp) ;
	cint		flen = DENSITYDB_FLTOP ;
	int		rs ;
	int		rv = 0 ; /* return-value */
	uchar		vbuf[4] = {} ;
	/* file magic */
	vbuf[0] = uchar(DENSITYDB_FILEVERSION) ;
	vbuf[1] = uchar(DENSITYDB_ENDIAN) ;
	vbuf[2] = 0 ;
	vbuf[3] = 0 ;
	if (FM fm ; (rs = fm.load(magstr,magsiz,vbuf)) >= 0) ylikely {
	    if (char fbuf[flen + 1] ; (rs = fm.rd(fbuf,flen)) >= 0) {
	        int bl = rs ;
	        /* file header */
	        if ((rs = fhp->rd(fbuf + bl)) >= 0) {
		    bl += rs ;
		    /* write them to the file */
	            if ((rs = u_writep(op->fd,fbuf,bl,0z)) > 0) {
	                op->filesz = rs ;
	                op->timod = dt ;
	                if (op->fl.remote) {
	                    u_fsync(op->fd) ;
		        }
	                rs = densitydb_bufupdate(op,0,bl,fbuf) ;
			rv = rs ;
	            } /* end if (u_writep) */
	            op->fl.fileinited = (rs >= 0) ;
	        } /* end if (dbhdr_rd) */
	    } /* end if (filemagic_rd) */
	} /* end if (filemagic_load) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (densitydb_fileinitzw) */

local int densitydb_fileinitn(DD *op,time_t dt) noex {
	dbhdr		*fhp = resumelife<dbhdr>(op->fhp) ;
	int		rs = SR_OK ;
	bool		f_locked = false ;
	/* read the file header */
	if (! op->fl.lockedrd) {
	   cmsgsub	frd = msgsub::rd ;
	   if ((rs = densitydb_lockacq(op,dt,frd)) >= 0) {
	        f_locked = true ;
	    }
	} /* end if (needed lock) */
	if (rs >= 0) ylikely {
	    cint	flen = DENSITYDB_FBUFLEN ;
	    char	fbuf[DENSITYDB_FBUFLEN + 1] ;
	    if ((rs = u_readp(op->fd,fbuf,flen,0z)) >= 0) ylikely {
		int  bl = 0 ;
	        if (FM fm ; (rs = fm.rd(fbuf + bl)) >= 0) ylikely {
		    bl += rs ;
		    if ((rs = fm.verify(magstr,magsiz)) > 0) ylikely {
	                if ((rs = fhp->rd(fbuf + bl)) >= 0) ylikely {
			    bool f = true ;
			    bl += rs ;
	                    f = f && (fm.vetu[0] <= DENSITYDB_FILEVERSION) ;
	                    f = f && (fm.vetu[1] == DENSITYDB_ENDIAN) ;
	                    if (! f) {
	                        rs = SR_BADFMT ;
		            }
	                    op->fl.fileinited = f ;
		        } /* end if (filehead_rd) */
		    } else {
			rs = SR_BADFMT ;
		    } /* end if (valid) */
		} /* end if (filemagic_rd) */
	    } /* end if (u_readp) */
	} /* end if (ok) */
	(void) f_locked ;
	return rs ;
} /* end subroutine (densitydb_fileinitn) */

local int densitydb_filechanged(DD *op) noex {
	int		rs ;
	int		fch = false ; /* return-value */
	if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) ylikely {
	    csize	fsize = size_t(sb.st_size) ;
	    if (cint fsz = intsat(fsize) ; fsz >= DENSITYDB_FOTAB) ylikely {
	        fch = fch || (fsz != op->filesz) ;
	        fch = fch || (sb.st_mtime != op->timod) ;
	        if ((! fch) && op->fl.fileinited) {
	            char	hbuf[hlen + 1] ;
	            if ((rs = u_readp(op->fd,hbuf,hlen,0z)) >= 0) {
	                if (rs < hlen) {
	                    op->fl.fileinited = false ;
	                }  /* end if */
			if (rs > 0) {
	                    if (FH h ; (rs = h.wr(hbuf)) >= 0) {
				dbhdr *fhp = resumelife<dbhdr>(op->fhp) ;
				fch = false ;
	                        fch = fch || (fhp->wtime != h.wtime) ;
				fch = fch || (fhp->wcount != h.wcount) ;
				fch = fch || (fhp->nents != h.nents) ;
	                        if (fch) {
	                            *fhp = h ; /* object copy */
		                }
		            } /* end if */
		        } /* end if (reading file header) */
	            } else {
	                op->fl.fileinited = false ;
	            } /* end if */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	            fch = true ;
	        } /* end if (stat) */
	    } /* end if (was already initialized) */
	    if (fch) {
		denbuf *dbp = resumelife<denbuf>(op->dbp) ;
		cint fsz = intsat(fsize) ;
	        dbp->len = 0 ;
	        op->filesz = fsz ;
	        op->timod = sb.st_mtime ;
	    } /* end if (file has changed) */
	} /* end if (u_fstat) */
	return (rs >= 0) ? fch : rs ;
} /* end subroutine (densitydb_filechanged) */

local int densitydb_lockacq(DD *op,time_t dt,cmsgsub fc) noex {
	int		rs ;
	bool		falready = false ;
	if ((rs = densitydb_fileopen(op,dt)) >= 0) ylikely {
	    int		lockcmd = -1 ;
	    if ((fc == msgsub::rd) || (! op->fl.writable)) {
	        falready = op->fl.lockedrd ;
	        op->fl.lockedrd = true ;
	        op->fl.lockedwr = false ;
	        lockcmd = F_RLOCK ;
	    } else if (fc == msgsub::wr) {
	        falready = op->fl.lockedwr ;
	        op->fl.lockedrd = false ;
	        op->fl.lockedwr = true ;
	        lockcmd = F_WLOCK ;
	    } else {
		rs = SR_BUGCHECK ;
	    } /* end if */
	    if ((! falready) && (lockcmd >= 0)) {
		cint	to = TO_LOCK ;
	        rs = lockfile(op->fd,lockcmd,0z,0z,to) ;
	    }
	} /* end if (ok) */
	return rs ;
} /* end subroutine (densitydb_lockacq) */

local int densitydb_lockrel(DD *op) noex {
	int		rs = SR_OK ;
	if ((op->fl.lockedrd || op->fl.lockedwr)) {
	    if (op->fd >= 0) {
	        rs = lockfile(op->fd,F_ULOCK,0z,0z,TO_LOCK) ;
	    }
	    op->fl.lockedrd = false ;
	    op->fl.lockedwr = false ;
	} /* end if (locked) */
	return rs ;
} /* end subroutine (densitydb_lockrel) */

local int densitydb_fileopen(DD *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,op->om)) >= 0) {
	        op->fd = rs ;
	        uc_closeonexec(op->fd,true) ;
	        op->tiopen = dt ;
	    } /* end if (u_open) */
	} /* end if (was not already open) */
	return (rs >= 0) ? op->fd : rs ;
} /* end subroutine (densitydb_fileopen) */

int densitydb_fileclose(DD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	    op->tiopen = 0 ;
	} /* end if (was open) */
	return rs ;
} /* end subroutine (densitydb_fileclose) */

local int densitydb_bufbegin(DD *op) noex {
	denbuf		*dbp = resumelife<denbuf>(op->dbp) ;
	cint		bsz = DENSITYDB_BUFSZ ;
	int		rs ;
	dbp->off = 0 ;
	dbp->len = 0 ;
	dbp->bsz = 0 ;
	dbp->bup = nullptr ;
	if (char *bp ; (rs = mem.mall(bsz,&bp)) >= 0) ylikely {
	    dbp->bup = bp ;
	    dbp->bsz = bsz ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (densitydb_bufbegin) */

local int densitydb_bufend(DD *op) noex {
	denbuf		*dbp = resumelife<denbuf>(op->dbp) ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (dbp->bup) {
	    rs1 = mem.free(dbp->bup) ;
	    if (rs >= 0) rs = rs1 ;
	    dbp->bup = nullptr ;
	} /* end if (memory-release) */
	dbp->bsz = 0 ;
	dbp->len = 0 ;
	return rs ;
} /* end subroutine (densitydb_bufend) */

local int densitydb_bufget(DD *op,int roff,int rlen,char **rpp) noex {
	denbuf		*dbp = resumelife<denbuf>(op->dbp) ;
	cint		rext = (roff + rlen) ;
	int		fext ;
	int		bend ;
	int		ext ;
	int		rs = SR_OK ;
	int		len = rlen ;
	fext = (dbp->off + dbp->len) ;
	/* do we need to read in more data? */
	if ((roff < dbp->off) || (rext > fext)) {
	    int  off ;
	    char *rbuf ;
	    /* can we do an "add-on" type read operation? */
	    bend = (dbp->off + dbp->bsz) ;
	    if ((roff >= dbp->off) && (rext <= bend)) {
	        rbuf = (dbp->bup + dbp->len) ;
		off = (dbp->off + dbp->len) ;
		{
	            ext = roff + MAX(rlen,DENSITYDB_READSZ) ;
	            fext = uceil(ext,op->pagesz) ;
		}
	        if (fext > bend) {
	            fext = bend ;
		}
	        len = (fext - off) ;
	        coff foff = off_t(off) ;
	        if ((rs = u_readp(op->fd,rbuf,len,foff)) >= 0) {
	            dbp->len += rs ;
	            len = MIN(((dbp->off + dbp->len) - roff),rlen) ;
	        } /* end if (u_readp) */
	    } else {
	        rbuf = dbp->bup ;
		off = roff ;
	        bend = roff + dbp->bsz ;
		{
	            dbp->off = roff ;
	            dbp->len = 0 ;
		}
		{
	            ext = roff + MAX(rlen,DENSITYDB_READSZ) ;
	            fext = uceil(ext,op->pagesz) ;
		}
	        if (fext > bend) {
	            fext = bend ;
		}
	        len = (fext - off) ;
	        coff foff = off_t(off) ;
	        if ((rs = u_readp(op->fd,dbp->bup,len,foff)) >= 0) {
	            dbp->len = rs ;
	            len = MIN(rs,rlen) ;
	        } /* end if (u_readp) */
	    } /* end if */
	} /* end if (needed to read more data) */
	if (rpp) {
	    *rpp = dbp->bup + (roff - dbp->off) ;
	}
	return (rs >= 0) ? len : rs ;
} /* end subroutine (densitydb_bufget) */

local int densitydb_bufupdate(DD *op,int roff,int rlen,cchar *rbuf) noex {
	denbuf		*dbp = resumelife<denbuf>(op->dbp) ;
	int		boff ;
	int		bext ;
	int		rext = (roff + rlen) ;
	int		buflen ;
	bool		fcont = true ;
	{
	    buflen	= int(dbp->len) ;
	    boff	= int(dbp->off) ;
	    bext	= (boff + buflen) ;
	}
	if (fcont && (roff < boff)) {
	    if (rext > boff) {
	        rbuf += (boff - roff) ;
	        rlen -= (boff - roff) ;
	        roff = boff ;
	    } else {
		rlen = 0 ;
	        fcont = false ;
	    }
	} /* end if */
	if (fcont && (rext > bext)) {
	    if (roff < bext) {
	        rlen -= (rext - bext) ;
	        rext = bext ;
	    } else {
		rlen = 0 ;
	        fcont = false ;
	    }
	} /* end if */
	if (fcont && (rlen > 0)) {
	    cint bdiff = (roff - boff) ;
	    memcopy((dbp->bup + bdiff),rbuf,rlen) ;
	} /* end if */
	return rlen ;
} /* end subroutine (densitydb_bufupdate) */

local int densitydb_writehead(DD *op) noex {
    	dbhdr		*fhp = resumelife<dbhdr>(op->fhp) ;
	int		rs ;
	char		fbuf[DENSITYDB_FBUFLEN + 1] ;
	if ((rs = fhp->rd(fbuf)) >= 0) ylikely {
	    coff	uoff = DENSITYDB_FOHEAD ;
	    cint	bl = rs ;
	    rs = u_writep(op->fd,fbuf,bl,uoff) ;
	} /* end if (dbhdr::rd) */
	return rs ;
} /* end subroutine (densitydb_writehead) */

vars::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if (densitydbe de ; (rs = de.start) >= 0) ylikely {
	    {
	        rs = de.entsz ;
	        entsz = rs ;
	    }
	    rs1 = de.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (densitydbe) */
	return rs ;
} /* end method (vars::operator) */


