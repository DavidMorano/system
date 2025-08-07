/* ucopy SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* copy a file (to another file) */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-07-13, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_copy

	Description:

	Synopsis:
	int u_copy(cchar *srcfname,cchar *dstfname) noex

	Arguments:
	srcfname	source file
	dstfname	destination file

	Returns:
	>=0		number of items removed
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

static int	copyover(int,int) noex ;
static int	copyovers(char *,int,int,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int u_writefd(int fd,int sfd) noex {
    	int		rs = SR_BADF ;
	if ((fd >= 0) && (sfd >= 0)) {
	    rs = copyover(sfd,fd) ;
	}
	return rs ;
} /* end subroutine (u_writefd) */

int u_writefile(int fd,cchar *fn) noex {
    	int		rs = SR_BADF ;
	int		rs1 ;
	int		len = 0 ;
	if (fd >= 0) {
	    rs = SR_FAULT ;
	    if (fn) {
		rs = SR_INVALID ;
		if (fn[0]) {
		    cint	of = O_RDONLY ;
		    if ((rs = u_open(fn,of,0)) >= 0) {
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
} /* end subroutine (u_writefd) */

int u_copy(cchar *srcfname,cchar *dstfname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		csz = 0 ;
	if (srcfname && dstfname) ylikely {
	    rs = SR_INVALID ;
	    if (srcfname[0] && dstfname[0]) ylikely {
		mode_t	om = O_RDONLY ;
		if ((rs = u_open(srcfname,om,0)) >= 0) {
		    cint fd = rs ;
		    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) {
			cmode dm = (sb.st_mode & 0777) ;
			om = (O_WRONLY | O_CREAT) ;
			if ((rs = u_open(dstfname,om,dm)) >= 0) {
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

static int copyover(int fd,int dfd) noex {
    	cnothrow	nt{} ;
    	cnullptr	np{} ;
    	int		rs ;
	int		csz = 0 ;
    	if (cint cmd = _SC_PAGESIZE ; (rs = usysconfval(cmd)) >= 0) {
	    cint	clen = rs ;
	    rs = SR_NOMEM ;
	    if (char *cbuf ; (cbuf = new(nt) char[clen + 1]) != np) {
		{
		    rs = copyovers(cbuf,clen,fd,dfd) ;
		    csz = 1 ;
		}
		delete [] cbuf ;
	    } /* end if (new-char) */
	} /* end if (usysconf) */
	return (rs >= 0) ? csz : rs ;
} /* end subroutine (copyover) */

static int copyovers(char *cbuf,int clen,int fd,int dfd) noex {
    	int		rs ;
	int		csz = 0 ;
	size_t		fsize = 0 ;
	while ((rs = u_read(fd,cbuf,clen)) > 0) {
	    rs = u_writen(dfd,cbuf,rs) ;
	    fsize += rs ;
	} /* end while */
	if (rs >= 0) csz = intsat(fsize) ;
	return (rs >= 0) ? csz : rs ;

} /* end subroutine (copyovers) */


