/* mkdir SUPPORT */
/* lang=C20 */

/* make a directory */
/* version %I% last-modified %G% */


/* revistion history:

	= 1982-07-10, David A­D­ Morano
	This was originally written.

*/

/* Copyright © 1987 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkdir

	Description:
	This is a library version of the |mkdir(1)| program.  They
	ought to get this sort of subroutine as a system call one
	of these days.  This is outragious!

	Synopsis:
	int mkdir(cchar *dir,mode_t m) noex

	Arguments:
	dir		string of directory name
	m		file permission bits (modified by UMASK)

	Returns:
	0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/wait.h>
#include	<sys/param.h>
#include	<fcntl.h>
#include	<localmisc.h>


/* exported subroutines */

int mkdir(cchar *dir,mode_t m) noex {
	int		rs = SR_FAULT ;
	if (dir) {
	    cchar	*dn = "/dev/null" ;
	    if ((rs = fork()) == 0) {
	        u_close(1) ;		/* close standard output */
	        u_open(dn,O_WRONLY,0664) ;
	        u_close(2) ;		/* close standard error */
	        u_open(dn,O_WRONLY,0664) ;
	        execl("/bin/mkdir","mkdir",dir,0) ;
	        u_exit(127) ;
	    }
	    if (rs > 0) {
	        const pid_t	pid = rs ;
	        int		child_stat ;
	        rs = u_waitpid(pid,&child_stat,0) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkdir) */


