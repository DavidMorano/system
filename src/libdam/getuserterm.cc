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
	int getuserterm(char *tbuf,int tlen,int *fdp,cchar *username) noex

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
#include	<mkpathx.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"getuserterm.h"


/* local defines */

#define	DEVDNAME	"/dev/"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	openatime(char *,time_t,time_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getuserterm(char *tbuf,int tlen,int *fdp,cchar *username) noex {
	int	rs ;
	int	rs1 ;
	int	len = 0 ;
	if (tbuf && username) {
	    rs = SR_INVALID ;
	    tbuf[0] = '\0' ;
	    if (username[0]) {
	tmpx		utmp ;
	time_t		ti_tmp ;
	time_t		ti_access = 0 ;
	int	fd_termdev = -1 ;
	cchar		devdname[] = DEVDNAME ;
	char	tmptermdev[MAXPATHLEN + 1] ;
	if ((rs = tmpx_open(&utmp,NULL,O_RDONLY)) >= 0) {
	    tmpx_cur	cur ;
	    tmpx_ent	ue ;

	    if ((rs = tmpx_curbegin(&utmp,&cur)) >= 0) {
		bool	f ;
	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(&utmp,&cur,&ue,username) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;

	            f = FALSE ;
	            f = f || (ue.ut_type != TMPX_TPROCUSER) ;
	            f = f || (ue.ut_line[0] == '\0') ;
	            if (f) continue ;

	            rs = mkpath2w(tmptermdev,devdname,ue.ut_line,32) ;
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

	        rs1 = tmpx_curend(&utmp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */

	    rs1 = tmpx_close(&utmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (UTMPX open) */

	if ((rs >= 0) && (tbuf[0] == '\0')) {
	    rs = SR_NOTFOUND ;
	    if (fd_termdev >= 0) {
	        u_close(fd_termdev) ;
	        fd_termdev = -1 ;
	    }
	} /* end if */

	if (fdp) {
	    *fdp = (rs >= 0) ? fd_termdev : -1 ;
	} else if (fd_termdev >= 0) {
	    u_close(fd_termdev) ;
	}

	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserterm) */


/* local subroutines */

static int openatime(char *termdev,time_t current,time_t *tp) noex {
	cint		of = O_WRONLY ;
	int		rs ;
	int		fd = -1 ;
	cmode		om = 0666 ;
	if ((rs = u_open(termdev,of,om)) >= 0) {
	    USTAT	sb ;
	    fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        *tp = sb.st_atime ;
	        bool	f = false ;
	        f = f || ((sb.st_mode & S_IWGRP) != S_IWGRP) ;
	        f = f || (sb.st_atime <= current) ;
	        if (f) rs = SR_INVALID ;
	    } /* end if */
	    if (rs < 0) {
	        u_close(fd) ;
	    }
	} /* end if (open) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openatime) */


