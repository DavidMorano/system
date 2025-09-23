/* mailspooldir SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check for a valid mail spool directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1997-01-10, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mailspooldir

	Description:
        This subroutine rates the value of a supplied mailspool directory name.
        A higher rated directory name returns a higher value on return.

	Synopsis:
	int mailspooldir(proginfo *pip,char *spooldname,int fl) noex

	Arguments:
	pip		- program information pointer
	spooldname	- buffer to receive candidate spool file
	fl		- optional length of directory name

	Returns:
	0		- spool file is minimally accessible
	1		- spool file is more accessible
	2		- et cetera
	<0		- not a good spool file (system-return)

        NOTE: 
	The big thing to note in this subroutine, and any other
	code that "pretends" to check for the existence of the mail
	spool directory, is that the directory may also be an
	AUTOMOUNT mount point.  This means that it might not be
	accessible when we go to check it alone.  Instead, we have
	to check something inside the directory to make sure that
	the AUTOMOUNTer first mounts the directory.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<sys/utsname.h>
#include	<unistd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<permx.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailspooldir(proginfo *pip,char *spooldname,int fl) noex {
	USTAT	sb ;
	int	rs, srs ;
	int	cl, glen ;

	char	tmpdname[MAXPATHLEN + 1] ;
	char	tmpfname[MAXPATHLEN + 1] ;
	char	*cp ;


	srs = -1 ;
	if ((spooldname == NULL) || (spooldname[0] == '\0'))
	    return srs ;

	strwcpy(tmpdname,spooldname,MIN(fl,(MAXPATHLEN - 1))) ;

/* try something inside the directory (because of automounting) */

	mkpath2(tmpfname,tmpdname,":saved") ;

	if ((u_stat(tmpfname,&sb) < 0) || (! S_ISDIR(sb.st_mode))) {

		mkpath2(tmpfname,tmpdname,"root") ;

	if (u_stat(tmpfname,&sb) < 0)
	    return srs ;

	}

/* OK, let's try to continue */

	rs = -1 ;

/* handle the common case first (it doesn't really matter) */

	if (pip->fl.setgid)
	    rs = perm(tmpdname,-1,pip->egid,NULL,W_OK) ;

	if (rs < 0)
	    rs = perm(tmpdname,-1,-1,NULL,W_OK) ;

	if ((rs < 0) && pip->fl.setuid)
	    rs = perm(tmpdname,pip->euid,-1,NULL,W_OK) ;

	if (rs < 0) {

/* try harder */

#ifdef	COMMENT
	    if ((pip->euid == 0) &&
	        (sb.st_gid != pip->egid) && (sb.st_gid != pip->gid)) {
	    }
#endif /* COMMENT */

	    if ((pip->uid != 0) && (pip->euid != 0))
	        return srs ;

	} /* end if */

	srs = 0 ;

/* is the last component of the MAILDIR path named "mail" ? */

	cl = sfbasename(tmpdname,-1,&cp) ;

	glen = strlen(MAILGNAME) ;

	if ((cl == 4) && (strncmp(cp,MAILGNAME,glen) == 0) )
		srs +=1 ;

/* is there a spool file there that the REAL user can read ? */

	if (u_access(tmpdname,W_OK) >= 0) 
		srs += 1 ;

	return srs ;
}
/* end subroutine (mailspooldir) */



