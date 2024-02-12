/* tcsetown SUPPORT */
/* lang=C++20 */

/* set ownership of a terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tcsetown

	Description:
	This subroutine is similar to |logindevperm(3c)| (or whatever
	equivalent your system has). It sets the ownership of a
	terminal.

	Synopsis:
	int tcsetown(int fd,cchar *tdev,uid_t uid,gid_t gid,mode_t tp) noex

	Arguments:
	fd		file-descriptor of terminal
	tdev		device path to terminal device
	uid		UID to set
	gid		GID to set
	tp		permissions to set

	Returns:
	>=0		success
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<termios.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int tcsetown(int fd,cchar *termdev,uid_t uid,gid_t gid,mode_t perms) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    rs = SR_NOTTY ;
	    perms &= S_IAMB ;
	    if (isatty(fd)) {
	        if ((rs = u_fchmod(fd,perms)) >= 0) {
	            if ((rs = u_fchown(fd,uid,gid)) >= 0) {
	                if (termdev && termdev[0]) {
	                    if ((rs = u_chmod(termdev,perms)) >= 0) {
	                        rs = u_chown(termdev,uid,gid) ;
			    }
		        }
	            }
	        }
	    } /* end if (isatty) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (tcsetown) */


