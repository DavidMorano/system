/* snshellunder SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make the shell-under information string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snshellunder

	Description:
	This subroutine creates the shell-under string for a child
	process.  The string looks like:
		*<ppid>*<execfname>
	where:
	ppid		parent process ID
	execfname	program exec-file-name

	Synopsis:
	int snshellunder(char *dbuf,int dlen,pid_t pid,cchar *) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	pid		parent process ID
	execdname	exec-file-name

	Returns:
	>=0		length of created string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forwards references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snshellunder(char *dbuf,int dlen,pid_t pid,cchar *execfname) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ;
	if (dbuf && execfname) {
	    storebuf db(dbuf,dlen) ; 
	    rs = SR_OK ;
	    if (pid >= 0) {
		if (rs >= 0) {
		    rs = sb.chr('*') ;
		}
	        if (rs >= 0) {
	            uint v = uint(pid) ;
	            rs = sb.dec(v) ;
	        }
	        if (rs >= 0) {
	            rs = sb.chr(mch) ;
	        }
	    } /* end if (PID included) */
	    if (rs >= 0) {
	        rs = sb.str(execfname) ;
	        idx = sb.idx ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (snshellunder) */


