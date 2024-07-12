/* getuserterm SUPPORT */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This subroutine was originally written.  It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserterm

	Description:
	This subroutine will find and return an open FD for the
	controlling terminal for the given username, if that user
	is logged in and even has a controlling terminal.

	Synopsis:
	int getuserterm(tbuf,tlen,fdp,username)
	char		tbuf[] ;
	int		tlen ;
	int		*fdp ;
	const char	username[] ;

	Arguments:
	- tbuf		user buffer to receive name of controlling terminal
	- tlen		length of user supplied buffer
	- fdp		pointer to open file-descriptor
	- username	session ID to find controlling terminal for

	Returns:
	>=	length of name of controlling terminal
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<tmpx.h>
#include	<localmisc.h>


/* local defines */

#define	DEVDNAME	"/dev/"


/* external subroutines */

extern int	sncpy1(char *,int,const char *) ;
extern int	mkpath1(char *,const char *) ;
extern int	mkpath2(char *,const char *,const char *) ;
extern int	mkpath1w(char *,const char *,int) ;
extern int	mkpath2w(char *,const char *,const char *,int) ;

extern char	*strwcpy(char *,const char *,int) ;


/* forward references */

static int	openatime(char *,time_t,time_t *) ;


/* exported variables */


/* exported subroutines */

int getuserterm(tbuf,tlen,fdp,username)
const char	username[] ;
char		tbuf[] ;
int		tlen ;
int		*fdp ;
{
	TMPX		utmp ;

	time_t	ti_tmp ;
	time_t	ti_access = 0 ;

	int	rs ;
	int	rs1 ;
	int	len = 0 ;
	int	fd_termdev = -1 ;
	int	f ;

	const char	*devdname = DEVDNAME ;

	char	tmptermdev[MAXPATHLEN + 1] ;
	char	termdev[MAXPATHLEN + 1] ;


	if (tbuf == NULL)
	    return SR_FAULT ;

	if (username == NULL)
	    return SR_FAULT ;

	tbuf[0] = '\0' ;
	if (username[0] == '\0')
	    return SR_INVALID ;

	termdev[0] = '\0' ;
	fd_termdev = -1 ;
	ti_access = 0 ;
	if ((rs = tmpx_open(&utmp,NULL,O_RDONLY)) >= 0) {
	    TMPX_CUR	cur ;
	    TMPX_ENT	ue ;

	    if ((rs = tmpx_curbegin(&utmp,&cur)) >= 0) {

	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(&utmp,&cur,&ue,username) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;

	            f = FALSE ;
	            f = f || (ue.ut_type != TMPX_TUSERPROC) ;
	            f = f || (ue.ut_line[0] == '\0') ;
	            if (f) continue ;

	            rs = mkpath2w(tmptermdev,devdname,ue.ut_line,32) ;

/* check the access time of this terminal and if it is enabled for notices */

	            if (rs >= 0) {

	                rs1 = openatime(tmptermdev,ti_access,&ti_tmp) ;

	                if (rs1 >= 0) {
	                    if (fd_termdev >= 0) u_close(fd_termdev) ;
	                    fd_termdev = rs1 ;

	                    ti_access = ti_tmp ;
	                    rs = sncpy1(tbuf,tlen,tmptermdev) ;
	                    len = rs ;

	                } /* end if (we had a better one) */

	            } /* end if */

	            if (rs < 0) break ;

	        } /* end while (looping through entries) */

	        tmpx_curend(&utmp,&cur) ;
	    } /* end if */

	    tmpx_close(&utmp) ;
	} /* end if (UTMPX open) */

	if (rs < 0) goto ret0 ;

	if (tbuf[0] == '\0') {
	    rs = SR_NOTFOUND ;
	    if (fd_termdev >= 0) {
	        u_close(fd_termdev) ;
	        fd_termdev = -1 ;
	    }
	} /* end if */

/* finish up */

	if (fdp != NULL) {
	    *fdp = (rs >= 0) ? fd_termdev : -1 ;
	} else if (fd_termdev >= 0)
	    u_close(fd_termdev) ;

ret0:

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserterm) */


/* local subroutines */


static int openatime(termdev,current,tp)
char	termdev[] ;
time_t	current ;
time_t	*tp ;
{
	struct ustat	sb ;
	int		rs ;
	int		fd = -1 ;

	rs = u_open(termdev,O_WRONLY,0666) ;
	fd = rs ;
	if (rs < 0) goto ret0 ;

	if ((rs = u_fstat(fd,&sb)) >= 0) {

	    *tp = sb.st_atime ;
	    if (((sb.st_mode & S_IWGRP) != S_IWGRP) ||
	        (sb.st_atime <= current))
	        rs = SR_INVALID ;

	} /* end if */

	if (rs < 0)
	    u_close(fd) ;

ret0:
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openatime) */



