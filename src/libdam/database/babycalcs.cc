/* babycalcs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* baby calculator */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This was created along with the DATE object.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	babycals

	Description:
	This small module takes a date (in UNIX® |time(2)| format)
	and uses it as a query to calculate the number of events
	corresponding to that date.  Both past and future dates are
	possible. Extrapolations are made for future requests.
	Implementation note: We use a heap-sort rather than a
	quick-sort on the database (which eventually needs to be
	sorted) since the data is normally or most probably already
	completely sorted.  This is supposed to give better
	performance!? (?) The database is kept in shared memory if
	at all possible.  Developers should note that the 'table'
	member of the object is shared alternatively between stages
	of DB loading or reloading.

	Postscript note: 
	This object allows for very robust dynamic creation and
	update of a shared-memory database.  The cost for this is
	quite complex and perhaps less capability could have been
	tolerated for (far) less implementation complexity.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/mman.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ptma.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<ucsysmisc.h>		/* LIBUC */
#include	<ucsigset.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<sigblocker.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<tmz.hh>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<cvtdater.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"babycalcs.h"
#include	"babieshdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BABYCALCS_OBJNAME	"babycalcs"
#define	BABYCALCS_DBDNAME	"share/misc"
#define	BABYCALCS_DBSUF		"txt"
#define	BABYCALCS_SHMPOSTFIX	"dbc"
#define	BABYCALCS_PREFIXLEN	5
#define	BABYCALCS_POSTFIXLEN	7
#define	BABYCALCS_PERMS		0666

#define	BC		babycalcs
#define	BC_ENT		babycalcs_ent
#define	BC_INFO		babycalcs_info

#define	HDR		babieshdr
#define	HDR_VERSION	BABIESHDR_VERSION

#ifndef	SHMNAMELEN
#define	SHMNAMELEN	14		/* shared-memory name length */
#endif

#ifndef	SHMPREFIXLEN
#define	SHMPREFIXLEN	8
#endif

#ifndef	SHMPOSTFIXLEN
#define	SHMPOSTFIXLEN	4
#endif

#define	HDRBUFLEN	(szof(HDR) * 2)

#ifndef	TO_WAITSHM
#define	TO_WAITSHM	20		/* seconds */
#endif

#define	TO_LASTCHECK	5		/* seconds */
#define	TO_DBWAIT	1		/* seconds */
#define	TO_DBPOLL	300		/* milliseconds */

#define	SHIFTINT	(6 * 60)	/* possible time-shift */


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */

typedef BABYCALCS_ENT *	entp ;


/* external subroutines */

extern "C" {
    extern int uc_msync(caddr_t,size_t,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pagesz ;
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int babycalcs_ctor(babycalcs *op,Args ... args) noex {
    	BABYCALCS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (babycalcs_ctor) */

local int babycalcs_dtor(babycalcs *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (babycalcs_dtor) */

template<typename ... Args>
local inline int babycalcs_magic(babycalcs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == BABYCALCS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (babycalcs_magic) */

local int	babycalcs_shmload(BC *,mode_t) noex ;
local int	babycalcs_shmopen(BC *,time_t,cchar *,mode_t) noex ;
local int	babycalcs_loadtxt(BC *) noex ;

local int	babycalcs_mapbeg(BC *,time_t,int) noex ;
local int	babycalcs_mapend(BC *) noex ;

local int	babycalcs_proctxt(BC *,vecobj *) noex ;
local int	babycalcs_proctxtln(BC *,vecobj *,cvtdater *,cc *,int) noex ;

local int	babycalcs_shmwr(BC *,time_t,int,mode_t) noex ;
local int	babycalcs_shmwrer(BC *,time_t,int,mode_t,HDR *) noex ;
local int	babycalcs_openshmwait(BC *,cchar *) noex ;
local int	babycalcs_mutexinit(BC *) noex ;
local int	babycalcs_procmap(BC *,time_t) noex ;
local int	babycalcs_verify(BC *,time_t) noex ;

local int	babycalcs_lookshm(BC *,time_t,time_t,uint *) noex ;
local int	babycalcs_lookproc(BC *,time_t,uint *) noex ;
local int	babycalcs_lookinfo(BC *,BC_INFO *) noex ;
local int	babycalcs_calc(BC *,int,time_t,uint *) noex ;
local int	babycalcs_dbcheck(BC *,time_t) noex ;
local int	babycalcs_dbwait(BC *,time_t,ustat *) noex ;
local int	babycalcs_reloadshm(BC *,time_t,con ustat *) noex ;
local int	babycalcs_reloadtxt(BC *,time_t) noex ;
local int	babycalcs_shmcheck(BC *,con ustat *) noex ;
local int	babycalcs_shmaccess(BC *,time_t) noex ;
local int	babycalcs_shmupdate(BC *,time_t,con ustat *,int) noex ;
local int	babycalcs_shmaddwrite(BC *,int) noex ;
local int	babycalcs_shminfo(BC *,BC_INFO *) noex ;

extern "C" {
    local int	vcmpentry(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr BC_ENT	defs[] = {
	{ 96526800, 0 },
	{ 1167627600, 47198810 },	/* from Guntmacker Institute */
	{ 0, 0 }
} ; /* end array */

constexpr cint		loadrs[] = {
	SR_NOENT,
	SR_NOTSUP,
	SR_NOSYS,
	0
} ; /* end array */

static vars		var ;


/* exported variables */

const babycalcs_obj	babycalcs_modinfo = {
	BABYCALCS_OBJNAME,
	szof(BABYCALCS),
	0
} ; /* end initialization */


/* exported subroutines */

local int	babycalcs_opens(BC *,cchar *) noex ;

int babycalcs_open(BC *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    dbname = BABYCALCS_DBNAME ;
	}
	if ((rs = babycalcs_ctor(op,pr)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if (static cint	rsv = var ; (rs = rsv) >= 0) ylikely {
		    op->pagesz = var.pagesz ;
	            if (cchar *cp ; (rs = mem.strw(pr,-1,&cp)) >= 0) ylikely {
	                op->pr = cp ;
			rs = babycalcs_opens(op,dbname) ;
	                if (rs < 0) {
	                    if (op->pr) {
	                        void *vp = voidp(op->pr) ;
	                        mem.free(vp) ;
	                        op->pr = nullptr ;
	                    } /* end if (memory-release) */
	                } /* end if (error) */
	            } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		babycalcs_dtor(op) ;
	    } /* end if (error) */
	} /* end if (babycalcs_ctor) */
	return rs ;
} /* end subroutine (babycalcs_open) */

local int babycalcs_opens(BC *op,cchar *dbname) noex {
    	cint		sz = ((var.maxpathlen + 1) * 2) ;
	cint		maxpath = var.maxpathlen ;
    	int		rs ;
	int		rs1 ;
	int		ai = 0 ; /* double allocation */
	if (char *a ; (rs = mem.mall(sz,&a)) >= 0) ylikely {
	    cchar	*suf = BABYCALCS_DBSUF ;
	    char	*dbcomp = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(dbcomp,maxpath,dbname,suf)) >= 0) ylikely {
	        cchar	*dbn = BABYCALCS_DBDNAME ;
		char	*dbfname = (a + ((maxpath + 1) * ai++)) ;
		if ((rs = mkpath(dbfname,op->pr,dbn,dbcomp)) >= 0) {
                     if (cc *cp ; (rs = mem.strw(dbfname,-1,&cp)) >= 0) {
                         cmode	om = BABYCALCS_PERMS ;
                         bool	f ;
                         op->dbfname = cp ;
                         rs = babycalcs_shmload(op,om) ;
                         f = isOneOf(loadrs,rs) ;
                         if (f && (op->table == nullptr)) {
                             rs = babycalcs_loadtxt(op) ;
                         } /* end if */
                         if (rs >= 0) {
                             op->magval = BABYCALCS_MAGIC ;
                         }
                         if (rs < 0) {
                             if (op->fl.txt && op->table) {
                                 op->fl.txt = false ;
                                 mem.free(op->table) ;
                                 op->table = nullptr ;
                             } /* end if (memory-release) */
                             if (op->dbfname) {
                                 void *vp = voidp(op->dbfname) ;
                                 mem.free(vp) ;
                                 op->dbfname = nullptr ;
                             } /* end if (memory-release) */
                         } /* end if (error handling) */
                     } /* end if (memory-allocation) */
		} /* end if (mkpath) */
	    } /* end if (snsds) */
	    rs1 = mem.free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (babycalcs_opens) */

int babycalcs_close(BC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = babycalcs_magic(op)) >= 0) ylikely {
	    {
	        rs1 = babycalcs_mapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.txt && op->table) {
	        op->fl.txt = false ;
	        rs1 = mem.free(op->table) ;
	        if (rs >= 0) rs = rs1 ;
	        op->table = nullptr ;
	    } /* end if (memory-release) */
	    if (op->shmname) {
		void *vp = voidp(op->shmname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->shmname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->dbfname) {
		void *vp = voidp(op->dbfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    } /* end if (memory-release) */
	    if (op->pr) {
		void *vp = voidp(op->pr) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = babycalcs_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (babycalcs_close) */

int babycalcs_check(BC *op,time_t dt) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op)) >= 0) ylikely {
	    rs = babycalcs_dbcheck(op,dt) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (babycalcs_check) */

int babycalcs_lookup(BC *op,time_t datereq,uint *rp) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op,rp)) >= 0) ylikely {
	    custime	dt = getustime ;
	    if (datereq == 0) {
	        datereq = dt ;
	    }
	    if ((rs = babycalcs_dbcheck(op,dt)) >= 0) ylikely {
	        if (op->fl.shm) {
	            rs = babycalcs_lookshm(op,dt,datereq,rp) ;
	        } else {
	            rs = babycalcs_lookproc(op,datereq,rp) ;
	        }
	    } /* end if (db-check) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (babycalcs_lookup) */

int babycalcs_getinfo(BC *op,BC_INFO *bip) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op,bip)) >= 0) ylikely {
	    if ((rs = babycalcs_dbcheck(op,0)) >= 0) ylikely {
	        if (op->fl.shm) {
	            rs = babycalcs_shminfo(op,bip) ;
	        } else {
	            memclear(bip) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (babycalcs_getinfo) */


/* private subroutines */

local int babycalcs_shmloads(BC *op,cchar *,int,mode_t om) noex ;

local int babycalcs_shmload(BC *op,mode_t om) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	cchar		*cp ;
	op->mapsize = 0 ;
	op->table = nullptr ;
	if (int cl ; (cl = sfbasename(op->pr,-1,&cp)) > 0) {
	    rs = babycalcs_shmloads(op,cp,cl,om) ;
	    c = rs ;
	} else {
	    rs = SR_INVALID ;
	} /* end if (sfbasename) */
	if (rs < 0) {
	    if (op->fl.txt && op->table) {
	        op->fl.txt = false ;
	        mem.free(op->table) ;
	        op->table = nullptr ;
	    } /* end if (memory-release) */
	    if (op->shmname) {
	        void *vp = voidp(op->shmname) ;
	        mem.free(vp) ;
	        op->shmname = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (error handling) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_shmload) */

local int babycalcs_shmloads(BC *op,cchar *cp,int cl,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	cchar		*postfix = BABYCALCS_SHMPOSTFIX ;
	if (char *shmname ; (rs = mem.mn(&shmname)) >= 0) ylikely {
	    if ((rs = mkshmname(shmname,cp,cl,postfix,-1)) >= 0) ylikely {
	        cl = rs ;
	        if (cchar *smp ; (rs = mem.strw(shmname,cl,&smp)) >= 0) {
	            custime	dt = getustime ;
	            op->shmname = smp ;
	            if ((rs = babycalcs_shmopen(op,dt,shmname,om)) >= 0) {
	                cint	fd = rs ;
	                if (op->shmsz == 0) {
	                    rs = u_fsize(fd) ;
	                    op->shmsz = rs ;
	                }
	                if (rs >= 0) ylikely {
	                    if ((rs = babycalcs_mapbeg(op,dt,fd)) >= 0) {
	                        c = rs ;
	                        if (op->fl.needinit) {
	                            if ((rs = babycalcs_mutexinit(op)) >= 0) {
	                                u_fchmod(fd,om) ;
	                            }
	                        }
	                        if (rs >= 0) {
	                            op->fl.shm = true ;
	                        } /* end if (ok) */
	                        if (rs < 0) {
	                            babycalcs_mapend(op) ;
	                            op->fl.shm = false ;
	                        } /* end if (error) */
	                    } /* end if (map) */
	                } /* end if (ok) */
	                rs1 = u_close(fd) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (shm-open) */
	            if (rs < 0) {
			void *vp = voidp(op->shmname) ;
	        	mem.free(vp) ;
	                op->shmname = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-acquure) */
	    } /* end if (mkshmname) */
	    rs1 = mem.free(shmname) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_shmloads) */

local int babycalcs_shmopen(BC *op,time_t dt,cchar *shmname,mode_t om) noex {
	cint		rsn = SR_NOENT ;
	int		of = O_RDWR ;
	int		rs ;
	int		fd = -1 ;
	if ((rs = uc_openshm(shmname,of,om)) == rsn) {
	    cmode	mom = (om & 0444) ;
	    of = (O_RDWR | O_CREAT | O_EXCL) ;
	    if ((rs = uc_openshm(shmname,of,mom)) >= 0) {
	        fd = rs ;
	        if (dt == 0) dt = getustime ;
	        op->ti_lastcheck = dt ;
	        if ((rs = babycalcs_loadtxt(op)) >= 0) {
	            if ((rs = babycalcs_shmwr(op,dt,fd,om)) >= 0) {
	                op->fl.needinit = true ;
	            }
	        }
	        if ((rs < 0) && (fd >= 0)) {
	            u_close(fd) ;
	            fd = -1 ;
		} /* end if (error) */
	    } /* end if (uc_openshm) */
	    if ((rs == SR_ACCESS) || (rs == SR_EXIST)) {
	        op->shmsz = 0 ;
	        rs = babycalcs_openshmwait(op,shmname) ;
	        fd = rs ;
	    } /* end if (error handling) */
	} else {
	    fd = rs ;
	}
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (babycalcs_shmopen) */

local int babycalcs_mapbeg(BC *op,time_t dt,int fd) noex {
    	cnullptr	np{} ;
	int		rs = SR_BADF ;
	int		c = 0 ;
	if (fd >= 0) ylikely {
	    csize	ms = op->shmsz ;
	    cint	mp = PROT_READ | PROT_WRITE ;
	    cint	mf = MAP_SHARED ;
	    if (dt == 0) dt = getustime ;
	    if (caddr_t md ; (rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	        op->mapdata = md ;
	        op->mapsize = ms ;
	        op->ti_map = dt ;
	        if (op->fl.txt && op->table) {
	            op->fl.txt = false ;
	            mem.free(op->table) ;
	            op->table = nullptr ;
	        } /* end if (memory-release) */
	        rs = babycalcs_procmap(op,dt) ;
	        c = rs ;
	        if (rs < 0) {
	            op->table = nullptr ;
	            op->fl.shm = false ;
	            u_mmapend(op->mapdata,op->mapsize) ;
	            op->mapdata = nullptr ;
	            op->mapsize = 0 ;
	            op->ti_map = 0 ;
	        } /* end if (error) */
	    } /* end if (map) */
	} /* end if (valid) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_mapbeg) */

local int babycalcs_mapend(BC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) {
	    caddr_t	md = op->mapdata ;
	    csize	ms = op->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->mxp = nullptr ;
	    op->ti_map = 0 ;
	    if (op->fl.shm && op->table) {
	        op->fl.shm = false ;
	        op->table = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (babycalcs_mapend) */

local int babycalcs_procmap(BC *op,time_t dt) noex {
	HDR		*hfp = &op->hf ;
	cint		hsz = intconv(op->mapsize) ;
	int		rs ;
	int		c = 0 ;
	if (dt == 0) dt = getustime ;
	if ((rs = babieshdr_wr(hfp,op->mapdata,hsz)) >= 0) ylikely {
	    if ((rs = babycalcs_verify(op,dt)) >= 0) ylikely {
	        op->table = (BC_ENT *) (op->mapdata + hfp->btoff) ;
	        op->mxp = (ptm *) (op->mapdata + hfp->muoff) ;
	        op->nentries = hfp->btlen ;
	        c = hfp->btlen ;
	    }
	} /* end if (babieshdr_wr) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_procmap) */

local int babycalcs_loadtxt(BC *op) noex {
	cint		vsz = szof(BC_ENT) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	op->table = nullptr ;
	op->nentries = 0 ;
	if (vecobj ents ; (rs = ents.start(vsz,0,0)) >= 0) ylikely {
	    if ((rs = babycalcs_proctxt(op,&ents)) >= 0) ylikely {
		int	n = rs ;
	        if ((rs == SR_NOENT) || (n == 0)) {
	            for (int i = 0 ; defs[i].date > 0 ; i += 1) {
	                rs = ents.add((defs + i)) ;
	                if (rs < 0) break ;
	            } /* end for */
	        }
	        if (rs >= 0) ylikely {
	            if ((rs = ents.count) >= 0) ylikely {
	                cint	sz = (rs + 1) * szof(BC_ENT) ;
	                if (void *p ; (rs = mem.mall(sz,&p)) >= 0) ylikely {
	                    op->table = entp(p) ;
			    void	*vp{} ;
	                    for (int i = 0 ; ents.get(i,&vp) >= 0 ; i += 1) {
			        BC_ENT	*ep = entp(vp) ;
	                        if (vp) {
	                            op->table[c++] = *ep ;
			        }
	                    } /* end for */
	                    op->table[c].date = 0 ;
	                    op->table[c].count = 0 ;
	                    op->nentries = c ;
	                    op->fl.txt = true ;
	                } else {
	                    op->table = nullptr ;
		        }
		    } /* end if (vecobj_count) */
	        } /* end if (ok) */
	    } /* end if (babycalcs_proctxt) */
	    rs1 = ents.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj-entries) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_loadtxt) */

local int babycalcs_proctxt(BC *op,vecobj *tlp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	op->fl.sorted = true ;
	if (cvtdater cd ; (rs = cd.start(0)) >= 0) ylikely {
	    if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) ylikely {
		cint	llen = rs ;
	        if (bfile tf ; (rs = tf.open(op->dbfname,"r",0666)) >= 0) {
	            if (ustat sb ; (rs = tf.control(BC_STAT,&sb)) >= 0) {
			csize	fsize = size_t(sb.st_size) ;
	                op->ti_mdb = sb.st_mtime ;
	                op->dbsz = intsat(fsize) ;
	                while ((rs = tf.readln(lbuf,llen)) > 0) {
			    cchar	*cp ;
			    if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
	                        c += 1 ;
	                        rs = babycalcs_proctxtln(op,tlp,&cd,cp,cl) ;
			    }
	                    if (rs < 0) break ;
	                } /* end while */
	            } /* end if (bcontrol) */
	            rs1 = tf.close ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (file-open) */
	        if ((rs >= 0) && (! op->fl.sorted)) {
	            op->fl.sorted = true ;
	            if (c > 1) {
	                tlp->sort(vcmpentry) ;
		    }
	        } /* end if (not sorted) */
		rs1 = mem.free(lbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = cd.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cvtdater) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_proctxt) */

local int babycalcs_proctxtln(BC *op,vecobj *tlp,cvtdater *cdp,
		cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		c = 0 ; /* return-value */
	cchar		*cp ;
	if (int cl ; (cl = sfnext(lbuf,llen,&cp)) > 0) {
	    time_t datereq ;
	    if ((rs = cvtdater_load(cdp,&datereq,cp,cl)) >= 0) ylikely {
	        int	ll = llen ;
	        cchar	*lp = lbuf ;
	        ll -= intconv((cp + cl) - lp) ;
	        lp = (cp + cl) ;
	        if ((cl = sfnext(lp,ll,&cp)) > 0) {
	            if (uint count ; (rs = cfdecui(cp,cl,&count)) >= 0) {
	                BC_ENT	e{} ;
	                c = 1 ;
	                e.date = datereq ;
	                e.count = count ;
	                if ((rs = vecobj_add(tlp,&e)) > 0) {
	                    cint	ei = (rs - 1) ;
			    void	*vp{} ;
	                    if ((rs = tlp->get(ei,&vp)) >= 0) {
	                        BC_ENT	*ep = entp(vp) ;
	                        if ((ep == nullptr) || (e.date < ep->date)) {
	                            op->fl.sorted = false ;
	                        }
	                    } else if (rs == SR_NOTFOUND) {
	                        rs = SR_OK ;
	                    }
	                } /* end if (vecobj_add) */
	            } /* end if (cfdec) */
	        } /* end if (sfnext) */
	    } /* end if (cvtdater_load) */
	} /* end if (sfnext) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_proctxtln) */

local int babycalcs_shmwr(BC *op,time_t dt,int fd,mode_t om) noex {
	HDR		hf{} ;
	int		rs ;
	int		rs1 ;
	int		foff = 0 ;
	op->shmsz = 0 ;
	if (dt == 0) dt = getustime ;
	/* prepare the file-header */
	hf.vetu[0] = HDR_VERSION ;
	hf.vetu[1] = uchar(ENDIAN) ;
	hf.vetu[2] = 0 ;
	hf.vetu[3] = 0 ;
	hf.dbsz = (uint) op->dbsz ;
	hf.dbtime = (uint) op->ti_mdb ;
	hf.wtime = (uint) dt ;
	/* process */
	if ((rs = babycalcs_shmwrer(op,dt,fd,om,&hf)) >= 0) ylikely {
	    foff = rs ;
	    if ((rs = u_rewind(fd)) >= 0) ylikely {
		cint sz = HDRBUFLEN ;
		if (char *hbuf ; (rs = mem.mall(sz,&hbuf)) >= 0) {
		    cint	hlen = rs ;
	            if ((rs = babieshdr_rd(&hf,hbuf,hlen)) >= 0) ylikely {
	                if ((rs = u_write(fd,hbuf,rs)) >= 0) {
	                    op->shmsz = foff ;
	                    rs = u_fchmod(fd,om) ;
		        }
	            } /* end if (babieshdr_rd) */
		    rs1 = mem.free(hbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (u_rewind) */
	} /* end if (babycalcs_shmwrer) */
	return (rs >= 0) ? foff : rs ;
} /* end subroutine (babycalcs_shmwr) */

local int babycalcs_shmwrer(BC *op,time_t dt,int fd,mode_t om,HDR *hfp) noex {
	cint		bsz = op->pagesz ;
	int		rs ;
	int		rs1 ;
	int		foff = 0 ; /* return-value */
	(void) dt ;
	(void) om ;
	if (filer bf ; (rs = bf.start(fd,0z,bsz,0)) >= 0) ylikely {
	    cint sz = HDRBUFLEN ;
	    if (char *hbuf ; (rs = mem.mall(sz,&hbuf)) >= 0) {
		cint	hlen = rs ;
	        if ((rs = babieshdr_rd(hfp,hbuf,hlen)) >= 0) ylikely {
	            cint bl = rs ;
		    int	tsz ;
		    /* write file-header */
	            if (rs >= 0) ylikely {
	                rs = bf.writefill(hbuf,bl) ;
	                foff += rs ;
	            } /* end if (ok) */
		    /* write the mutex (aligned) */
	            if (rs >= 0) ylikely {
	                cint	noff = iceil(foff,8) ;
	                if (noff != foff) {
	                    rs = bf.writezero((noff - foff)) ;
	                    foff += rs ;
	                }
	            } /* end if (ok) */
	            hfp->muoff = foff ;
	            hfp->musz = uceil(szof(ptm),szof(uint)) ;
	            if (rs >= 0) ylikely {
	                rs = bf.writezero(hfp->musz) ;
	                foff += rs ;
	            } /* end if */
		    /* write the table */
	            hfp->btoff = foff ;
	            hfp->btlen = op->nentries ;
	            tsz = (op->nentries + 1) * szof(BC_ENT) ;
	            if (rs >= 0) ylikely {
	                rs = bf.write(op->table,tsz) ;
	                foff += rs ;
	            } /* end if (ok) */
	            hfp->shmsz = foff ;
	        } /* end if (babieshdr_rd) */
		rs1 = mem.free(hbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = bf.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? foff : rs ;
} /* end subroutine (babycalcs_shmwrer) */

local int babycalcs_mutexinit(BC *op) noex {
	HDR		*hfp = &op->hf ;
	ptm		*mxp ;
	int		rs ;
	int		rs1 ;
	mxp = (ptm *) (op->mapdata + hfp->muoff) ;
	if (ptma ma ; (rs = ma.create) >= 0) {
	    cint	cmd = PTHREAD_PROCESS_SHARED ;
	    if ((rs = ma.setpshared(cmd)) >= 0) {
	        rs = mxp->create(&ma) ;
	    }
	    rs1 = ma.destroy ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-lock attribute) */
	return rs ;
} /* end subroutine (babycalcs_mutexinit) */

local int babycalcs_openshmwait(BC *op,cchar *shmname) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (op) {
	    cint	of = O_RDWR ;
	    int		to = TO_WAITSHM ;
	    cmode	om = BABYCALCS_PERMS ;
	    rs = SR_OK ;
	    while (to-- > 0) {
	        rs = uc_openshm(shmname,of,om) ;
	        fd = rs ;
	        if (rs >= 0) break ;
	        if (rs != SR_ACCESS) break ;
	        msleep(1) ;
	    } /* end while */
	    if ((rs < 0) && (to == 0)) {
	        rs = SR_TIMEDOUT ;
	    } /* end if (error) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (babycalcs_openshmwait) */

local int babycalcs_verify(BC *op,time_t dt) noex {
	HDR		*hfp = &op->hf ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		sz ;
	int		f = true ;
	{
	    f = f && (hfp->shmsz == op->mapsize) ;
	}
	if (hfp->wtime > 0) {
	    f = f && (hfp->wtime <= (utime + SHIFTINT)) ;
	}
	{
	    f = f && (hfp->muoff <= op->mapsize) ;
	}
	sz = hfp->musz ;
	if (sz > 0) {
	    f = f && ((hfp->muoff + sz) <= hfp->btoff) ;
	    f = f && ((hfp->muoff + sz) <= op->mapsize) ;
	}
	{
	    f = f && (hfp->btoff <= op->mapsize) ;
	}
	sz = hfp->btlen * szof(BC_ENT) ;
	f = f && ((hfp->btoff + sz) <= op->mapsize) ;
	/* get out */
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
} /* end subroutine (babycalcs_verify) */

local int babycalcs_lookshm(BC *op,time_t dt,time_t datereq,uint *rp) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (op) ylikely {
	    if (op->mapdata && op->mxp) ylikely {
	        if (sigblocker sb ; (rs = sb.start) >= 0) ylikely {
	            if (ptm *mxp = op->mxp ; (rs = mxp->lockbegin) >= 0) {
	                if ((rs = babycalcs_shmaccess(op,dt)) >= 0) {
	                    rs = babycalcs_lookproc(op,datereq,rp) ;
		            rv = rs ;
		        }
	                rs1 = mxp->lockend ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex lock) */
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblocker) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (babycalcs_lookshm) */

local int babycalcs_lookproc(BC *op,time_t datereq,uint *rp) noex {
	int		rs = SR_OK ;
	int		i ; /* used-afterwards */
	if (datereq == 0) {
	    datereq = getustime ;
	}
	for (i = 0 ; i < op->nentries ; i += 1) {
	    if (datereq <= op->table[i].date) break ;
	} /* end for */
	if ((i > 0) && (i >= op->nentries)) {
	    i -= 1 ;
	}
	babycalcs_calc(op,i,datereq,rp) ;
	return rs ;
} /* end subroutine (babycalcs_lookproc) */

local int babycalcs_calc(BC *op,int i,time_t rd,uint *rp) noex {
	time_t		bd = (i > 0) ? op->table[i-1].date : 0L ;
	double		x0, x1, dx ;
	double		y0, y1, dy ;
	double		xr, yr, yb ;
	uint		bc = (i > 0) ? op->table[i-1].count : 0 ;
	int		rs = SR_OK ;
	{
	    x0 = double(bd) ;
	    x1 = double(op->table[i].date) ;
	    dx = (x1 - x0) ;
	}
	{
	    y0 = double(bc) ;
	    y1 = double(op->table[i].count) ;
	    dy = (y1 - y0) ;
	}
	{
	    yb = double(bc) ;
	    xr = double(rd - bd) ;
	    yr = (xr * dy / dx) + yb ;
	}
	*rp = int(yr) ;
	return rs ;
} /* end subroutine (babycalcs_calc) */

local int babycalcs_dbcheck(BC *op,time_t dt) noex {
    	cint		to = TO_LASTCHECK ;
	int		rs = SR_OK ;
	int		f = false ;
	if (dt == 0) dt = getustime ;
	if (int tint = int(dt - op->ti_lastcheck) ; tint >= to) {
	    op->ti_lastcheck = dt ;
	    if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	        if (op->fl.shm) {
	            f = (sb.st_mtime > op->hf.dbtime) ;
	            f = f || (sb.st_size != op->hf.dbsz) ;
	            if (f) {
	                if ((rs = babycalcs_dbwait(op,dt,&sb)) >= 0) {
	                    rs = babycalcs_reloadshm(op,dt,&sb) ;
			}
	            }
	        } else {
	            f = (sb.st_mtime > op->ti_mdb) ;
	            f = f || (sb.st_size != op->dbsz) ;
	            if (f) {
	                if ((rs = babycalcs_dbwait(op,dt,&sb)) >= 0) {
	                    rs = babycalcs_reloadtxt(op,dt) ;
			}
	            }
	        } /* end if */
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    } /* end if (stat) */
	} /* end if (time-out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (babycalcs_dbcheck) */

local int babycalcs_shminfo(BC *op,BC_INFO *bip) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (op) ylikely {
	    if (op->mapdata && op->mxp) ylikely {
	        if (sigblocker sb ; (rs = sb.start) >= 0) ylikely {
	            ptm *mxp = op->mxp ;
	            if ((rs = mxp->lockbegin) >= 0) {
		        {
	                    rs = babycalcs_lookinfo(op,bip) ;
		            rv = rs ;
		        }
	                rs1 = mxp->lockend ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (mutex lock) */
	            rs1 = sb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblocker) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (babycalcs_shminfo) */

local int babycalcs_lookinfo(BC *op,BC_INFO *bip) noex {
	uint		*hwp ;
	int		rs = SR_OK ;
	memclear(bip) ;
	hwp = uintp(op->mapdata + BABIESHDR_IDLEN) ;
	bip->wtime	= hwp[babieshdrh_wtime] ;
	bip->atime	= hwp[babieshdrh_atime] ;
	bip->acount	= hwp[babieshdrh_acount] ;
	return rs ;
} /* end subroutine (babycalcs_lookinfo) */

local int babycalcs_dbwait(BC *op,time_t dt,ustat *sbp) noex {
	int		rs = SR_OK ;
	bool		f = ((dt - sbp->st_mtime) >= TO_DBWAIT) ;
	if (! f) {
	    while (rs >= 0) {
	        msleep(TO_DBPOLL) ;
		if (ustat nsb ; (rs = u_stat(op->dbfname,&nsb)) >= 0) {
	            f = (sbp->st_size == nsb.st_size) ;
	            f = f && (sbp->st_mtime == nsb.st_mtime) ;
	            f = f && ((dt - nsb.st_mtime) >= TO_DBWAIT) ;
	            if (f) break ;
	            *sbp = nsb ;
	            dt = getustime ;
	        } /* end if */
	    } /* end while */
	} /* end if (needed) */
	return rs ;
} /* end subroutine (babycalcs_dbwait) */

local int babycalcs_reloadshm(BC *op,time_t dt,con ustat *sbp) noex {
    	cint		of = O_RDWR ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cmode		om = BABYCALCS_PERMS ;
	if ((rs = uc_openshm(op->shmname,of,om)) >= 0) ylikely {
	    cint	fd = rs ;
	    int		neo = op->nentries ;
	    int		mapsz = intsat(op->mapsize) ;
	    int		mapextent ;
	    bool	f = false ;
	    if (sigblocker sb ; (rs = sb.start) >= 0) ylikely {
	        ptm *mxp = op->mxp ;
	        if ((rs = mxp->lockbegin) >= 0) ylikely {
	            if ((rs = babycalcs_shmcheck(op,sbp)) >= 0) ylikely {
	                f = (rs > 0) ;
	                if (f) {
	                    rs = babycalcs_shmupdate(op,dt,sbp,fd) ;
	                }
		    }
	            rs1 = mxp->lockend ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex lock) */
	        rs1 = sb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblocker) */
	    if ((rs >= 0) && f) {
	        uc_msync(op->mapdata,op->mapsize,MS_ASYNC) ;
	    } /* end if (ok) */
	    c = op->nentries ;
	    if ((rs >= 0) && f && (c != neo)) {
	        mapextent = uceil(mapsz,op->pagesz) ;
	        if (op->shmsz > mapextent) {
	            babycalcs_mapend(op) ;
	            rs = babycalcs_mapbeg(op,dt,fd) ;
	            c = rs ;
	        } else {
	            op->mapsize = size_t(op->shmsz) ;
	        } /* end if (SHM-segment exceeded the last page) */
	    } /* end if */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (babycalcs_reloadshm) */

local int babycalcs_shmupdate(BC *op,time_t dt,con ustat *sbp,int fd) noex {
	BC_ENT		*tblp = op->table ; /* used-afterwards */
	int		rs ;
	if ((rs = babycalcs_loadtxt(op)) >= 0) ylikely {
	    uint	*hwp ;
	    cint	esz = szof(BC_ENT) ;
	    int		nen = op->nentries ;
	    int		neo = op->nentries ;
	    int		shmsz = 0 ;
	    int		f ;
	    f = (nen != neo) ;
	    if (f) {
	        csize	tblsize = size_t(nen * esz) ;
	        f = (memcmp(tblp,op->table,tblsize) != 0) ;
	    }
	    if (f) {
	        if ((rs = babycalcs_shmaddwrite(op,fd)) >= 0) ylikely {
	            shmsz = rs ;
		    {
	                op->shmsz = shmsz ;
	                hwp = uintp(op->mapdata + BABIESHDR_IDLEN) ;
	                hwp[babieshdrh_shmsz]	= shmsz ;
	                hwp[babieshdrh_dbsz]	= (uint) sbp->st_size ;
	                hwp[babieshdrh_dbtime]	= (uint) sbp->st_mtime ;
	                hwp[babieshdrh_wtime]	= (uint) dt ;
	                hwp[babieshdrh_btlen]	= op->nentries ;
		    }
		    {
	                op->hf.shmsz	= hwp[babieshdrh_shmsz] ;
	                op->hf.dbsz	= hwp[babieshdrh_dbsz] ;
	                op->hf.dbtime	= hwp[babieshdrh_dbtime] ;
	                op->hf.wtime	= hwp[babieshdrh_wtime] ;
	                op->hf.btlen	= hwp[babieshdrh_btlen] ;
		    }
	        } /* end if */
	    } /* end if (update needed) */
	    if (op->table) {
	        op->fl.txt = false ;
	        mem.free(op->table) ;
	        op->table = nullptr ;
	    } /* end if (memory-release) */
	} /* end if (babycalcs_loadtxt) */
	op->table = tblp ;
	return rs ;
} /* end subroutine (babycalcs_shmupdate) */

local int babycalcs_shmaddwrite(BC *op,int fd) noex {
	off_t		tbloff ;
	uint		*hwp = uintp(op->mapdata + BABIESHDR_IDLEN) ;
	cint		esz = szof(BC_ENT) ;
	int		rs ;
	int		shmsz = 0 ;
	{
	    tbloff = hwp[babieshdrh_btoff] ;
	}
	{
	    long loff = long(tbloff) ;
	    shmsz = intsat(loff) ;
	}
	if ((rs = u_seek(fd,tbloff,SEEK_SET)) >= 0) ylikely {
	    int		tblsz = ((op->nentries + 1) * esz) ;
	    if ((rs = u_write(fd,op->table,tblsz)) >= 0) ylikely {
	        tbloff += rs ;
		shmsz += rs ;
	        rs = u_ftruncate(fd,tbloff) ;
	    } /* end if (u_write) */
	} /* end if (u_seek) */
	return (rs >= 0) ? shmsz : rs ;
} /* end subroutine (babycalcs_shmaddwrite) */

local int babycalcs_reloadtxt(BC *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	(void) dt ;
	if (op->fl.txt && op->table) {
	    op->fl.txt = false ;
	    rs1 = mem.free(op->table) ;
	    if (rs >= 0) rs = rs1 ;
	    op->table = nullptr ;
	} /* end if (memory-release) */
	if (rs >= 0) {
	    rs = babycalcs_loadtxt(op) ;
	} /* end if (ok) */
	return rs ;
} /* end subroutine (babycalcs_reloadtxt) */

local int babycalcs_shmcheck(BC *op,con ustat *sbp) noex {
	uint		*hwp	= uintp(op->mapdata + BABIESHDR_IDLEN) ;
	cuint		dbtime	= uint(sbp->st_mtime) ;
	cuint		dbsz	= uint(sbp->st_size) ;
	cuint		shmsz	= uint(op->shmsz) ;
	int		rs = SR_OK ;
	int		f = false ;
	f = f || (dbtime > hwp[babieshdrh_dbtime]) ;
	f = f || (dbsz != hwp[babieshdrh_dbsz]) ;
	f = f || (shmsz != hwp[babieshdrh_shmsz]) ;
	return (rs >= 0) ? f : rs ;
} /* end subroutine (babycalcs_shmcheck) */

local int babycalcs_shmaccess(BC *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (op->mapdata) {
	    if (dt == 0) dt = getustime ;
	    uint *hwp = uintp(op->mapdata + BABIESHDR_IDLEN) ;
	    hwp[babieshdrh_atime] = uint(dt) ;
	    hwp[babieshdrh_acount] += 1 ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (babycalcs_shmaccess) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = ucpagesize) >= 0) ylikely {
	    pagesz = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
		maxpathlen = rs ;
	    }
	} /* end if (pagesz) */
	return rs ;
} /* end method (vars::operator) */

local int entcmp(con BC_ENT *e1p,con BC_ENT *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    if (e1p) {
	        if (e2p) {
    		    rc = intconv(e1p->date - e2p->date) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	} /* end if */
    	return rc ;
} /* end subroutine */

local int vcmpentry(cvoid **v1pp,cvoid **v2pp) noex {
	int		rc = 0 ;
	if (v1pp && v2pp) ylikely {
	    con BC_ENT	*e1p = (BC_ENT *) *v1pp ;
	    con BC_ENT	*e2p = (BC_ENT *) *v2pp ;
	    rc = entcmp(e1p,e2p) ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (vcmpentry) */


