/* fsdir_xx SUPPORT (xx) */
/* encoding=ISO8859-1 */
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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/dirent.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<new>			/* |nothrow(3c++)| */
#include	<memory>		/* |destroy_at(3c++)| */
#include	<usystem.h>
#include	<intsat.h>
#include	<snwcpy.h>
#include	<cfdec.h>
#include	<localmisc.h>
#include	<posixdirent.hh>

#include	"fsdir.h"


/* local defines */


/* imported namespaces */

using std::destroy_at ;			/* subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward referecnces */


/* local variables */


/* exported variables */


/* exported subroutines */


/* local subroutines */

int fsdir::isdir(cchar *dname) noex {
	int		rs ;
	int		f = false ;
	if (USTAT sb ; (rs = u_stat(dname,&sb)) >= 0) {
	    rs = SR_NOTDIR ;
	    if (S_ISDIR(sb.st_mode)) {
		dev = sb.st_dev ;
		f = true ;
		rs = SR_OK ;
	    } /* end if (directory indicated) */
	} /* end if (stat) */
	return (rs >= 0) ? f : rs ;
} /* end method (fsdir::isdir) */

int fsdir_opener::operator () (cchar *n) noex {
	if (n == nullptr) n = name ;
	return fsdir_open(op,n) ;
}
/* end method (fsdir_opener:operator) */

fsdir_opener::operator int () noex {
	return fsdir_open(op,name) ;
}
/* end method (fsdir_opener:operator) */

int fsdir_te::operator () (off_t *offp) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case fsdirmem_tell:
	        rs = fsdir_tell(op,offp) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (fsdir_co::operator) */

fsdir_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
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
	} /* end if (non-null) */
	return rs ;
}
/* end method (fsdir_co::operator) */

int fsdir::read(fsdir_ent *ep,char *ebuf,int elen) noex {
	return fsdir_read(this,ep,ebuf,elen) ;
}

int fsdir::seek(off_t o) noex {
	return fsdir_seek(this,o) ;
}

void fsdir::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("fsdir",rs,"dtor-close") ;
	}
}


