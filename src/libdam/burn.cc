/* burn SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* subroutine to burn (shred) a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	burn

	Description:
	This subroutine is used to "burn" (shred) the file given.

	Synopsis:
	int burn(cchar *name,int bcount = 1,randomvar *rvp = np) noex

	Arguments:
	name		name of file to burn
	bcount		number of over-writes
	rvp		RANDOMVAR object pointer

	Returns:
	>0		skip this directory entry in directory walk
	0		continue with directory walk as usual
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<randomvar.h>
#include	<hash.h>		/* |hash_elf(3dam)| */
#include	<localmisc.h>

#include	"burn.h"


/* local defines */

#define	NPAGES		16		/* number of pages to write */


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	uc_seek(int,off_t,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct burner_fl {
	uint		randomvar:1 ;
    } ; /* end struct (burner_fl) */
    struct burner {
	randomvar	*rvp = nullptr ;
	randomvar	x ;
	off_t		fsz ;
	int		bcount ;
	int		ps ;
	burner_fl	fl{} ;
	burner(randomvar *abp,int bc) noex : bcount(bc) {
	    rvp = abp ;
	} ;
	int operator () (cchar *) noex ;
	int rvbegin(cchar *) noex ;
	int rvend() noex ;
	int filer(char *,int,cchar *) noex ;
	int writer(int,char *,int) noex ;
	int rewind(int,int) noex ;
	int loadbuf(char *,int) noex ;
    } ; /* end struct (burner) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int burn(cchar *name,int bcount,randomvar *rvp) noex {
    	int		rs = SR_FAULT ;
	if (bcount <= 0) bcount = 1 ;
	if (name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        burner	bo(rvp,bcount) ;
	        rs = bo(name) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (burn) */


/* local subroutines */

int burner::rvbegin(cchar *fn) noex {
    	int		rs = SR_OK ;
	if (rvp == nullptr) {
	    uint	h = hash_elf(fn,-1) ;
	    if ((rs = x.start(true,h)) >= 0) {
		rvp = &x ;
		fl.randomvar = true ;
	    }
	} /* end if (non-null) */
	return rs ;
}

int burner::rvend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (fl.randomvar && rvp) {
	    rs1 = x.finish ;
	    if (rs >= 0) rs = rs1 ;
	    rvp = nullptr ;
	    fl.randomvar = false ;
	}
	return rs ;
}

int burner::operator () (cchar *fn) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = ucpagesize) >= 0) {
	    ps = rs ;
	    if ((rs = rvbegin(fn)) >= 0) {
		cint	flen = (NPAGES * ps) ;
		if (char *fbuf{} ; (rs = uc_valloc(flen,&fbuf)) >= 0) {
	            {
		        rs = filer(fbuf,flen,fn) ;
		    }
		    rs = rsfree(rs,fbuf) ;
		    fbuf = nullptr ;
	        } /* end if (m-a-f) */
	        rs1 = rvend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (randomvar) */
	} /* end if (ucpagesize) */
	return rs ;
}
/* end method (burner::operator) */

int burner::filer(char *fbuf,int flen,cchar *fn) noex {
    	cint		of = O_WRONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
	if ((rs = uc_open(fn,of,om)) >= 0) {
	    cint	fd = rs ;
	    if (USTAT sb{} ; (rs = uc_fstat(fd,&sb)) >= 0) {
		if (S_ISREG(sb.st_mode) && (sb.st_size > 0)) {
		    fsz = sb.st_size ;
		    rs = writer(fd,fbuf,flen) ;
		} /* end if (regular non-zero file) */
	    } /* end if (uc_fstat) */
	    rs1 = uc_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return rs ;
}
/* end method (burner::filer) */

int burner::writer(int fd,char *fbuf,int flen) noex {
    	coff		msz = off_t(flen) ;
    	int		rs = SR_OK ;
	for (int i = 0 ; (rs >= 0) && (i < bcount) ; i += 1) {
	    off_t	tsz = 0 ;
	    if ((rs = rewind(fd,i)) >= 0) {
	        while ((rs >= 0) && (fsz > tsz)) {
		    cint	wl = int(min((fsz - tsz),msz)) ;
		    if ((rs = loadbuf(fbuf,wl)) >= 0) {
	                rs = uc_writen(fd,fbuf,wl) ;
	                tsz += rs ;
		    } /* end if (loadbuf) */
	        } /* end while */
	    } /* end if (rewind) */
	} /* end for (burn-count) */
	return rs ;
}
/* end mthod (burner::writer) */

int burner::rewind(int fd,int idx) noex {
    	int		rs = SR_OK ;
	if (idx > 1) {
	    rs = uc_seek(fd,0z,SEEK_SET) ;
	    uc_fdatasync(fd) ;
	}
	return rs ;
}
/* end method (burner::rewind) */

int burner::loadbuf(char *fbuf,int flen) noex {
    	return rvp->get(fbuf,flen) ;
}
/* end mthod (burner::loadbuf) */


