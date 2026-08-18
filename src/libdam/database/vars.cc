/* vars SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read or audit a VAR database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vars

	Description:
	This subroutine opens and allows for reading or auditing
	of a VAR database (which currently consists of two files).

	Synopsis:
	int vars_open(vars *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/mman.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mkfnamesuf.h>		/* LIBUC */
#include	<hash.h>		/* LIBUC */
#include	<hashindex.h>		/* LIBUC */
#include	<nleadstr.h>		/* LIBUC */
#include	<absfn.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"vars.h"
#include	"varhdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	FE_VI		VARHDR_FSUF

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#ifndef	MAXMAPSIZE
#define	MAXMAPSIZE	(512*1024*1024)
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum itentries {
	itentry_ri,
	itentry_info,
	itentry_nhi,
	itentry_overlast
} ; /* end enum */


/* forward references */

template<typename ... Args>
local inline int vars_ctor(vars *op,Args ... args) noex {
    	VARS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vars_ctor) */

local int vars_dtor(vars *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vars_dtor) */

template<typename ... Args>
local inline int vars_magic(vars *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == VARS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (vars_magic) */


local int	vars_dbloadbegin(vars *,time_t) noex ;
local int	vars_dbloadend(vars *) noex ;
local int	vars_dbmapcreate(vars *,time_t) noex ;
local int	vars_dbmapdestroy(vars *) noex ;
local int	vars_filemapcreate(vars *,vars_fm *,cchar *,time_t) noex ;
local int	vars_filemapdestroy(vars *,vars_fm *) noex ;
local int	vars_dbproc(vars *,time_t) noex ;
local int	vars_viverify(vars *,time_t) noex ;
local int	vars_ouraudit(vars *) noex ;

local bool	ismatkey(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */

extern const vars_obj	vars_modinfo = {
	"vars",
	szof(vars),
	szof(vars_cur)
} ; /* end initialization */


/* exported subroutines */

int vars_open(vars *op,cchar *dbname) noex {
	time_t		dt = time(nullptr) ;
	int		rs ;
	int		rs1 ;
	if ((rs = vars_ctor(op,dbname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (dbname[0]) ylikely {
		cchar *ndbn ;
		if (absfn nfn ; (rs = nfn.start(dbname,-1,&ndbn)) >= 0) {
		    cint pl = rs ;
	    	    if (cchar *cp ; (rs = mem.strw(dbname,pl,&cp)) >= 0) {
	                op->dbname = cp ;
		        if ((rs = vars_dbloadbegin(op,dt)) >= 0) ylikely {
			    op->ti_lastcheck = dt ;
			    op->magval = VARS_MAGIC ;
		        } /* end if */
		        if (rs < 0) {
	    		    voidp vp = voidp(op->dbname) ;
	    		    mem.free(vp) ;
	    		    op->dbname = nullptr ;
		        } /* end if (error) */
		    } /* end if (memory-allocation) */
		    rs1 = nfn.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (absfn) */
	    } /* end if (valid) */
	    if (rs < 0) {
		vars_dtor(op) ;
	    } /* end if (error) */
	} /* end if (vars_ctor) */
	return rs ;
} /* end subroutine (vars_open) */

int vars_close(vars *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = vars_magic(op)) >= 0) ylikely {
	    {
	        rs1 = vars_dbloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        voidp vp = voidp(op->dbname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = vars_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (vars_magic) */
	return rs ;
} /* end subroutine (vars_close) */

int vars_getinfo(vars *op,vars_info *vip) noex {
	int		rs ;
	if ((rs = vars_magic(op,vip)) >= 0) ylikely {
	    memclear(vip) ;
	    vars_fm	*fip = &op->vf ;
	    varhdr	*hip = &op->ifi ;
	    {
	        vip->mtime = fip->ti_mod ;
	        vip->wtime = (time_t) hip->wtime ;
	    }
	    {
	        vip->nvars = hip->nvars ;
	        vip->nskip = hip->nskip ;
	    }
	} /* end if (vars_magic) */
	return rs ;
} /* end subroutine (vars_info) */

int vars_audit(vars *op) noex {
	int		rs ;
	if ((rs = vars_magic(op)) >= 0) ylikely {
	    rs = vars_ouraudit(op) ;
	} /* end if (vars_magic) */
	return rs ;
} /* end subroutine (vars_audit) */

int vars_count(vars *op) noex {
	int		rs ;
	int		n = 0 ; /* return-value */
	if ((rs = vars_magic(op)) >= 0) ylikely {
	    varhdr	*hip = &op->ifi ;
	    n = hip->nvars ;
	} /* end if (vars_magic) */
	return (rs >= 0) ? n : rs ;
} /* end subroutine (vars_count) */

int vars_curbegin(vars *op,vars_cur *curp) noex {
    	int		rs ;
	if ((rs = vars_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	    curp->chash = 0 ;
	    op->ncursors += 1 ;
	} /* end if (vars_magic) */
	return rs ;
} /* end subroutine (vars_curbegin) */

int vars_curend(vars *op,vars_cur *curp) noex {
    	int		rs ;
	if ((rs = vars_magic(op,curp)) >= 0) ylikely {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (vars_magic) */
	return rs ;
} /* end subroutine (vars_curend) */

int vars_curenum(vars *op,vars_cur *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ; /* return-value */
	if ((rs = vars_magic(op,curp,kbuf)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        vars_mi		*mip ;
	        varhdr		*hip ;
	        uint		ri, ki, vi ;
	        cchar	*kp, *vp ;
		rs = SR_OK ;
	        if (vbuf) {
	            vbuf[0] = '\0' ;
	        }
	        mip = &op->mi ;
	        hip = &op->ifi ;
	        ri = (curp->i < 1) ? 1 : (curp->i + 1) ;
	        if (ri < hip->rtlen) {
	            ki = mip->rt[ri][0] ;
	            vi = mip->rt[ri][1] ;
	            if ((ki < hip->kslen) && (vi < hip->vslen)) {
	                kp = mip->kst + ki ;
		        if ((rs = sncpy1(kbuf,klen,kp)) >= 0) {
		            vp = (mip->vst + vi) ;
		            if (vbuf) {
			        rs = sncpy1(vbuf,vlen,vp) ;
			        vl = rs ;
		            } else {
			        vl = lenstr(vp) ;
		            }
		        } /* end if */
		        if (rs >= 0) {
		            curp->i = ri ;
		        } /* end if (ok) */
	            } else {
		        rs = SR_BADFMT ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (vars_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (vars_curenum) */

int vars_fetch(vars *op,cchar *kp,int kl,vars_cur *curp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = vars_magic(op,kp)) >= 0) ylikely {
	   vars_cur	dcur ;
	   vars_mi	*mip = &op->mi ;
	   varhdr	*hip = &op->ifi ;
	   uint		khash, nhash, chash ;
	   uint		hi ;
	   uint		ki, vi ;
	   int		ri, c ;
	   int		(*rt)[2] ;
	   int		(*it)[3] ;
	   int		f_mat = false ;
	   cchar	*kst, *vst ;
	   cchar	*vp ;
	   cchar	*cp ;
	   if (curp == nullptr) {
	       curp = &dcur ;
	       curp->i = 0 ;
	   }
	   if (kl < 0) kl = lenstr(kp) ;
	   if (vbuf) {
	       vbuf[0] = '\0' ;
	   }
	   {
	       kst = mip->kst ;
	       vst = mip->vst ;
	       rt = mip->rt ;
	       it = mip->it ;
	   }
	   if (curp->i <= 0) {
	       /* unhappy or not, the index-table uses "same-hash-linking"! */
	       khash = hash_elf(kp,kl) ;
	       nhash = khash ;
	       chash = (khash & INT_MAX) ;
	       curp->chash = chash ;	/* store "check" hash */
	       hi = hashindex(khash,hip->itlen) ;
	       c = 0 ;
	       while ((ri = it[hi][itentry_ri]) > 0) {
	           f_mat = ((it[hi][itentry_info] & INT_MAX) == chash) ;
	           if (f_mat) {
	               ki = rt[ri][0] ;
	               cp = (kst + ki) ;
	               f_mat = (cp[0] == kp[0]) && ismatkey(cp,kp,kl) ;
	           }
	           if (f_mat) break ;
	           if ((it[hi][itentry_info] & (~ INT_MAX)) == 0) break ;
	           if (c >= conv<int>(hip->itlen + hip->nskip)) break ;
	           nhash = hash_again(nhash,c++,hip->nskip) ;
	           hi = hashindex(nhash,hip->itlen) ;
	       } /* end while */
	       if ((rs >= 0) && (! f_mat)) {
	           rs = SR_NOTFOUND ;
	       }
	   } else {
	       chash = curp->chash ;
	       hi = curp->i ;
	       if (hi < hip->itlen) {
	           ri = it[hi][itentry_ri] ;
	           if (ri > 0) {
	               hi = it[hi][itentry_nhi] ;
	               if (hi != 0) {
	                   ri = it[hi][itentry_ri] ;
	                   f_mat = ((it[hi][itentry_info] & INT_MAX) == chash) ;
	                   if ((ri > 0) && f_mat) {
	                       ki = rt[ri][0] ;
	                       f_mat = ismatkey((kst + ki),kp,kl) ;
	                   }
	                   if (! f_mat) {
	                       rs = SR_NOTFOUND ;
			   }
	               } else {
	                   rs = SR_NOTFOUND ;
		       }
	           } else {
	               rs = SR_NOTFOUND ;
		   }
	       } else {
	           rs = SR_NOTFOUND ;
	       }
	   } /* end if (preparation) */
	   /* if successful, retrieve value */
	   if (rs >= 0) ylikely {
	       vi = rt[ri][1] ;
	       vp = (vst + vi) ;
	       if (vbuf != nullptr) {
	           rs = sncpy1(vbuf,vlen,vp) ;
	           vl = rs ;
	       } else {
	           vl = lenstr(vp) ;
	       }
	       if (rs >= 0) {
	           curp->i = hi ;
	       }
	   } /* end if (got one) */
	} /* end if (vars_magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (vars_fetch) */


/* private subroutines */

local int vars_dbloadbegin(vars *op,time_t dt) noex {
	int		rs ;
	if ((rs = vars_dbmapcreate(op,dt)) >= 0) {
	    rs = vars_dbproc(op,dt) ;
	    if (rs < 0) {
		vars_dbmapdestroy(op) ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (vars_dbloadbegin) */

local int vars_dbloadend(vars *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vars_dbmapdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    vars_mi	*mip = &op->mi ;
	    mip->rt = nullptr ;
	    mip->it = nullptr ;
	    mip->kst = nullptr ;
	    mip->vst = nullptr ;
	}
	return rs ;
} /* end subroutine (vars_dbloadend) */

local int vars_dbmapcreate(vars *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
	    cchar	*strend = ENDIANSTR ;
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,FE_VI,strend)) >= 0) {
	        rs = vars_filemapcreate(op,&op->vf,tbuf,dt) ;
	    }
	    rs1 = mem.free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end subroutine (vars_dbmapcreate) */

local int vars_dbmapdestroy(vars *op) noex {
	int		rs ;
	{
	    rs = vars_filemapdestroy(op,&op->vf) ;
	}
	return rs ;
} /* end subroutine (vars_dbmapdestroy) */

local int vars_filemapcreate(vars *op,vars_fm *fip,cchar *fn,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op) ylikely {
	    cint of = O_RDONLY ;
	    if ((rs = u_open(fn,of,0)) >= 0) ylikely {
	        cint	fd = rs ;
	        if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
		    csize fsize = size_t(sb.st_size) ;
	  	    if (fsize <= MAXMAPSIZE) {
	                csize	ms = fsize ;
	                int	mp = PROT_READ ;
	                int	mf = MAP_SHARED ;
	                void	*md ;
	                if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                    fip->mdata = charp(md) ;
	                    fip->msize = ms ;
	                    fip->ti_mod = sb.st_mtime ;
	                    fip->ti_map = dt ;
	                } /* end if (u_mmapbegin) */
		    } else {
		        rs = SR_TOOBIG ;
		    }
	        } /* end if (stat) */
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mapped file) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vars_filemapcreate) */

local int vars_filemapdestroy(vars *op,vars_fm *fip) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (fip->mdata) ylikely {
	        rs1 = u_mmapend(fip->mdata,fip->msize) ;
	        if (rs >= 0) rs = rs1 ;
	        fip->mdata = nullptr ;
	        fip->msize = 0 ;
	        fip->ti_map = 0 ;
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (vars_filemapdestroy) */

local int vars_dbproc(vars *op,time_t dt) noex {
	vars_fm		*fip = &op->vf ;
	vars_mi		*mip = &op->mi ;
	varhdr		*hip = &op->ifi ;
	int		msz{} ;
	int		rs ;
	msz = conv<int>(fip->msize) ;
	if ((rs = hip->wr(fip->mdata,msz)) >= 0) ylikely {
	    if ((rs = vars_viverify(op,dt)) >= 0) ylikely {
	        mip->rt		= (int (*)[2]) (fip->mdata + hip->rtoff) ;
	        mip->it		= (int (*)[3]) (fip->mdata + hip->itoff) ;
	        mip->kst	= (char *) (fip->mdata + hip->ksoff) ;
	        mip->vst	= (char *) (fip->mdata + hip->vsoff) ;
	    } /* end if (vars_viverify) */
	} /* end if (varhdr) */
	return rs ;
} /* end subroutine (vars_dbproc) */

local int vars_viverify(vars *op,time_t dt) noex {
	vars_fm		*fip = &op->vf ;
	varhdr		*hip = &op->ifi ;
	uint		utime = uint(dt) ;
	uint		umsz{} ;
	int		rs = SR_OK ;
	int		sz ;
	int		f = true ;
	{
	    umsz = conv<uint>(fip->msize) ;
	}
	{
	    f = f && (hip->fsz == umsz) ;
	    f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	}
	{
	    f = f && (hip->ksoff <= fip->msize) ;
	    f = f && ((hip->ksoff + hip->kslen) <= umsz) ;
	}
	{
	    f = f && (hip->vsoff <= fip->msize) ;
	    f = f && ((hip->vsoff + hip->vslen) <= umsz) ;
	}
	{
	    f = f && (hip->rtoff <= umsz) ;
	    sz = (hip->rtlen + 1) * 2 * szof(int) ;
	    f = f && ((hip->rtoff + sz) <= umsz) ;
	}
	{
	    f = f && (hip->itoff <= fip->msize) ;
	    sz = (hip->itlen + 1) * 3 * szof(int) ;
	    f = f && ((hip->itoff + sz) <= umsz) ;
	}
	/* an extra (redundant) value */
	{
	    f = f && (hip->nvars == (hip->rtlen - 1)) ;
	}
	/* get out */
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
} /* end subroutine (vars_viverify) */

local int vars_ouraudit(vars *op) noex {
	vars_mi		*mip = &op->mi ;
	varhdr		*hip = &op->ifi ;
	cnullptr	np{} ;
	uint		ri, ki, vi, hi ;
	uint		khash, chash ;
	int		rs = SR_OK ;
	int		cl ;
	int		(*rt)[2] ;
	int		(*it)[3] ;
	cchar	*kst ;
	cchar	*cp ;
	rt = mip->rt ;
	it = mip->it ;
	kst = mip->kst ;
	/* record table */
	if ((rt[0][0] != 0) || (rt[0][1] != 0)) {
	    rs = SR_BADFMT ;
	}
	for (ri = 1 ; (rs >= 0) && (ri < hip->rtlen) ; ri += 1) {
	    ki = rt[ri][0] ;
	    vi = rt[ri][1] ;
	    if (ki >= hip->kslen) {
	        rs = SR_BADFMT ;
	    }
	    if ((rs >= 0) && (vi >= hip->vslen)) {
	        rs = SR_BADFMT ;
	    }
	    if (rs >= 0) {
	        cp = (kst + ki) ;
	        cl = lenstr(cp) ;
	        if (cp[-1] != '\0') {
	            rs = SR_BADFMT ;
		}
	    } /* end if (ok) */
	    if (rs >= 0) {
	        rs = vars_fetch(op,cp,cl,np,np,0) ;
	    }
	    if (rs < 0) break ;
	} /* end for (record table entries) */
	/* index table */
	if ((rs >= 0) && 
	    ((it[0][0] != 0) || (it[0][1] != 0) || (it[0][2] != 0))) {
	    rs = SR_BADFMT ;
	}
	cint n = conv<int>(hip->itlen) ;
	for (int i = 1 ; (rs >= 0) && (i < n) ; i += 1) {
	    if (it[i][0] != 0) {
	        ri = it[i][0] ;
	        hi = it[i][2] ;
	        if (ri >= hip->rtlen) {
	            rs = SR_BADFMT ;
		}
	        if ((rs >= 0) && (hi >= hip->itlen)) {
	            rs = SR_BADFMT ;
		}
	        if (rs >= 0) {
	            ki = rt[ri][0] ;
	            khash = hash_elf((kst + ki),-1) ;
	            chash = (khash & INT_MAX) ;
	            if (chash != (it[i][1] & INT_MAX)) {
	                rs = SR_BADFMT ;
		    }
	        } /* end if (ok) */
	    } else {
	        if ((it[i][1] != 0) || (it[i][2] != 0)) {
	            rs = SR_BADFMT ;
		}
	    } /* end if */
	} /* end for (index table entries) */
	return rs ;
} /* end subroutine (vars_ouraudit) */

local bool ismatkey(cchar *key,cchar *kp,int kl) noex {
	bool		f = (key[0] == kp[0]) ;
	if (f) {
	    cint	m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
} /* end subroutine (ismatkey) */


