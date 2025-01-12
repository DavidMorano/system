/* checklockfile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check if we still own the lock file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Name:
	checklockfile

	Description:
	We want to check that we still own the lock file and also
	update it by writting a current time into it.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<cfdec.h>
#include	<timestr.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progchecklock(proginfo *pip,bfile *fp,
		cc *filename,cc *banner,cc *daytime,pid_t pid) noex {
	USTAT	sb1 ;
	USTAT	sb2 ;
	pid_t	oldpid ;
	int	rs ;
	int	iw, len ;
	char	buf[BUFLEN + 1] ;
	char	timebuf[TIMEBUFLEN + 1] ;

	if (fp == NULL) 
		return -1001 ;

/* rewind */

	if ((rs = bseek(fp,0L,SEEK_SET)) < 0)
		return rs ;

/* read the first line */

	if ((rs = breadln(fp,buf,BUFLEN)) <= 0) 
		return -1002 ;

	len = rs ;
	if (cfdeci(buf,len,&iw) < 0) 
		return -1003 ;

	oldpid = (pid_t) iw ;

	if (oldpid != pip->pid) 
		return ((int) pid) ;

/* is the file the same file ? */

	if (u_stat(filename,&sb1) >= 0) {

	    if (bcontrol(fp,BC_STAT,&sb2) < 0)
	        return -1004 ;

	    if ((sb1.st_dev != sb2.st_dev) ||
	        (sb1.st_ino != sb2.st_ino)) 
		return -1005 ;

	} /* end if */

/* read the second line (the 'node!user' thing) */

	breadln(fp,buf,BUFLEN) ;

/* write an updated time at this offset in the file */

	bprintf(fp,"%s", timestr_logz(daytime,timebuf)) ;

	bflush(fp) ;

	return OK ;
}
/* end subroutine (progchecklockfile) */


