/* fsdir SUPPORT */
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
	of UNIX® file system directory access.

	This module uses the system call |getdent(2)| to read the
	directories and to format them into entries. This is on
	Solaris® and perhaps some others (even some more but in
	slightly different forms), but it is not generally portable.
	A portable version of this object is located in |fsdirport(3dam)|.
	It would have been colocated in this file (with appropriate
	compile-time define switches) but it is just way too ugly
	to look at. Besides, depending on platform, it itself is
	not always multi-thread-safe or reentrant. If you want it
	you know where to find it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/dirent.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<snwcpy.h>
#include	<cfdec.h>
#include	<localmisc.h>
#include	<posixdirent.hh>

#include	"darwin_fsdir.h"


/* local defines */


/* local namespaces */


/* external subroutines */


/* external variables */


/* local typedefs */


/* local structures */


/* forward referecnces */

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
static int	fsdir_isdir(fsdir *,cchar *) noex ;


/* local variables */


/* exported subroutines */

int fsdir_open(fsdir *op,cchar *dname) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_INVALID ;
	    op->magic = 0 ;
	    op->posixp = nullptr ;
	    op->f = {} ;
	    op->dfd = -1 ;
	    if (dname[0]) {
		if ((rs = fsdir_isdir(op,dname)) >= 0) {
	            if ((rs = fsdir_begin(op,dname)) >= 0) {
			op->magic = FSDIR_MAGIC ;
		    } /* end if (fsdir_begin) */
		} /* end if (isdir) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (fsdir_open) */

int fsdir_close(fsdir *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    {
	        rs1 = fsdir_end(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_close) */

#ifdef	COMMENT
typedef struct dirent {
	ino_t		d_ino;		/* "inode number" of entry */
	unsigned short	d_namlen;	/* length of this record */
	uchar		d_type ;	/* type of file */
	char		d_name[1];	/* name of file */
} dirent_t;
#endif /* COMMENT */

int fsdir_read(fsdir *op,fsdir_ent *dep,char *nbuf,int nlen) noex {
	int		rs ;
	if ((rs = fsdir_magic(op,dep,nbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (nlen > 0) {
		posixdirent	*objp = (posixdirent *) op->posixp ;
		dirent		de ;
		if ((rs = objp->read(&de,nbuf,nlen)) >= 0) {
		    dep->ino = de.d_ino ;
		    dep->nlen = de.d_namlen ;
		    dep->name = nbuf ;
		    dep->type = de.d_type ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_read) */

int fsdir_tell(fsdir *op,off_t *rp) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    posixdirent	*objp = (posixdirent *) op->posixp ;
	    off_t	o{} ;
	    if ((rs = objp->tell(&o)) >= 0) {
		if (rp) *rp = o ;
		rs = int(o & INT_MAX) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_tell) */

int fsdir_seek(fsdir *op,off_t o) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    posixdirent	*objp = (posixdirent *) op->posixp ;
	    rs = objp->seek(o) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_seek) */

int fsdir_rewind(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    posixdirent	*objp = (posixdirent *) op->posixp ;
	    rs = objp->rewind() ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_rewind) */

int fsdir_audit(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    rs = 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_audit) */


/* private subroutines */

static int fsdir_begin(fsdir *op,cchar *dname) noex {
	cint		objl = sizeof(posixdirent) ;
	int		rs ;
	void		*vp{} ;
	if ((rs = uc_libmalloc(objl,&vp)) >= 0) {
	    posixdirent	*objp ;
	    rs = SR_NOMEM ;
	    if ((objp = new(vp) posixdirent) != nullptr) {
		if ((rs = objp->open(dname)) >= 0) {
	            op->posixp = vp ;
		}
		if (rs < 0) {
		    objp->~posixdirent() ;
		}
	    } /* end if (operator-new) */
	    if (rs < 0) {
		uc_libfree(vp) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (fsdir_begin) */

static int fsdir_end(fsdir *op) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op->posixp) {
	    rs = SR_OK ;
	    {
	        posixdirent	*objp = (posixdirent *) op->posixp ;
	        rs1 = objp->close() ;
	        if (rs >= 0) rs = rs1 ;
	        objp->~posixdirent() ;
	    }
	    {
		rs1 = uc_libfree(op->posixp) ;
	        if (rs >= 0) rs = rs1 ;
		op->posixp = nullptr ;
	    }
	} /* end if (was allocated) */
	return rs ;
}
/* end subroutine (fsdir_end) */

static int fsdir_isdir(fsdir *op,cchar *dname) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op) {
	    USTAT	sb ;
	    if ((rs = uc_stat(dname,&sb)) >= 0) {
		rs = SR_NOTDIR ;
		if (S_ISDIR(sb.st_mode)) {
		    f = true ;
		    rs = SR_OK ;
		} /* end if (directory indicated) */
	    } /* end if (stat) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (fsdir_isdir) */

int fsdir_opener::operator () (cchar *n) noex {
	if (n == nullptr) n = name ;
	return fsdir_open(op,n) ;
}
/* end method (fsdir_opener:operator) */

fsdir_opener::operator int () noex {
	return fsdir_open(op,name) ;
}
/* end method (fsdir_opener:operator) */

fsdir_co::operator int () noex {
	int	rs = SR_BUGCHECK ;
	switch (w) {
	case fsdirmem_rewind:
	    rs = fsdir_rewind(op) ;
	    break ;
	case fsdirmem_audit:
	    rs = fsdir_audit(op) ;
	    break ;
	case fsdirmem_close:
	    rs = fsdir_close(op) ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end method (fsdir_co::operator) */

int fsdir::read(fsdir_ent *ep,char *ebuf,int elen) noex {
	return fsdir_read(this,ep,ebuf,elen) ;
}

int fsdir::tell(off_t *offp) noex {
	return fsdir_tell(this,offp) ;
}

int fsdir::seek(off_t o) noex {
	return fsdir_seek(this,o) ;
}


