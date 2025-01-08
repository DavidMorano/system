/* fsdir SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* object to read directory entries in the UNIX® file system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written so that we could use a single
	file-system directory interface due to all of the changes
	in the POSIX standard and previous UNIX® standards.


*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dirent.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<intsat.h>
#include	<snwcpy.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"sunos_fsdir.h"


/* local defines */

#define	FSDIR_BMODULUS	512		/* seems to be what OS uses */
#define	FSDIR_MINBUFLEN	512		/* minimum buffer length */

#if	defined(OSNAME_SunOS) && (OSNAME_SunOS > 0)
#define	F_SUNOS		1
#else /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */
#define	F_SUNOS		0
#endif /* defined(OSNAME_SunOS) && (OSNAME_SunOS > 0) */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef	dirent_t *	direntp ;


/* external subroutines */

extern "C" {
    extern int u_getdents(int,dirent_t *,int) noex ;
}


/* external variables */


/* local typedefs */


/* local structures */


/* forward referecnces */

template<typename ... Args>
static int fsdir_ctor(fsdir *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	    op->f = {} ;
	    op->bdata = nullptr ;
	    op->bsize = 0 ;
	    op->blen = 0 ;
	    op->doff = 0 ;
	    op->eoff = 0 ;
	    op->dfd = 0 ;
	    op->ei = 0 ;
	}
	return rs ;
}

template<typename ... Args>
static inline int fsdir_magic(fsdir *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FSDIR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static int	fsdir_begin(fsdir *,cchar *) noex ;
static int	fsdir_end(fsdir *) noex ;


/* local variables */

constexpr bool	f_sunos = F_SUNOS ;


/* exported variables */


/* exported subroutines */

int fsdir_open(fsdir *op,cchar *dname) noex {
	int		rs ;
	if ((rs = fsdir_ctor(op,dname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
	        if ((rs = fsdir_begin(op,dname)) >= 0) {
	            if (USTAT sb ; (rs = u_fstat(op->dfd,&sb)) >= 0) {
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
		                    op->bdata = bp ;
			            op->bsize = sz ;
	                            op->magic = FSDIR_MAGIC ;
	                        } /* end if (memory-allocation) */
			    } /* end if (pagesize) */
		        } /* end if (direcyory indicated) */
	            } /* end if (stat) */
	            if (rs < 0) {
	                fsdir_end(op) ;
	            }
		} /* end if (fsdir_begin) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdir_open) */

int fsdir_close(fsdir *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    if (op->bdata) {
	        rs1 = uc_free(op->bdata) ;
	        if (rs >= 0) rs = rs1 ;
	        op->bdata = nullptr ;
	    }
	    rs1 = fsdir_end(op) ;
	    if (rs >= 0) rs = rs1 ;
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
	        if (op->ei >= op->blen) {
	            dirent_t	*dp = direntp(op->bdata) ;
	            rs = u_getdents(op->dfd,dp,op->bsize) ;
	            op->blen = rs ;
	            op->doff += rs ;
	            op->ei = 0 ;
	        }
	        if ((rs >= 0) && (op->blen > 0)) { /* greater-than-zero */
	            dirent_t	*dp = direntp(op->bdata + op->ei) ;
	            int		ml ;
	            ml = (dp->d_reclen-18) ;
	            dep->ino = (ino_t) dp->d_ino ;
	            dep->nlen = (ushort) dp->d_namlen ;
	            dep->type = 0 ;
	            if ((rs = snwcpy(nbuf,nlen,dp->d_name,ml)) >= 0) {
#if	F_SUNOS
	                op->eoff = dp->d_off ;
#endif /* F_SUNOS */
		        op->ei += dp->d_reclen ;
	            }
	        } else {
	            dep->ino = 0 ;
	            dep->nlen = 0 ;
	            dep->name = nullptr ;
	            dep->type = 0 ;
	        } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_read) */

int fsdir_tell(fsdir *op,off_t *offp) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    if (offp) *offp = op->eoff ;
	    rs = int(op->eoff & INT_MAX) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_tell) */

int fsdir_seek(fsdir *op,off_t o) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = SR_INVALID ;
	    if (o >= 0) {
	        if ((rs = u_seek(op->dfd,o,SEEK_SET)) >= 0) {
	            op->blen = 0 ;
	            op->doff = int(o) ;
	            op->eoff = int(o) ;
	            op->ei = 0 ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_seek) */

int fsdir_rewind(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    if (op->doff > 0) {
	        op->blen = 0 ;
	        op->doff = 0 ;
	        rs = u_rewind(op->dfd) ;
	    }
	    op->eoff = 0 ;
	    op->ei = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_rewind) */

int fsdir_audit(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    if (op->bdata || (op->bsize == 0)) rs = SR_BADFMT ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_audit) */


/* private subroutines */

static int fsdir_begin(fsdir *op,cchar *dname) noex {
	int		rs ;
	if (dname[0] == '*') {
	    if (int v ; (rs = cfdeci((dname+1),-1,&v)) >= 0) {
		if ((rs = u_dup(v)) >= 0) {
		    op->f.descname = true ;
		}
	    }
	} else {
	    rs = uc_open(dname,O_RDONLY,0666) ;
	}
	if (rs >= 0) {
	    op->dfd = rs ;
	    rs = uc_closeonexec(op->dfd,true) ;
	    if (rs < 0) {
		u_close(op->dfd) ;
		op->dfd = -1 ;
	    }
	}
	return rs ;
}
/* end subroutine (fsdir_begin) */

static int fsdir_end(fsdir *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->dfd >= 0) {
	    rs1 = u_close(op->dfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dfd = -1 ;
	}
	return rs ;
}
/* end subroutine (fsdir_end) */

void fsdir::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("fsdir",rs,"fini-finish") ;
	}
}


