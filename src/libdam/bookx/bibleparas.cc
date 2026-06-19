/* bibleparas SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARAS implementation */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* non-switchable debug print-outs */
#define	CF_SAFE		0		/* normal safety */
#define	CF_INORDER	0		/* create indices as encountered */
#define	CF_TMPPRNAME	1		/* put under a PRNAME in /tmp */
#define	CF_STARTAUDIT	1		/* start w/ audit on BPI object */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bibleparas

	Description:
	This module implements an interface (a trivial one) that
	allows access to the BIBLEPARAS datbase.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<sys/mman.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibsubs.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */
#include	<dirseen.h>		/* LIBUC */
#include	<biblecite.h>		/* LIBDAM */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"bpi.h"
#include	"bpimk.h"
#include	"bibleparas.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |memclear(3u)| */
import uconstants ;

/* local defines */

#define	BIBLEPARAS_DBDNAME	"share/bibleparas"
#define	BIBLEPARAS_DBSUF	"txt"
#define	BIBLEPARAS_ENT		biblecite
#define	BIBLEPARAS_NLE		4	/* default number line entries */
#define	BIBLEPARAS_DIRMODE	0777
#define	BIBLEPARAS_IDXMODE	0664

#define	ISWHT(ch)	CHAR_ISWHITE(ch)

#ifndef	TMPVARDNAME
#define	TMPVARDNAME	"/var/tmp"
#endif

#define	INDCNAME	"bibleparas"
#define	INDSUF		"pi"

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)
#define	TO_CHECK	4

#define	EC		expcook

#define	DS		dirseen
#define	DS_C		dirseen_cur

#define	BPAS		bibleparas
#define	BPAS_Q		bibleparas_q
#define	BPAS_C		bibleparas_cur
#define	BPAS_I		bibleparas_info
#define	BPAS_E		biblecite

#define	SI		subinfo
#define	SI_FL		subinfo_fl


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_fl {
	uint		dummy:1 ;
} ; /* end struct */

struct subinfo {
	time_t		dt ;
	SI_FL		fl ;
} ; /* end struct */


/* forward references */

local int	bibleparas_dbloadbegin	(BPAS *,SI *) noex ;
local int	bibleparas_dbloadend	(BPAS *) noex ;
local int	bibleparas_dbmapcreate	(BPAS *,time_t) noex ;
local int	bibleparas_dbmapdestroy	(BPAS *) noex ;
local int	bibleparas_checkup	(BPAS *,time_t) noex ;
local int	bibleparas_indopen	(BPAS *,SI *) noex ;

local int	bibleparas_indopencheck	(BPAS *,cchar *) noex ;

local int	bibleparas_indclose	(BPAS *) noex ;
local int	bibleparas_indmk	(BPAS *,cchar *,time_t) noex ;
local int	bibleparas_indmkdata	(BPAS *,cchar *,mode_t) noex ;
local int	bibleparas_indopenseq	(BPAS *,SI *) noex ;
local int	bibleparas_indopenseqer	(BPAS *,SI *,
			DS *,vecstr *,EC *) noex ;
local int	bibleparas_loadcooks	(BPAS *,EC *) noex ;
local int	bibleparas_indopenalt	(BPAS *,SI *,DS *) noex ;

#ifdef	COMMENT
local int	bibleparas_mksysvarsi	(BPAS *,cchar *) noex ;
#endif

local int	subinfo_start		(SI *) noex ;
local int	subinfo_finish		(SI *) noex ;

local int	entry_start		(BPAS_E *,BIBLEPARAS_Q *) noex ;
local int	entry_finish		(BPAS_E *) noex ;

#ifdef	COMMENT
local int	entry_release		(BPAS_E *) noex ;
#endif

local int	bvemk_start		(bpimk_v *,BPAS_E *) noex ;
local int	bvemk_finish		(bpimk_v *) noex ;

local int	mkdname			(cchar *,mode_t) noex ;
local int	checkdname		(cchar *) noex ;

local int	isempty		(cchar *,int) noex ;
local int	sstart		(cchar *,int,BIBLEPARAS_Q *,int *) noex ;


/* local variables */

constexpr cpcchar	idxdirs[] = {
	"%R/var/%S",
	"/var/tmp/%{PRN}",
	"/var/tmp",
	"/tmp/%{PRN}",
	"/tmp",
	"%T",
	nullptr
} ; /* end array */


/* exported variables */

const bibleparas_obj	bibleparas_modinfo = {
	"bibleparas",
	szof(bibleparas),
	szof(int)
} ; /* end initialization */


/* exported variables */


/* exported subroutines */

int bibleparas_open(BPAS *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	cchar		*suf = BIBLEPARAS_DBSUF ;
	char		cname[MAXNAMELEN + 1] ;
	char		dbfname[MAXPATHLEN + 1] ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;
#endif

	if (pr == nullptr) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	if ((dbname == nullptr) || (dbname[0] == '\0')) {
	    dbname = BIBLEPARAS_DBNAME ;
	}

	memclear(op) ;
	op->pr = pr ;
	op->dbname = dbname ;

	if ((rs = snsds(cname,MAXNAMELEN,dbname,suf)) >= 0) {
	    cchar	*dn = BIBLEPARAS_DBDNAME ;
	    if ((rs = mkpath3(dbfname,pr,dn,cname)) >= 0) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(dbfname,-1,&cp)) >= 0) {
		    SI	si ;
	    	    op->dbfname = cp ;
	            if ((rs = subinfo_start(&si)) >= 0) {
	    	        if ((rs = bibleparas_dbloadbegin(op,&si)) >= 0) {
			    nv = op->nverses ;
			    op->magval = BIBLEPARAS_MAGIC ;
	                }
	    	        subinfo_finish(&si) ;
		    } /* end if (subinfo) */
		    if (rs < 0) {
	        	voidp vp = voidp(op->dbfname) ;
	        	rs1 = mem.free(vp) ;
	                op->dbfname = nullptr ;
		    } /* end if (error) */
	        } /* end if (m-a) */
	    } /* end if (mkpath) */
	} /* end if (snsds) */

	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_open) */

/* free up the entire vector string data structure object */
int bibleparas_close(BPAS *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    {
	        rs1 = bibleparas_dbloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbfname) {
	        voidp vp = voidp(op->dbfname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = bibleparas_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_close) */

int bibleparas_count(BPAS *op) noex {
	int		rs ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    rs = op->nverses ;
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_count) */

int bibleparas_audit(BPAS *op) noex {
	int		rs ;
	if ((rs = bibleparas_magic(op)) >= 0) {
	    if (op->fl.vind) {
	        rs = bpi_audit(&op->vind) ;
	    }
        } /* end if (biblebooks_magic) */
	return rs ;
} /* end subroutine (bibleparas_audit) */

/* get a string by its index */
int bibleparas_ispara(BPAS *op,con BIBLEPARAS_Q *qp) {
	int		rs ;
	int		f = false ;
	if ((rs = bibleparas_magic(op,qp)) >= 0) {
	    /* check for update */
	    time_t	dt = 0 ;
	    if (op->ncursors == 0) {
	        rs = bibleparas_checkup(op,dt) ;
	    }
	    if (rs >= 0) {
	        BPI_Q	viq ;
	        viq.b = qp->b ;
	        viq.c = qp->c ;
	        viq.v = qp->v ;
	        if ((rs = bpi_get(&op->vind,&viq)) >= 0) {
	            f = true ;
	        } else if (rs == SR_NOTFOUND) 
	            rs = SR_OK ;
	    } /* end if (ok) */
        } /* end if (biblebooks_magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleparas_ispara) */

int bibleparas_curbegin(BPAS *op,BIBLEPARAS_CUR *curp) noex {
	int		rs ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARAS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;

	if ((rs = bpi_curbegin(&op->vind,&curp->vicur)) >= 0) {
	    op->ncursors += 1 ;
	}

	return rs ;
} /* end subroutine (bibleparas_curbegin) */

int bibleparas_curend(BPAS *op,BIBLEPARAS_CUR *curp) noex {
	int		rs ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARAS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;

	rs = bpi_curend(&op->vind,&curp->vicur) ;
	if (op->ncursors > 0)
	    op->ncursors -= 1 ;

	return rs ;
} /* end subroutine (bibleparas_curend) */

int bibleparas_curenum(BPAS *op,BIBLEPARAS_CUR *curp,BIBLEPARAS_Q *qp) noex {
	time_t		dt = 0 ;
	int		rs = SR_OK ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARAS_MAGIC) return SR_NOTOPEN ;
#endif

	if (curp == nullptr) return SR_FAULT ;
	if (qp == nullptr) return SR_FAULT ;

	if (op->ncursors == 0) {
	    rs = bibleparas_checkup(op,dt) ;
	}

	if (rs >= 0) {
	    BPI_VERSE	viv ;
	    if ((rs = bpi_curenum(&op->vind,&curp->vicur,&viv)) >= 0) {
	        if (qp) {
		    qp->b = viv.b ;
		    qp->c = viv.c ;
		    qp->v = viv.v ;
	        }
	    } /* end if (bpi_curenum) */
	} /* end if (ok) */

#if	CF_DEBUG
	debugprintf("bibleparas_enum: ret rs=%d\n", rs) ;
#endif

	return rs ;
} /* end subroutine (bibleparas_curenum) */

int bibleparas_getinfo(BPAS *op,BIBLEPARAS_INFO *ip) noex {
	int		rs ;
	int		nverses = 0 ;

#if	CF_SAFE
	if (op == nullptr) return SR_FAULT ;

	if (op->magval != BIBLEPARAS_MAGIC) return SR_NOTOPEN ;
#endif

	bpi_info	bi ;
	if ((rs = bpi_getinfo(&op->vind,&bi)) >= 0) {
	    nverses = bi.count ;
	    if (ip != nullptr) {
	        memclear(ip) ;
	        ip->dbtime = op->ti_db ;
	        ip->vitime = op->ti_vind ;
	        ip->maxbook = bi.maxbook ;
	        ip->maxchapter = bi.maxchapter ;
	        ip->nverses = bi.count ;
	        ip->nzverses = bi.nzverses ;
	    }
	} /* end if (bpi_getinfo) */

	return (rs >= 0) ? nverses : rs ;
} /* end subroutine (bibleparas_getinfo) */


/* private subroutines */

local int bibleparas_dbloadbegin(BPAS *op,SI *sip) noex {
	int		rs ;

#if	CF_DEBUG
	debugprintf("bibleparas_dbloadbegin: ent\n") ;
#endif

	if ((rs = bibleparas_dbmapcreate(op,sip->dt)) >= 0) {
	    rs = bibleparas_indopen(op,sip) ;
	    if (rs < 0)
		bibleparas_dbmapdestroy(op) ;
	}

#if	CF_DEBUG
	debugprintf("bibleparas_dbloadbegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
} /* end subroutine (bibleparas_dbloadbegin) */

local int bibleparas_dbloadend(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	rs1 = bibleparas_indclose(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = bibleparas_dbmapdestroy(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleparas_dbloadend) */

local int bibleparas_dbmapcreate(BPAS *op,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	debugprintf("bibleparas_dbmapcreate: dbfname=%s\n",op->dbfname) ;
	if ((rs = u_open(op->dbfname,O_RDONLY,0666)) >= 0) {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
	            if (sb.st_size <= INT_MAX) {
	                size_t	ms ;
	                cint	mp = PROT_READ ;
	                cint	mf = MAP_SHARED ;
	                void	*md ;
			op->filesize = (sb.st_size & UINT_MAX) ;
			op->ti_db = sb.st_mtime ;
	    		ms = (size_t) op->filesize ;
	    		if ((rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
			    cint	madv = MADV_RANDOM ;
			    const caddr_t	ma = md ;
	        	    if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	            		op->mapdata = md ;
	            		op->mapsize = ms ;
	            		op->ti_map = dt ;
	            		op->ti_lastcheck = dt ;
			    }
	        	    if (rs < 0) {
		    	        u_munmap(md,ms) ;
	                        op->mapdata = nullptr ;
	                        op->mapsize = 0 ;
	                    } /* end if (error) */
	                } /* end if (u_mmap) */
	            } else
	                rs = SR_TOOBIG ;
	        } else
	            rs = SR_NOTSUP ;
	    } /* end if (stat) */
	    u_close(fd) ;
	} /* end if (file) */

	return rs ;
} /* end subroutine (bibleparas_dbmapcreate) */

local int bibleparas_dbmapdestroy(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata) {
	    rs1 = u_munmap(op->mapdata,op->mapsize) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
} /* end subroutine (bibleparas_dbmapdestroy) */

local int bibleparas_checkup(BPAS *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->ncursors == 0) {
	    if (dt <= 0) dt = time(nullptr) ;
	    if ((dt - op->ti_lastcheck) >= TO_CHECK) {
	        op->ti_lastcheck = dt ;
		if (ustat sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	            f = f || (sb.st_mtime > op->ti_db) ;
	            f = f || (sb.st_mtime > op->ti_map) ;
	            if (f) {
		        SI	si ;
	                bibleparas_dbloadend(op) ;
	                if ((rs = subinfo_start(&si)) >= 0) {
	                    rs = bibleparas_dbloadbegin(op,&si) ;
		            subinfo_finish(&si) ;
	                } /* end if */
	            } /* end if (update) */
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (timed-out) */
	} /* end if (no cursors out) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (bibleparas_checkup) */

local int bibleparas_indopen(BPAS *op,SI *sip) noex {
	int		rs ;
	if ((rs = bibleparas_indopenseq(op,sip)) >= 0) {
	    if (op->fl.vind) {
	        rs = bpi_count(&op->vind) ;
	        op->nverses = rs ;
	    }
	}
	return rs ;
} /* end subroutine (bibleparas_indopen) */

local int bibleparas_indopenseq(BPAS *op,SI *sip) noex {
	int		rs ;
	int		rs1 ;
	if (DS ds ; (rs = dirseen_start(&ds)) >= 0) {
	    vecstr	sdirs ;
	    cint	vopts = vecstrm.compact ;
	    if ((rs = vecstr_start(&sdirs,6,vopts)) >= 0) {
	        EC	cooks ;
	        if ((rs = expcook_start(&cooks)) >= 0) {
	            if ((rs = bibleparas_loadcooks(op,&cooks)) >= 0) {
	                rs = bibleparas_indopenseqer(op,sip,&ds,&sdirs,&cooks) ;
	            }
	            rs1 = expcook_finish(&cooks) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cooks) */
		rs1 = vecstr_finish(&sdirs) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sdirs) */
	    rs1 = dirseen_finish(&ds) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (dirseen) */
	return rs ;
} /* end subroutines (bibleparas_indopenseq) */

local int bibleparas_indopenseqer(BPAS *op,SI *sip,DS *dsp,
		vecstr *sdp,EC *ecp) noex {
	cint	rsn = SR_NOTFOUND ;
	cint	elen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i ;
	char		ebuf[MAXPATHLEN + 1] ;
	char		pbuf[MAXPATHLEN + 1] ;

/* first phase: expand possible directory paths */

	for (i = 0 ; (rs >= 0) && (idxdirs[i] != nullptr) ; i += 1) {
	    if ((rs = expcook_exp(ecp,0,ebuf,elen,idxdirs[i],-1)) > 0) {
		if ((rs = pathclean(pbuf,ebuf,rs)) > 0) {
		    cint	plen = rs ;
		    if ((rs = dirseen_havename(dsp,pbuf,plen)) == rsn) {
			rs = dirseen_add(dsp,pbuf,plen,nullptr) ;
		    }
		}
	    }
	} /* end for */

/* next phase: create DB file-paths from directories */

	if (rs >= 0) {
	    DS_C	cur ;
	    if ((rs = dirseen_curbegin(dsp,&cur)) >= 0) {
		int	el ;
	        while ((el = dirseen_curenum(dsp,&cur,ebuf,elen)) >= 0) {
		    if ((rs = mkpath2(pbuf,ebuf,op->dbname)) >= 0) {
		        rs = vecstr_add(sdp,pbuf,rs) ;
		    }
		    if (rs < 0) break ;
	        } /* end while */
		if ((rs >= 0) && (el != SR_NOTFOUND)) rs = el ;
	        dirseen_curend(dsp,&cur) ;
	    } /* end if (dirseen-cur) */
	} /* end if (ok) */

/* final phase: try to open one of them trying them in-sequence */

	if (rs >= 0) {
	    cchar	**dv ;
	    if ((rs = vecstr_getvec(sdp,&dv)) >= 0) {
		for (i = 0 ; dv[i] != nullptr ; i += 1) {
		    rs = bibleparas_indopencheck(op,dv[i]) ;
		    if ((rs >= 0) || (! isNotPresent(rs))) break ;
		} /* end for */
	    } /* end if (vecstr_getvex) */
	    if ((rs < 0) && isNotPresent(rs)) {
	        rs = bibleparas_indopenalt(op,sip,dsp) ;
	    }
	} /* end if (ok) */

	return rs ;
} /* end subroutines (bibleparas_indopenseqer) */

local int bibleparas_loadcooks(BPAS *op,EC *ecp) noex {
	static cchar	*tmpdname = getenver(varname.tmpdir) ;
	int		rs = SR_OK ;
	cchar		*ks = "RST" ;
	char		kbuf[2] ;

	if (tmpdname == nullptr) tmpdname = sysword.w_tmpdir ;

	    kbuf[1] = '\0' ;
	    for (int i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	        vap = nullptr ;
	        val = -1 ;
	        switch (int kch = MKCHAR(ks[i]) ; kch) {
	        case 'R':
	            vap = op->pr ;
	            break ;
	        case 'S':
	            vap = INDCNAME ;
	            break ;
	        case 'T':
	            vap = tmpdname ;
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && vap) {
	            kbuf[0] = kch ;
	            rs = expcook_add(ecp,kbuf,vap,val) ;
	        }
	    } /* end for */
	if (rs >= 0) {
	    cchar	*prname ;
	    if ((rs = sfbasename(op->pr,-1,&prname)) >= 0) {
	        rs = SR_NOENT ;
	        if (prname != nullptr) {
	            rs = expcook_add(ecp,"PRN",prname,-1) ;
		}
	    }
	}

	return rs ;
} /* end subroutines (bibleparas_loadcooks) */

local int bibleparas_indopenalt(BPAS *op,SI *sip,DS *dsp) noex {
	DS_C		cur ;
	cint		elen = MAXPATHLEN ;
	int		rs ;

#if	CF_DEBUG
	debugprintf("bibleparas_indopenalt: ent\n") ;
#endif

	if ((rs = dirseen_curbegin(dsp,&cur)) >= 0) {
	    int		el ;
	    char	ebuf[MAXPATHLEN + 1] ;
	    char	indname[MAXPATHLEN + 1] ;

	    while ((el = dirseen_curenum(dsp,&cur,ebuf,elen)) >= 0) {
	        if (el > 0) {
	            if ((rs = bibleparas_indmk(op,ebuf,sip->dt)) >= 0) {
		        if ((rs = mkpath2(indname,ebuf,op->dbname)) >= 0) {
	                    rs = bpi_open(&op->vind,indname)  ;
	                    op->fl.vind = (rs >= 0) ;
		        }
	            }
		}
		if ((rs >= 0) || (! isNotPresent(rs))) break ;
	    } /* end while */

	    dirseen_curend(dsp,&cur) ;
	} /* end if (cursor) */

#if	CF_DEBUG
	debugprintf("bibleparas_indopenalt: mid rs=%d f_vind=%u\n",rs,
	    op->fl.vind) ;
#endif

#if	CF_STARTAUDIT
	if ((rs >= 0) && op->fl.vind) {
	    rs = bpi_audit(&op->vind) ;
	}
#endif /* CF_STARTAUDIT */

#if	CF_DEBUG
	debugprintf("bibleparas_indopenalt: ret rs=%d n",rs) ;
#endif

	return rs ;
} /* end subroutines (bibleparas_indopenalt) */

local int bibleparas_indopencheck(BPAS *op,cchar *dbname) noex {
	int		rs ;
	int		nv = 0 ; /* return-value */
	if ((rs = bpi_open(&op->vind,dbname)) >= 0) {
	    if (bpi_info binfo ; (rs = bpi_getinfo(&op->vind,&binfo)) >= 0) {
		if (binfo.ctime >= op->ti_db) {
		    op->fl.vind = true ;
		} else {
		    rs = SR_STALE ;
		}
	    } /* end if (bpi_getinfo) */
	    if (rs < 0) {
	        bpi_close(&op->vind) ;
	    } /* end if (error) */
	} /* end if (bpi_open) */
	return (rs >= 0) ? nv : rs ;
} /* end subroutine (bibleparas_indopencheck) */

local int bibleparas_indmk(BPAS *op,cchar dname[],time_t dt) noex {
	const mode_t	dm = BIBLEPARAS_DIRMODE ;
	int		rs ;
	int		c = 0 ; /* return-value */
	if ((rs = mkdname(dname,dm)) >= 0) {
	    cchar	*db = op->dbname ;
	    char	indname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(indname,dname,db)) >= 0) {
		cmode	om = BIBLEPARAS_IDXMODE ;
		if ((rs = bibleparas_indmkdata(op,indname,om)) >= 0) {
		    c += rs ;
	            op->ti_vind = dt ;
		}
	     }
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleparas_indmk) */

local int bibleparas_indmkdata(BPAS *op,cchar indname[],mode_t om) noex {
	BPIMK		bpind ;
	bpimk_v	bve ;
	cint	of = 0 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = bpimk_open(&bpind,indname,of,om)) >= 0) {
	    c = rs ;
	    if (rs == 0) {
	        BPAS_E	e ;
	        BIBLEPARAS_Q	q ;
	        uint		fileoff = 0 ;
	        int		len ;
	        int		ml = op->mapsize ;
	        int		ll ;
	        int		si ;
	        bool		f_ent = false ;
	        cchar		*mp = op->mapdata ;
		cchar		*lp ;
	        cchar		*tp ;
	        while ((tp = strnchr(mp,ml,'\n')) != nullptr) {

	    len = ((tp + 1) - mp) ;
	    lp = mp ;
	    ll = (len - 1) ;

	    if ((ll > 0) && (! isempty(lp,ll))) {

#if	CF_DEBUG
	debugprintf("bibleparas_indmkdata: line>%r<\n",
		lp,strnlen(lp,MIN(ll,40))) ;
#endif

	        if ((tp = strnchr(lp,ll,'#')) != nullptr) {
	            ll = (tp - lp) ;
		}
	        if ((rs = isstart(lp,ll,&q,&si)) > 0) {

	            if (f_ent) {
	                c += 1 ;
	                if ((rs = bvemk_start(&bve,&e)) >= 0) {
	                    rs = bpimk_add(&bpind,&bve) ;
			    bvemk_finish(&bve) ;
		        }
			f_ent = false ;
	                entry_finish(&e) ;
	            }

	            if (rs >= 0) {
	                rs = entry_start(&e,&q) ;
			if (rs >= 0) f_ent = true ;
		    }

	        } /* end if (entry start of add) */

	    } /* end if (not empty) */

	    fileoff += len ;
	    ml -= len ;
	    mp += len ;

	    if (rs < 0) break ;
	} /* end while (readling lines) */

	if ((rs >= 0) && f_ent) {
	    c += 1 ;
	    if ((rs = bvemk_start(&bve,&e)) >= 0) {
	        rs = bpimk_add(&bpind,&bve) ;
		bvemk_finish(&bve) ;
	    }
	    f_ent = false ;
	    entry_finish(&e) ;
	}

	if (f_ent) {
	    f_ent = false ;
	    entry_finish(&e) ;
	}

#if	CF_DEBUG
	{
	    BPIMK_INFO	bi ;
	    rs1 = bpimk_getinfo(&bpind,&bi) ;
	    debugprintf("bibleparas_indmkdata: maxbook=%u\n",
		bi.maxbook) ;
	    debugprintf("bibleparas_indmkdata: maxchapter=%u\n",
		bi.maxchapter) ;
	    debugprintf("bibleparas_indmkdata: maxverse=%u\n",
		bi.maxverse) ;
	    debugprintf("bibleparas_indmkdata: nverses=%u\n",
		bi.nverses) ;
	    debugprintf("bibleparas_indmkdata: nzverses=%u\n",
		bi.nzverses) ;
	}
#endif /* CF_DEBUG */

	    } /* end if (creation needed) */
	    rs1 = bpimk_close(&bpind) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bpimk) */

#if	CF_DEBUG
	debugprintf("bibleparas_indmkdata: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
} /* end subroutine (bibleparas_indmkdata) */

local int bibleparas_indclose(BPAS *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.vind) {
	    op->fl.vind = false ;
	    rs1 = bpi_close(&op->vind) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (bibleparas_indclose) */

local int subinfo_start(SI *sip) noex {
	int		rs = SR_BUGCHECK ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->dt = time(nullptr) ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_BUGCHECK ;
	if (sip) {
	    rs = SR_OK ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (subinfo_finish) */

local int entry_start(BPAS_E *ep,BIBLEPARAS_Q *qp) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->b = qp->b ;
	    ep->c = qp->c ;
	    ep->v = qp->v ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_finish(BPAS_E *ep) noex {
    	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (entry_finish) */

local int bvemk_start(bpimk_v *bvep,BPAS_E *ep) noex {
	int		rs = SR_BUGCHECK ;
	if (ep) {
	    rs = SR_OK ;
	    bvep->nlines = 0 ;
	    bvep->b = ep->b ;
	    bvep->c = ep->c ;
	    bvep->v = ep->v ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvemk_start) */

local int bvemk_finish(bpimk_v *bvep) noex {
    	int		rs = SR_BUGCHECK ;
	if (bvep) {
	    rs = SR_OK ;
	} /* end if (bug-check) */
	return rs ;
} /* end subroutine (bvemk_finish) */

local int mkdname(cchar *dname,mode_t dm) noex {
	cint		nrs = SR_NOENT ;
	int		rs ;
	if ((rs = checkdname(dname)) == nrs) {
	    rs = mkdirs(dname,dm) ;
	}
	return rs ;
} /* end subroutine (mkdname) */

local int checkdname(cchar *dname) noex {
	int		rs = SR_OK ;
	if (dname[0] == '/') {
	    if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
		if (S_ISDIR(sb.st_mode)) {
		    rs = perm(dname,-1,-1,nullptr,W_OK) ;
		} /* end if (is-dir) */
	    } /* end if (u_stat) */
	} else {
	    rs = SR_INVALID ;
	}
	return rs ;
} /* end subroutine (checkdname) */

#ifdef	COMMENT
local int isempty(cchar *lp,int ll) noex {
	int		cl ;
	bool		f = false ;
	f = f || (ll == 0) ;
	f = f || (lp[0] == '#') ;
	if ((! f) && CHAR_ISWHITE(*lp)) {
	    cchar	*cp ;
	    cl = sfskipwhite(lp,ll,&cp) ;
	    f = f || (cl == 0) ;
	    f = f || (cp[0] == '#') ;
	}
	return f ;
} /* end subroutine (isempty) */
#endif /* COMMENT */


