/* snshellunder SUPPORT */
/* charset=ISO8859-1 */
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

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |pid_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forwards references */


/* local variables */

cint		dch = '*' ;


/* exported variables */


/* exported subroutines */

int snshellunder(char *dbuf,int dlen,pid_t pid,cchar *execfname) noex {
	int		rs = SR_FAULT ;
	int		idx = 0 ;
	if (dbuf && execfname) ylikely {
	    storebuf	b(dbuf,dlen) ; 
	    rs = SR_OK ;
	    if (pid >= 0) ylikely {
		if (rs >= 0) ylikely {
		    rs = b.chr(dch) ;
		}
	        if (uint uv = uint(pid) ; rs >= 0) ylikely {
	            rs = b.dec(uv) ;
	        }
	        if (rs >= 0) ylikely {
	            rs = b.chr(dch) ;
	        }
	    } /* end if (PID included) */
	    if (rs >= 0) ylikely {
	        rs = b.str(execfname) ;
	        idx = b.idx ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (snshellunder) */


