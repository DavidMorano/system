/* fsdirtree SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* file-system directory tree (traversing) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-04-27, David A­D­ Morano
	I wanted an interative enumeration.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fsdirtree

	Description:
	This subroutine is not similar to the 'ftw' subroutine.
	That subroutine "walks" the directory tree and calls a
	user-supplied function at each entry. This module is quite
	different from 'ftw' and 'wdt' in that it allows the caller
	to intermix directory functions with other caller activities.
	It allows this by "reading" and entry in a similar way as
	FSDIR allows.

	The caller "opens" an object and then performs operations
	on it like reading a directory entry. After the caller is
	finished with all directory functions on this object, the
	object should be "closed".

	Synopsis:
	int fsdirtree_open(fsdirtree *op,cchar *dname,int opts) noex

	Arguments:
	op		object pointer
	dname		directory name to process
	opts		options for processing

	Returns:
	>=0		good
	<0		bad in some way! (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<usystem.h>
#include	<fsdir.h>
#include	<fifostr.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<mknpathx.h>
#include	<matxstr.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<ismisc.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"fsdirtree.h"


/* local defines */

#define	FSDIRTREE_MTYPE1	(FSDIRTREE_MREG | FSDIRTREE_MBLOCK)
#define	FSDIRTREE_MTYPE2	(FSDIRTREE_MPIPE | FSDIRTREE_MSOCK)
#define	FSDIRTREE_MTYPE3	(FSDIRTREE_MDIR | FSDIRTREE_MCHAR)
#define	FSDIRTREE_MTYPE		\
	    (FSDIRTREE_MTYPE1 | FSDIRTREE_MTYPE2 | FSDIRTREE_MTYPE3)

#undef	DIRID
#define	DIRID		struct fsdirtree_dirid

#define	MAXLINKLEN_MULT	4


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef uint32_t	ui ;


/* external subroutines */


/* external variables */


/* local structures */

struct fsdirtree_dirid {
	ino_t		ino ;
	dev_t		dev ;
} ;

namespace {
    struct vars {
	int		maxpathlen ;
	int		maxlinklen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

/****
	fifostr		*dqp ;		directory-queue-pointer
	fsdir		*dirp ;		directory-pointer
	hdb		*dip ;		directory-id-pointer
****/

template<typename ... Args>
static int fsdirtree_ctor(fsdirtree *op,Args ... args) noex {
    	FSDIRTREE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->dqp = new(nothrow) fifostr) != np) {
	        if ((op->dirp = new(nothrow) fsdir) != np) {
	            if ((op->dip = new(nothrow) hdb) != np) {
			rs = SR_OK ;
		    } /* end if (new-hdb) */
		    if (rs < 0) {
			delete op->dirp ;
			op->dirp = nullptr ;
		    }
		} /* end if (new-fsdir) */
		if (rs < 0) {
		    delete op->dqp ;
		    op->dqp = nullptr ;
		}
	    } /* end if (new-fifostr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdirtree_ctor) */

static int fsdirtree_dtor(fsdirtree *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dip) {
		delete op->dip ;
		op->dirp = nullptr ;
	    }
	    if (op->dirp) {
		delete op->dirp ;
		op->dirp = nullptr ;
	    }
	    if (op->dqp) {
		delete op->dqp ;
		op->dqp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdirtree_dtor) */

template<typename ... Args>
static inline int fsdirtree_magic(fsdirtree *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FSDIRTREE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fsdirtree_magic) */

static int	fsdirtree_opener(fsdirtree *,cchar *) noex ;

static int	fsdirtree_dirbegin(fsdirtree *) noex ;
static int	fsdirtree_diradd(fsdirtree *,dev_t,ino_t) noex ;
static int	fsdirtree_dirhave(fsdirtree *,dev_t,ino_t,DIRID **) noex ;
static int	fsdirtree_dirend(fsdirtree *) noex ;

static int	dirid_start(DIRID *,dev_t,ino_t) noex ;
static int	dirid_finish(DIRID *) noex ;

static int	diridcmp(DIRID *,DIRID *,int) noex ;

static uint	diridhash(cvoid *,int) noex ;

static bool	interested(int,mode_t) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int fsdirtree_open(fsdirtree *op,cchar *dname,int opts) noex {
	int		rs ;
	if ((rs = fsdirtree_ctor(op,dname)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        op->opts = opts ;
	        if ((rs = fifostr_start(op->dqp)) >= 0) {
		    cint	sz = (var.maxlinklen + 1) ;
		    if (char *lp ; (rs = uc_malloc(sz,&lp)) >= 0) {
			op->llen = var.maxlinklen ;
			op->lbuf = lp ;
			{
			    rs = fsdirtree_opener(op,dname) ;
			}
			if (rs < 0) {
			    uc_free(op->lbuf) ;
			    op->lbuf = nullptr ;
			}
		    } /* end if (memoryªallocation) */
	            if (rs < 0) {
	                fifostr_finish(op->dqp) ;
	            }
	        } /* end if (fifostr_start) */
	    } /* end if (vars) */
	    if (rs < 0) {
		fsdirtree_dtor(op) ;
	    }
	} /* end if (fsdirtree_ctor) */
	return rs ;
}
/* end subroutine (fsdirtree_open) */

int fsdirtree_close(fsdirtree *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = fsdirtree_magic(op)) >= 0) {
	        if (op->f.dirids) {
	            rs1 = fsdirtree_dirend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->f.dir) {
	            op->f.dir = false ;
	            rs1 = fsdir_close(op->dirp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->bnbuf) {
	            rs1 = uc_libfree(op->bnbuf) ;
	            if (rs >= 0) rs = rs1 ;
	            op->bnbuf = nullptr ;
	        }
		{
	            rs1 = fifostr_finish(op->dqp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = fsdirtree_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdirtree_close) */

int fsdirtree_read(fsdirtree *op,USTAT *sbp,char *rbuf,int rlen) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = fsdirtree_magic(op,sbp,rbuf)) >= 0) {
	USTAT		se ;
	FSDIR_ENT	de ;
	int		mlen ;
	int		clen, flen ;
	int		ndir = 0 ;
	cchar		*enp ;
	char		*fnp ;
	char		*cdnp = nullptr ;

	if (rlen < 0) rlen = var.maxpathlen ;

	while ((rs >= 0) && (! op->f.feof)) {
	    cdnp = nullptr ;
	    if ((rs = fsdir_read(op->dirp,&de)) > 0) {
	        int	enl = rs ;
	        int	f_proc = true ;
	        enp = de.name ;
	        if (hasNotDots(enp,enl)) {
		    if (op->prune != nullptr) {
			f_proc = (matstr(op->prune,enp,enl) < 0) ;
		    }

		    if (f_proc) {
	            if ((op->cdnlen > 0) && 
	                (op->bnbuf[op->cdnlen - 1] != '/')) {
	                op->bnbuf[op->cdnlen++] = '/' ;
		    }
	            fnp = op->bnbuf + op->cdnlen ;
	            mlen = var.maxpathlen - op->cdnlen ;
	            flen = strwcpy(fnp,enp,mlen) - fnp ;
	            if ((rs = u_lstat(op->bnbuf,sbp)) >= 0) {
	                if (S_ISLNK(sbp->st_mode)) {
	                    if (op->opts & FSDIRTREE_MFOLLOW) {
	                        cint	llen = op->llen ;
	                        cchar	*fn = op->bnbuf ;
	                        char	*lbuf = op->lbuf ;
	                        if ((rs = uc_readlink(fn,lbuf,llen)) >= 0) {
				    if (! isDotDir(lbuf)) {
	                                if ((rs = uc_stat(fn,&se)) >= 0) {
	                                    sbp = &se ;
	                                } else if (rs == SR_NOENT) {
	                                    rs = SR_OK ;
	                                }
				    } /* end if (not-dots) */
	                        }
	                    } /* end if (follow-option specified) */
	                } /* end if (is-link) */
	            } /* end if (u_lstat) */
		    } /* end if (not pruned) */
	        } else {
	            f_proc = false ;
		}
	        if ((rs >= 0) && f_proc) {
		    cbool	f_uniq = (op->opts & FSDIRTREE_MUNIQ) ;
	            if (S_ISDIR(sbp->st_mode) && f_uniq) {
	                dev_t	dev = sbp->st_dev ;
	                ino_t	ino = sbp->st_ino ;
	                int	rs1 = fsdirtree_dirhave(op,dev,ino,nullptr) ;
	                if (rs1 >= 0) {
	                    f_proc = false ;
	                } else if (rs1 == SR_NOTFOUND) {
	                    rs = fsdirtree_diradd(op,dev,ino) ;
	                } else {
	                    rs = rs1 ;
			}
	            } /* end if (directory-uniqueness check) */
	        } /* end if (directory-uniqueness check) */
	        if ((rs >= 0) && f_proc) {
	            cdnp = op->bnbuf + op->bndlen ;
	            clen = op->cdnlen - op->bndlen + flen ;
	            if (S_ISDIR(sbp->st_mode)) {
	                ndir += 1 ;
	                rs = fifostr_add(op->dqp,cdnp,clen) ;
	                if (rs < 0) break ;
	            } /* end if (special handling for directories) */
	            if ((op->opts & FSDIRTREE_MTYPE) == 0) break ;
	            if (interested(op->opts,sbp->st_mode)) break ;
	        } else if ((rs == SR_NOENT) || (rs == SR_ACCESS)) {
	            if (op->opts & FSDIRTREE_MNOENT) break ;
	            rs = SR_OK ;
	        } /* end if */
	    } else if (rs == 0) { /* EOF on directory read */
	        len = 0 ;
	        op->f.dir = false ;
	        rs = fsdir_close(op->dirp) ;
	        if ((op->bndlen > 0) && 
	            (op->bnbuf[op->bndlen - 1] != '/')) {
	            op->bnbuf[op->bndlen++] = '/' ;
		}

	        while (rs >= 0) {
	            cdnp = op->bnbuf + op->bndlen ;
	            mlen = var.maxpathlen - op->bndlen ;
	            rs = fifostr_rem(op->dqp,cdnp,mlen) ;
	            len = rs ;
	            if ((rs < 0) && (rs != SR_NOTFOUND)) break ;
	            if (rs == SR_NOTFOUND) {
	                cdnp = nullptr ;
	                rs = SR_OK ;
	                len = 0 ;
	                op->f.feof = true ;
	                break ;
	            }
	            cdnp[len] = '\0' ; /* not needed? */
	            op->cdnlen = op->bndlen + len ;
	            if ((rs = fsdir_open(op->dirp,op->bnbuf)) >= 0) {
	                op->f.dir = true ;
	                break ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	        } /* end while */
	    } /* end if (directory-read) */
	} /* end while (outer) */
	rbuf[0] = '\0' ;
	if ((rs >= 0) && (cdnp != nullptr) && (! op->f.feof)) {
	    rs = mknpath1(rbuf,rlen,cdnp) ;
	    len = rs ;
	}
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fsdirtree_read) */

int fsdirtree_prune(fsdirtree *op,cchar **prune) noex {
    	int		rs ;
	if ((rs = fsdirtree_magic(op,prune)) >= 0) {
	    op->prune = prune ;
	}
	return rs ;
}
/* end subroutine (fsdirtree_prune) */


/* private subroutines */

static int fsdirtree_opener(fsdirtree *op,cchar *dname) noex {
    	cint		maxpath = var.maxpathlen ;
	int             rs = SR_OK ;
        cchar   *bdp = dname ;
        if ((bdp == nullptr) || (strcmp(bdp,".") == 0)) bdp = "" ;
        if (bdp[0] != '/') {
            if ((rs = uc_getcwd(op->bnbuf,maxpath)) >= 0) {
                op->bndlen = rs ;
                if (bdp[0] != '\0')
                    op->bnbuf[op->bndlen++] = '/' ;
            }
        }
        if (rs >= 0) {
            if (bdp[0] != '\0') {
                cint	cl = maxpath - op->bndlen ;
                rs = sncpy1((op->bnbuf + op->bndlen),cl,bdp) ;
                op->bndlen += rs ;
            }
            if (rs >= 0) {
                if ((rs = fsdir_open(op->dirp,op->bnbuf)) >= 0) {
                    op->f.dir = true ;
                    if (op->bnbuf[op->bndlen - 1] != '/') {
                        op->bnbuf[op->bndlen++] = '/' ;
                    }
                    if (opts & FSDIRTREE_MUNIQ) {
                        if ((rs = fsdirtree_dirbegin(op)) >= 0) {
                            USTAT       sb ;
                            if ((rs = uc_stat(op->bnbuf,&sb)) >= 0) {
                                dev_t   dev = sb.st_dev ;
                                ino_t   ino = sb.st_ino ;
                                rs = fsdirtree_diradd(op,dev,ino) ;
                            } /* end if (stat) */
                            if (rs < 0) {
                                fsdirtree_dirend(op) ;
                            }
                        } /* end if (dir-tracking) */
                    } /* end if (uniq traversal requested) */
                    if (rs >= 0) {
                        op->cdnlen = op->bndlen ;
                        op->magic = FSDIRTREE_MAGIC ;
                    }
                    if (rs < 0) {
                        fsdir_close(op->dirp) ;
                    }
                } /* end if (fsdir) */
            } /* end if (ok) */
        } /* end if (ok) */
	return rs ;
}
/* end ubroutine (fsdirtree_opener) */

static int fsdirtree_dirbegin(fsdirtree *pip) noex {
	HDB		*dbp = &pip->dirids ;
	cint		n = 50 ;
	cint		at = 1 ;	/* use |lookaside(3dam)| */
	int		rs ;
	if ((rs = hdb_start(dbp,n,at,diridhash,diridcmp)) >= 0) {
	    pip->f.dirids = true ;
	}
	return rs ;
}
/* end subroutine (fsdirtree_dirbegin) */

static int fsdirtree_dirend(fsdirtree *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pip->f.dirids) {
	    HDB		*dbp = &pip->dirids ;
	    HDB_CUR	cur ;
	    HDB_DATUM	key, val ;
	    pip->f.dirids = false ;
	    if ((rs1 = hdb_curbegin(dbp,&cur)) >= 0) {
	        DIRID	*dip ;
	        while (hdb_curenum(dbp,&cur,&key,&val) >= 0) {
	            dip = (DIRID *) val.buf ;
	            if (dip != nullptr) {
	                rs1 = dirid_finish(dip) ;
	                if (rs >= 0) rs = rs1 ;
	                rs1 = uc_free(dip) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end while */
	        hdb_curend(dbp,&cur) ;
	    } /* end if */
	    if (rs >= 0) rs = rs1 ;
	    rs1 = hdb_finish(&pip->dirids) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (was activated) */
	return rs ;
}
/* end subroutine (fsdirtree_dirend) */

static int fsdirtree_diradd(fsdirtree *pip,dev_t dev,ino_t ino) noex {
	HDB		*dbp = &pip->dirids ;
	HDB_DATUM	key, val ;
	DIRID		*dip ;
	cint		sz = szof(DIRID) ;
	int		rs ;
	if ((rs = uc_malloc(sz,&dip)) >= 0) {
	    if ((rs = dirid_start(dip,dev,ino)) >= 0) {
	        key.buf = dip ;
	        key.len = szof(ino_t) + szof(dev_t) ;
	        val.buf = dip ;
	        val.len = sz ;
	        rs = hdb_store(dbp,key,val) ;
	        if (rs < 0)
	            dirid_finish(dip) ;
	    } /* end if (dirid-start) */
	    if (rs < 0)
	        uc_free(dip) ;
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (fsdirtree_diradd) */

static int fsdirtree_dirhave(fsdirtree *pip,dev_t d,ui ino,DIRID **rpp) noex {
	HDB		*dbp = &pip->dirids ;
	HDB_DATUM	key, val ;
	DIRID		did ;
	int		rs ;
	did.ino = ino ;
	did.dev = d ;
	key.buf = &did ;
	key.len = szof(ino_t) + szof(dev_t) ;
	if ((rs = hdb_fetch(dbp,key,nullptr,&val)) >= 0) {
	    if (rpp) *rpp = (DIRID *) val.buf ;
	}
	return rs ;
}
/* end subroutine (fsdirtree_dirhave) */

static int dirid_start(DIRID *dip,dev_t dev,ino_t ino) noex {
	int		rs = SR_OK ;
	dip->dev = dev ;
	dip->ino = ino ;
	return rs ;
}
/* end subroutine (dirid_start) */

static int dirid_finish(DIRID *dip) noex {
	int		rs = SR_FAULT ;
	if (dip) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (dirid_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    maxlinklen = (rs * MAXLINKLEN_MULT) ;
	}
    	return rs ;
}

/* end method (vars::operator) */

static uint diridhash(cvoid *vp,int vl) noex {
	uint		h = 0 ;
	ushort		*sa = (ushort *) vp ;
	h = h ^ ((sa[1] << 16) | sa[0]) ;
	h = h ^ ((sa[0] << 16) | sa[1]) ;
	if (vl > szof(uint)) {
	    h = h ^ ((sa[3] << 16) | sa[2]) ;
	    h = h ^ ((sa[2] << 16) | sa[3]) ;
	    if (vl > szof(ulong)) {
	        h = h ^ ((sa[5] << 16) | sa[4]) ;
	        h = h ^ ((sa[4] << 16) | sa[5]) ;
	        if (vl > (4*3)) {
	            h = h ^ ((sa[7] << 16) | sa[6]) ;
	            h = h ^ ((sa[6] << 16) | sa[7]) ;
	        }
	    }
	}
	return h ;
}
/* end subroutine (diridhash) */

static int diridcmp(DIRID *e1p,DIRID *e2p,int len) noex {
	int64_t		d ;
	int64_t		d ;
	int		rc = 0 ;
	{
	    d = (e1p->dev - e2p->dev) ;
	    if (d == 0) d = (e1p->ino - e2p->ino) ;
	}
	{
	    if (d > 0) rc = 1 ;
	    else if (d < 0) rc = -1 ;
	}
	return rc ;
}
/* end subroutine (diridcmp) */

static bool interested(int opts,mode_t mode) noex {
	bool		f = false ;
	f = f || ((opts & FSDIRTREE_MREG) && S_ISREG(mode)) ;
	f = f || ((opts & FSDIRTREE_MDIR) && S_ISDIR(mode)) ;
	f = f || ((opts & FSDIRTREE_MLINK) && S_ISLNK(mode)) ;
	f = f || ((opts & FSDIRTREE_MPIPE) && S_ISFIFO(mode)) ;
	f = f || ((opts & FSDIRTREE_MSOCK) && S_ISSOCK(mode)) ;
	f = f || ((opts & FSDIRTREE_MCHAR) && S_ISCHR(mode)) ;
	f = f || ((opts & FSDIRTREE_MBLOCK) && S_ISBLK(mode)) ;
	return f ;
}
/* end subroutine (interested) */


