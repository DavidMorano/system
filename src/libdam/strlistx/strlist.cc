/* strlist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read or audit a STRLIST database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	strlist

	Description:
	This subroutine opens and allows for reading or auditing
	of a STRLIST database (which currently consists of two
	files).

	Synopsis:
	int strlist_open(strlist *op,cchar *dbname) noex

	Arguments:
	- op		object pointer
	- dbname	name of (path-to) DB

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getpwd.h>
#include	<absfn.h>
#include	<endian.h>
#include	<hash.h>
#include	<hashindex.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<nleadstr.h>
#include	<localmisc.h>		/* |NATURAULWORDLEN| */

#include	"strlist.h"
#include	"strlisthdr.h"

import libutil ;

/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	SL		strlist
#define	SL_INFO		strlist_info
#define	SL_CUR		strlist_cur
#define	SL_OBJ		strlist_obj
#define	SL_FM		strlist_fm
#define	SL_MI		strlist_mi

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#ifndef	MODP2
#define	MODP2(v,n)	((v) & ((n) - 1))
#endif

#ifndef	MAXMAPSIZE
#define	MAXMAPSIZE	(512*1024*1024)
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum itentries {
	itentry_ri,
	itentry_info,
	itentry_nhi,
	itentry_overlast
} ;


/* forward references */

template<typename ... Args>
static int strlist_ctor(strlist *op,Args ... args) noex {
    	STRLIST		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->fhp = new(nothrow) strlisthdr) != np) {
		rs = SR_OK ;
	    } /* end if (new-strlisthdr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlist_ctor) */

static int strlist_dtor(strlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fhp) {
		delete op->fhp ;
		op->fhp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlist_dtor) */

template<typename ... Args>
static inline int strlist_magic(strlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == STRLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (strlist_magic) */

static int strlist_dbloadbegin(SL *,time_t) noex ;
static int strlist_dbloadend(SL *) noex ;
static int strlist_dbmapcreate(SL *,time_t) noex ;
static int strlist_dbmapdestroy(SL *) noex ;
static int strlist_filemapcreate(SL *,SL_FM *,cc *,time_t) noex ;
static int strlist_filemapdestroy(SL *,SL_FM *) noex ;
static int strlist_dbproc(SL *,time_t) noex ;
static int strlist_viverify(SL *,time_t) noex ;
static int strlist_ouraudit(SL *) noex ;

static bool	ismatkey(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */

SL_OBJ	strlist_modinfo = {
	"strlist",
	szof(strlist),
	szof(strlist_cur)
} ;


/* exported subroutines */

int strlist_open(SL *op,cchar *dbname) noex {
	custime		dt = getustime ;
	int		rs ;
	int		rs1 ;
	if ((rs = strlist_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		cchar	*fnp{} ;
		if (absfn db ; (rs = db.start(dbname,-1,&fnp)) >= 0) {
		    cint	fnl = rs ;
	            if (cchar *cp{} ; (rs = uc_mallocstrw(fnp,fnl,&cp)) >= 0) {
	                op->dbname = cp ;
		        if ((rs = strlist_dbloadbegin(op,dt)) >= 0) {
			    op->ti_lastcheck = dt ;
			    op->magic = STRLIST_MAGIC ;
		        }
		        if (rs < 0) {
	    		    uc_free(op->dbname) ;
	    		    op->dbname = nullptr ;
		        }
		    } /* end if (memory-allocation) */
		    rs1 = db.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (absfn) */
	    } /* end if (valid) */
	    if (rs < 0) {
		strlist_dtor(op) ;
	    }
	} /* end if (strlist_ctor) */
	return rs ;
}
/* end subroutine (strlist_open) */

int strlist_close(SL *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strlist_magic(op)) >= 0) {
	    {
	        rs1 = strlist_dbloadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        rs1 = uc_free(op->dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    }
	    {
		rs1 = strlist_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_close) */

int strlist_getinfo(SL *op,SL_INFO *vip) noex {
	int		rs ;
	if ((rs = strlist_magic(op,vip)) >= 0) {
	    SL_FM	*fip = &op->vf ;
	    strlisthdr	*hip = op->fhp ;
	    memclear(vip) ;
	    {
	        vip->mtime = fip->ti_mod ;
	        vip->wtime = time_t(hip->wtime) ;
	    }
	    {
	        vip->nstrlist = hip->nstrs ;
	        vip->nskip = hip->nskip ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_info) */

int strlist_audit(SL *op) noex {
	int		rs ;
	if ((rs = strlist_magic(op)) >= 0) {
	    rs = strlist_ouraudit(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_audit) */

int strlist_count(SL *op) noex {
	int		rs ;
	int		nstrs = 0 ;
	if ((rs = strlist_magic(op)) >= 0) {
	    strlisthdr	*hip = op->fhp ;
	    nstrs = hip->nstrs ;
	} /* end if (magic) */
	return (rs >= 0) ? nstrs : rs ;
}
/* end subroutine (strlist_count) */

int strlist_curbegin(SL *op,SL_CUR *curp) noex {
	int		rs ;
	if ((rs = strlist_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    curp->chash = 0 ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_curbegin) */

int strlist_curend(SL *op,SL_CUR *curp) noex {
	int		rs ;
	if ((rs = strlist_magic(op,curp)) >= 0) {
	    curp->i = 0 ;
	    if (op->ncursors > 0) {
	        op->ncursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_curend) */

int strlist_curlook(SL *op,SL_CUR *curp,cchar *kp,int kl) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = strlist_magic(op,curp,kp)) >= 0) {
            SL_CUR          dcur ;
            SL_MI           *mip = &op->mi ;
            strlisthdr      *hip = op->fhp ;
            uint            khash, nhash, chash ;
            uint            hi ;
            uint            ki ;
            int             ri, c ;
            int             (*rt)[1] ;
            int             (*it)[3] ;
            bool            f_mat = false ;
            cchar   *kst ;
            cchar   *cp ;
            if (curp == nullptr) {
                curp = &dcur ;
                curp->i = 0 ;
            }
            if (kl < 0) kl = lenstr(kp) ;
            kst = mip->kst ;
            rt = mip->rt ;
            it = mip->it ;
            if (curp->i <= 0) {
    /* unhappy or not, the index-table uses same-hash-linking! */
                khash = hash_elf(kp,kl) ;
                nhash = khash ;
                chash = (khash & INT_MAX) ;
                curp->chash = chash ;       /* store "check" hash */
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
                    if (c >= int(hip->itlen + hip->nskip)) break ;
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
			    {
				cuint thash = (it[hi][itentry_info] & INT_MAX) ;
                                f_mat = (thash == chash) ;
			    }
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
            if (rs >= 0) {
                curp->i = hi ;
            } /* end if (got one) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (strlist_curlook) */

int strlist_curenum(SL *op,SL_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	if ((rs = strlist_magic(op,curp,kbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        SL_MI		*mip = &op->mi ;
	        strlisthdr	*hip = op->fhp ;
	        uint		ri = (curp->i < 1) ? 1 : (curp->i + 1) ;
		rs = SR_OK ;
	        kbuf[0] = '\0' ;
                /* ok, we are good to go */
	        if (ri < hip->rtlen) {
	            uint	ki = mip->rt[ri][0] ;
	            if (ki < hip->stlen) {
	                cchar	*kp = mip->kst + ki ;
	                if ((rs = sncpy1(kbuf,klen,kp)) >= 0) {
	                    curp->i = ri ;
		        }
	            } else {
	                rs = SR_BADFMT ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlist_curenum) */


/* private subroutines */

static int strlist_dbloadbegin(SL *op,time_t dt) noex {
	int		rs ;
	if ((rs = strlist_dbmapcreate(op,dt)) >= 0) {
	    rs = strlist_dbproc(op,dt) ;
	    if (rs < 0) {
		strlist_dbmapdestroy(op) ;
	    }
	}
	return rs ;
}
/* end subroutine (strlist_dbloadbegin) */

static int strlist_dbloadend(SL *op) noex {
	int		rs ;
	if ((rs = strlist_dbmapdestroy(op)) >= 0) {
	    SL_MI	*mip = &op->mi ;
	    mip->rt = nullptr ;
	    mip->it = nullptr ;
	    mip->kst = nullptr ;
	}
	return rs ;
}
/* end subroutine (strlist_dbloadend) */

static int strlist_dbmapcreate(SL *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	cchar		*suf = STRLISTHDR_FSUF ;
	cchar		*end = ENDIANSTR ;
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,suf,end)) >= 0) {
	        rs = strlist_filemapcreate(op,&op->vf,tbuf,dt) ;
	    }
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (strlist_dbmapcreate) */

static int strlist_dbmapdestroy(SL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = strlist_filemapdestroy(op,&op->vf) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlist_dbmapdestroy) */

static int strlist_filemapcreate(SL *op,SL_FM *fip,cchar *fn,time_t dt) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && fip) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0666 ;
	    if ((rs = u_open(fn,of,om)) >= 0) {
	        cint	fd = rs ;
	        if (USTAT sb ; (rs = u_fstat(fd,&sb)) >= 0) {
		    cnullptr	np{} ;
	  	    if (sb.st_size <= MAXMAPSIZE) {
	                csize	ms = size_t(sb.st_size) ;
	                int	mp = PROT_READ ;
	                int	mf = MAP_SHARED ;
	                void	*md{} ;
	                if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	                    fip->mdata = charp(md) ;
	                    fip->msize = ms ;
	                    fip->ti_mod = sb.st_mtime ;
	                    fip->ti_map = dt ;
	                }
		    } else {
		        rs = SR_TOOBIG ;
		    }
	        } /* end if (stat) */
	        rs1 = u_close(fd) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mapped file) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlist_filemapcreate) */

static int strlist_filemapdestroy(SL *op,SL_FM *fip) noex {
	int		rs = SR_FAULT ;
	if (op && fip) {
	    rs = R_OK ;
	    if (fip->mdata) {
		void	*md = fip->mdata ;
		csize	ms = fip->msize ;
	        rs = u_mmapend(md,ms) ;
	        fip->mdata = nullptr ;
	        fip->msize = 0 ;
	        fip->ti_map = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlist_filemapdestroy) */

static int strlist_dbproc(SL *op,time_t dt) noex {
	SL_FM		*fip = &op->vf ;
	SL_MI		*mip = &op->mi ;
	int		rs = SR_FAULT ;
	if (strlisthdr *hip = op->fhp ; hip) {
	    cint	hl = int(fip->msize) ;
	    cchar	*hp = charp(fip->mdata) ;
	    if ((rs = strlisthdr_wr(hip,hp,hl)) >= 0) { /* write-object */
	        if ((rs = strlist_viverify(op,dt)) >= 0) {
	            mip->rt = (int (*)[1]) (fip->mdata + hip->rtoff) ;
	            mip->it = (int (*)[3]) (fip->mdata + hip->itoff) ;
	            mip->kst = charp(fip->mdata + hip->stoff) ;
	        }
	    } /* end if (strlsthdr_wr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlist_dbproc) */

static int strlist_viverify(SL *op,time_t dt) noex {
	SL_FM		*fip = &op->vf ;
	strlisthdr	*hip = op->fhp ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		sz ;
	bool		f = true ;
	{
	    f = f && (hip->fsize == fip->msize) ;
	    f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	    f = f && (hip->stoff <= fip->msize) ;
	    f = f && ((hip->stoff + hip->stlen) <= fip->msize) ;
	}
	{
	    f = f && (hip->rtoff <= fip->msize) ;
	    sz = (hip->rtlen + 1) * 2 * szof(int) ;
	    f = f && ((hip->rtoff + sz) <= fip->msize) ;
	}
	{
	    f = f && (hip->itoff <= fip->msize) ;
	    sz = (hip->itlen + 1) * 3 * szof(int) ;
	    f = f && ((hip->itoff + sz) <= fip->msize) ;
	}
/* an extra (redundant) value */
	{
	    f = f && (hip->nstrs == (hip->rtlen - 1)) ;
	}
	if (! f) {
	    rs = SR_BADFMT ;
	}
	return rs ;
}
/* end subroutine (strlist_viverify) */

static int strlist_ouraudit(SL *op) noex {
	SL_MI		*mip = &op->mi ;
	strlisthdr	*hip = op->fhp ;
	uint		khash, chash ;
	int		rs = SR_OK ;
	int		cl ;
	int		(*rt)[1] ;
	int		(*it)[3] ;
	cchar		*kst ;
	cchar		*cp ;
	rt = mip->rt ;
	it = mip->it ;
	kst = mip->kst ;
/* record table */
	if ((rt[0][0] != 0) || (rt[0][1] != 0)) {
	    rs = SR_BADFMT ;
	}
	for (uint ri = 1 ; (rs >= 0) && (ri < hip->rtlen) ; ri += 1) {
	    uint	ki = rt[ri][0] ;
	    if (ki >= hip->stlen) {
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
	        rs = strlist_curlook(op,nullptr,cp,cl) ;
	    }
	    if (rs < 0) break ;
	} /* end for (record table entries) */
/* index table */
	if (rs >= 0) {
	    if ((it[0][0] != 0) || (it[0][1] != 0) || (it[0][2] != 0)) {
	        rs = SR_BADFMT ;
	    }
	}
	for (uint i = 1 ; (rs >= 0) && (i < hip->itlen) ; i += 1) {
	    if (it[i][0] != 0) {
	        uint	ri = it[i][0] ;
	        uint	hi = it[i][2] ;
	        if (ri >= hip->rtlen) {
	            rs = SR_BADFMT ;
		}
	        if ((rs >= 0) && (hi >= hip->itlen)) {
	            rs = SR_BADFMT ;
		}
	        if (rs >= 0) {
	            int		ki = rt[ri][0] ;
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
}
/* end subroutine (strlist_ouraudit) */

static bool ismatkey(cchar *key,cchar *kp,int kl) noex {
	int		m ;
	int		f = (key[0] == kp[0]) ;
	if (f) {
	    m = nleadstr(key,kp,kl) ;
	    f = (m == kl) && (key[m] == '\0') ;
	}
	return f ;
}
/* end subroutine (ismatkey) */


