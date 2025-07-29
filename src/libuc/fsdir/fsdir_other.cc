/* fsdir_other SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to read directory entries in the UNIX® file system */
/* version %I% last-modified %G% */

#define	CF_TEST		0		/* test */

/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written so that we could use a single
	file-system directory interface due to all of the changes
	in the POSIX® standard and previous UNIX® standards.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fsdir

	Description:
	This code module provides a platform independent implementation
	of UNIX® file system directory access.  This module uses
	the system call |getdents(2)| to read the directories and
	to format them into entries. This is on Solaris® and perhaps
	some others (even some more but in slightly different forms),
	but it is not generally portable.  A portable version of
	this object is located in |fsdirport(3dam)|.  It would have
	been colocated in this file (with appropriate compile-time
	define switches) but it is just way too ugly to look at.
	Besides, depending on platform, it itself is not always
	multi-thread-safe or reentrant. If you want it you know
	where to find it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysflag.h>

#if	F_SUNOS || F_LINUX || (defined(CF_TEST) && (CF_TEST > 0))

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dirent.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<intsat.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"fsdir.h"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	FSDIR_MINBUFLEN	512		/* minimum buffer length */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::destroy_at ;			/* subroutine-template */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libu::cfdec ;			/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef	dirent_t *	direntp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct othermgr {
	fsdir		*op ;
	void		*bdata ;
	intoff_t	doff ;
	intoff_t	eoff ;
	int		bsz ;
	int		blen ;
	int		dfd ;
	int		ei ;
	othermgr(fsdir *p) noex : op(p) {
	    bdata = nullptr ;
	    blen = 0 ;
	    bsz = 0 ;
	    dfd = -1 ;
	} ; /* end ctor */
	destruct othermgr() {
	    bdata = nullptr ;
	    bsz = 0 ;
	    blen = 0 ;
	    dfd = -1 ;
	} ; /* end dtor */
	int open(cchar *) noex ;
	int close() noex ;
	int read(fsdir_ent *,char *,int) noex ;
	int tell(off_t *) noex ;
	int seek(off_t) noex ;
	int rewind() noex ;
	int audit() noex ;
	int fdbegin(cchar *) noex ;
	int fdend() noex ;
    } ; /* end struct (othermgr) */
    typedef othermgr *	othermgrp ;
} /* end namespace */


/* forward referecnces */

template<typename ... Args>
static int fsdir_ctor(fsdir *op,Args ... args) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cint	sz = szof(othermgr) ;
	    op->magic = 0 ;
	    op->fl = {} ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		rs = SR_NOTCONN ;
		memclear(vp,sz) ;
	        if (othermgr *omp ; (omp = new(vp) othermgr(op)) != np) {
		    omp->op = op ;
	            op->posixp = omp ;
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    uc_free(op->posixp) ;
		    op->posixp = nullptr ;
		} /* end if (error) */
	    } /* end if (memory-allocation) */
	}
	return rs ;
} /* end subroutine (fsdir_ctor) */

static int fsdir_dtor(fsdir *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
		rs = SR_OK ;
		{
		    destroy_at(omp) ;
		}
		{
		    rs1 = uc_free(op->posixp) ;
		    if (rs >= 0) rs = rs1 ;
		    op->posixp = nullptr ;
		}
	    } /* end if (non-null) */
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdir_dtor) */

template<typename ... Args>
static inline int fsdir_magic(fsdir *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FSDIR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (fsdir_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

int fsdir_open(fsdir *op,cchar *dname) noex {
	int		rs ;
	if ((rs = fsdir_ctor(op,dname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
		rs = omp->open(dname) ;
	    } /* end if (valid) */
	    if (rs < 0) {
		fsdir_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdir_open) */

int fsdir_close(fsdir *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
		rs = SR_OK ;
		{
		    rs1 = omp->close() ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
		    rs1 = fsdir_dtor(op) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (open) */
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_close) */

#ifdef	COMMENT
typedef struct dirent {
	ino_t		d_ino;		/* "inode number" of entry */
	off_t		d_off;		/* offset of disk directory entry */
	unsigned short	d_reclen;	/* length of this record */
	char		d_name[1];	/* name of file */
} dirent_t;
#endif /* COMMENT */

int fsdir_read(fsdir *op,fsdir_ent *dep,char *nbuf,int nlen) noex {
	int		rs ;
	if ((rs = fsdir_magic(op,dep,nbuf)) >= 0) {
	    nbuf[0] = '\0' ;
	    dep->name = nbuf ;
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
		rs = omp->read(dep,nbuf,nlen) ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_read) */

int fsdir_tell(fsdir *op,off_t *offp) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
	        rs = omp->tell(offp) ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_tell) */

int fsdir_seek(fsdir *op,off_t o) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
	        rs = omp->seek(o) ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_seek) */

int fsdir_rewind(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
	        rs = omp->rewind() ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_rewind) */

int fsdir_audit(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_NOTCONN ;
	    if (op->posixp) {
		othermgr *omp = resumelife<othermgr>(op->posixp) ;
	        rs = omp->audit() ;
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_audit) */


/* private subroutines */

int othermgr::open(cchar *dname) noex {
    	int		rs ;
	        if ((rs = fdbegin(dname)) >= 0) {
	            if (USTAT sb ; (rs = u_fstat(dfd,&sb)) >= 0) {
		        rs = SR_NOTDIR ;
		        if (S_ISDIR(sb.st_mode)) {
			    if ((rs = ucpagesize) >= 0) {
	    	                cint	ps = rs ;
	    	                int	ds = intsat(sb.st_size) ;
			        int	sz ;
			        if (ds < FSDIR_MINBUFLEN) {
				    ds = FSDIR_MINBUFLEN ;
			        }
			        sz = min(ps,ds) ;
	    	                if (char *bp ; (rs = uc_valloc(sz,&bp)) >= 0) {
		                    bdata = bp ;
			            bsz = sz ;
	                            op->magic = FSDIR_MAGIC ;
	                        } /* end if (memory-allocation) */
			    } /* end if (pagesize) */
		        } /* end if (direcyory indicated) */
	            } /* end if (stat) */
	            if (rs < 0) {
	                fdend() ;
	            }
		} /* end if (othermgr_begin) */
		return rs ;
} /* end method (othermgr::open) */

int othermgr::close() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	    if (bdata) {
	        rs1 = uc_free(bdata) ;
	        if (rs >= 0) rs = rs1 ;
	        bdata = nullptr ;
	    }
	    {
	        rs1 = fdend() ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    return rs ;
} /* end method (othermgr::close) */

int othermgr::read(fsdir_ent *dep,char *nbuf,int nlen) noex {
	caddr_t		bufp = caddr_t(bdata) ;
    	int		rs = SR_OK ;
	        if (ei >= blen) {
	            dirent_t	*dp = direntp(bdata) ;
	            rs = u_getdents(dfd,dp,bsz) ;
	            blen = rs ;
	            doff += rs ;
	            ei = 0 ;
	        }
	        if ((rs >= 0) && (blen > 0)) { /* greater-than-zero */
	            dirent_t	*dp = direntp(bufp + ei) ;
	            int		ml ;
	            ml = (dp->d_reclen - 18) ;
	            dep->ino = dp->d_ino ;
	            dep->nlen = dp->d_namlen ;
	            dep->type = 0 ;
	            if ((rs = snwcpy(nbuf,nlen,dp->d_name,ml)) >= 0) {
		        ei += dp->d_reclen ;
	            }
	        } else {
	            dep->ino = 0 ;
	            dep->nlen = 0 ;
	            dep->name = nullptr ;
	            dep->type = 0 ;
	        } /* end if */
	return rs ;
} /* end method (othermgr::read) */

int othermgr::tell(off_t *offp) noex {
	int		rs ;
	if (offp) *offp = eoff ;
	rs = int(eoff & INT_MAX) ;
	return rs ;
} /* end method (othermgr::tell) */

int othermgr::seek(off_t o) noex {
	int		rs ;
	if ((rs = u_seek(dfd,o,SEEK_SET)) >= 0) {
	    blen = 0 ;
	    doff = intoff_t(o) ;
	    eoff = intoff_t(o) ;
	    ei = 0 ;
	}
	return rs ;
} /* end method (othermgr::seek) */

int othermgr::rewind() noex {
    	int		rs = SR_OK ;
	    if (doff > 0) {
	        blen = 0 ;
	        doff = 0 ;
	        rs = u_rewind(dfd) ;
	    }
	    eoff = 0 ;
	    ei = 0 ;
	return rs ;
} /* end method (othermgr::rewind) */

int othermgr::audit() noex {
	int		rs ;
	if (bdata || (bsz == 0)) {
	    rs = SR_BADFMT ;
	}
	return rs ;
} /* end method (othermgr::audit) */

int othermgr::fdbegin(cchar *dname) noex {
	int		rs ;
	if (dname[0] == '*') {
	    if (int v ; (rs = cfdec((dname+1),-1,&v)) >= 0) {
		if ((rs = u_dup(v)) >= 0) {
		    dfd = rs ;
		    op->fl.descname = true ;
		}
	    }
	} else {
	    rs = uc_open(dname,O_RDONLY,0666) ;
	    dfd = rs ;
	}
	if (rs >= 0) {
	    rs = uc_closeonexec(dfd,true) ;
	    if (rs < 0) {
		u_close(dfd) ;
		dfd = -1 ;
	    }
	} /* end if (ok) */
	return rs ;
} /* end subroutine (othermgr::fdbegin) */

int othermgr::fdend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (dfd >= 0) {
	    rs1 = u_close(dfd) ;
	    if (rs >= 0) rs = rs1 ;
	    dfd = -1 ;
	}
	return rs ;
} /* end subroutine (othermgr::fdend) */


#endif /* F_SUNOS || F_LINUX */


