/* fsdirtree */
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
	int fsdirtree_open(fsdirtree *op,cchar *dname,int) noex

	Arguments:
	op		object pointer
	dname		directory name to process
	opts		options for processing

	Returns:
	>=0		good
	<0		bad in some way!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstdint>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<fsdir.h>
#include	<fifostr.h>
#include	<strwcpy.h>
#include	<sncpyx.h>
#include	<mknpathx.h>
#include	<matxstr.h>
#include	<hasnot.h>
#include	<ischarx.h>
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

#ifndef	MAXLINKLEN
#define	MAXLINKLEN	(4*MAXPATHLEN)
#endif


/* local namespaces */


/* local typedefs */

typedef uint32_t	ui ;


/* external subroutines */

extern "C" {
    extern int	isDotDir(const char *) noex ;
}


/* external variables */


/* local structures */

struct fsdirtree_dirid {
	uino_t		ino ;
	dev_t		dev ;
} ;


/* forward references */

static uint	diridhash(const void *,int) noex ;

static int	fsdirtree_dirbegin(fsdirtree *) noex ;
static int	fsdirtree_diradd(fsdirtree *,dev_t,uino_t) noex ;
static int	fsdirtree_dirhave(fsdirtree *,dev_t,uino_t,DIRID **) noex ;
static int	fsdirtree_dirend(fsdirtree *) noex ;

static int	dirid_start(DIRID *,dev_t,uino_t) noex ;
static int	dirid_finish(DIRID *) noex ;

static int	diridcmp(DIRID *,DIRID *,int) noex ;

static bool	interested(int,mode_t) noex ;


/* local variables */


/* exported subroutines */

int fsdirtree_open(fsdirtree *op,cchar *dname,int opts) noex {
	int		rs ;
	if (op == nullptr) return SR_FAULT ;
	memclear(op) ;
	op->opts = opts ;
	if ((rs = fifostr_start(&op->dirq)) >= 0) {
	    const char	*bdp = dname ;
	    if ((bdp == nullptr) || (strcmp(bdp,".") == 0)) bdp = "" ;
	    if (bdp[0] != '/') {
	        if ((rs = uc_getcwd(op->bnbuf,MAXPATHLEN)) >= 0) {
	            op->bdnlen = rs ;
	            if (bdp[0] != '\0')
	                op->bnbuf[op->bdnlen++] = '/' ;
	        }
	    }
	    if (rs >= 0) {
	        if (bdp[0] != '\0') {
	            int		cl = MAXPATHLEN - op->bdnlen ;
	            rs = sncpy1((op->bnbuf + op->bdnlen),cl,bdp) ;
	            op->bdnlen += rs ;
	        }
	        if (rs >= 0) {
	            if ((rs = fsdir_open(&op->dir,op->bnbuf)) >= 0) {
	                op->f.dir = true ;
	                if (op->bnbuf[op->bdnlen - 1] != '/') {
	                    op->bnbuf[op->bdnlen++] = '/' ;
			}
	                if (opts & FSDIRTREE_MUNIQ) {
	                    if ((rs = fsdirtree_dirbegin(op)) >= 0) {
	                        USTAT	sb ;
	                        if ((rs = uc_stat(op->bnbuf,&sb)) >= 0) {
	                            dev_t	dev = sb.st_dev ;
	                            uino_t	ino = sb.st_ino ;
	                            rs = fsdirtree_diradd(op,dev,ino) ;
	                        } /* end if (stat) */
	                        if (rs < 0)
	                            fsdirtree_dirend(op) ;
	                    } /* end if (dir-tracking) */
	                } /* end if (uniq traversal requested) */
	                if (rs >= 0) {
	                    op->cdnlen = op->bdnlen ;
	                    op->magic = FSDIRTREE_MAGIC ;
	                }
	                if (rs < 0) {
	                    fsdir_close(&op->dir) ;
			}
	            } /* end if (fsdir) */
	        } /* end if (ok) */
	    } /* end if (ok) */
	    if (rs < 0) {
	        fifostr_finish(&op->dirq) ;
	    }
	} /* end if (fifostr) */
	return rs ;
}
/* end subroutine (fsdirtree_open) */

int fsdirtree_close(fsdirtree *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == FSDIRTREE_MAGIC) {
		rs = SR_OK ;
	        if (op->f.dirids) {
	            rs1 = fsdirtree_dirend(op) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->f.dir) {
	            op->f.dir = false ;
	            rs1 = fsdir_close(&op->dir) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        if (op->bnbuf) {
	            rs1 = uc_libfree(op->bnbuf) ;
	            if (rs >= 0) rs = rs1 ;
	            op->bnbuf = nullptr ;
	        }
		{
	            rs1 = fifostr_finish(&op->dirq) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->magic = 0 ;
	    /* end if (was open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdirtree_close) */

int fsdirtree_read(fsdirtree *op,FSDIRTREE_STAT *sbp,char *rbuf,int rlen) noex {
	USTAT		se ;
	FSDIR_ENT	de ;
	int		rs = SR_OK ;
	int		mlen ;
	int		clen, flen ;
	int		ndir = 0 ;
	int		len = 0 ;
	const char	*enp ;
	char		*fnp ;
	char		*cdnp = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if ((sbp == nullptr) || (rbuf == nullptr)) return SR_FAULT ;

	if (op->magic != FSDIRTREE_MAGIC) return SR_NOTOPEN ;

	if (rlen < 0) rlen = MAXPATHLEN ;

	while ((rs >= 0) && (! op->f.eof)) {
	    cdnp = nullptr ;
	    if ((rs = fsdir_read(&op->dir,&de)) > 0) {
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
	            mlen = MAXPATHLEN - op->cdnlen ;
	            flen = strwcpy(fnp,enp,mlen) - fnp ;
	            if ((rs = u_lstat(op->bnbuf,sbp)) >= 0) {
	                if (S_ISLNK(sbp->st_mode)) {
	                    if (op->opts & FSDIRTREE_MFOLLOW) {
	                        cint	llen = MAXLINKLEN ;
	                        cchar	*fn = op->bnbuf ;
	                        char	lbuf[MAXLINKLEN+1] ;
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
	                uino_t	ino = sbp->st_ino ;
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
	            cdnp = op->bnbuf + op->bdnlen ;
	            clen = op->cdnlen - op->bdnlen + flen ;
	            if (S_ISDIR(sbp->st_mode)) {
	                ndir += 1 ;
	                rs = fifostr_add(&op->dirq,cdnp,clen) ;
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
	        rs = fsdir_close(&op->dir) ;
	        if ((op->bdnlen > 0) && 
	            (op->bnbuf[op->bdnlen - 1] != '/'))
	            op->bnbuf[op->bdnlen++] = '/' ;

	        while (rs >= 0) {
	            cdnp = op->bnbuf + op->bdnlen ;
	            mlen = MAXPATHLEN - op->bdnlen ;
	            rs = fifostr_remove(&op->dirq,cdnp,mlen) ;
	            len = rs ;
	            if ((rs < 0) && (rs != SR_NOTFOUND)) break ;
	            if (rs == SR_NOTFOUND) {
	                cdnp = nullptr ;
	                rs = SR_OK ;
	                len = 0 ;
	                op->f.eof = true ;
	                break ;
	            }
	            cdnp[len] = '\0' ; /* not needed? */
	            op->cdnlen = op->bdnlen + len ;
	            if ((rs = fsdir_open(&op->dir,op->bnbuf)) >= 0) {
	                op->f.dir = true ;
	                break ;
	            } else if (isNotPresent(rs)) {
	                rs = SR_OK ;
	            }
	        } /* end while */
	    } /* end if (directory-read) */
	} /* end while (outer) */
	rbuf[0] = '\0' ;
	if ((rs >= 0) && (cdnp != nullptr) && (! op->f.eof)) {
	    rs = mknpath1(rbuf,rlen,cdnp) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fsdirtree_read) */

int fsdirtree_prune(fsdirtree *op,cchar **prune) noex {
	if (op == nullptr) return SR_FAULT ;
	if (prune == nullptr) return SR_FAULT ;
	if (op->magic != FSDIRTREE_MAGIC) return SR_NOTOPEN ;
	op->prune = prune ;
	return SR_OK ;
}
/* end subroutine (fsdirtree_prune) */


/* private subroutines */

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
	        while (hdb_enum(dbp,&cur,&key,&val) >= 0) {
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

static int fsdirtree_diradd(fsdirtree *pip,dev_t dev,uino_t ino) noex {
	HDB		*dbp = &pip->dirids ;
	HDB_DATUM	key, val ;
	DIRID		*dip ;
	const int	size = sizeof(DIRID) ;
	int		rs ;
	if ((rs = uc_malloc(size,&dip)) >= 0) {
	    if ((rs = dirid_start(dip,dev,ino)) >= 0) {
	        key.buf = dip ;
	        key.len = sizeof(uino_t) + sizeof(dev_t) ;
	        val.buf = dip ;
	        val.len = size ;
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
	key.len = sizeof(uino_t) + sizeof(dev_t) ;
	if ((rs = hdb_fetch(dbp,key,nullptr,&val)) >= 0) {
	    if (rpp) *rpp = (DIRID *) val.buf ;
	}
	return rs ;
}
/* end subroutine (fsdirtree_dirhave) */

static int dirid_start(DIRID *dip,dev_t dev,uino_t ino) noex {
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

static uint diridhash(cvoid *vp,int vl) noex {
	uint		h = 0 ;
	ushort		*sa = (ushort *) vp ;
	h = h ^ ((sa[1] << 16) | sa[0]) ;
	h = h ^ ((sa[0] << 16) | sa[1]) ;
	if (vl > sizeof(uint)) {
	    h = h ^ ((sa[3] << 16) | sa[2]) ;
	    h = h ^ ((sa[2] << 16) | sa[3]) ;
	    if (vl > sizeof(ulong)) {
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


