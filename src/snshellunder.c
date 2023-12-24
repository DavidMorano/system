/* snshellunder */
/* lang=C20 */

/* make the shell-under information string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"snx.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int snshellunder(char *dbuf,int dlen,pid_t pid,cchar *execfname) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf && execfname) {
	    rs = SR_OK ;
	    if (pid >= 0) {
	        cint	mch = '*' ;
	        if (rs >= 0) {
	            rs = storebuf_char(dbuf,dlen,i,mch) ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            uint	v = (int) pid ;
	            rs = storebuf_decui(dbuf,dlen,i,v) ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_char(dbuf,dlen,i,mch) ;
	            i += rs ;
	        }
	    } /* end if (PID included) */
	    if (rs >= 0) {
	        rs = storebuf_strw(dbuf,dlen,i,execfname,-1) ;
	        i += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (snshellunder) */


