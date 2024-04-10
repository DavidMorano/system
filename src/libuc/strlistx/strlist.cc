/* strlist SUPPORT */
/* lang=C++20 */

/* read or audit a STRLIST database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<nleadstr.h>
#include	<localmisc.h>

#include	"strlist.h"
#include	"strlisthdr.h"


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

#define	MODP2(v,n)	((v) & ((n) - 1))

#ifndef	MAXMAPSIZE
#define	MAXMAPSIZE	(512*1024*1024)
#endif


/* external subroutines */


/* external variables */


/* exported variables */

SL_OBJ	strlist_mod = {
	"strlist",
	sizeof(strlist),
	sizeof(strlist_cur)
} ;


/* local structures */

enum itentries {
	itentry_ri,
	itentry_info,
	itentry_nhi,
	itentry_overlast
} ;


/* forward references */

static int strlist_dbloadbegin(SL *,time_t) noex ;
static int strlist_dbloadend(SL *) noex ;
static int strlist_dbmapcreate(SL *,time_t) noex ;
static int strlist_dbmapdestroy(SL *) noex ;
static int strlist_filemapcreate(SL *,SL_FM *,cc *,time_t) noex ;
static int strlist_filemapdestroy(SL *,SL_FM *) noex ;
static int strlist_dbproc(SL *,time_t) noex ;
static int strlist_viverify(SL *,time_t) noex ;
static int strlist_ouraudit(SL *) noex ;

static int	hashindex(uint,int) noex ;
static bool	ismatkey(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int strlist_open(SL *op,cchar *dbname) noex {
	const time_t	dt = time(nullptr) ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	{
	    int		pl = -1 ;
	    char	adb[MAXPATHLEN+1] ;
	    if (dbname[0] != '/') {
	        char	pwd[MAXPATHLEN+1] ;
	        if ((rs = getpwd(pwd,MAXPATHLEN)) >= 0) {
	            rs = mkpath2(adb,pwd,dbname) ;
	            pl = rs ;
	            dbname = adb ;
	        }
	    }
	    if (rs >= 0) {
	    	cchar	*cp ;
	        if ((rs = uc_mallocstrw(dbname,pl,&cp)) >= 0) {
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
	    } /* end if */
	} /* end block */

	return rs ;
}
/* end subroutine (strlist_open) */

int strlist_close(SL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	rs1 = strlist_dbloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->dbname != nullptr) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (strlist_close) */

int strlist_getinfo(SL *op,SL_INFO *vip) noex {
	SL_FM	*fip ;
	strlisthdr	*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (vip == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	memclear(vip) ;

	fip = &op->vf ;
	hip = op->fhp ;

	vip->mtime = fip->ti_mod ;
	vip->wtime = (time_t) hip->wtime ;

	vip->nstrlist = hip->nstrs ;
	vip->nskip = hip->nskip ;

	return rs ;
}
/* end subroutine (strlist_info) */

int strlist_audit(SL *op) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

/* verify that all list pointers and list entries are valid */

	rs = strlist_ouraudit(op) ;

	return rs ;
}
/* end subroutine (strlist_audit) */

int strlist_count(SL *op) noex {
	strlisthdr	*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	hip = op->fhp ;
	return (rs >= 0) ? hip->nstrs : rs ;
}
/* end subroutine (strlist_count) */

int strlist_curbegin(SL *op,SL_CUR *curp) noex {

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	curp->chash = 0 ;
	op->ncursors += 1 ;

	return SR_OK ;
}
/* end subroutine (strlist_curbegin) */

int strlist_curend(SL *op,SL_CUR *curp) noex {

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	if (op->ncursors > 0) {
	    op->ncursors -= 1 ;
	}

	return SR_OK ;
}
/* end subroutine (strlist_curend) */

int strlist_look(SL *op,SL_CUR *curp,cchar *kp,int kl) noex {
	SL_CUR	dcur ;
	SL_MI	*mip ;
	strlisthdr	*hip ;
	uint		khash, nhash, chash ;
	uint		hi ;
	uint		ki ;
	int		rs = SR_OK ;
	int		ri, c ;
	int		(*rt)[1] ;
	int		(*it)[3] ;
	int		vl = 0 ;
	int		f_mat = FALSE ;
	cchar	*kst ;
	cchar	*cp ;

	if (op == nullptr) return SR_FAULT ;
	if (kp == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	if (curp == nullptr) {
	    curp = &dcur ;
	    curp->i = 0 ;
	}

	if (kl < 0)
	    kl = strlen(kp) ;

	mip = &op->mi ;
	hip = op->fhp ;

	kst = mip->kst ;
	rt = mip->rt ;
	it = mip->it ;

	if (curp->i <= 0) {

/* unhappy or not, the index-table uses same-hash-linking! */

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

	        if (f_mat)
	            break ;

	        if ((it[hi][itentry_info] & (~ INT_MAX)) == 0)
	            break ;

	        if (c >= int(hip->itlen + hip->nskip))
	            break ;

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

	if (rs >= 0) {
	    curp->i = hi ;
	} /* end if (got one) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (strlist_look) */

int strlist_enum(SL *op,SL_CUR *curp,char *kbuf,int klen) noex {
	SL_MI	*mip ;
	strlisthdr	*hip ;
	uint		ri, ki ;
	int		rs = SR_OK ;
	cchar	*kp ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) return SR_INVALID ;

	kbuf[0] = '\0' ;

	mip = &op->mi ;
	hip = op->fhp ;

	ri = (curp->i < 1) ? 1 : (curp->i + 1) ;

/* ok, we're good to go */

	if (ri < hip->rtlen) {
	    ki = mip->rt[ri][0] ;
	    if (ki < hip->stlen) {
	        kp = mip->kst + ki ;
	        if ((rs = sncpy1(kbuf,klen,kp)) >= 0) {
	            curp->i = ri ;
		}
	    } else {
	        rs = SR_BADFMT ;
	    }
	} else {
	    rs = SR_NOTFOUND ;
	}

	return rs ;
}
/* end subroutine (strlist_enum) */


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
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
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
	int		rs ;
	rs = strlist_filemapdestroy(op,&op->vf) ;
	return rs ;
}
/* end subroutine (strlist_dbmapdestroy) */

static int strlist_filemapcreate(SL *op,SL_FM *fip,cchar *fn,time_t dt) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs = u_open(fn,of,om)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	  	if (sb.st_size <= MAXMAPSIZE) {
		    cnullptr	np{} ;
	            size_t	ms = size_t(sb.st_size) ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
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

	return rs ;
}
/* end subroutine (strlist_filemapcreate) */

static int strlist_filemapdestroy(SL *op,SL_FM *fip) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (fip->mdata) {
	    rs = u_mmapend(fip->mdata,fip->msize) ;
	    fip->mdata = nullptr ;
	    fip->msize = 0 ;
	    fip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (strlist_filemapdestroy) */

static int strlist_dbproc(SL *op,time_t dt) noex {
	SL_FM	*fip = &op->vf ;
	SL_MI	*mip = &op->mi ;
	strlisthdr	*hip = op->fhp ;
	int		rs ;
	if ((rs = strlisthdr_msg(hip,1,fip->mdata,fip->msize)) >= 0) {
	    if ((rs = strlist_viverify(op,dt)) >= 0) {
	        mip->rt = (int (*)[1]) (fip->mdata + hip->rtoff) ;
	        mip->it = (int (*)[3]) (fip->mdata + hip->itoff) ;
	        mip->kst = (char *) (fip->mdata + hip->stoff) ;
	    }
	}
	return rs ;
}
/* end subroutine (strlist_dbproc) */

static int strlist_viverify(SL *op,time_t dt) noex {
	SL_FM	*fip = &op->vf ;
	strlisthdr	*hip = op->fhp ;
	uint		utime = (uint) dt ;
	int		rs = SR_OK ;
	int		sz ;
	int		f = TRUE ;

	f = f && (hip->fsize == fip->msize) ;
	f = f && (hip->wtime > 0) && (hip->wtime <= (utime + SHIFTINT)) ;
	f = f && (hip->stoff <= fip->msize) ;
	f = f && ((hip->stoff + hip->stlen) <= fip->msize) ;

	f = f && (hip->rtoff <= fip->msize) ;
	sz = (hip->rtlen + 1) * 2 * sizeof(int) ;
	f = f && ((hip->rtoff + sz) <= fip->msize) ;

	f = f && (hip->itoff <= fip->msize) ;
	sz = (hip->itlen + 1) * 3 * sizeof(int) ;
	f = f && ((hip->itoff + sz) <= fip->msize) ;

/* an extra (redundant) value */

	f = f && (hip->nstrs == (hip->rtlen - 1)) ;

	if (! f)
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (strlist_viverify) */

static int strlist_ouraudit(SL *op) noex {
	SL_MI	*mip = &op->mi ;
	strlisthdr	*hip = op->fhp ;
	uint		khash, chash ;
	int		rs = SR_OK ;
	int		cl ;
	int		(*rt)[1] ;
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
	for (uint ri = 1 ; (rs >= 0) && (ri < hip->rtlen) ; ri += 1) {
	    uint	ki = rt[ri][0] ;
	    if (ki >= hip->stlen) {
	        rs = SR_BADFMT ;
	    }
	    if (rs >= 0) {
	        cp = (kst + ki) ;
	        cl = strlen(cp) ;
	        if (cp[-1] != '\0') {
	            rs = SR_BADFMT ;
	        }
	    } /* end if (ok) */
	    if (rs >= 0) {
	        rs = strlist_look(op,nullptr,cp,cl) ;
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

static int hashindex(uint i,int n) noex {
	int		hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end subroutine (hashindex) */

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


