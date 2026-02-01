/* ucopy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a file (to another file or a file-descriptor) */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_writefd
	u_writefile
	u_copy

	Description:
	These subroutines provide various copy-file operations.

	Synopsis:
	int u_writefd(int fd,int sfd) noex
	int u_writefile(int fd,cchar *fn) noex
	int u_copy(cchar *srcfname,cchar *dstfname) noex

	Arguments:
	fd		file-descriptor to write to
	sfd		file-descriptor of source file (to copy from)
	fn		file to copy-out and write to given FD
	srcfname	source file
	dstfname	destination file

	Returns:
	>=0		number of bytes copied (up to INT_MAX)
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<numeric>		/* |sat_add(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>
#include	<intsat.h>
#include	<localmisc.h>

#include	"ucopy.h"

#pragma		GCC dependency		"mod/usysconf.ccm"

import usysconf ;			/* |usysconfval(3u)| */

/* local defines */

#define	PAGEMULT		4	/* page-multiplier */
#define	ALLOCMIN		128	/* allocation-minimum */


/* imported namespaces */

using std::min ;			/* subroutine */
using std::max ;			/* subroutine */
using libu::um ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	pagesz ;
	operator int () noex ;
    } ; /* end struct */
} /* end namespace */


/* forward references */

local int	getsize(int) noex ;
local int	copyover(int,int) noex ;
local int	copyovers(char *,int,int,int) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int u_writefd(int fd,int sfd) noex {
    	int		rs = SR_BADF ;
	if ((fd >= 0) && (sfd >= 0)) ylikely {
	    rs = copyover(sfd,fd) ;
	}
	return rs ;
} /* end subroutine (u_writefd) */

int u_writefile(int fd,cchar *fn) noex {
    	int		rs = SR_BADF ;
	int		rs1 ;
	int		len = 0 ;
	if (fd >= 0) ylikely {
	    rs = SR_FAULT ;
	    if (fn) ylikely {
		rs = SR_INVALID ;
		if (fn[0]) ylikely {
		    cint	of = O_RDONLY ;
		    if ((rs = u_open(fn,of,0)) >= 0) ylikely {
			cint	sfd = rs ;
			{
	    	            rs = copyover(sfd,fd) ;
			    len = rs ;
			}
			rs1 = u_close(sfd) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (u_open) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (valid FD) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (u_writefile) */

int u_copy(cchar *srcfname,cchar *dstfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		csz = 0 ;
	if (srcfname && dstfname) ylikely {
	    rs = SR_INVALID ;
	    if (srcfname[0] && dstfname[0]) ylikely {
		int	of = O_RDONLY ;
		if ((rs = u_open(srcfname,of,0)) >= 0) ylikely {
		    cint fd = rs ;
		    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
			cmode dm = (sb.st_mode & S_IAMB) ;
			of = (O_WRONLY | O_CREAT) ;
			if ((rs = u_open(dstfname,of,dm)) >= 0) ylikely {
			    cint	dfd = rs ;
			    {
				rs = copyover(fd,dfd) ;
				csz = rs ;
			    }
			    rs1 = u_close(dfd) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (u_open) */
		    } /* end if (u_fstat) */
		    rs1 = u_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (u_open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? csz : rs ;
} /* end subroutine (u_copy) */


/* local subroutines */

local int copyover(int fd,int dfd) noex {
	static cint	rsv = var ;
    	int		rs ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
    	if ((rs = rsv) >= 0) ylikely {
	    if ((rs = getsize(fd)) >= 0) ylikely {
		cint fsz = rs ;
	        cint csz = (var.pagesz * PAGEMULT) ;
		{
		    cint	clen = min(fsz,csz) ;
	            if (char *cbuf ; (rs = um.mall(clen,&cbuf)) >= 0) ylikely {
		        {
		            rs = copyovers(cbuf,clen,fd,dfd) ;
		            tlen = rs ;
		        }
		        rs1 = um.free(cbuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end block */
	    } /* end if (getsize) */
	} /* end if (vars) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (copyover) */

local int copyovers(char *cbuf,int clen,int fd,int dfd) noex {
    	int		rs ;
	int		csz = 0 ; /* return-value */
	size_t		fsize = 0 ;
	while ((rs = u_read(fd,cbuf,clen)) > 0) {
	    rs = u_writen(dfd,cbuf,rs) ;
	    fsize += rs ;
	    if (rs < 0) break ;
	} /* end while */
	if (rs >= 0) csz = intsat(fsize) ;
	return (rs >= 0) ? csz : rs ;
} /* end subroutine (copyovers) */

local int getsize(int fd) noex {
    	int		rs ;
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	    if (S_ISREG(sb.st_mode)) ylikely {
		if (sb.st_size == 0z) {
		    rs = min(var.pagesz,ALLOCMIN) ;
		} else {
		    clong fsize = long(sb.st_size) ;
		    rs = intsat(fsize) ;
		}
	    } else {
		rs = var.pagesz ;
	    }
	} /* end if (u_fstat) */
    	return rs ;
} /* end subroutine (getsize) */

vars::operator int () noex {
    	int		rs ;
    	if (cint cmd = _SC_PAGESIZE ; (rs = usysconfval(cmd)) >= 0) ylikely {
	    pagesz = rs ;
	}
    	return rs ;
} /* end method (vars::operator) */


