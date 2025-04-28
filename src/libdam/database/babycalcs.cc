/* babycalcs SUPPORT */
/* encoding=ISO8859-1 */
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
	performance!? (?)

	The database is kept in shared memory if at all possible.
	Developers should note that the 'table' member of the object
	is shared alternatively between stages of DB loading or
	reloading.

	Postscript note: This object allows for very robust dynamic
	creation and update of a shared-memory database.  The cost
	for this is quite complex and perhaps less capability could
	have been tolerated for (far) less implementation complexity.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<endian.h>
#include	<sigblocker.h>
#include	<bfile.h>
#include	<estrings.h>
#include	<vecobj.h>
#include	<tmz.hh>
#include	<tmtime.hh>
#include	<filer.h>
#include	<storebuf.h>
#include	<ptma.h>
#include	<ptm.h>
#include	<cvtdater.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkx.h>
#include	<sncpyx.h>
#include	<cfdec.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"babycalcs.h"
#include	"babieshdr.h"


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

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	SHMNAMELEN
#define	SHMNAMELEN	14		/* shared-memory name length */
#endif

#ifndef	SHMPREFIXLEN
#define	SHMPREFIXLEN	8
#endif

#ifndef	SHMPOSTFIXLEN
#define	SHMPOSTFIXLEN	4
#endif

#define	HDRBUFLEN	(szof(HDR) + MAXNAMELEN)

#ifndef	TO_WAITSHM
#define	TO_WAITSHM	20		/* seconds */
#endif

#define	TO_LASTCHECK	5		/* seconds */
#define	TO_DBWAIT	1		/* seconds */
#define	TO_DBPOLL	300		/* milliseconds */

#define	SHIFTINT	(6 * 60)	/* possible time-shift */


/* imported namespaces */


/* local typedefs */

typedef BABYCALCS_ENT *	entp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		pagesize ;
	int		maxpathlen ;
	operator int () noex ;
    } ;
}


/* forward references */

template<typename ... Args>
static int babycalcs_ctor(babycalcs *op,Args ... args) noex {
    	BABYCALCS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (babycalcs_ctor) */

static int babycalcs_dtor(babycalcs *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (babycalcs_dtor) */

template<typename ... Args>
static inline int babycalcs_magic(babycalcs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == BABYCALCS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (babycalcs_magic) */

static int	babycalcs_shmload(BC *,mode_t) noex ;
static int	babycalcs_shmopen(BC *,time_t,cchar *,mode_t) noex ;
static int	babycalcs_loadtxt(BC *) noex ;

static int	babycalcs_mapbegin(BC *,time_t,int) noex ;
static int	babycalcs_mapend(BC *) noex ;

static int	babycalcs_proctxt(BC *,vecobj *) noex ;
static int	babycalcs_proctxtln(BC *,vecobj *,cvtdater *,cc *,int) noex ;

static int	babycalcs_shmwr(BC *,time_t,int,mode_t) noex ;
static int	babycalcs_shmwrer(BC *,time_t,int,mode_t,HDR *) noex ;
static int	babycalcs_openshmwait(BC *,cchar *) noex ;
static int	babycalcs_mutexinit(BC *) noex ;
static int	babycalcs_procmap(BC *,time_t) noex ;
static int	babycalcs_verify(BC *,time_t) noex ;

static int	babycalcs_lookshm(BC *,time_t,time_t,uint *) noex ;
static int	babycalcs_lookproc(BC *,time_t,uint *) noex ;
static int	babycalcs_lookinfo(BC *,BC_INFO *) noex ;
static int	babycalcs_calc(BC *,int,time_t,uint *) noex ;
static int	babycalcs_dbcheck(BC *,time_t) noex ;
static int	babycalcs_dbwait(BC *,time_t,USTAT *) noex ;
static int	babycalcs_reloadshm(BC *,time_t,USTAT *) noex ;
static int	babycalcs_reloadtxt(BC *,time_t) noex ;
static int	babycalcs_shmcheck(BC *,USTAT *) noex ;
static int	babycalcs_shmaccess(BC *,time_t) noex ;
static int	babycalcs_shmupdate(BC *,time_t,USTAT *,int) noex ;
static int	babycalcs_shmaddwrite(BC *,int) noex ;
static int	babycalcs_shminfo(BC *,BC_INFO *) noex ;

extern "C" {
    static int	vcmpentry(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr static BC_ENT	defs[] = {
	{ 96526800, 0 },
	{ 1167627600, 47198810 },	/* from Guntmacker Institute */
	{ 0, 0 }
} ;

constexpr static cint		loadrs[] = {
	SR_NOENT,
	SR_NOTSUP,
	SR_NOSYS,
	0
} ;

static vars			var ;


/* exported variables */

extern const babycalcs_obj	babycalcs_modinfo = {
	BABYCALCS_OBJNAME,
	szof(BABYCALCS)
} ;


/* exported subroutines */

static int	babycalcs_opens(BC *,cchar *) noex ;

int babycalcs_open(BC *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    dbname = BABYCALCS_DBNAME ;
	}
	if ((rs = babycalcs_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    op->pagesize = var.pagesize ;
	            if (cchar *cp ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	                op->pr = cp ;
			rs = babycalcs_opens(op,dbname) ;
	                if (rs < 0) {
	                    if (op->pr) {
	                        uc_free(op->pr) ;
	                        op->pr = nullptr ;
	                    }
	                }
	            } /* end if (memory-allocation) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		babycalcs_dtor(op) ;
	    }
	} /* end if (babycalcs_ctor) */
	return rs ;
}
/* end subroutine (babycalcs_open) */

static int babycalcs_opens(BC *op,cchar *dbname) noex {
    	cint		sz = ((var.maxpathlen + 1) * 2) ;
	cint		maxpath = var.maxpathlen ;
    	int		rs ;
	int		ai = 0 ;
	if (char *a ; (rs = uc_malloc(sz,&a)) >= 0) {
	    cchar	*suf = BABYCALCS_DBSUF ;
	    char	*dbcomp = (a + ((maxpath + 1) * ai++)) ;
	    if ((rs = snsds(dbcomp,maxpath,dbname,suf)) >= 0) {
	        cchar	*dbn = BABYCALCS_DBDNAME ;
		char	*dbfname = (a + ((maxpath + 1) * ai++)) ;
		if ((rs = mkpath(dbfname,op->pr,dbn,dbcomp)) >= 0) {
                     if (cc *cp ; (rs = uc_mallocstrw(dbfname,-1,&cp)) >= 0) {
                         cmode	om = BABYCALCS_PERMS ;
                         int	f ;
                         op->dbfname = cp ;
                         rs = babycalcs_shmload(op,om) ;
                         f = isOneOf(loadrs,rs) ;
                         if (f && (op->table == nullptr)) {
                             rs = babycalcs_loadtxt(op) ;
                         } /* end if */
                         if (rs >= 0) {
                             op->magic = BABYCALCS_MAGIC ;
                         }
                         if (rs < 0) {
                             if (op->fl.txt && (op->table != nullptr)) {
                                 op->fl.txt = false ;
                                 uc_free(op->table) ;
                                 op->table = nullptr ;
                             }
                             if (op->dbfname != nullptr) {
                                 uc_free(op->dbfname) ;
                                 op->dbfname = nullptr ;
                             }
                         } /* end if (error handling) */
                     } /* end if (memory-allocation) */
		} /* end if (mkpath) */
	    } /* end if (snsds) */
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (babycalcs_opens) */

int babycalcs_close(BC *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = babycalcs_magic(op)) >= 0) {
	    {
	        rs1 = babycalcs_mapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.txt && (op->table != nullptr)) {
	        op->fl.txt = false ;
	        rs1 = uc_free(op->table) ;
	        if (rs >= 0) rs = rs1 ;
	        op->table = nullptr ;
	    }
	    if (op->shmname != nullptr) {
	        rs1 = uc_free(op->shmname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->shmname = nullptr ;
	    }
	    if (op->dbfname != nullptr) {
	        rs1 = uc_free(op->dbfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    }
	    if (op->pr != nullptr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
	        rs1 = babycalcs_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (babycalcs_close) */

int babycalcs_check(BC *op,time_t dt) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op)) >= 0) {
	    rs = babycalcs_dbcheck(op,dt) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalcs_check) */

int babycalcs_lookup(BC *op,time_t datereq,uint *rp) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op,rp)) >= 0) {
	    custime	dt = getustime ;
	    if (datereq == 0) {
	        datereq = dt ;
	    }
	    if ((rs = babycalcs_dbcheck(op,dt)) >= 0) {
	        if (op->fl.shm) {
	            rs = babycalcs_lookshm(op,dt,datereq,rp) ;
	        } else {
	            rs = babycalcs_lookproc(op,datereq,rp) ;
	        }
	    } /* end if (db-check) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalcs_lookup) */

int babycalcs_getinfo(BC *op,BC_INFO *bip) noex {
	int		rs ;
	if ((rs = babycalcs_magic(op,bip)) >= 0) {
	    if ((rs = babycalcs_dbcheck(op,0)) >= 0) {
	        if (op->fl.shm) {
	            rs = babycalcs_shminfo(op,bip) ;
	        } else {
	            memclear(bip) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (babycalcs_getinfo) */


/* private subroutines */

static int babycalcs_shmload(BC *op,mode_t om) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;
	op->mapsize = 0 ;
	op->table = nullptr ;
	if (int cl ; (cl = sfbasename(op->pr,-1,&cp)) > 0) {
	    cchar	*postfix = BABYCALCS_SHMPOSTFIX ;
	    char	shmname[MAXNAMELEN + 1] ;
	    if ((rs = mkshmname(shmname,cp,cl,postfix,-1)) >= 0) {
	        cl = rs ;
	        if (cchar *smp ; (rs = uc_mallocstrw(shmname,cl,&smp)) >= 0) {
	            const time_t	dt = getustime ;
	            op->shmname = smp ;
	            if ((rs = babycalcs_shmopen(op,dt,shmname,om)) >= 0) {
	                cint	fd = rs ;

	                if (op->shmsize == 0) {
	                    rs = uc_fsize(fd) ;
	                    op->shmsize = rs ;
	                }

	                if (rs >= 0) {
	                    if ((rs = babycalcs_mapbegin(op,dt,fd)) >= 0) {
	                        c = rs ;
	                        if (op->fl.needinit) {
	                            if ((rs = babycalcs_mutexinit(op)) >= 0) {
	                                u_fchmod(fd,om) ;
	                            }
	                        }
	                        if (rs >= 0) {
	                            op->fl.shm = true ;
	                        }
	                        if (rs < 0) {
	                            babycalcs_mapend(op) ;
	                            op->fl.shm = false ;
	                        }
	                    } /* end if (map) */
	                } /* end if (ok) */

	                u_close(fd) ;
	            } /* end if (shm-open) */
	            if (rs < 0) {
	                uc_free(op->shmname) ;
	                op->shmname = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (mkshmname) */
	} else {
	    rs = SR_INVALID ;
	} /* end if (sfbasename) */
	if (rs < 0) {
	    if (op->fl.txt && (op->table != nullptr)) {
	        op->fl.txt = false ;
	        uc_free(op->table) ;
	        op->table = nullptr ;
	    }
	    if (op->shmname != nullptr) {
	        uc_free(op->shmname) ;
	        op->shmname = nullptr ;
	    }
	} /* end if (error handling) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_shmload) */

static int babycalcs_shmopen(BC *op,time_t dt,cchar *shmname,mode_t om) noex {
	cint	rsn = SR_NOENT ;
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
	        }
	    } /* end if (uc_openshm) */

	    if ((rs == SR_ACCESS) || (rs == SR_EXIST)) {
	        op->shmsize = 0 ;
	        rs = babycalcs_openshmwait(op,shmname) ;
	        fd = rs ;
	    }

	} else {
	    fd = rs ;
	}

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (babycalcs_shmopen) */

static int babycalcs_mapbegin(BC *op,time_t dt,int fd) noex {
    	cnullptr	np{} ;
	int		rs = SR_BADF ;
	int		c = 0 ;
	if (fd >= 0) {
	    csize		ms = op->shmsize ;
	    cint		mp = PROT_READ | PROT_WRITE ;
	    cint		mf = MAP_SHARED ;
	    if (dt == 0) dt = getustime ;
	    if (caddr_t md ; (rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	        op->mapdata = md ;
	        op->mapsize = ms ;
	        op->ti_map = dt ;
	        if (op->fl.txt && (op->table != nullptr)) {
	            op->fl.txt = false ;
	            uc_free(op->table) ;
	            op->table = nullptr ;
	        }
	        rs = babycalcs_procmap(op,dt) ;
	        c = rs ;
	        if (rs < 0) {
	            op->table = nullptr ;
	            op->fl.shm = false ;
	            u_mmapend(op->mapdata,op->mapsize) ;
	            op->mapdata = nullptr ;
	            op->mapsize = 0 ;
	            op->ti_map = 0 ;
	        }
	    } /* end if (map) */
	} /* end if (valid) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_mapbegin) */


static int babycalcs_mapend(BC *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->mapdata != nullptr) {
	    caddr_t	md = op->mapdata ;
	    csize	ms = op->mapsize ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	    op->mxp = nullptr ;
	    op->ti_map = 0 ;
	    if (op->fl.shm && (op->table != nullptr)) {
	        op->fl.shm = false ;
	        op->table = nullptr ;
	    }
	}

	return rs ;
}
/* end subroutine (babycalcs_mapend) */

static int babycalcs_procmap(BC *op,time_t dt) noex {
	HDR		*hfp = &op->hf ;
	cint		hsz = intconv(op->mapsize) ;
	int		rs ;
	int		c = 0 ;
	if (dt == 0) dt = getustime ;
	if ((rs = babieshdr_wr(hfp,op->mapdata,hsz)) >= 0) {
	    if ((rs = babycalcs_verify(op,dt)) >= 0) {
	        op->table = (BC_ENT *) (op->mapdata + hfp->btoff) ;
	        op->mxp = (ptm *) (op->mapdata + hfp->muoff) ;
	        op->nentries = hfp->btlen ;
	        c = hfp->btlen ;
	    }
	} /* end if (babieshdr_wr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_procmap) */

static int babycalcs_loadtxt(BC *op) noex {
	cint		vsz = szof(BC_ENT) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	op->table = nullptr ;
	op->nentries = 0 ;
	if (vecobj ents ; (rs = vecobj_start(&ents,vsz,0,0)) >= 0) {
	    if ((rs = babycalcs_proctxt(op,&ents)) >= 0) {
		int	n = rs ;
	        if ((rs == SR_NOENT) || (n == 0)) {
	            for (int i = 0 ; defs[i].date > 0 ; i += 1) {
	                rs = vecobj_add(&ents,(defs + i)) ;
	                if (rs < 0) break ;
	            } /* end for */
	        }
	        if (rs >= 0) {
	            if ((rs = ents.count) >= 0) {
	                cint	sz = (rs + 1) * szof(BC_ENT) ;
	                if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
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
	    rs1 = vecobj_finish(&ents) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj-entries) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_loadtxt) */

static int babycalcs_proctxt(BC *op,vecobj *tlp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	op->fl.sorted = true ;
	if (cvtdater cd ; (rs = cd.start(0)) >= 0) {
	    if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if (bfile tf ; (rs = tf.open(op->dbfname,"r",0666)) >= 0) {
	            if (USTAT sb ; (rs = tf.control(BC_STAT,&sb)) >= 0) {
	                op->ti_mdb = sb.st_mtime ;
	                op->dbsize = intsat(sb.st_size) ;
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
	        }
		rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	    rs1 = cd.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cvtdater) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_proctxt) */

static int babycalcs_proctxtln(BC *op,vecobj *tlp,cvtdater *cdp,
		cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;
	if (int cl ; (cl = sfnext(lbuf,llen,&cp)) > 0) {
	    time_t datereq ;
	    if ((rs = cvtdater_load(cdp,&datereq,cp,cl)) >= 0) {
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
	                    if ((rs = vecobj_get(tlp,ei,&vp)) >= 0) {
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
}
/* end subroutine (babycalcs_proctxtln) */

static int babycalcs_shmwr(BC *op,time_t dt,int fd,mode_t om) noex {
	HDR		hf{} ;
	int		rs ;
	int		foff = 0 ;
	op->shmsize = 0 ;
	if (dt == 0) dt = getustime ;
	/* prepare the file-header */
	hf.vetu[0] = HDR_VERSION ;
	hf.vetu[1] = uchar(ENDIAN) ;
	hf.vetu[2] = 0 ;
	hf.vetu[3] = 0 ;
	hf.dbsize = (uint) op->dbsize ;
	hf.dbtime = (uint) op->ti_mdb ;
	hf.wtime = (uint) dt ;
	/* process */
	if ((rs = babycalcs_shmwrer(op,dt,fd,om,&hf)) >= 0) {
	    foff = rs ;
	    if ((rs = u_rewind(fd)) >= 0) {
		cint	hlen = HDRBUFLEN ;
		char	hbuf[HDRBUFLEN+1] ;
	        if ((rs = babieshdr_rd(&hf,hbuf,hlen)) >= 0) {
	            if ((rs = u_write(fd,hbuf,rs)) >= 0) {
	                op->shmsize = foff ;
	                rs = u_fchmod(fd,om) ;
		    }
	        } /* end if (babieshdr_rd) */
	    } /* end if (u_rewind) */
	} /* end if (babycalcs_shmwrer) */
	return (rs >= 0) ? foff : rs ;
}
/* end subroutine (babycalcs_shmwr) */

static int babycalcs_shmwrer(BC *op,time_t dt,int fd,mode_t om,HDR *hfp) noex {
	cint		bsz = op->pagesize ;
	int		rs ;
	int		rs1 ;
	int		foff = 0 ;
	(void) dt ;
	(void) om ;
	if (filer babyfile ; (rs = filer_start(&babyfile,fd,0z,bsz,0)) >= 0) {
	    cint	hlen = HDRBUFLEN ;
	    int		bl ;
	    char	hbuf[HDRBUFLEN + 1] ;
	    if ((rs = babieshdr_rd(hfp,hbuf,hlen)) >= 0) {
		int	tsize ;
	        bl = rs ;

/* write file-header */

	        if (rs >= 0) {
	            rs = filer_writefill(&babyfile,hbuf,bl) ;
	            foff += rs ;
	        }

/* write the mutex (align up to the next 8-byte boundary) */

	        if (rs >= 0) {
	            int		noff = iceil(foff,8) ;
	            if (noff != foff) {
	                rs = filer_writezero(&babyfile,(noff - foff)) ;
	                foff += rs ;
	            }
	        }

	        hfp->muoff = foff ;
	        hfp->musize = uceil(szof(ptm),szof(uint)) ;
	        if (rs >= 0) {
	            rs = filer_writezero(&babyfile,hfp->musize) ;
	            foff += rs ;
	        }

/* write the table */

	        hfp->btoff = foff ;
	        hfp->btlen = op->nentries ;
	        tsize = (op->nentries + 1) * szof(BC_ENT) ;
	        if (rs >= 0) {
	            rs = filer_write(&babyfile,op->table,tsize) ;
	            foff += rs ;
	        }

	        hfp->shmsize = foff ;
	    } /* end if (babieshdr_rd) */

	    rs1 = filer_finish(&babyfile) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? foff : rs ;
}
/* end subroutine (babycalcs_shmwrer) */

static int babycalcs_mutexinit(BC *op) noex {
	HDR		*hfp = &op->hf ;
	ptm		*mxp ;
	int		rs ;
	int		rs1 ;
	mxp = (ptm *) (op->mapdata + hfp->muoff) ;
	if (ptma ma ; (rs = ptma_create(&ma)) >= 0) {
	    cint	cmd = PTHREAD_PROCESS_SHARED ;
	    if ((rs = ptma_setpshared(&ma,cmd)) >= 0) {
	        rs = ptm_create(mxp,&ma) ;
	    }
	    rs1 = ptma_destroy(&ma) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (mutex-lock attribute) */
	return rs ;
}
/* end subroutine (babycalcs_mutexinit) */

static int babycalcs_openshmwait(BC *op,cchar *shmname) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (op) {
	    cint	of = O_RDWR ;
	    int		to = TO_WAITSHM ;
	    cmode	om = BABYCALCS_PERMS ;
	    while (to-- > 0) {
	        rs = uc_openshm(shmname,of,om) ;
	        fd = rs ;
	        if (rs >= 0) break ;
	        if (rs != SR_ACCESS) break ;
	        msleep(1) ;
	    } /* end while */
	    if ((rs < 0) && (to == 0)) {
	        rs = SR_TIMEDOUT ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (babycalcs_openshmwait) */

static int babycalcs_verify(BC *op,time_t dt) noex {
	HDR		*hfp = &op->hf ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		size ;
	int		f = true ;

	f = f && (hfp->shmsize == op->mapsize) ;

	if (hfp->wtime > 0) {
	    f = f && (hfp->wtime <= (utime + SHIFTINT)) ;
	}

	f = f && (hfp->muoff <= op->mapsize) ;

	size = hfp->musize ;
	if (size > 0) {
	    f = f && ((hfp->muoff + size) <= hfp->btoff) ;
	    f = f && ((hfp->muoff + size) <= op->mapsize) ;
	}

	f = f && (hfp->btoff <= op->mapsize) ;

	size = hfp->btlen * szof(BC_ENT) ;
	f = f && ((hfp->btoff + size) <= op->mapsize) ;

/* get out */

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (babycalcs_verify) */

static int babycalcs_lookshm(BC *op,time_t dt,time_t datereq,
		uint *rp) noex {
	sigset_t	oldsigmask, newsigmask ;
	int		rs ;

	if (op->mapdata == nullptr) return SR_BUGCHECK ;
	if (op->mxp == nullptr) return SR_BUGCHECK ;

	uc_sigsetfill(&newsigmask) ;

	if ((rs = u_sigprocmask(SIG_BLOCK,&newsigmask,&oldsigmask)) >= 0) {

	    if ((rs = ptm_lock(op->mxp)) >= 0) {

	        if ((rs = babycalcs_shmaccess(op,dt)) >= 0) {
	            rs = babycalcs_lookproc(op,datereq,rp) ;
		}

	        ptm_unlock(op->mxp) ;
	    } /* end if (mutex lock) */

	    u_sigprocmask(SIG_SETMASK,&oldsigmask,nullptr) ;
	} /* end if (u_sigprocmask) */

	return rs ;
}
/* end subroutine (babycalcs_lookshm) */

static int babycalcs_lookproc(BC *op,time_t datereq,uint *rp) noex {
	int		rs = SR_OK ;
	int		i ;

	if (datereq == 0)
	    datereq = getustime ;

	for (i = 0 ; i < op->nentries ; i += 1) {
	    if (datereq <= op->table[i].date) break ;
	} /* end for */

	if ((i > 0) && (i >= op->nentries))
	    i -= 1 ;

	babycalcs_calc(op,i,datereq,rp) ;

	return rs ;
}
/* end subroutine (babycalcs_lookproc) */

static int babycalcs_calc(BC *op,int i,time_t rd,uint *rp) noex {
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
}
/* end subroutine (babycalcs_calc) */

static int babycalcs_dbcheck(BC *op,time_t dt) noex {
    	cint		to = TO_LASTCHECK ;
	int		rs = SR_OK ;
	int		f = false ;
	if (dt == 0) dt = getustime ;
	if (int tint = int(dt - op->ti_lastcheck) ; tint >= to) {
	    op->ti_lastcheck = dt ;
	    if (USTAT sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	        if (op->fl.shm) {
	            f = (sb.st_mtime > op->hf.dbtime) ;
	            f = f || (sb.st_size != op->hf.dbsize) ;
	            if (f) {
	                if ((rs = babycalcs_dbwait(op,dt,&sb)) >= 0) {
	                    rs = babycalcs_reloadshm(op,dt,&sb) ;
			}
	            }
	        } else {
	            f = (sb.st_mtime > op->ti_mdb) ;
	            f = f || (sb.st_size != op->dbsize) ;
	            if (f) {
	                if ((rs = babycalcs_dbwait(op,dt,&sb)) >= 0) {
	                    rs = babycalcs_reloadtxt(op,dt) ;
			}
	            }
	        }
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    } /* end if (stat) */
	} /* end if (time-out) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (babycalcs_dbcheck) */

static int babycalcs_shminfo(BC *op,BC_INFO *bip) noex {
	sigset_t	oldsigmask, newsigmask ;
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;

	if (op->mapdata == nullptr) return SR_BUGCHECK ;
	if (op->mxp == nullptr) return SR_BUGCHECK ;

	uc_sigsetfill(&newsigmask) ;

	if ((rs = u_sigprocmask(SIG_BLOCK,&newsigmask,&oldsigmask)) >= 0) {
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
		{
	            rs = babycalcs_lookinfo(op,bip) ;
		    rv = rs ;
		}
	        rs1 = ptm_unlock(op->mxp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mutex lock) */
	    rs1 = u_sigprocmask(SIG_SETMASK,&oldsigmask,nullptr) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (procmask) */

	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (babycalcs_shminfo) */

static int babycalcs_lookinfo(BC *op,BC_INFO *bip) noex {
	uint		*hwp ;
	int		rs = SR_OK ;
	memclear(bip) ;
	hwp = (uint *) (op->mapdata + BABIESHDR_IDLEN) ;
	bip->wtime = hwp[babieshdrh_wtime] ;
	bip->atime = hwp[babieshdrh_atime] ;
	bip->acount = hwp[babieshdrh_acount] ;
	return rs ;
}
/* end subroutine (babycalcs_lookinfo) */

static int babycalcs_dbwait(BC *op,time_t dt,USTAT *sbp) noex {
	USTAT		nsb ;
	int		rs = SR_OK ;
	int		f ;

	f = ((dt - sbp->st_mtime) >= TO_DBWAIT) ;
	if (! f) {
	    while (rs >= 0) {

	        msleep(TO_DBPOLL) ;

	        if ((rs = u_stat(op->dbfname,&nsb)) >= 0) {

	            f = (sbp->st_size == nsb.st_size) ;
	            f = f && (sbp->st_mtime == nsb.st_mtime) ;
	            f = f && ((dt - nsb.st_mtime) >= TO_DBWAIT) ;
	            if (f)
	                break ;

	            *sbp = nsb ;
	            dt = getustime ;

	        } /* end if */

	    } /* end while */
	} /* end if (needed) */

	return rs ;
}
/* end subroutine (babycalcs_dbwait) */

static int babycalcs_reloadshm(BC *op,time_t dt,USTAT *sbp) noex {
	cint		of = O_RDWR ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cmode		om = BABYCALCS_PERMS ;
	if ((rs = uc_openshm(op->shmname,of,om)) >= 0) {
	    cint	fd = rs ;
	    int		neo = op->nentries ;
	    int		mapsz = intsat(op->mapsize) ;
	    int		mapextent ;
	    int		f = false ;
	    if (sigblocker sb ; (rs = sigblocker_start(&sb,nullptr)) >= 0) {
	        if ((rs = ptm_lock(op->mxp)) >= 0) {
	            rs = babycalcs_shmcheck(op,sbp) ;
	            f = (rs > 0) ;
	            if ((rs >= 0) && f) {
	                rs = babycalcs_shmupdate(op,dt,sbp,fd) ;
	            }
	            rs1 = ptm_unlock(op->mxp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mutex lock) */
	        rs1 = sigblocker_finish(&sb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sigblock) */
	    if ((rs >= 0) && f) {
	        uc_msync(op->mapdata,op->mapsize,MS_ASYNC) ;
	    }
	    c = op->nentries ;
	    if ((rs >= 0) && f && (c != neo)) {
	        mapextent = uceil(mapsz,op->pagesize) ;
	        if (op->shmsize > mapextent) {
	            babycalcs_mapend(op) ;
	            rs = babycalcs_mapbegin(op,dt,fd) ;
	            c = rs ;
	        } else {
	            op->mapsize = op->shmsize ;
	        } /* end if (SHM-segment exceeded the last page) */
	    } /* end if */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (babycalcs_reloadshm) */

static int babycalcs_shmupdate(BC *op,time_t dt,USTAT *sbp,int fd) noex {
	BC_ENT		*tblp = op->table ;
	int		rs ;
	if ((rs = babycalcs_loadtxt(op)) >= 0) {
	    uint	*hwp ;
	    cint	esz = szof(BC_ENT) ;
	    int		nen = op->nentries ;
	    int		neo = op->nentries ;
	    int		shmsize = 0 ;
	    int		f ;
	    f = (nen != neo) ;
	    if (f) {
	        csize	tblsize = size_t(nen * esz) ;
	        f = (memcmp(tblp,op->table,tblsize) != 0) ;
	    }
	    if (f) {
	        if ((rs = babycalcs_shmaddwrite(op,fd)) >= 0) {
	            shmsize = rs ;
		    {
	                op->shmsize = shmsize ;
	                hwp = (uint *) (op->mapdata + BABIESHDR_IDLEN) ;
	                hwp[babieshdrh_shmsize] = shmsize ;
	                hwp[babieshdrh_dbsize] = (uint) sbp->st_size ;
	                hwp[babieshdrh_dbtime] = (uint) sbp->st_mtime ;
	                hwp[babieshdrh_wtime] = (uint) dt ;
	                hwp[babieshdrh_btlen] = op->nentries ;
		    }
		    {
	                op->hf.shmsize = hwp[babieshdrh_shmsize] ;
	                op->hf.dbsize = hwp[babieshdrh_dbsize] ;
	                op->hf.dbtime = hwp[babieshdrh_dbtime] ;
	                op->hf.wtime = hwp[babieshdrh_wtime] ;
	                op->hf.btlen = hwp[babieshdrh_btlen] ;
		    }
	        } /* end if */
	    } /* end if (update needed) */
	    if (op->table != nullptr) {
	        op->fl.txt = false ;
	        uc_free(op->table) ;
	        op->table = nullptr ;
	    }
	} /* end if (babycalcs_loadtxt) */
	op->table = tblp ;
	return rs ;
}
/* end subroutine (babycalcs_shmupdate) */

static int babycalcs_shmaddwrite(BC *op,int fd) noex {
	off_t		tbloff ;
	uint		*hwp = (uint *) (op->mapdata + BABIESHDR_IDLEN) ;
	cint		esz = szof(BC_ENT) ;
	int		rs ;
	int		shmsz = 0 ;
	{
	    tbloff = hwp[babieshdrh_btoff] ;
	}
	shmsz = intsat(tbloff) ;
	if ((rs = u_seek(fd,tbloff,SEEK_SET)) >= 0) {
	    int		tblsz = ((op->nentries + 1) * esz) ;
	    if ((rs = u_write(fd,op->table,tblsz)) >= 0) {
	        tbloff += rs ;
		shmsz += rs ;
	        rs = uc_ftruncate(fd,tbloff) ;
	    }
	}
	return (rs >= 0) ? shmsz : rs ;
}
/* end subroutine (babycalcs_shmaddwrite) */

static int babycalcs_reloadtxt(BC *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	(void) dt ;
	if (op->fl.txt && (op->table != nullptr)) {
	    op->fl.txt = false ;
	    rs1 = uc_free(op->table) ;
	    if (rs >= 0) rs = rs1 ;
	    op->table = nullptr ;
	}
	if (rs >= 0) {
	    rs = babycalcs_loadtxt(op) ;
	}
	return rs ;
}
/* end subroutine (babycalcs_reloadtxt) */

static int babycalcs_shmcheck(BC *op,USTAT *sbp) noex {
	uint		*hwp = (uint *) (op->mapdata + BABIESHDR_IDLEN) ;
	cuint		dbtime = uint(sbp->st_mtime) ;
	cuint		dbsz = uint(sbp->st_size) ;
	cuint		shmsz = uint(op->shmsize) ;
	int		rs = SR_OK ;
	int		f = false ;
	f = f || (dbtime > hwp[babieshdrh_dbtime]) ;
	f = f || (dbsz != hwp[babieshdrh_dbsize]) ;
	f = f || (shmsz != hwp[babieshdrh_shmsize]) ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (babycalcs_shmcheck) */

static int babycalcs_shmaccess(BC *op,time_t dt) noex {
	int		rs = SR_BUGCHECK ;
	if (op->mapdata) {
	    if (dt == 0) dt = getustime ;
	    uint	*hwp = (uint *) (op->mapdata + BABIESHDR_IDLEN) ;
	    hwp[babieshdrh_atime] = uint(dt) ;
	    hwp[babieshdrh_acount] += 1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (babycalcs_shmaccess) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = ucpagesize) >= 0) {
	    pagesize = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
		maxpathlen = rs ;
	    }
	}
	return rs ;
}

static int entcmp(BC_ENT *e1p,BC_ENT *e2p) noex {
    	return intconv(e1p->date - e2p->date) ;
}

static int vcmpentry(cvoid **v1pp,cvoid **v2pp) noex {
	BC_ENT		*e1p = (BC_ENT *) *v1pp ;
	BC_ENT		*e2p = (BC_ENT *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
		    rc = entcmp(e1p,e2p) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpentry) */


