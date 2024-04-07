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

#include	<envstandards.h>	/* must be before others */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<climits>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<absfn.h>
#include	<endian.h>
#include	<hash.h>
#include	<localmisc.h>

#include	"strlist.h"
#include	"strlisthdr.h"


/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	FE_VI		STRLISTHDR_FSUF

#define	SHIFTINT	(6 * 60)	/* possible time-shift */

#define	MODP2(v,n)	((v) & ((n) - 1))

#ifndef	MAXMAPSIZE
#define	MAXMAPSIZE	(512*1024*1024)
#endif


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	getpwd(char *,int) ;
extern int	hasuc(cchar *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;


/* external variables */


/* exported variables */

STRLIST_OBJ	strlist = {
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

static int strlist_dbloadbegin(STRLIST *,time_t) noex ;
static int strlist_dbloadend(STRLIST *) noex ;
static int strlist_dbmapcreate(STRLIST *,time_t) noex ;
static int strlist_dbmapdestroy(STRLIST *) noex ;
static int strlist_filemapcreate(STRLIST *,STRLIST_FM *,cc *,time_t) noex ;
static int strlist_filemapdestroy(STRLIST *,STRLIST_FM *) noex ;
static int strlist_dbproc(STRLIST *,time_t) noex ;
static int strlist_viverify(STRLIST *,time_t) noex ;
static int strlist_ouraudit(STRLIST *) noex ;

static int	hashindex(uint,int) noex ;
static bool	ismatkey(cchar *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int strlist_open(STRLIST *op,cchar *dbname) noex {
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

int strlist_close(STRLIST *op) noex {
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

int strlist_info(STRLIST *op,STRLIST_INFO *vip) noex {
	STRLIST_FM	*fip ;
	STRLISTHDR	*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (vip == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	memclear(vip) ;

	fip = &op->vf ;
	hip = &op->hdr ;

	vip->mtime = fip->ti_mod ;
	vip->wtime = (time_t) hip->wtime ;

	vip->nstrlist = hip->nstrs ;
	vip->nskip = hip->nskip ;

	return rs ;
}
/* end subroutine (strlist_info) */

int strlist_audit(STRLIST *op) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

/* verify that all list pointers and list entries are valid */

	rs = strlist_ouraudit(op) ;

	return rs ;
}
/* end subroutine (strlist_audit) */

int strlist_count(STRLIST *op) noex {
	strlisthdr	*hip ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	hip = &op->hdr ;
	return (rs >= 0) ? hip->nstrs : rs ;
}
/* end subroutine (strlist_count) */

int strlist_curbegin(STRLIST *op,STRLIST_CUR *curp) noex {

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != STRLIST_MAGIC) return SR_NOTOPEN ;

	curp->i = 0 ;
	curp->chash = 0 ;
	op->ncursors += 1 ;

	return SR_OK ;
}
/* end subroutine (strlist_curbegin) */

int strlist_curend(STRLIST *op,STRLIST_CUR *curp) noex {

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

int strlist_look(STRLIST *op,STRLIST_CUR *curp,cchar *kp,int kl) noex {
	STRLIST_CUR	dcur ;
	STRLIST_MI	*mip ;
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
	hip = &op->hdr ;

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

	        if (c >= (hip->itlen + hip->nskip))
	            break ;

	        nhash = has_again(nhash,c++,hip->nskip) ;

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

int strlist_enum(STRLIST *op,STRLIST_CUR *curp,char *kbuf,int klen) noex {
	STRLIST_MI	*mip ;
	strlistdr	*hip ;
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
	hip = &op->hdr ;

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

static int strlist_dbloadbegin(STRLIST *op,time_t dt) noex {
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

static int strlist_dbloadend(STRLIST *op) noex {
	int		rs ;
	if ((rs = strlist_dbmapdestroy(op)) >= 0) {
	    STRLIST_MI	*mip = &op->mi ;
	    mip->rt = nullptr ;
	    mip->it = nullptr ;
	    mip->kst = nullptr ;
	}
	return rs ;
}
/* end subroutine (strlist_dbloadend) */

static int strlist_dbmapcreate(STRLIST *op,time_t dt) noex {
	int		rs ;
	cchar		*end = ENDIANSTR ;
	char		tmpfname[MAXPATHLEN + 1] ;
	if ((rs = mkfnamesuf2(tmpfname,op->dbname,FE_VI,end)) >= 0) {
	    rs = strlist_filemapcreate(op,&op->vf,tmpfname,dt) ;
	}
	return rs ;
}
/* end subroutine (strlist_dbmapcreate) */

static int strlist_dbmapdestroy(STRLIST *op) noex {
	int		rs ;
	rs = strlist_filemapdestroy(op,&op->vf) ;
	return rs ;
}
/* end subroutine (strlist_dbmapdestroy) */

static int strlist_filemapcreate(STRLIST *op,STRLIST_FM *fip,cchar *fname,
		time_t dt) noex {
{
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if ((rs = u_open(fname,of,0666)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	  	if (sb.st_size <= MAXMAPSIZE) {
		    cnullptr	np{} ;
	            size_t	ms = size_t(sb.st_size) ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            void	*md ;
	            if ((rs = u_mmap(np,ms,mp,mf,fd,0L,&md)) >= 0) {
	                fip->mdata = md ;
	                fip->msize = ms ;
	                fip->ti_mod = sb.st_mtime ;
	                fip->ti_map = dt ;
	            }
		} else {
		    rs = SR_TOOBIG ;
		}
	    } /* end if (stat) */
	    u_close(fd) ;
	} /* end if (mapped file) */

	return rs ;
}
/* end subroutine (strlist_filemapcreate) */

static int strlist_filemapdestroy(STRLIST *op,STRLIST_FM *fip) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (fip->mdata != nullptr) {
	    rs = u_munmap(fip->mdata,fip->msize) ;
	    fip->mdata = nullptr ;
	    fip->msize = 0 ;
	    fip->ti_map = 0 ;
	}

	return rs ;
}
/* end subroutine (strlist_filemapdestroy) */

static int strlist_dbproc(STRLIST *op,time_t dt) noex {
	STRLIST_FM	*fip = &op->vf ;
	STRLIST_MI	*mip = &op->mi ;
	strlisthdr	*hip = &op->hdr ;
	int		rs ;
	if ((rs = strlisthdr(hip,1,fip->mdata,fip->msize)) >= 0) {
	    if ((rs = strlist_viverify(op,dt)) >= 0) {
	        mip->rt = (int (*)[1]) (fip->mdata + hip->rtoff) ;
	        mip->it = (int (*)[3]) (fip->mdata + hip->itoff) ;
	        mip->kst = (char *) (fip->mdata + hip->stoff) ;
	    }
	}
	return rs ;
}
/* end subroutine (strlist_dbproc) */

static int strlist_viverify(STRLIST *op,time_t dt) noex {
	STRLIST_FM	*fip = &op->vf ;
	STRLISTHDR	*hip = &op->hdr ;
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

static int strlist_ouraudit(STRLIST *op) noex {
	STRLIST_MI	*mip = &op->mi ;
	STRLISTHDR	*hip = &op->hdr ;
	uint		ri, ki, hi ;
	uint		khash, chash ;
	int		rs = SR_OK ;
	int		i ;
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
	for (ri = 1 ; (rs >= 0) && (ri < hip->rtlen) ; ri += 1) {
	    ki = rt[ri][0] ;
	    if (ki >= hip->stlen) {
	        rs = SR_BADFMT ;
	    }
	    if (rs >= 0) {
	        cp = (kst + ki) ;
	        cl = strlen(cp) ;
	        if (cp[-1] != '\0') {
	            rs = SR_BADFMT ;
	        }
	    }
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

	for (i = 1 ; (rs >= 0) && (i < hip->itlen) ; i += 1) {

	    if (it[i][0] != 0) {

	        ri = it[i][0] ;
	        hi = it[i][2] ;
	        if (ri >= hip->rtlen)
	            rs = SR_BADFMT ;

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

	        } /* end if */

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


