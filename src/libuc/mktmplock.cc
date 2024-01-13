/* mktmplock SUPPORT */
/* lang=C++20 */

/* make a temporary file which is "lockable" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmplock

	Description:
	Make a temporary file that is also record lockable.  Why
	does this even exist (you might ask)?  Because with the
	introduction of the 'tmpfs' file-system in System V Release
	4 (like Sun Solaris as primary example) in 1988, files put
	into that file-system are not lockable !!  Unfortunately,
	'/tmp' is mounted as a 'tmpfs' file-system !!  That is a
	place where file locks were often kept (sigh)!

	Synopsis:
	int mktmplock(tmpdirs,fname,perm,pathname)
	cchar	*tmpdirs[] ;
	cchar	fname[] ;
	mode_t		perm ;
	char		pathname[] ;

	Arguments:
	tmpdirs		array of strings of directory names (NULL terminated)
	fname		file creation template string
	perm		file creation mode
	pathname	resultant buffer to hold final file path name

	Returns:
	>=0		length of created temporary file if successful
	<0		error code (systerm-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<mkfile.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* forward references */

static int	lockable(cchar *) noex ;


/* exported subroutines */

int mktmplock(tmpdirs,fname,perm,pathname)
cchar	*tmpdirs[] ;
cchar	fname[] ;
mode_t		perm ;
char		pathname[] ;
{
	int		rs = SR_INVALID ;
	int		sl ;
	int		i ;
	char		buf[MAXPATHLEN + 1] ;
	char		*cp ;

	if (pathname == NULL) return SR_FAULT ;

	for (i = 0 ; tmpdirs[i] != NULL ; i += 1) {

	    cp = strwcpy(buf,tmpdirs[i],MAXPATHLEN) ;

	    if ((cp - buf + 2) <= (MAXPATHLEN + 1))
		*cp++ = '/' ;

	    if ((cp - buf + 1) <= (MAXPATHLEN + 1))
		cp = strwcpy(cp,fname,(MAXPATHLEN - (cp - buf))) ;

	    if ((cp - buf) > MAXPATHLEN) {
		rs = SR_TOOBIG ;
		continue ;
	    }

	    sl = mktmpfile(pathname,perm,buf) ;

	    if (sl >= 0) {

		rs = lockable(pathname) ;
		if (rs >= 0) break ;

		u_unlink(pathname) ;

	    } /* end if (made one) */

	} /* end for */

	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (mktmplock) */


/* local subroutines */

static int lockable(cchar *fname) noex {
	int		rs ;
	if ((rs = u_open(fname,O_RDWR,0666)) >= 0) {
	    const int	fd = rs ;
	    rs = uc_lockf(fd,F_TLOCK,0L) ;
	    u_close(fd) ;
	}
	return rs ;
}
/* end subroutine (lockable) */


