/* dirwatch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* directory watch */
/* version %I% last-modified %G% */

#define	CF_FNAMECMP	1		/* ? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for use in the RBBPOST daemon
	program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dw

	Description:
	This object is used to watch a directory path for additions
	or changes in the file objects under that path.  Subdirectories
	are allowed.  All files under the directory path (including
	those in subdirectories) are treated the same as an entry.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucfileop.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<absfn.h>		/* LIBUC */
#include	<fsdir.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<pathadd.h>		/* LIBUC */
#include	<strwcpy.h>		/* LIBUC */
#include	<strdcpy.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"dirwatch.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	DW		dirwatch
#define	IENT		dirwatch_ient
#define	DW_FL		dirwatch_fl
#define	DW_ENT		dirwatch_ent
#define	DW_CUR		dirwatch_cur
#define	DW_MAG		DIRWATCH_MAGIC
#define	DW_INTCK	DIRWATCH_INTCK

#define	MININTCHECK	2		/* minumun check interval (seconds) */
#define	MAXINTOPEN	300		/* maximum FD cache interval */
#define	MAXINTIDLE	240		/* maximum allowable idle interval */

#ifndef	CF_FNAMECMP
#define	CF_FNAMECMP	0
#endif

/* returns */
#define	DW_SRESERVED	dwstate_reserved
#define	DW_SNEW		dwstate_new
#define	DW_SCHECK	dwstate_check	/* ready for validation check */
#define	DW_SREADY	dwstate_ready
#define	DW_SNOTSUP	dwstate_notdup
#define	DW_SRUNNING	dwstate_running
#define	DW_SRETRY	dwstate_retry
#define	DW_SUSER	dwstate_user


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structure */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct intvals {
	static constexpr int	mincheck	= MININTCHECK ;
	static constexpr int	maxopen		= MAXINTOPEN ;
	static constexpr int	maxidle		= MAXINTIDLE ;
    } ; /* end struct (intvals) */
} /* end namespace */

struct dirwatch_ient {
	cchar		*name ;
	time_t		itime ;
	time_t		timod ;
	size_t		fsize ;
	int		state ;
} ; /* end struct (dirwatch_ient) */


/* forward references */

template<typename ... Args>
local int dirwatch_ctor(DW *op,Args ... args) noex {
    	DW		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->elp = new(nothrow) vecobj) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dirwatch_ctor) */

local int dirwatch_dtor(DW *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->elp) ylikely {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dirwatch_dtor) */

template<typename ... Args>
local inline int dirwatch_magic(DW *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == DW_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (dirwatch_magic) */

local int	dirwatch_starter(DW *,cchar *) noex ;
local int	dirwatch_finents(DW *) noex ;
local int	dirwatch_checker(DW *,time_t) noex ;
local int	dirwatch_checkx(DW *,time_t) noex ;
local int	dirwatch_checknew(DW *,time_t,char *) noex ;
local int	dirwatch_checknewent(DW *,time_t,IENT *,char *) noex ;
local int	dirwatch_checkrm(DW *,time_t,char *) noex ;
local int	dirwatch_checkrmer(DW *,char *,time_t) noex ;
local int	dirwatch_checkrment(DW *,time_t,IENT *,cchar *) noex ;
local int	dirwatch_scan(DW *,time_t) noex ;
local int	dirwatch_scaner(DW *,char *,int,time_t) noex ;
local int	dirwatch_scanent(DW *,cchar *,cchar *,time_t) noex ;
local int	dirwatch_findi(DW *,cchar *,IENT **) noex ;
local int	dirwatch_diropen(DW *,time_t) noex ;
local int	dirwatch_dirclose(DW *) noex ;

local int	ient_start(IENT *,DW *,cchar *,ustat *) noex ;
local int	ient_finish(IENT *,DW *) noex ;

local int	entry_load(DW_ENT *,IENT *,cchar *) noex ;

extern "C" {
    local int	vcmpfn(cvoid **,cvoid **) noex ;
}


/* local variables */

constexpr intvals	intval ;
static vars		var ;
const int		rsn = SR_NOTFOUND ;
const bool		f_fnamecmp = CF_FNAMECMP ;


/* exported variables */


/* exported subroutines */

int dirwatch_start(DW *op,cchar *dirname,int intck) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirwatch_ctor(op,dirname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dirname[0]) ylikely {
		cchar	*dn ;
		if (absfn d ; (rs = d.start(dirname,-1,&dn)) >= 0) ylikely {
		    {
	    		op->intcheck = (intck > 2) ? intck : DW_INTCK ;
		        rs = dirwatch_starter(op,dn) ;
		    }
		    rs1 = d.finish ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (absfn) */
	    } /* end if (valid) */
	    if (rs < 0) {
		dirwatch_dtor(op) ;
	    } /* end if (error) */
	} /* end if (dirwatch_ctor) */
	return rs ;
} /* end subroutine (dirwatch_start) */

local int dirwatch_starter(DW *op,cchar *dn) noex {
	int		rs ;
	if (static cint	rsv = var ; (rs = rsv) >= 0) ylikely {
	    cint	sz = szof(DW_ENT) ;
	    cint	vn = 10 ;
	    cint	vo = (vecobjm.stationary | vecobjm.conserve) ;
	    op->fd = -1 ;
	    op->count_new = 0 ;
	    op->count_checkable = 0 ;
            /* initialize */
	    if ((rs = vecobj_start(op->elp,sz,vn,vo)) >= 0) ylikely {
	        custime		dt = getustime ;
	        op->timod = 0 ;
	        op->tiopen = 0 ;
	        op->ticheck = dt ;
	        op->tiremove = dt ;
	        if (cchar *cp ; (rs = lm_strw(dn,-1,&cp)) >= 0) ylikely {
	            if (rs <= var.maxpathlen) {
	                op->dirname = cp ;
	                rs = dirwatch_scan(op,dt) ;
	                if (rs == SR_NOENT) rs = SR_OK ;
			if (rs >= 0) {
	                    op->magval = DW_MAG ;
			}
		    } else {
		        rs = SR_TOOBIG ;
	            } /* end if (size OK) */
	            if (rs < 0) {
			void *vp = voidp(cp) ;
	                lm_free(vp) ;
		    } /* end if (error) */
	        } /* end if (m-a) */
	        if (rs < 0) {
	            vecobj_finish(op->elp) ;
	        } /* end if (error) */
	    } /* end if (vecobj-start) */
	} /* end if (vars) */
	return rs ;
} /* end subroutine (dirwatch_starter) */

int dirwatch_finish(DW *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirwatch_magic(op)) >= 0) ylikely {
	    {
		rs1 = dirwatch_finents(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->elp) {
		rs1 = vecobj_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = dirwatch_dirclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dirname) {
	        void *vp = voidp(op->dirname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dirname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = dirwatch_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dirwatch_finish) */

local int dirwatch_finents(DW *op) noex {
    	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	int		rs2 ;
	if (op->elp) ylikely {
	    vecobj	*elp = op->elp ;
	    void	*vp{} ;
	    rs = SR_OK ;
	    for (int i = 0 ; (rs2 = elp->get(i,&vp)) >= 0 ; i += 1) {
		IENT	*iep = (IENT *) vp ;
	        if (vp) {
	            rs1 = ient_finish(iep,op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	}
	return rs ;
} /* end subroutine (dirwatch_finents) */

int dirwatch_curbegin(DW *op,DW_CUR *curp) noex {
    	int		rs ;
	if ((rs = dirwatch_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dirwatch_curbegin) */

int dirwatch_curend(DW *op,DW_CUR *curp) noex {
    	int		rs ;
	if ((rs = dirwatch_magic(op)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dirwatch_curend) */

int dirwatch_curenum(DW *op,DW_CUR *curp,DW_ENT *dep,char *rbuf,int rlen) noex {
	int		rs ;
	int		nlen = 0 ;
	if ((rs = dirwatch_magic(op,curp)) >= 0) ylikely {
	    IENT	*iep = nullptr ;
	    int		i ; /* used-afterwards */
	    if ((curp == nullptr) || (curp->i < 0)) {
	        i = 0 ;
	    } else {
	        i = curp->i + 1 ;
	    }
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        iep = (IENT *) vp ;
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && iep) {
	        if ((rs >= 0) && dep) {
	            rs = entry_load(dep,iep,rbuf) ;
		    nlen = rs ;
	        }
		if ((rs >= 0) && rbuf) {
		    rs = sncpy(rbuf,rlen,iep->name) ;
		    nlen = rs ;
		}
	    } /* end if (ok) */
	    if ((rs >= 0) && curp) {
	        curp->i = i ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? nlen : rs ;
} /* end subroutine (dirwatch_curenum) */

/* enumerate those entries that are "checkable" */
int dirwatch_curenumck(DW *op,DW_CUR *curp,DW_ENT *dep,
		char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = dirwatch_magic(op)) >= 0) ylikely {
	    IENT	*iep = nullptr ; /* used across blocks */
	    /* get the next entry with a checkable file */
	    if ((curp == nullptr) || (curp->i < 0)) {
	        i = 0 ;
	    } else {
	        i = curp->i + 1 ;
	    }
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	        if (vp) {
		    iep = (IENT *) vp ;
		    if (iep->state == DW_SCHECK) break ;
	        }
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && iep) {
	        if (dep) {
	            rs = entry_load(dep,iep,rbuf) ;
	        }
		if ((rs >= 0) && rbuf) {
		    rs = sncpy(rbuf,rlen,iep->name) ;
		}
	        if ((rs >= 0) && curp) {
	            curp->i = i ;
	        }
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (dirwatch_curenumck) */

int dirwatch_del(DW *op,DW_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirwatch_magic(op,curp)) >= 0) ylikely {
	    void	*vp{} ;
	    if ((rs = vecobj_get(op->elp,curp->i,&vp)) >= 0) ylikely {
		IENT	*iep = (IENT *) vp ;
	        if (vp) {
		    {
	                rs1 = ient_finish(iep,op) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = vecobj_del(op->elp,curp->i) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        }
	    } /* end if */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dirwatch_del) */

int dirwatch_find(DW *op,cchar *name,DW_ENT *dep,char *rbuf,int rlen) noex {
	int		rs ;
	int		i = 0 ; /* return-value */
	if ((rs = dirwatch_magic(op,name)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) {
		vecobj	*elp = op->elp ;
	        IENT	*iep = nullptr ;
	        void	*vp{} ;
		if_constexpr (f_fnamecmp) {
	            IENT	ie{} ;
	            ie.name = charp(name) ;
	            if ((rs = elp->search(&ie,vcmpfn,&vp)) >= 0) {
	                iep = (IENT *) vp ;
	            }
		} else {
	            for (i = 0 ; (rs = elp->get(i,&vp)) >= 0 ; i += 1) {
	                if (vp) {
		            iep = (IENT *) vp ;
	                    if (strcmp(name,iep->name) == 0) break ;
	                }
	            } /* end for (looping through entries) */
		} /* end if_constexpr (f_fnamecmp) */
	        if ((rs >= 0) && iep) {
		    if (dep) {
	                rs = entry_load(dep,iep,rbuf) ;
		    }
		    if ((rs >= 0) && rbuf) {
		        rs = sncpy(rbuf,rlen,iep->name) ;
		    }
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? i : rs ;
} /* end subroutine (dirwatch_find) */

int dirwatch_check(DW *op,time_t dt) noex {
    	int		rs ;
	int		n = 0 ;
	if ((rs = dirwatch_magic(op)) >= 0) ylikely {
	    if (dt == 0) dt = getustime ;
	    if ((dt - op->ticheck) >= intval.mincheck) {
		op->ticheck = dt ;
	        if ((rs = dirwatch_diropen(op,dt)) >= 0) {
		    rs = dirwatch_checker(op,dt) ;
		    n = rs ;
	        } /* end if (dirwatch_diropen) */
	    } /* end if (check-time) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_check) */

local int dirwatch_checker(DW *op,time_t dt) noex {
    	int		rs ;
	int		n = 0 ;
	if (ustat sb ; (rs = uc_fstat(op->fd,&sb)) >= 0) ylikely {
	    bool	f = false ;
	    f = f || (sb.st_mtime >= op->timod) ;
	    f = f || ((dt = op->ticheck) >= intval.maxidle) ;
	    if (f) {
		op->timod = sb.st_mtime ;
		rs = dirwatch_scan(op,dt) ;
		n = rs ;
	    } else if ((dt = op->tiopen) >= intval.maxopen) {
		uc_close(op->fd) ;
		op->fd = -1 ;
	    } /* end if */
	    if (rs >= 0) ylikely {
	        rs = dirwatch_checkx(op,dt) ;
	    } /* end if (ok) */
	} /* end if (stat) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_checker) */

local int dirwatch_checkx(DW *op,time_t dt) noex {
    	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) ylikely {
	    if ((rs = dirwatch_checknew(op,dt,dbuf)) >= 0) {
	        rs = dirwatch_checkrm(op,dt,dbuf) ;
		n = rs ;
	    }
	    rs1 = lm_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_checkx) */

local int dirwatch_checknew(DW *op,time_t dt,char *dbuf) noex {
    	int		rs = SR_OK ;
	int		rs2 ;
	if (op->count_new > 0) ylikely {
		vecobj	*elp = op->elp ;
	        void	*vp{} ;
	        for (int i = 0 ; (rs2 = elp->get(i,&vp)) >= 0 ; i += 1) {
		    IENT	*iep = (IENT *) vp ;
	            if (vp) {
	                if (iep->state == DW_SNEW) {
	                    if ((dt - iep->itime) > (op->intcheck / 4)) {
			    	rs = dirwatch_checknewent(op,dt,iep,dbuf) ;
	                    } /* end if (intcheck) */
		        } /* end if (equal) */
		    } /* end if (non-null) */
	        } /* end for */
	} /* end if (non-zero positive new entries) */
	return rs ;
} /* end subroutine (dirwatch_checknew) */

local int dirwatch_checknewent(DW *op,time_t dt,IENT *iep,char *dbuf) noex {
	int		rs ;
	int		n = 0 ;
	if (ustat sb ; (rs = uc_stat(dbuf,&sb)) >= 0) ylikely {
	    if ((dt - sb.st_mtime) >= op->intcheck) {
		iep->state = DW_SCHECK ;
		op->count_new -= 1 ;
		op->count_checkable += 1 ;
		n = 1 ;
	    } /* end if */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (stat) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_checknewent) */

local int dirwatch_checkrm(DW *op,time_t dt,char *dbuf) noex {
    	int		rs = SR_OK ;
	if ((dt - op->tiremove) >= intval.maxidle) ylikely {
	    vecobj	*elp = op->elp ;
	    op->tiremove = dt ;
	    if ((rs = elp->count) > 0) {
		rs = dirwatch_checkrmer(op,dbuf,dt) ;
	    } /* end if (vecobj_count) */
	} /* end if (idle interval) */
	return rs ;
} /* end subroutine (dirwatch_checkrm) */

local int dirwatch_checkrmer(DW *op,char *dbuf,time_t dt) noex {
    	vecobj		*elp = op->elp ;
    	int		rs = SR_OK ;
	int		rs2 ;
	void		*vp{} ;
	for (int i = 0 ; (rs2 = elp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		IENT	*iep = (IENT *) vp ;
		if ((dt - iep->itime) >= intval.maxidle) {
		    cchar	*den = iep->name ;
		    if ((rs = mkpath(dbuf,op->dirname,den)) >= 0) {
			if ((rs = dirwatch_checkrment(op,dt,iep,dbuf)) > 0) {
			    rs = elp->del(i--) ;
			}
		    } /* end if (idle period) */
		} /* end if (idle) */
		if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	} /* end for */
	return rs ;
} /* end subroutine (dirwatch_checkrmer) */

local int dirwatch_checkrment(DW *op,time_t dt,IENT *iep,cchar *dbuf) noex {
    	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (ustat sb ; (rs = uc_stat(dbuf,&sb)) >= 0) {
	    iep->itime = dt ;
	} else if (isNotPresent(rs)) {
	    f = true ;
	    {
	        rs1 = ient_finish(iep,op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (could not 'stat') */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (dirwatch_checkrment) */

int dirwatch_state(DW *op,int i,int state) noex {
	int		rs ;
	int		state_prev = 0 ;
	if ((rs = dirwatch_magic(op)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (state >= 0) {
	        void	*vp{} ;
	        if ((rs = vecobj_get(op->elp,i,&vp)) >= 0) {
	            IENT	*iep = (IENT *) vp ;
	            state_prev = iep->state ;
	            iep->state = state ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? state_prev : rs ;
} /* end subroutine (dirwatch_state) */


/* private subroutines */

local int dirwatch_scan(DW *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) ylikely {
	    if ((rs = mkpath(dbuf,op->dirname)) >= 0) ylikely {
		if ((rs = dirwatch_scaner(op,dbuf,rs,dt)) >= 0) ylikely {
		    n = rs ;
		}
	    } /* end if (mkpath) */
	    rs1 = lm_free(dbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_scan) */

/* "do" the subdirectory */
local int dirwatch_scaner(DW *op,char *dbuf,int dl,time_t dt) noex {
    	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (char *nbuf ; (rs = lm_mn(&nbuf)) >= 0) ylikely {
	    cint	nlen = rs ;
	    if (fsdir d ; (rs = d.open(dbuf)) >= 0) ylikely {
	        fsdir_ent	ds ;
		while ((rs = d.read(&ds,nbuf,nlen)) > 0) {
		    if (nbuf[0] != '.') {
			if ((rs = pathadd(dbuf,dl,nbuf)) >= 0) {
			    rs = dirwatch_scanent(op,dbuf,nbuf,dt) ;
			    n += rs ;
			}
		    } /* end if (non-dot) */
	    	} /* end while (reading directory entries) */
		rs1 = d.close ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_scaner) */

local int dirwatch_scanent(DW *op,cchar *dbuf,cchar *dn,time_t dt) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if (ustat sb ; (rs = u_stat(dbuf,&sb)) >= 0) ylikely {
	    if (S_ISREG(sb.st_mode)) {
	        if (IENT *iep ; (rs = dirwatch_findi(op,dn,&iep)) >= 0) {
                    if (iep->state == DW_SNEW) {
                        iep->itime = dt ;
                        if ((dt - sb.st_mtime) > op->intcheck) {
                            iep->state = DW_SCHECK ;
                            op->count_new -= 1 ;
                            op->count_checkable += 1 ;
                            n += 1 ;
                        } /* end if */
                    } else {
                        iep->itime = dt ;
                    }
		} else if (rs == rsn) {
	            if (IENT ie ; (rs = ient_start(&ie,op,dn,&sb)) >= 0) {
			if ((rs = vecobj_add(op->elp,&ie)) >= 0) {
                            op->count_new += 1 ;
			}
		    }
		} /* end if */
	    } /* end if (regular file) */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? n : rs ;
} /* end subroutine (dirwatch_scanent) */

local int dirwatch_diropen(DW *op,time_t dt) noex {
    	int		rs = SR_OK ;
	if (op->fd < 0) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0 ;
	    if ((rs = uc_open(op->dirname,of,om)) >= 0) {
	        op->fd = rs ;
		{
	            op->tiopen = dt ;
	            rs = uc_closeonexec(op->fd,true) ;
		}
		if (rs < 0) {
		    uc_close(op->fd) ;
		    op->fd = -1 ;
		} /* end if (error handling) */
	    } /* end if (u_open) */
	} /* end if (opened FD for caching) */
	return rs ;
} /* end subroutine (dirwatch_diropen) */

local int dirwatch_dirclose(DW *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
} /* end subroutine (dirwatch_dirclose) */

#ifdef	COMMENT

local int dirwatch_delname(DW *op,cchar *name) noex {
	int		rs ;
	int		i = 0 ;
	IENT		ie{} ;
	IENT		*iep ;
	ie.name = name ;
	if ((rs = vecobj_search(op->elp,&ie,vcmpfn,&iep)) >= 0) {
	    i = rs ;
	    rs = vecobj_del(op->elp,i) ;
	}
	return (rs >= 0) ? i : rs ;
} /* end subroutine (dirwatch_delname) */

#endif /* COMMENT */

local int dirwatch_findi(DW *op,cchar *name,IENT **iepp) noex {
	int		rs ;
	IENT		ie ;
	ie.name = charp(name) ;
	void		*vp{} ;
	if ((rs = vecobj_search(op->elp,&ie,vcmpfn,&vp)) >= 0) {
	    IENT	*iep = (IENT *) vp ;
	    *iepp = iep ;
	}
	return rs ;
} /* end subroutine (dirwatch_findi) */

/* initialize an entry */
local int ient_start(IENT *iep,DW *op,cchar *name,ustat *sbp) noex {
	int		rs ;
	int		rs1 ;
	iep->state = DW_SNEW ;
	if (cchar *cp ; (rs = lm_strw(name,-1,&cp)) >= 0) ylikely {
	    iep->name = cp ;
	    /* do we need to get some status on the file? */
	    if (sbp == nullptr) {
		if (char *dbuf ; (rs = lm_mp(&dbuf)) >= 0) {
	            if ((rs = mkpath(dbuf,op->dirname,name)) >= 0) {
		        if (ustat sb ; (rs = u_stat(dbuf,&sb)) >= 0) {
	                    sbp = &sb ;
		        }
	            }
		    rs1 = lm_free(dbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (non-null) */
	    iep->itime = ((sbp) ? sbp->st_mtime : 0) ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end subroutine (ient_start) */

local int ient_finish(IENT *iep,DW *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (iep->state == DW_SNEW) {
	    op->count_new -= 1 ;
	} else if (iep->state == DW_SCHECK) {
	    op->count_checkable -= 1 ;
	}
	if (iep->name) {
	    void *vp = voidp(iep->name) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    iep->name = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (ient_finish) */

local int entry_load(DW_ENT *dep,IENT *iep,cchar *rbuf) noex {
	int		rs = SR_FAULT ;
	if (dep && iep) ylikely {
	    rs = SR_OK ;
	    dep->itime = iep->itime ;
	    dep->timod = iep->timod ;
	    dep->fsize = iep->fsize ;
	    dep->state = iep->state ;
	    dep->name = rbuf ;
	    if (rbuf) {
		rs = lenstr(rbuf) ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_load) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	} /* end if (bufsizeget) */
    	return rs ;
} /* end method (vars::operator) */

local int vcmpfn(cvoid **v1pp,cvoid **v2pp) noex {
	IENT		**e1pp = (IENT **) v1pp ;
	IENT		**e2pp = (IENT **) v2pp ;
	int		rc = 0 ;
	{
	    IENT	*e1p = *e1pp ;
	    IENT	*e2p = *e2pp ;
	    if (e1p || e2p) ylikely {
	        if (e1p) {
	            if (e2p) {
	                rc = strcmp(e1p->name,e2p->name) ;
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = +1 ;
	        }
	    }
	} /* end block */
	return rc ;
} /* end subroutine (vcmpfn) */


