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
	These provide various copy-file operations.

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
#include	<usyscalls.h>
#include	<intsat.h>
#include	<localmisc.h>

#pragma		GCC dependency	"mod/usysconf.ccm"

#include	"ucopy.h"

import usysconf ;			/* |usysconval(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	copyover(int,int) noex ;
local int	copyovers(char *,int,int,int) noex ;


/* local variables */


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
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
    	int		rs ;
	int		csz = 0 ;
    	if (cint cmd = _SC_PAGESIZE ; (rs = usysconfval(cmd)) >= 0) ylikely {
	    cint	clen = rs ;
	    rs = SR_NOMEM ;
	    if (char *cbuf ; (cbuf = new(nt) char[clen + 1]) != np) ylikely {
		{
		    rs = copyovers(cbuf,clen,fd,dfd) ;
		    csz = rs ;
		}
		delete [] cbuf ;
	    } /* end if (new-char) */
	} /* end if (usysconf) */
	return (rs >= 0) ? csz : rs ;
} /* end subroutine (copyover) */

local int copyovers(char *cbuf,int clen,int fd,int dfd) noex {
    	int		rs ;
	int		csz = 0 ;
	size_t		fsize = 0 ;
	while ((rs = u_read(fd,cbuf,clen)) > 0) {
	    rs = u_writen(dfd,cbuf,rs) ;
	    fsize += rs ;
	    if (rs < 0) break ;
	} /* end while */
	if (rs >= 0) csz = intsat(fsize) ;
	return (rs >= 0) ? csz : rs ;
} /* end subroutine (copyovers) */


