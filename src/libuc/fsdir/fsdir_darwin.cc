/* fsdir_darwin SUPPORT (Darwin) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to read directory entries in the UNIX® file system */
/* version %I% last-modified %G% */


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
	the system call |getdent(2)| to read the directories and
	to format them into entries.  This is on Solaris® and perhaps
	some others (even some more but in slightly different forms),
	but it is not generally portable.  A portable version of
	this object is located in |fsdirport(3dam)|.  It would have
	been colocated in this file (with appropriate compile-time
	define switches) but it is just way too ugly to look at.
	Besides, depending on platform, it itself is not always
	multi-thread-safe or reentrant.  If you want it you know
	where to find it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(OSNAME_Darwin) && (OSNAME_Darwin > 0)
#define	F_DARWIN		1
#else /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */
#define	F_DARWIN		0
#endif /* defined(OSNAME_Darwin) && (OSNAME_Darwin > 0) */

#if	F_DARWIN

#include	<sys/dirent.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<memory>		/* |destroy_at(3c++)| */
#include	<usyscalls.h>
#include	<intsat.h>
#include	<localmisc.h>
#include	<posixdirent.hh>

#include	"fsdir.h"


/* local defines */


/* imported namespaces */

using std::destroy_at ;			/* subroutine */
using libu::snwcpy ;			/* subroutine */
using libu::umem ;			/* variable */


/* local typedefs */

typedef posixdirent *	posixdirentp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward referecnces */

template<typename ... Args>
static inline int fsdir_magic(fsdir *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    if ((rs = (op->magic == FSDIR_MAGIC)) >= 0) {
		if (! op->posixp) rs = SR_NOTOPEN ;
	    } else {
		rs = SR_NOTOPEN ;
	    }
	}
	return rs ;
}

static int	fsdir_begin(fsdir *,cchar *) noex ;
static int	fsdir_end(fsdir *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int fsdir_open(fsdir *op,cchar *dname) noex {
	int		rs = SR_FAULT ;
	if (op && dname) {
	    rs = SR_INVALID ;
	    op->magic = 0 ;
	    op->posixp = nullptr ;
	    op->fl = {} ;
	    if (dname[0]) {
		if ((rs = op->isdir(dname)) >= 0) {
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
		posixdirent	*objp = posixdirentp(op->posixp) ;
		if (dirent_t de ; (rs = objp->read(&de,nbuf,nlen)) >= 0) {
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
	    posixdirent	*objp = posixdirentp(op->posixp) ;
	    if (off_t o ; (rs = objp->tell(&o)) >= 0) {
		if (rp) *rp = o ;
		rs = intsat(o) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_tell) */

int fsdir_seek(fsdir *op,off_t o) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    posixdirent	*objp = posixdirentp(op->posixp) ;
	    rs = objp->seek(o) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (fsdir_seek) */

int fsdir_rewind(fsdir *op) noex {
	int		rs ;
	if ((rs = fsdir_magic(op)) >= 0) {
	    posixdirent	*objp = posixdirentp(op->posixp) ;
	    rs = objp->rewind ;
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
	cnullptr	np{} ;
	cint		objl = szof(posixdirent) ;
	int		rs ;
	if (void *vp ; (rs = umem.malloc(objl,&vp)) >= 0) {
	    rs = SR_NOMEM ;
	    if (posixdirent *objp ; (objp = new(vp) posixdirent) != np) {
		if ((rs = objp->open(dname)) >= 0) {
	            op->posixp = vp ;
		}
		if (rs < 0) {
		    destroy_at(objp) ;
		}
	    } /* end if (operator-new) */
	    if (rs < 0) {
		umem.free(vp) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (fsdir_begin) */

static int fsdir_end(fsdir *op) noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (op->posixp) {
	    posixdirent	*objp = posixdirentp(op->posixp) ;
	    rs = SR_OK ;
	    {
	        rs1 = objp->close ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    destroy_at(objp) ;
	    {
		rs1 = umem.free(op->posixp) ;
	        if (rs >= 0) rs = rs1 ;
		op->posixp = nullptr ;
	    }
	} /* end if (was allocated) */
	return rs ;
}
/* end subroutine (fsdir_end) */


#endif /* F_DARWIN */


